#include "proxy.h"
#include "raylib.h"
#include "rlgl.h"
#include "rcore.c"

#undef GetScreenWidth
#undef GetScreenHeight

typedef struct SIZE
{
	long cx;
	long cy;
} SIZE;

SIZE g_screenSize;

// Get current screen width
int GetScreenWidth(void)
{
	return (int)g_screenSize.cx;
}

// Get current screen height
int GetScreenHeight(void)
{
	return (int)g_screenSize.cy;
}


#pragma comment(lib, "winmm.lib")
