#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "geometry.h"
#include "screen.h"
#include "project_screen.h"
#include "list.h"

#define PI 3.14159265358979323846

float dcos(float x)
{
    return cos(x*PI/180);
}

float dsin(float x)
{
    return sin(x*PI/180);
}

float dtan(float x)
{
    return tan(x*PI/180);
}

void reset_triangle_index(SurfacePoint *triangle_index, int screen_x, int screen_y)
{
    for (int x=0; x<screen_x; x++)
    {
        for (int y=0; y<screen_y; y++)
        {
            triangle_index[indexing(screen_x, x, y)].depth = 1e9;
            triangle_index[indexing(screen_x, x, y)].triangle_index = -1;
        }
    }
}

Point get_screen_point(Camera cmr, int x2, int y2, int screen_x, int screen_y, float distance_to_screen)
{
    float x, y, z;
    float cx = dcos(-cmr.ex);
    float sx = dsin(-cmr.ex);
    float cy = dcos(-cmr.ey);
    float sy = dsin(-cmr.ey);
    Point p = {x2-screen_x/2, -y2+screen_y/2, distance_to_screen};
    x = p.x;
    y = cy*p.y+sy*p.z;
    z = cy*p.z-sy*p.y;
    p.x = cx*x-sx*z;
    p.y = y;
    p.z = cx*z+sx*x;    

    return p;
}

void project_screen(
    SurfacePoint *triangle_index,
    Point *screen_point,
    Triangle model_data[],
    BVHTREE *model_bvhtree,
    Camera cmr,
    int screen_x,
    int screen_y,
    float distance_to_screen
)
{
    int x, y, i, index, intersect;
    float d, k;
    Point S = {cmr.x, cmr.y, cmr.z};
    Point O = {0, 0, 0};
    Point A, P;
    Vector V;
    int T;
    clock_t t;
    BVHTREE **stack = malloc(sizeof(BVHTREE *) * (DEPTH_LIMIT+1));
    int stack_index;
    List *data;
    BVHTREE *node, *left, *right;

    reset_triangle_index(triangle_index, screen_x, screen_y);
    t = clock();
    for (x=0; x<screen_x; x++)
    {
        for (y=0; y<screen_y; y++)
        {
            if (clock()-t > 200)
            {
                printf("%.3f%%\n", (float)(x*screen_y+y)/(screen_x*screen_y)*100);
                t = clock();
            }
            A = get_screen_point(cmr, x, y, screen_x, screen_y, distance_to_screen);
            V = get_vector(O, A);
            stack[0] = model_bvhtree;
            stack_index = 1;
            while (stack_index > 0)
            {
                node = stack[stack_index-1];
                stack_index--;
                if (node->left == 0 && node->right == 0)
                {
                    data = node->data;
                    for (i=0; i<data->length; i++)
                    {
                        T = list_index(data, i);
                        P = get_vector_triangle_intersect_point(S, V, model_data[T], &intersect, &k);
                        if (intersect && k >= 0)
                        {
                            d = p_distance(S, P);
                            index = indexing(screen_x, x, y);
                            if (triangle_index[index].depth > d)
                            {
                                triangle_index[index].depth = d;
                                triangle_index[index].triangle_index = T;
                                screen_point[index] = P;
                            }
                        }
                    }
                }
                else
                {
                    left = node->left;
                    right = node->right;
                    P = get_vector_cuboid_intersect_point(S, V, left->min_p, left->max_p, &intersect, &k);
                    if (intersect && k >= 0)
                    {
                        if (stack_index > DEPTH_LIMIT)
                        {
                            printf("stack_index error %d\n", stack_index);
                            return;
                        }
                        stack[stack_index] = left;
                        stack_index++;
                    }
                    P = get_vector_cuboid_intersect_point(S, V, right->min_p, right->max_p, &intersect, &k);
                    if (intersect && k >= 0)
                    {
                        if (stack_index > DEPTH_LIMIT)
                        {
                            printf("stack_index error %d\n", stack_index);
                            return;
                        }
                        stack[stack_index] = right;
                        stack_index++;
                    }
                }
            }
        }
    }
    free(stack);
}

void project_screen_ray(SurfacePoint *triangle_index, Point *screen_point, int triangle_num, Triangle model_data[], Camera cmr, int screen_x, int screen_y, float distance_to_screen)
{
    int x, y, i, index, intersect;
    float d, k;
    clock_t t;
    Point S = {cmr.x, cmr.y, cmr.z};
    Point O = {0, 0, 0};
    Point A, P;
    Vector V;
    Triangle T;

    reset_triangle_index(triangle_index, screen_x, screen_y);
    t = clock();
    for (x=0; x<screen_x; x += 1)
    {
        for (y=0; y<screen_y; y += 1)
        {
            if (clock()-t > 200)
            {
                printf("%.3f%%\n", (float)(x*screen_y+y)/(screen_x*screen_y)*100);
                t = clock();
            }
            A = get_screen_point(cmr, x, y, screen_x, screen_y, distance_to_screen);
            V = get_vector(O, A);
            for (i=0; i<triangle_num; i++)
            {
                T = model_data[i];
                P = get_vector_triangle_intersect_point(S, V, T, &intersect, &k);
                if (intersect && k >= 0) // 오류 발생 시 k>0으로 수정
                {
                    d = p_distance(S, P);
                    index = indexing(screen_x, x, y);
                    if (triangle_index[index].depth > d)
                    {
                        triangle_index[index].depth = d;
                        triangle_index[index].triangle_index = i;
                        screen_point[index] = P;
                    }
                }
            }
        }
    }
}