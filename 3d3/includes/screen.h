#pragma once

#include "geometry.h"

typedef struct _Brightness{
    float r;
    float g;
    float b;
} Brightness;

typedef struct _Color{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} Color;

typedef struct _Camera{
    float x, y, z;
    float ex, ey;
} Camera;

typedef struct _Light{
    Point p;
    Brightness brightness;
} Light;

typedef struct _SurfacePoint{
    float depth;
    int triangle_index;
} SurfacePoint;

int indexing(int max_w, int x, int y);

Brightness add_lb(Brightness A, Brightness B);

Brightness mul_lb(Brightness A, Brightness B);

Brightness div_lb(Brightness A, Brightness B);

Brightness realitic_light(Brightness brightness);

Brightness get_brightness(Vector light, Vector R);