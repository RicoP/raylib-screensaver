#define _GLFW_WIN32 1
extern "C" {
//#include "platform.h"
//#include "win32_platform.h"
#include "internal.h"
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
