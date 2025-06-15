#pragma once

#include "geometry.h"
#include "screen.h"

Color get_color(
    Color *screen,
    Point S,
    Camera cmr,
    int target_triangle_index,
    int model_triangle_num,
    Triangle *model_data,
    Brightness brightness,
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
    Color *screen,
    SurfacePoint *triangle_index,
    Point *screen_point,
    int triangle_num,
    Triangle *model_data,
    BVHTREE *model_bvhtree,
    Camera cmr,
    Brightness environmental_light,
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
    Color background_color);