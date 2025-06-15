#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "geometry.h"
#include "list.h"
#include "screen.h"
#include "shading.h"

#define PI 3.14159265358979323846
#define MIN_K -1e-4
#define min(x, y) ((x) < (y) ? (x) : (y))
#define max(x, y) ((x) > (y) ? (x) : (y))

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

float spot_light_intensity(float degree, float light_degree)
{
    float a = 30;
    if (degree < light_degree-a) return 1;
    else return 1 - pow((degree-light_degree+a)/a, 2);
}

Brightness get_brightness(Vector light, Vector R)
{
    float cosx = dot_product(light, R) / (v_abs(light) * v_abs(R));
    Brightness brightness;
    if (cosx < 0) brightness.r = 0, brightness.g = 0, brightness.b = 0;
    else brightness.r = cosx, brightness.g = cosx, brightness.b = cosx;
    return brightness;
}

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
)
{
    int i, j;
    Brightness light_intensity;
    float light_distance, k;
    float degree;
    float intensity;
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

    // directional light
    for (i=0; i<directional_light_num; i++)
    {
        intersect = 0;
        original_light = directional_light[i];
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

    // point light
    for (i=0; i<point_light_num; i++)
    {
        intersect = 0;
        original_light = point_light[i];
        vlight = get_vector(original_light.p, S);
        V = get_vector(S, original_light.p);
        for (j=0; j<model_triangle_num; j++)
        {
            if (target_triangle_index == j) continue;
            T = model_data[j];
            P = get_vector_triangle_intersect_point(S, V, T, &intersect, &k);
            if (intersect && 0 <= k && k <= 1) break;
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
            light_distance = p_distance(original_light.p, S);
            light_intensity.r = 1 / (light_distance * light_distance + 1), light_intensity.g = light_intensity.r, light_intensity.b = light_intensity.r;
            brightness =  add_lb(mul_lb(get_brightness(vlight, R), mul_lb(original_light.brightness, light_intensity)), brightness);
        }
    }

    // spot light
    for (i=0; i<spot_light_num; i++)
    {
        intersect = 0;
        original_light = spot_light[i];
        vlight = get_vector(original_light.p, S);
        V = get_vector(S, original_light.p);
        degree = acos(dot_product(vlight, spot_light_vector[i]) / (v_abs(vlight) * v_abs(spot_light_vector[i])))*180/PI;
        if (degree > spot_light_degree[i]) continue;
        intensity = spot_light_intensity(degree, spot_light_degree[i]);
        for (j=0; j<model_triangle_num; j++)
        {
            if (target_triangle_index == j) continue;
            T = model_data[j];
            P = get_vector_triangle_intersect_point(S, V, T, &intersect, &k);
            if (intersect && 0 <= k && k <= 1) break;
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
            light_distance = p_distance(original_light.p, S);
            light_intensity.r = intensity / (light_distance * light_distance + 1), light_intensity.g = light_intensity.r, light_intensity.b = light_intensity.r;
            brightness =  add_lb(mul_lb(get_brightness(vlight, R), mul_lb(original_light.brightness, light_intensity)), brightness);
        }
    }
    // brightness = realitic_light(brightness);
    color.r = min(brightness.r, 1.0f) * 255;
    color.g = min(brightness.g, 1.0f) * 255;
    color.b = min(brightness.b, 1.0f) * 255;
    return color;
    return color;
}

// using BVH
Color get_color2(
    Color *screen,
    Point S,
    Camera cmr,
    int target_triangle_index,
    int model_triangle_num,
    Triangle *model_data,
    BVHTREE *model_bvhtree,
    Brightness brightness,
    int directional_light_num,
    Light *directional_light,
    int point_light_num,
    Light *point_light,
    int spot_light_num,
    Light *spot_light,
    Vector *spot_light_vector,
    float *spot_light_degree
)
{
    int i, j;
    Brightness light_intensity;
    float light_distance, k;
    int intersect;
    Light original_light;
    Vector vlight;
    float degree;
    float intensity;
    Point O = {0, 0, 0}, P;
    Vector V;
    int T;
    Vector vAB, vAC;
    Vector R;
    Color color;
    Vector vcmr = {S.x-cmr.x, S.y-cmr.y, S.z-cmr.z};
    BVHTREE **stack = malloc(sizeof(BVHTREE *) * (DEPTH_LIMIT+1));
    int stack_index;
    List *data;
    BVHTREE *node, *left, *right;

    // directional light
    for (i=0; i<directional_light_num; i++)
    {
        original_light = directional_light[i];
        vlight = get_vector(O, original_light.p);
        V = get_vector(original_light.p, O);
        stack[0] = model_bvhtree;
        stack_index = 1;
        while (stack_index > 0)
        {
            intersect = 0;
            node = stack[stack_index-1];
            stack_index--;
            if (node->left == 0 && node->right == 0)
            {
                intersect = 0;
                data = node->data;
                for (j=0; j<data->length; j++)
                {
                    T = list_index(data, j);
                    if (T == -1)
                    {
                        printf("list index error %d\n", j);
                        return color; //////////////////////////////////////////////////////////
                    }
                    if (target_triangle_index == T) continue;
                    P = get_vector_triangle_intersect_point(S, V, model_data[T], &intersect, &k);
                    if (intersect && k >= 0) break;
                    else intersect = 0;
                }
                if (intersect) break;
            }
            else
            {
                left = node->left;
                right = node->right;
                P = get_vector_cuboid_intersect_point(S, V, left->min_p, left->max_p, &intersect, &k);
                if (intersect && k >= MIN_K)
                {
                    if (stack_index > DEPTH_LIMIT)
                    {
                        printf("stack_index error %d\n", stack_index);
                        return color;
                    }
                    stack[stack_index] = left;
                    stack_index++;
                }
                intersect = 0;
                P = get_vector_cuboid_intersect_point(S, V, right->min_p, right->max_p, &intersect, &k);
                if (intersect && k >= MIN_K)
                {
                    if (stack_index > DEPTH_LIMIT)
                    {
                        printf("stack_index error %d\n", stack_index);
                        return color;
                    }
                    stack[stack_index] = right;
                    stack_index++;
                }
                intersect = 0;
            }
        }
        if (intersect == 0)
        {
            T = target_triangle_index;
            vAB = get_vector(model_data[T].a, model_data[T].b);
            vAC = get_vector(model_data[T].a, model_data[T].c);
            R = cross_product(vAB, vAC);
            if (dot_product(vcmr, R) < 0)
            {
                R.x = -R.x, R.y = -R.y, R.z = -R.z;
            }

            brightness = add_lb(mul_lb(get_brightness(vlight, R), original_light.brightness), brightness);
        }
    }

    // point light
    for (i=0; i<point_light_num; i++)
    {
        intersect = 0;
        original_light = point_light[i];
        vlight = get_vector(original_light.p, S);
        V = get_vector(S, original_light.p);
        stack[0] = model_bvhtree;
        stack_index = 1;
        while (stack_index > 0)
        {
            node = stack[stack_index-1];
            stack_index--;
            if (node->left == 0 && node->right == 0)
            {
                intersect = 0;
                data = node->data;
                for (j=0; j<data->length; j++)
                {
                    T = list_index(data, j);
                    if (target_triangle_index == T) continue;
                    P = get_vector_triangle_intersect_point(S, V, model_data[T], &intersect, &k);
                    if (intersect && 0 <= k && k <= 1) break;
                    else intersect = 0;
                }
                if (intersect) break;
            }
            else
            {
                left = node->left;
                right = node->right;
                P = get_vector_cuboid_intersect_point(S, V, left->min_p, left->max_p, &intersect, &k);
                if (intersect && k >= MIN_K)
                {
                    if (stack_index > DEPTH_LIMIT)
                    {
                        printf("stack_index error %d\n", stack_index);
                        return color;
                    }
                    stack[stack_index] = left;
                    stack_index++;
                }
                P = get_vector_cuboid_intersect_point(S, V, right->min_p, right->max_p, &intersect, &k);
                if (intersect && k >= MIN_K)
                {
                    if (stack_index > DEPTH_LIMIT)
                    {
                        printf("stack_index error %d\n", stack_index);
                        return color;
                    }
                    stack[stack_index] = right;
                    stack_index++;
                }
            }
        }
        if (intersect == 0)
        {
            T = target_triangle_index;
            vAB = get_vector(model_data[T].a, model_data[T].b);
            vAC = get_vector(model_data[T].a, model_data[T].c);
            R = cross_product(vAB, vAC);
            if (dot_product(vcmr, R) < 0)
            {
                R.x = -R.x, R.y = -R.y, R.z = -R.z;
            }
            light_distance = p_distance(original_light.p, S);
            light_intensity.r = 1 / (light_distance * light_distance + 1), light_intensity.g = light_intensity.r, light_intensity.b = light_intensity.r;
            brightness =  add_lb(mul_lb(get_brightness(vlight, R), mul_lb(original_light.brightness, light_intensity)), brightness);
        }
    }

    // spot light
    for (i=0; i<spot_light_num; i++)
    {
        intersect = 0;
        original_light = spot_light[i];
        vlight = get_vector(original_light.p, S);
        V = get_vector(S, original_light.p);
        degree = acos(dot_product(vlight, spot_light_vector[i]) / (v_abs(vlight) * v_abs(spot_light_vector[i])))*180/PI;
        if (degree > spot_light_degree[i]) continue;
        intensity = spot_light_intensity(degree, spot_light_degree[i]);
        stack[0] = model_bvhtree;
        stack_index = 1;
        while (stack_index > 0)
        {
            node = stack[stack_index-1];
            stack_index--;
            if (node->left == 0 && node->right == 0)
            {
                intersect = 0;
                data = node->data;
                for (j=0; j<data->length; j++)
                {
                    T = list_index(data, j);
                    if (target_triangle_index == T) continue;
                    P = get_vector_triangle_intersect_point(S, V, model_data[T], &intersect, &k);
                    if (intersect && 0 <= k && k <= 1) break;
                    else intersect = 0;
                }
                if (intersect) break;
            }
            else
            {
                left = node->left;
                right = node->right;
                P = get_vector_cuboid_intersect_point(S, V, left->min_p, left->max_p, &intersect, &k);
                if (intersect && k >= MIN_K)
                {
                    if (stack_index > DEPTH_LIMIT)
                    {
                        printf("stack_index error %d\n", stack_index);
                        return color;
                    }
                    stack[stack_index] = left;
                    stack_index++;
                }
                P = get_vector_cuboid_intersect_point(S, V, right->min_p, right->max_p, &intersect, &k);
                if (intersect && k >= MIN_K)
                {
                    if (stack_index > DEPTH_LIMIT)
                    {
                        printf("stack_index error %d\n", stack_index);
                        return color;
                    }
                    stack[stack_index] = right;
                    stack_index++;
                }
            }
        }
        if (intersect == 0)
        {
            T = target_triangle_index;
            vAB = get_vector(model_data[T].a, model_data[T].b);
            vAC = get_vector(model_data[T].a, model_data[T].c);
            R = cross_product(vAB, vAC);
            if (dot_product(vcmr, R) < 0)
            {
                R.x = -R.x, R.y = -R.y, R.z = -R.z;
            }
            light_distance = p_distance(original_light.p, S);
            light_intensity.r = intensity / (light_distance * light_distance + 1), light_intensity.g = light_intensity.r, light_intensity.b = light_intensity.r;
            brightness =  add_lb(mul_lb(get_brightness(vlight, R), mul_lb(original_light.brightness, light_intensity)), brightness);
        }
    }

    free(stack);
    brightness = realitic_light(brightness);
    color.r = min(brightness.r, 1.0f) * 255;
    color.g = min(brightness.g, 1.0f) * 255;
    color.b = min(brightness.b, 1.0f) * 255;
    return color;
}

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
    Color backgroud_color)
{
    int i, j, index;
    clock_t t;
    Color color;
    // Color color2;

    reset_screen(screen, screen_x, screen_y, backgroud_color);
    t = clock();
    for (i=0; i<screen_x; i++)
    {
        for (j=0; j<screen_y; j++)
        {
            if (clock()-t > 200)
            {
                printf("%.3f%%\n", (float)(i*screen_y+j)/(screen_x*screen_y)*100);
                t = clock();
            }
            index = indexing(screen_x, i, j);
            if (triangle_index[index].triangle_index != -1)
            {
                // color = get_color(
                //     screen,
                //     screen_point[index],
                //     cmr,
                //     triangle_index[index].triangle_index,
                //     triangle_num,
                //     model_data,
                //     environmental_light,
                //     directional_light_num,
                //     directional_light,
                //     point_light_num,
                //     point_light,
                //     spot_light_num,
                //     spot_light,
                //     spot_light_vector,
                //     spot_light_degree);

                color = get_color2(
                    screen,
                    screen_point[index],
                    cmr,
                    triangle_index[index].triangle_index,
                    triangle_num,
                    model_data,
                    model_bvhtree,
                    environmental_light,
                    directional_light_num,
                    directional_light,
                    point_light_num,
                    point_light,
                    spot_light_num,
                    spot_light,
                    spot_light_vector,
                    spot_light_degree);
                // if (color.r != color2.r)
                // {
                //     printf("%d %d %d\n", i, j, color2.r);
                // }
                screen[index] = color;
            }
        }
    }
}