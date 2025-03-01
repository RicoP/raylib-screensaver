// Based on https://github.com/raysan5/raylib/blob/master/examples/others/rlgl_standalone.c

// NOTE: rlgl can be configured just re-defining the following values:
//#define RL_DEFAULT_BATCH_BUFFER_ELEMENTS   8192    // Default internal render batch elements limits
//#define RL_DEFAULT_BATCH_BUFFERS              1    // Default number of batch buffers (multi-buffering)
//#define RL_DEFAULT_BATCH_DRAWCALLS          256    // Default number of batch draw calls (by state changes: mode, texture)
//#define RL_DEFAULT_BATCH_MAX_TEXTURE_UNITS    4    // Maximum number of textures units that can be activated on batch drawing (SetShaderValueTexture())
//#define RL_MAX_MATRIX_STACK_SIZE             32    // Maximum size of internal Matrix stack
//#define RL_MAX_SHADER_LOCATIONS              32    // Maximum number of shader locations supported
//#define RL_CULL_DISTANCE_NEAR              0.01    // Default projection matrix near cull distance
//#define RL_CULL_DISTANCE_FAR             1000.0    // Default projection matrix far cull distance

//#define RLGL_IMPLEMENTATION
#define RAYMATH_STATIC_INLINE
#define _GLFW_WIN32 1

extern "C" {
#include "rlgl.h"               // OpenGL abstraction layer to OpenGL 1.1, 3.3+ or ES2
#include "raymath.h"            // Vector2, Vector3, Quaternion and Matrix functionality
#include "internal.h"
#include "GLFW/glfw3.h"         // Windows/Context and inputs management
}

extern "C" HWND g_hWnd;

// Creates the GLFW window
//
static int createNativeWindow(_GLFWwindow* window,
	const _GLFWwndconfig* wndconfig,
	const _GLFWfbconfig* fbconfig)
{
#if 0
	int xpos, ypos, fullWidth, fullHeight;
	WCHAR* wideTitle;
	DWORD style = getWindowStyle(window);
	DWORD exStyle = getWindowExStyle(window);


	if (window->monitor)
	{
		MONITORINFO mi = { sizeof(mi) };
		GetMonitorInfoW(window->monitor->win32.handle, &mi);

		// NOTE: This window placement is temporary and approximate, as the
		//       correct position and size cannot be known until the monitor
		//       video mode has been picked in _glfwSetVideoModeWin32
		xpos = mi.rcMonitor.left;
		ypos = mi.rcMonitor.top;
		fullWidth = mi.rcMonitor.right - mi.rcMonitor.left;
		fullHeight = mi.rcMonitor.bottom - mi.rcMonitor.top;
	}
	else
	{
		RECT rect = { 0, 0, wndconfig->width, wndconfig->height };

		window->win32.maximized = wndconfig->maximized;
		if (wndconfig->maximized)
			style |= WS_MAXIMIZE;

		AdjustWindowRectEx(&rect, style, FALSE, exStyle);

		if (wndconfig->xpos == GLFW_ANY_POSITION && wndconfig->ypos == GLFW_ANY_POSITION)
		{
			xpos = CW_USEDEFAULT;
			ypos = CW_USEDEFAULT;
		}
		else
		{
			xpos = wndconfig->xpos + rect.left;
			ypos = wndconfig->ypos + rect.top;
		}

		fullWidth = rect.right - rect.left;
		fullHeight = rect.bottom - rect.top;
	}

	wideTitle = _glfwCreateWideStringFromUTF8Win32(wndconfig->title);
	if (!wideTitle)
		return GLFW_FALSE;

	window->win32.handle = CreateWindowExW(exStyle,
		MAKEINTATOM(_glfw.win32.mainWindowClass),
		wideTitle,
		style,
		xpos, ypos,
		fullWidth, fullHeight,
		NULL, // No parent window
		NULL, // No window menu
		_glfw.win32.instance,
		(LPVOID)wndconfig);

	_glfw_free(wideTitle);

	if (!window->win32.handle)
	{
		_glfwInputErrorWin32(GLFW_PLATFORM_ERROR,
			"Win32: Failed to create window");
		return GLFW_FALSE;
	}

	SetPropW(window->win32.handle, L"GLFW", window);

	if (IsWindows7OrGreater())
	{
		ChangeWindowMessageFilterEx(window->win32.handle,
			WM_DROPFILES, MSGFLT_ALLOW, NULL);
		ChangeWindowMessageFilterEx(window->win32.handle,
			WM_COPYDATA, MSGFLT_ALLOW, NULL);
		ChangeWindowMessageFilterEx(window->win32.handle,
			WM_COPYGLOBALDATA, MSGFLT_ALLOW, NULL);
	}

	window->win32.scaleToMonitor = wndconfig->scaleToMonitor;
	window->win32.keymenu = wndconfig->win32.keymenu;

	if (!window->monitor)
	{
		RECT rect = { 0, 0, wndconfig->width, wndconfig->height };
		WINDOWPLACEMENT wp = { sizeof(wp) };
		const HMONITOR mh = MonitorFromWindow(window->win32.handle,
			MONITOR_DEFAULTTONEAREST);

		// Adjust window rect to account for DPI scaling of the window frame and
		// (if enabled) DPI scaling of the content area
		// This cannot be done until we know what monitor the window was placed on
		// Only update the restored window rect as the window may be maximized

		if (wndconfig->scaleToMonitor)
		{
			float xscale, yscale;
			_glfwGetHMONITORContentScaleWin32(mh, &xscale, &yscale);

			if (xscale > 0.f && yscale > 0.f)
			{
				rect.right = (int)(rect.right * xscale);
				rect.bottom = (int)(rect.bottom * yscale);
			}
		}

		if (_glfwIsWindows10Version1607OrGreaterWin32())
		{
			AdjustWindowRectExForDpi(&rect, style, FALSE, exStyle,
				GetDpiForWindow(window->win32.handle));
		}
		else
			AdjustWindowRectEx(&rect, style, FALSE, exStyle);

		GetWindowPlacement(window->win32.handle, &wp);
		OffsetRect(&rect,
			wp.rcNormalPosition.left - rect.left,
			wp.rcNormalPosition.top - rect.top);

		wp.rcNormalPosition = rect;
		wp.showCmd = SW_HIDE;
		SetWindowPlacement(window->win32.handle, &wp);

		// Adjust rect of maximized undecorated window, because by default Windows will
		// make such a window cover the whole monitor instead of its workarea

		if (wndconfig->maximized && !wndconfig->decorated)
		{
			MONITORINFO mi = { sizeof(mi) };
			GetMonitorInfoW(mh, &mi);

			SetWindowPos(window->win32.handle, HWND_TOP,
				mi.rcWork.left,
				mi.rcWork.top,
				mi.rcWork.right - mi.rcWork.left,
				mi.rcWork.bottom - mi.rcWork.top,
				SWP_NOACTIVATE | SWP_NOZORDER);
		}
	}

	DragAcceptFiles(window->win32.handle, TRUE);

	if (fbconfig->transparent)
	{
		updateFramebufferTransparency(window);
		window->win32.transparent = GLFW_TRUE;
	}

	_glfwGetWindowSizeWin32(window, &window->win32.width, &window->win32.height);
#endif
	window->win32.handle = g_hWnd;
	return GLFW_TRUE;
}

GLFWbool CustomCreateWindowHook(_GLFWwindow* window,
	const _GLFWwndconfig* wndconfig,
	const _GLFWctxconfig* ctxconfig,
	const _GLFWfbconfig* fbconfig)
{
	if (!createNativeWindow(window, wndconfig, fbconfig))
		return GLFW_FALSE;

	if (ctxconfig->client != GLFW_NO_API)
	{
		if (ctxconfig->source == GLFW_NATIVE_CONTEXT_API)
		{
			if (!_glfwInitWGL())
				return GLFW_FALSE;
			if (!_glfwCreateContextWGL(window, ctxconfig, fbconfig))
				return GLFW_FALSE;
		}
		else if (ctxconfig->source == GLFW_EGL_CONTEXT_API)
		{
			if (!_glfwInitEGL())
				return GLFW_FALSE;
			if (!_glfwCreateContextEGL(window, ctxconfig, fbconfig))
				return GLFW_FALSE;
		}
		else if (ctxconfig->source == GLFW_OSMESA_CONTEXT_API)
		{
			if (!_glfwInitOSMesa())
				return GLFW_FALSE;
			if (!_glfwCreateContextOSMesa(window, ctxconfig, fbconfig))
				return GLFW_FALSE;
		}

		if (!_glfwRefreshContextAttribs(window, ctxconfig))
			return GLFW_FALSE;
	}

	if (wndconfig->mousePassthrough)
		_glfwSetWindowMousePassthroughWin32(window, GLFW_TRUE);

	if (window->monitor)
	{
		_glfwShowWindowWin32(window);
		_glfwFocusWindowWin32(window);
		//acquireMonitor(window);
		//fitToMonitor(window);

		if (wndconfig->centerCursor)
			_glfwCenterCursorInContentArea(window);
	}
	else
	{
		if (wndconfig->visible)
		{
			_glfwShowWindowWin32(window);
			if (wndconfig->focused)
				_glfwFocusWindowWin32(window);
		}
	}

	return GLFW_TRUE;
}

extern "C" _GLFWlibrary _glfw;

//----------------------------------------------------------------------------------
// Module specific Functions Declaration
//----------------------------------------------------------------------------------
static void ErrorCallback(int error, const char* description);

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------

extern "C" void Init();
extern "C" void Update();
extern "C" void Destroy();
extern "C" void ScreenSaveInitWindow();

extern "C" SIZE g_screenSize;
extern "C" GLFWwindow* g_window;

#define screenWidth  ((int)g_screenSize.cx)
#define screenHeight ((int)g_screenSize.cy)

void RayInit()
{
	// Initialization
	//--------------------------------------------------------------------------------------

	// GLFW3 Initialization + OpenGL 3.3 Context + Extensions
	//--------------------------------------------------------
	glfwSetErrorCallback(ErrorCallback);

	if (!glfwInit())
	{
		return;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_DEPTH_BITS, 16);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	_glfw.platform.createWindow = CustomCreateWindowHook;

	g_window = glfwCreateWindow(screenWidth, screenHeight, "rlgl standalone", NULL, NULL);

	if (!g_window)
	{
		glfwTerminate();
		return;
	}

	//glfwSetWindowPos(g_window, 2000, 200);

	glfwMakeContextCurrent(g_window);
	glfwSwapInterval(0);

	// Load OpenGL 3.3 supported extensions
	rlLoadExtensions(glfwGetProcAddress);
	//--------------------------------------------------------

	// Initialize OpenGL context (states and resources)
	rlglInit(screenWidth, screenHeight);

	// Initialize viewport and internal projection/modelview matrices
	rlViewport(0, 0, screenWidth, screenHeight);
	rlMatrixMode(RL_PROJECTION);                        // Switch to PROJECTION matrix
	rlLoadIdentity();                                   // Reset current matrix (PROJECTION)
	rlOrtho(0, screenWidth, screenHeight, 0, 0.0f, 1.0f); // Orthographic projection with top-left corner at (0,0)
	rlMatrixMode(RL_MODELVIEW);                         // Switch back to MODELVIEW matrix
	rlLoadIdentity();                                   // Reset current matrix (MODELVIEW)

	rlClearColor(245, 245, 245, 255);                   // Define clear color
	rlEnableDepthTest();                                // Enable DEPTH_TEST for 3D

	//--------------------------------------------------------------------------------------

	ScreenSaveInitWindow();
}

void RayDraw()
{
	static int frameCounter = 0;

	if(frameCounter == 0) 
	{
		Init();
	}

	Update();

	frameCounter++;
}

void RayDestroy()
{
	Destroy();

	// De-Initialization
	//--------------------------------------------------------------------------------------
	rlglClose();                    // Unload rlgl internal buffers and default shader/texture

	glfwDestroyWindow(g_window);      // Close window
	glfwTerminate();                // Free GLFW3 resources
	//--------------------------------------------------------------------------------------
}

//----------------------------------------------------------------------------------
// Module specific Functions Definitions
//----------------------------------------------------------------------------------

// GLFW3: Error callback
static void ErrorCallback(int error, const char* description)
{
	//fprintf(stderr, "%s", description);
	exit(1);
}
