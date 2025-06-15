#pragma once

#include "geometry.h"
#include "screen.h"

PixelColor get_color(
    PixelColor *screen,
    Point S,
    Camera cmr,
    int target_triangle_index,
    int model_triangle_num,
    Triangle *model_data,
    Color color,
    int directional_light_num,
    Light *directional_light,
    int point_light_num,
    Light *point_light,
    int spot_light_num,
    Light *spot_light,
    Vector *spot_light_vector,
    float *spot_light_degree
);

void shading(
    PixelColor *screen,
    SurfacePoint *triangle_index,
    Point *screen_point,
    int triangle_num,
    Triangle *model_data,
    BVHTREE *model_bvhtree,
    Camera cmr,
    Color environmental_light,
    int directional_light_num,
    Light *directional_light,
    int point_light_num,
    Light *point_light,
    int spot_light_num,
    Light *spot_light,
    Vector *spot_light_vector,
    float *spot_light_degree,
    int screen_x,
    int screen_y,
    PixelColor background_color);