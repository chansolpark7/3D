#include <math.h>
#include <stdio.h>

#include "geometry.h"
#include "screen.h"
#include "shading.h"

void reset_screen(Color *screen, int screen_x, int screen_y, Color background_color)
{
    for (int x=0; x<screen_x; x++)
    {
        for (int y=0; y<screen_y; y++)
        {
            screen[indexing(screen_x, x, y)] = background_color;
        }
    }
}

Brightness realitic_light(Brightness brightness)
{
    float a = 6.0/29;
    if (brightness.r > pow(a, 3)) brightness.r = 1.16*pow(brightness.r, 1.0/3) - 0.16;
    else brightness.r = 1.16*(brightness.r/pow(a, 2)/3 + 4.0/29) - 0.16;
    if (brightness.g > pow(a, 3)) brightness.g = 1.16*pow(brightness.g, 1.0/3) - 0.16;
    else brightness.g = 1.16*(brightness.g/pow(a, 2)/3 + 4.0/29) - 0.16;
    if (brightness.b > pow(a, 3)) brightness.b = 1.16*pow(brightness.b, 1.0/3) - 0.16;
    else brightness.b = 1.16*(brightness.b/pow(a, 2)/3 + 4.0/29) - 0.16;
    return brightness;
}

Brightness get_brightness(Vector light, Vector R)
{
    float cosx = dot_product(light, R) / (v_abs(light) * v_abs(R));
    Brightness brightness;
    if (cosx < 0) brightness.r = 0, brightness.g = 0, brightness.b = 0;
    else brightness.r = cosx, brightness.g = cosx, brightness.b = cosx;
    return realitic_light(brightness);
}

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
)
{
    int i, j;
    Brightness brightness = {0.1, 0.1, 0.1}; // 환경 조명
    Brightness light_intensity;
    float light_distance, d, k;
    int intersect;
    Light original_light;
    Vector vlight;
    Point O = {0, 0, 0}, P;
    Vector V;
    Triangle T;
    Vector vAB, vAC;
    Vector R;
    Color color;
    Vector vcmr = {S.x-cmr.x, S.y-cmr.y, S.z-cmr.z};

    for (i=0; i<infinite_light_num; i++)
    {
        intersect = 0;
        original_light = infinite_light[i];
        vlight = get_vector(O, original_light.p);
        V = get_vector(original_light.p, O);
        for (j=0; j<model_triangle_num; j++)
        {
            if (target_triangle_index == j) continue;
            T = model_data[j];
            P = get_vector_triangle_intersect_point(S, V, T, &intersect, &k);
            if (intersect && k >= 0) break;
            else intersect = 0;
        }
        if (intersect == 0)
        {
            T = model_data[target_triangle_index];
            vAB = get_vector(T.a, T.b);
            vAC = get_vector(T.a, T.c);
            R = cross_product(vAB, vAC);
            if (dot_product(vcmr, R) < 0)
            {
                R.x = -R.x, R.y = -R.y, R.z = -R.z;
            }

            brightness = add_lb(mul_lb(get_brightness(vlight, R), original_light.brightness), brightness);
        }
    }

    for (i=0; i<point_light_num; i++)
    {
        intersect = 0;
        original_light = point_light[i];
        vlight = get_vector(original_light.p, S);
        V = get_vector(S, original_light.p);
        d = p_distance(S, original_light.p);
        for (j=0; j<model_triangle_num; j++)
        {
            if (target_triangle_index == j) continue;
            T = model_data[j];
            P = get_vector_triangle_intersect_point(S, V, T, &intersect, &k);
            if (intersect && k >= 0)
            {
                if (d > p_distance(S, P)) break;
                else intersect = 0;
            }
        }
        if (intersect == 0)
        {
            T = model_data[target_triangle_index];
            vAB = get_vector(T.a, T.b);
            vAC = get_vector(T.a, T.c);
            R = cross_product(vAB, vAC);
            if (dot_product(vcmr, R) < 0)
            {
                R.x = -R.x, R.y = -R.y, R.z = -R.z;
            }
            light_distance = p_distance(original_light.p, S)/10;
            light_intensity.r = light_distance * light_distance, light_intensity.g = light_intensity.r, light_intensity.b = light_intensity.r;
            if (light_distance < 1) brightness = add_lb(mul_lb(get_brightness(vlight, R), original_light.brightness), brightness);
            else brightness =  add_lb(mul_lb(get_brightness(vlight, R), div_lb(original_light.brightness, light_intensity)), brightness);
        }
    }
    // color.r = target_triangle_index*20;
    if (brightness.r > 1) color.r = 255;
    else color.r = brightness.r*255;
    if (brightness.g > 1) color.g = 255;
    else color.g = brightness.g*255;
    if (brightness.b > 1) color.b = 255;
    else color.b = brightness.b*255;
    return color;
}

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
    Color backgroud_color)
{
    int i, j, index;
    Color color;

    reset_screen(screen, screen_x, screen_y, backgroud_color);
    for (i=0; i<screen_x; i++)
    {
        for (j=0; j<screen_y; j++)
        {
            index = indexing(screen_x, i, j);
            if (triangle_index[index].triangle_index != -1)
            {
                color = get_color(
                    screen,
                    screen_point[index],
                    cmr,
                    triangle_index[index].triangle_index,
                    triangle_num,
                    model_data,
                    infinite_light_num,
                    infinite_light,
                    light_num,
                    light);
                screen[index] = color;
            }
        }
    }
}