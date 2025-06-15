#pragma once

#include "geometry.h"
#include "screen.h"

void project_screen(
    SurfacePoint *triangle_index,
    Point *screen_point,
    int triangle_num,
    Triangle model_data[],
    Camera cmr,
    int screen_x,
    int screen_y,
    float distance_to_screen);