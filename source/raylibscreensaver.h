#pragma once


#if defined(_WINDOWS_) || defined(_INC_WINDOWS) 
typedef struct Color {
	unsigned char r;        // Color red value
	unsigned char g;        // Color green value
	unsigned char b;        // Color blue value
	unsigned char a;        // Color alpha value
} Color;
#else
#include <raylib.h>
#endif

typedef struct {
	Color backgroundColor;
	bool stretch;
} ScreensaverInfo;


#ifdef __cplusplus
extern "C" {
#endif

ScreensaverInfo Configure();
void Init();
void Update();
void Destroy();

#ifdef __cplusplus
}
#endif

