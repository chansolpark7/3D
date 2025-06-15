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
    int infinite_light_num,
    Light *infinite_light,
    int point_light_num,
    Light *point_light
);

void shading(
    Color *screen,
    SurfacePoint *triangle_index,
    Point *screen_point,
    int triangle_num,
    Triangle *model_data,
    Camera cmr,
    int infinite_light_num,
    Light *infinite_light,
    int light_num,
    Light *light,
    int screen_x,
    int screen_y,
    Color background_color);