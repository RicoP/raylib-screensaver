/*******************************************************************************************
*
*   raylib [rlgl] example - Using rlgl module as standalone module
*
*   rlgl library is an abstraction layer for multiple OpenGL versions (1.1, 2.1, 3.3 Core, ES 2.0)
*   that provides a pseudo-OpenGL 1.1 immediate-mode style API (rlVertex, rlTranslate, rlRotate...)
*
*   NOTE: This example requires OpenGL 3.3 or OpenGL ES 2.0 for shaders support,
*         OpenGL 1.1 does not support shaders but it can also be used.
*
*   DEPENDENCIES:
*       glfw3     - Windows and context initialization library
*       rlgl.h    - OpenGL abstraction layer to OpenGL 1.1, 3.3 or ES2
*       glad.h    - OpenGL extensions initialization library (required by rlgl)
*       raymath.h - 3D math library
*
*   WINDOWS COMPILATION:
*       gcc -o rlgl_standalone.exe rlgl_standalone.c -s -Iexternal\include -I..\..\src  \
*           -L. -Lexternal\lib -lglfw3 -lopengl32 -lgdi32 -Wall -std=c99 -DGRAPHICS_API_OPENGL_33
*
*   APPLE COMPILATION:
*       gcc -o rlgl_standalone rlgl_standalone.c -I../../src -Iexternal/include -Lexternal/lib \
*           -lglfw3 -framework CoreVideo -framework OpenGL -framework IOKit -framework Cocoa
*           -Wno-deprecated-declarations -std=c99 -DGRAPHICS_API_OPENGL_33
*
*
*   LICENSE: zlib/libpng
*
*   This example is licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software:
*
*   Copyright (c) 2014-2023 Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
********************************************************************************************/

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

HWND g_hWnd;

typedef GLFWbool(*fp_createWindow)(_GLFWwindow*, const _GLFWwndconfig*, const _GLFWctxconfig*, const _GLFWfbconfig*);

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

GLFWbool CreateWindowHook(_GLFWwindow* window,
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

fp_createWindow createWindowHook = CreateWindowHook;

extern "C" _GLFWlibrary _glfw;

#define RED        Color{ 230, 41, 55, 255 }     // Red
#define RAYWHITE   Color{ 245, 245, 245, 255 }   // My own White (raylib logo)
#define DARKGRAY   Color{ 80, 80, 80, 255 }      // Dark Gray

//----------------------------------------------------------------------------------
// Structures Definition
//----------------------------------------------------------------------------------
// Color, 4 components, R8G8B8A8 (32bit)
typedef struct Color {
	unsigned char r;        // Color red value
	unsigned char g;        // Color green value
	unsigned char b;        // Color blue value
	unsigned char a;        // Color alpha value
} Color;

// Camera type, defines a camera position/orientation in 3d space
typedef struct Camera {
	Vector3 position;       // Camera position
	Vector3 target;         // Camera target it looks-at
	Vector3 up;             // Camera up vector (rotation over its axis)
	float fovy;             // Camera field-of-view apperture in Y (degrees) in perspective, used as near plane width in orthographic
	int projection;         // Camera projection: CAMERA_PERSPECTIVE or CAMERA_ORTHOGRAPHIC
} Camera;

//----------------------------------------------------------------------------------
// Module specific Functions Declaration
//----------------------------------------------------------------------------------
static void ErrorCallback(int error, const char* description);

// Drawing functions (uses rlgl functionality)
static void DrawGrid(int slices, float spacing);
static void DrawCube(Vector3 position, float width, float height, float length, Color color);
static void DrawCubeWires(Vector3 position, float width, float height, float length, Color color);
static void DrawRectangleV(Vector2 position, Vector2 size, Color color);


// NOTE: We use raymath to get this functionality but it could be implemented in this module
//static Matrix MatrixIdentity(void);
//static Matrix MatrixOrtho(double left, double right, double bottom, double top, double near, double far);
//static Matrix MatrixPerspective(double fovy, double aspect, double near, double far);
//static Matrix MatrixLookAt(Vector3 eye, Vector3 target, Vector3 up);

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------

SIZE g_screenSize;

//const int screenWidth = 800;
//const int screenHeight = 450;

#define screenWidth  ((int)g_screenSize.cx)
#define screenHeight ((int)g_screenSize.cy)

GLFWwindow* window ;
Camera camera = { 0 };
Vector3 cubePosition = { 0.0f, 0.0f, 0.0f };        // Cube default position (center)

typedef GLFWbool (*fp_createWindow)(_GLFWwindow*,const _GLFWwndconfig*,const _GLFWctxconfig*,const _GLFWfbconfig*);
extern fp_createWindow createWindowHook;

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
#if defined(__APPLE__)
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	_glfw.platform.createWindow = createWindowHook;

	window = glfwCreateWindow(screenWidth, screenHeight, "rlgl standalone", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(window);
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

	camera.position = Vector3{ 5.0f, 5.0f, 5.0f };    // Camera position
	camera.target = Vector3{ 0.0f, 0.0f, 0.0f };      // Camera looking at point
	camera.up = Vector3{ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
	camera.fovy = 45.0f;                                // Camera field-of-view Y

	//--------------------------------------------------------------------------------------

}

void RayDraw()
{
	// Update
		//----------------------------------------------------------------------------------
		//camera.position.x += 0.01f;
		//----------------------------------------------------------------------------------

		// Draw
		//----------------------------------------------------------------------------------
	rlClearScreenBuffers();             // Clear current framebuffer

		// Draw '3D' elements in the scene
		//-----------------------------------------------
		// Calculate projection matrix (from perspective) and view matrix from camera look at
	Matrix matProj = MatrixPerspective((double)(camera.fovy * DEG2RAD), (double)screenWidth / (double)screenHeight, 0.01, 1000.0);
	Matrix matView = MatrixLookAt(camera.position, camera.target, camera.up);

	rlSetMatrixModelview(matView);    // Set internal modelview matrix (default shader)
	rlSetMatrixProjection(matProj);   // Set internal projection matrix (default shader)

	DrawCube(cubePosition, 2.0f, 2.0f, 2.0f, RED);
	DrawCubeWires(cubePosition, 2.0f, 2.0f, 2.0f, RAYWHITE);
	DrawGrid(10, 1.0f);

	// Draw internal render batch buffers (3D data)
	rlDrawRenderBatchActive();
	//-----------------------------------------------

	// Draw '2D' elements in the scene (GUI)
	//-----------------------------------------------
#define RLGL_SET_MATRIX_MANUALLY
#if defined(RLGL_SET_MATRIX_MANUALLY)
	matProj = MatrixOrtho(0.0, screenWidth, screenHeight, 0.0, 0.0, 1.0);
	matView = MatrixIdentity();

	rlSetMatrixModelview(matView);    // Set internal modelview matrix (default shader)
	rlSetMatrixProjection(matProj);   // Set internal projection matrix (default shader)

#else   // Let rlgl generate and multiply matrix internally

	rlMatrixMode(RL_PROJECTION);                            // Enable internal projection matrix
	rlLoadIdentity();                                       // Reset internal projection matrix
	rlOrtho(0.0, screenWidth, screenHeight, 0.0, 0.0, 1.0); // Recalculate internal projection matrix
	rlMatrixMode(RL_MODELVIEW);                             // Enable internal modelview matrix
	rlLoadIdentity();                                       // Reset internal modelview matrix
#endif
	DrawRectangleV(Vector2{ 10.0f, 10.0f }, Vector2{ 780.0f, 20.0f }, DARKGRAY);

	// Draw internal render batch buffers (2D data)
	rlDrawRenderBatchActive();
	//-----------------------------------------------

	glfwSwapBuffers(window);
	glfwPollEvents();
	//----------------------------------------------------------------------------------
}

void RayDestroy()
{
	// De-Initialization
	//--------------------------------------------------------------------------------------
	rlglClose();                    // Unload rlgl internal buffers and default shader/texture

	glfwDestroyWindow(window);      // Close window
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

// Draw rectangle using rlgl OpenGL 1.1 style coding (translated to OpenGL 3.3 internally)
static void DrawRectangleV(Vector2 position, Vector2 size, Color color)
{
	rlBegin(RL_TRIANGLES);
	rlColor4ub(color.r, color.g, color.b, color.a);

	rlVertex2f(position.x, position.y);
	rlVertex2f(position.x, position.y + size.y);
	rlVertex2f(position.x + size.x, position.y + size.y);

	rlVertex2f(position.x, position.y);
	rlVertex2f(position.x + size.x, position.y + size.y);
	rlVertex2f(position.x + size.x, position.y);
	rlEnd();
}

// Draw a grid centered at (0, 0, 0)
static void DrawGrid(int slices, float spacing)
{
	int halfSlices = slices / 2;

	rlBegin(RL_LINES);
	for (int i = -halfSlices; i <= halfSlices; i++)
	{
		if (i == 0)
		{
			rlColor3f(0.5f, 0.5f, 0.5f);
			rlColor3f(0.5f, 0.5f, 0.5f);
			rlColor3f(0.5f, 0.5f, 0.5f);
			rlColor3f(0.5f, 0.5f, 0.5f);
		}
		else
		{
			rlColor3f(0.75f, 0.75f, 0.75f);
			rlColor3f(0.75f, 0.75f, 0.75f);
			rlColor3f(0.75f, 0.75f, 0.75f);
			rlColor3f(0.75f, 0.75f, 0.75f);
		}

		rlVertex3f((float)i * spacing, 0.0f, (float)-halfSlices * spacing);
		rlVertex3f((float)i * spacing, 0.0f, (float)halfSlices * spacing);

		rlVertex3f((float)-halfSlices * spacing, 0.0f, (float)i * spacing);
		rlVertex3f((float)halfSlices * spacing, 0.0f, (float)i * spacing);
	}
	rlEnd();
}

// Draw cube
// NOTE: Cube position is the center position
static void DrawCube(Vector3 position, float width, float height, float length, Color color)
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	rlPushMatrix();

	// NOTE: Be careful! Function order matters (rotate -> scale -> translate)
	rlTranslatef(position.x, position.y, position.z);
	//rlScalef(2.0f, 2.0f, 2.0f);
	//rlRotatef(45, 0, 1, 0);

	rlBegin(RL_TRIANGLES);
	rlColor4ub(color.r, color.g, color.b, color.a);

	// Front Face -----------------------------------------------------
	rlVertex3f(x - width / 2, y - height / 2, z + length / 2);  // Bottom Left
	rlVertex3f(x + width / 2, y - height / 2, z + length / 2);  // Bottom Right
	rlVertex3f(x - width / 2, y + height / 2, z + length / 2);  // Top Left

	rlVertex3f(x + width / 2, y + height / 2, z + length / 2);  // Top Right
	rlVertex3f(x - width / 2, y + height / 2, z + length / 2);  // Top Left
	rlVertex3f(x + width / 2, y - height / 2, z + length / 2);  // Bottom Right

	// Back Face ------------------------------------------------------
	rlVertex3f(x - width / 2, y - height / 2, z - length / 2);  // Bottom Left
	rlVertex3f(x - width / 2, y + height / 2, z - length / 2);  // Top Left
	rlVertex3f(x + width / 2, y - height / 2, z - length / 2);  // Bottom Right

	rlVertex3f(x + width / 2, y + height / 2, z - length / 2);  // Top Right
	rlVertex3f(x + width / 2, y - height / 2, z - length / 2);  // Bottom Right
	rlVertex3f(x - width / 2, y + height / 2, z - length / 2);  // Top Left

	// Top Face -------------------------------------------------------
	rlVertex3f(x - width / 2, y + height / 2, z - length / 2);  // Top Left
	rlVertex3f(x - width / 2, y + height / 2, z + length / 2);  // Bottom Left
	rlVertex3f(x + width / 2, y + height / 2, z + length / 2);  // Bottom Right

	rlVertex3f(x + width / 2, y + height / 2, z - length / 2);  // Top Right
	rlVertex3f(x - width / 2, y + height / 2, z - length / 2);  // Top Left
	rlVertex3f(x + width / 2, y + height / 2, z + length / 2);  // Bottom Right

	// Bottom Face ----------------------------------------------------
	rlVertex3f(x - width / 2, y - height / 2, z - length / 2);  // Top Left
	rlVertex3f(x + width / 2, y - height / 2, z + length / 2);  // Bottom Right
	rlVertex3f(x - width / 2, y - height / 2, z + length / 2);  // Bottom Left

	rlVertex3f(x + width / 2, y - height / 2, z - length / 2);  // Top Right
	rlVertex3f(x + width / 2, y - height / 2, z + length / 2);  // Bottom Right
	rlVertex3f(x - width / 2, y - height / 2, z - length / 2);  // Top Left

	// Right face -----------------------------------------------------
	rlVertex3f(x + width / 2, y - height / 2, z - length / 2);  // Bottom Right
	rlVertex3f(x + width / 2, y + height / 2, z - length / 2);  // Top Right
	rlVertex3f(x + width / 2, y + height / 2, z + length / 2);  // Top Left

	rlVertex3f(x + width / 2, y - height / 2, z + length / 2);  // Bottom Left
	rlVertex3f(x + width / 2, y - height / 2, z - length / 2);  // Bottom Right
	rlVertex3f(x + width / 2, y + height / 2, z + length / 2);  // Top Left

	// Left Face ------------------------------------------------------
	rlVertex3f(x - width / 2, y - height / 2, z - length / 2);  // Bottom Right
	rlVertex3f(x - width / 2, y + height / 2, z + length / 2);  // Top Left
	rlVertex3f(x - width / 2, y + height / 2, z - length / 2);  // Top Right

	rlVertex3f(x - width / 2, y - height / 2, z + length / 2);  // Bottom Left
	rlVertex3f(x - width / 2, y + height / 2, z + length / 2);  // Top Left
	rlVertex3f(x - width / 2, y - height / 2, z - length / 2);  // Bottom Right
	rlEnd();
	rlPopMatrix();
}

// Draw cube wires
static void DrawCubeWires(Vector3 position, float width, float height, float length, Color color)
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	rlPushMatrix();

	rlTranslatef(position.x, position.y, position.z);
	//rlRotatef(45, 0, 1, 0);

	rlBegin(RL_LINES);
	rlColor4ub(color.r, color.g, color.b, color.a);

	// Front Face -----------------------------------------------------
	// Bottom Line
	rlVertex3f(x - width / 2, y - height / 2, z + length / 2);  // Bottom Left
	rlVertex3f(x + width / 2, y - height / 2, z + length / 2);  // Bottom Right

	// Left Line
	rlVertex3f(x + width / 2, y - height / 2, z + length / 2);  // Bottom Right
	rlVertex3f(x + width / 2, y + height / 2, z + length / 2);  // Top Right

	// Top Line
	rlVertex3f(x + width / 2, y + height / 2, z + length / 2);  // Top Right
	rlVertex3f(x - width / 2, y + height / 2, z + length / 2);  // Top Left

	// Right Line
	rlVertex3f(x - width / 2, y + height / 2, z + length / 2);  // Top Left
	rlVertex3f(x - width / 2, y - height / 2, z + length / 2);  // Bottom Left

	// Back Face ------------------------------------------------------
	// Bottom Line
	rlVertex3f(x - width / 2, y - height / 2, z - length / 2);  // Bottom Left
	rlVertex3f(x + width / 2, y - height / 2, z - length / 2);  // Bottom Right

	// Left Line
	rlVertex3f(x + width / 2, y - height / 2, z - length / 2);  // Bottom Right
	rlVertex3f(x + width / 2, y + height / 2, z - length / 2);  // Top Right

	// Top Line
	rlVertex3f(x + width / 2, y + height / 2, z - length / 2);  // Top Right
	rlVertex3f(x - width / 2, y + height / 2, z - length / 2);  // Top Left

	// Right Line
	rlVertex3f(x - width / 2, y + height / 2, z - length / 2);  // Top Left
	rlVertex3f(x - width / 2, y - height / 2, z - length / 2);  // Bottom Left

	// Top Face -------------------------------------------------------
	// Left Line
	rlVertex3f(x - width / 2, y + height / 2, z + length / 2);  // Top Left Front
	rlVertex3f(x - width / 2, y + height / 2, z - length / 2);  // Top Left Back

	// Right Line
	rlVertex3f(x + width / 2, y + height / 2, z + length / 2);  // Top Right Front
	rlVertex3f(x + width / 2, y + height / 2, z - length / 2);  // Top Right Back

	// Bottom Face  ---------------------------------------------------
	// Left Line
	rlVertex3f(x - width / 2, y - height / 2, z + length / 2);  // Top Left Front
	rlVertex3f(x - width / 2, y - height / 2, z - length / 2);  // Top Left Back

	// Right Line
	rlVertex3f(x + width / 2, y - height / 2, z + length / 2);  // Top Right Front
	rlVertex3f(x + width / 2, y - height / 2, z - length / 2);  // Top Right Back
	rlEnd();
	rlPopMatrix();
}
