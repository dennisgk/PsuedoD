#ifndef QUICK_MATH_H
#define QUICK_MATH_H

//FOR LCD_WIDTH AND LCD_HEIGHT
#include <tice.h>

static float fLCD_WIDTH = (float)LCD_WIDTH;
static float fLCD_HEIGHT = (float)LCD_HEIGHT;

static float PI = 3.141592f;
static float doublePI = 6.28318531f;
static float negPI = -3.141592f;
static float halfPI = 1.57079633f;
static float negHalfPI = -1.57079633f;
static float oneFourthPI = 0.78539816f;

static float twoOverPI = 0.63661977f;
static float fourOverPI = 1.27323954474f;
static float negFourOverPISquared = -0.405284734569f;
static float PIp = 0.225f;

float absf_s(float value);
float negf_s(float f);

float cosf(float rad);
float sinf(float rad);
float tanf(float rad);
float cosf_s(float rad);
float sinf_s(float rad);
float tanf_s(float rad);
float cotanf_s(float rad);

float sinf_sc(float* rad);

float arcsinf(float rad);
float arcsinf_s(float rad);
float arccosf_s(float rad);
float arctanf_s(float rad);
float arccotf_s(float rad);

float sqrt_f(float x);
float isqrt_f( float number );

float flmod(float og, float mod);
float toTrigBounds(float rad);
float toTrigBoundsB(float rad, unsigned char* quadrant);

void swap(void** a, void** b);
int partition (void** arr, int low, int high, int (*compare)(void*, void*));
void quickSort(void** arr, int low, int high, int (*compare)(void*, void*));

#endif