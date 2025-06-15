#pragma once

typedef struct _Color{
    float r;
    float g;
    float b;
} Color;

typedef struct _PixelColor{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} PixelColor;

typedef struct _Camera{
    float x, y, z;
    float ex, ey;
} Camera;

typedef struct _SurfacePoint{
    float depth;
    int triangle_index;
} SurfacePoint;

int indexing(int max_w, int x, int y);

Color add_lb(Color A, Color B);

Color mul_lb(Color A, Color B);

Color div_lb(Color A, Color B);