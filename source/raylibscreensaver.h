#pragma once

#include <intrin.h> // for __rdtsc

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

inline unsigned long long GetRandomULong()
{
	static unsigned long long ULL = 0;
	if (ULL == 0) ULL = __rdtsc();

	ULL ^= ULL >> 12;
	ULL ^= ULL << 25;
	ULL ^= ULL >> 27;
	return ULL;
}

inline long long GetRandomLongRange(long long min, long long max)
{
	if (min > max)
	{
		long long tmp = min;
		min = max;
		max = tmp;
	}

	unsigned long long L = GetRandomULong();
	unsigned long long diff = max - min;
	L %= diff;
	L *= min != max; // when min == max then set L to zero
	return min + L;
}

inline int GetRandomIntRange(int min, int max)
{
	return (int)(GetRandomLongRange(min, max));
}

inline bool GetRandomBool()
{
	return (GetRandomULong() % 2) == 0;
}

inline double GetRandomDoubleScaler()
{
	unsigned long long mantissa = (unsigned long long) GetRandomLongRange(0, 1ULL << 53 - 1); // random mantissa
	unsigned long long bitfield = mantissa ^ 0x3FF0000000000000ULL; //set upper part to 1.mmmmmmmmmmmmm
	double f = 0;
	memcpy(&f, &bitfield, sizeof(double)); // This is actually fast and will be optimized into a single instruction

	//f is in range [1..2[
	return f - 1.0; // change range from [1..2[ to [0..1[
}

inline double GetRandomDoubleRange(double min, double max)
{
	if (min > max)
	{
		double tmp = min;
		min = max;
		max = tmp;
	}

	return min + GetRandomDoubleScaler() * (max - min);
}

inline double GetRandomDoubleRangeRandomSign(double min, double max)
{
	double f = GetRandomDoubleRange(min, max);
	return GetRandomBool() ? f : -f;
}
