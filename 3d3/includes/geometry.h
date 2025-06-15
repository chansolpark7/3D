#pragma once

#define DEPTH_LIMIT 300
#define MAX_DATA_SIZE 10

typedef struct _Point{
    float x;
    float y;
    float z;
} Point;

typedef struct _Vector{
    float x;
    float y;
    float z;
} Vector;

typedef struct _Triangle{
    Point a;
    Point b;
    Point c;
} Triangle;

typedef struct _BVHTREE{
    struct BVHTREE *left;
    struct BVHTREE *right;
    void *data;
    Point min_p;
    Point max_p;
} BVHTREE;

Vector get_vector(Point A, Point B);

Vector cross_product(Vector u, Vector v);

float dot_product(Vector u, Vector v);

float v_abs(Vector A);

float p_distance(Point A, Point B);

int is_in_triangle(Point P, Triangle T);

Point get_vector_triangle_intersect_point(Point S, Vector V, Triangle T, int *intersect, float *k);

int is_triangle_cuboid_intersect(Triangle T, Point min_p, Point max_p);

BVHTREE *make_BVHtree(int triangle_num, Triangle model_data[]);