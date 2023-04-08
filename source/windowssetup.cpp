#define VC_EXTRALEAN
#define _USE_MATH_DEFINES

#include <windows.h>
#include <commctrl.h>
#include <scrnsave.h>
#include <gdiplus.h>
#include <time.h>
#include <math.h>
#include <string>
#include "resource.h"

#pragma comment(lib, "scrnsavw.lib")
#pragma comment (lib, "comctl32.lib")

using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")

static HDC g_hDC = NULL;
static GdiplusStartupInput g_gdiplusStartupInput;
static ULONG_PTR g_gdiplusToken;

extern HINSTANCE hMainInstance;


TCHAR szAppName[APPNAMEBUFFERLEN];
TCHAR szIniFile[MAXFILELEN];

#define DRAW_TIMER 0x1

// Globals
extern "C" HWND g_hWnd;
extern "C" SIZE g_screenSize;
extern "C" RECT g_screenScissor;

RECT g_screenScissor;

void WinInit(HWND hWnd);
void WinDestroy(HWND hWnd);

void RayInit();
void RayDraw();
void RayDestroy();


BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	MONITORINFOEX monitorInfo;
	monitorInfo.cbSize = sizeof(MONITORINFOEX);

	if (GetMonitorInfo(hMonitor, &monitorInfo))
	{
		if (monitorInfo.dwFlags == MONITORINFOF_PRIMARY)
		{
			g_screenScissor = monitorInfo.rcMonitor;

			/*
			std::cout << "Primary monitor found!" << std::endl;
			std::cout << "Monitor rectangle: left=" 
				<< monitorInfo.rcMonitor.left
				<< ", top=" << monitorInfo.rcMonitor.top
				<< ", right=" << monitorInfo.rcMonitor.right
				<< ", bottom=" << monitorInfo.rcMonitor.bottom << std::endl;
			*/
		}
	}
	return TRUE;
}


LRESULT WINAPI ScreenSaverProcW(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_CREATE:
		{
			g_hWnd = hWnd;
			WinInit(hWnd);
			RayInit();
			RayDraw();
			break;
		}
	case WM_DESTROY:
		{
			static bool first_time = true;
			if(first_time)
			{
				first_time = false;
				RayDestroy();
				WinDestroy(hWnd);

				//Why do I need this?
				exit(0);
			}
			break;
		}
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			BitBlt(hdc, 0, 0, g_screenSize.cx, g_screenSize.cy, g_hDC, 0, 0, SRCCOPY);
			EndPaint(hWnd, &ps);
			return 0;
			break;
		}
	case WM_TIMER:
		{
			if (wParam == DRAW_TIMER)
			{
				RayDraw();
			}
			break;
		}
	}

	return DefScreenSaverProc(hWnd, message, wParam, lParam);
}

void LoadConfiguration() 
{
	LoadString(hMainInstance, idsAppName, szAppName, APPNAMEBUFFERLEN);
	LoadString(hMainInstance, idsIniFile, szIniFile, MAXFILELEN);
}

void WinInit(HWND hWnd)
{
	LoadConfiguration();

	GdiplusStartup(&g_gdiplusToken, &g_gdiplusStartupInput, NULL);

	GetClientRect(hWnd, &g_screenScissor);
	g_screenSize.cy = g_screenScissor.bottom - g_screenScissor.top;
	g_screenSize.cx = g_screenScissor.right - g_screenScissor.left;

	HDC hDc = GetDC(hWnd);
	g_hDC = CreateCompatibleDC(hDc);
	ReleaseDC(hWnd, hDc);

	SetTextColor(g_hDC, RGB(255, 255, 255));
	SetBkColor(g_hDC, RGB(255, 255, 255));

	SetTimer(hWnd, DRAW_TIMER, 1, NULL);

	EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, 0);
}

void WinDestroy(HWND hWnd)
{
	KillTimer(hWnd, DRAW_TIMER);
	ReleaseDC(hWnd, g_hDC);
	DeleteObject(g_hDC);
	GdiplusShutdown(g_gdiplusToken);
}

BOOL WINAPI ScreenSaverConfigureDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message) 
	{ 
	case WM_INITDIALOG:
		{
			LoadConfiguration();
			//CheckDlgButton(hDlg, IDC_24HOUR_CHECK, g_is24Hour);
			//CheckDlgButton(hDlg, IDC_ANIMATEHANDS_CHECK, g_AnimatedClockHands);
			return TRUE;
		}
	case WM_COMMAND:
		{
			switch(LOWORD(wParam)) 
			{ 
			case ID_OK:
				{
				}
			case ID_CANCEL:
				{
					EndDialog(hDlg, LOWORD(wParam) == ID_OK); 
					return TRUE; 
				}
			}
			break;
		}
	case WM_NOTIFY:
		{
			LPNMHDR pnmh = (LPNMHDR)lParam;
			if (pnmh->idFrom == IDC_SYSLINK_WEBSITE)
			{
				if ((pnmh->code == NM_CLICK) || (pnmh->code == NM_RETURN))
				{
					PNMLINK link = (PNMLINK)lParam;
					ShellExecute(NULL, TEXT("open"), link->item.szUrl, NULL, NULL, SW_SHOWNORMAL);
				}
			}
			break;
		} 
	case WM_CLOSE:
		{
			EndDialog(hDlg, FALSE); 
			return TRUE; 
		}
	}
	return FALSE;
}

BOOL WINAPI RegisterDialogClasses(HANDLE /*hInst*/)
{
	InitCommonControls();
	return TRUE;
}
