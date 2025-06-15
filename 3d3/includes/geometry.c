#include <math.h>
#include <stdlib.h>

#include "geometry.h"
#include "list.h"

Vector get_vector(Point A, Point B)
{
    Vector V = {B.x-A.x, B.y-A.y, B.z-A.z};
    return V;
}

Vector cross_product(Vector u, Vector v)
{
    Vector V;
    V.x = u.y*v.z-u.z*v.y;
    V.y = u.z*v.x-u.x*v.z;
    V.z = u.x*v.y-u.y*v.x;
    return V;
}

float dot_product(Vector u, Vector v)
{
    return u.x*v.x+u.y*v.y+u.z*v.z;
}

float v_abs(Vector A)
{
    return sqrt(pow(A.x, 2) + pow(A.y, 2) + pow(A.z, 2));
}

float p_distance(Point A, Point B)
{
    return sqrt(pow(B.x - A.x, 2) + pow(B.y - A.y, 2) + pow(B.z - A.z, 2));
}

int is_in_triangle(Point P, Triangle T)
{
    Vector AB = get_vector(T.a, T.b);
    Vector BC = get_vector(T.b, T.c);
    Vector CA = get_vector(T.c, T.a);
    Vector AP = get_vector(T.a, P);
    Vector BP = get_vector(T.b, P);
    Vector CP = get_vector(T.c, P);
    
    Vector cpA = cross_product(AB, AP);
    Vector cpB = cross_product(BC, BP);
    Vector cpC = cross_product(CA, CP);

    float dpA = dot_product(cpA, cpB);
    float dpB = dot_product(cpB, cpC);
    float dpC = dot_product(cpC, cpA);

    if (dpA < 0 || dpB < 0 || dpC < 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

Point get_vector_triangle_intersect_point(Point S, Vector V, Triangle T, int *intersect, float *k)
{
    Vector AB = get_vector(T.a, T.b);
    Vector AC = get_vector(T.a, T.c);
    Vector SA = get_vector(S, T.a);
    Point P;
    Vector R = cross_product(AB, AC);
    float a = dot_product(V, R);
    float b;
    /*
    S+kV = P
    AP . R = 0
    (s+kV-A) * R = 0
    (S.x+kV.x-A.x)*R.x + (S.y+kV.y-A.y)*R.y + (S.z+kV.z-A.z)*R.z = 0
    k(V.x*R.x + V.y*R.y + V.z*R.z) + R.x*(S.x-A.x) + R.y*(S.y-A.y) + R.z*(S.z-A.z) = 0
    */
    if (a == 0)
    {
        *intersect = 0;
        return P;
    }
    else
    {
        b = dot_product(R, SA);
        *k = b/a;
        P.x = S.x + V.x*(*k);
        P.y = S.y + V.y*(*k);
        P.z = S.z + V.z*(*k);
        if (is_in_triangle(P, T)) *intersect = 1;
        else *intersect = 0;
        return P;
    }
}

int is_point_in_cuboid(Point A, Point min_p, Point max_p)
{
    if (A.x >= min_p.x && A.x <= max_p.x && A.y >= min_p.y && A.y <= max_p.y && A.z >= min_p.z && A.z <= max_p.z) return 1;
    else return 0;
}

int is_triangle_cuboid_intersect(Triangle T, Point min_p, Point max_p)
{
    if (is_point_in_cuboid(T.a, min_p, max_p) || is_point_in_cuboid(T.b, min_p, max_p) || is_point_in_cuboid(T.c, min_p, max_p)) return 1;
    Vector AB = get_vector(T.a, T.b);
    Vector AC = get_vector(T.a, T.c);
    Vector SE;
    Point start, end, P;
    int i, intersect;
    float k;
    Point top_point[4];
    Point bottom_point[4];
    top_point[0].x = min_p.x; top_point[0].y = min_p.y; top_point[0].z = min_p.z;
    top_point[1].x = max_p.x; top_point[1].y = min_p.y; top_point[1].z = min_p.z;
    top_point[2].x = max_p.x; top_point[2].y = min_p.y; top_point[2].z = max_p.z;
    top_point[3].x = min_p.x; top_point[3].y = min_p.y; top_point[3].z = max_p.z;
    bottom_point[0].x = min_p.x; bottom_point[0].y = max_p.y; bottom_point[0].z = min_p.z;
    bottom_point[1].x = max_p.x; bottom_point[1].y = max_p.y; bottom_point[1].z = min_p.z;
    bottom_point[2].x = max_p.x; bottom_point[2].y = max_p.y; bottom_point[2].z = max_p.z;
    bottom_point[3].x = min_p.x; bottom_point[3].y = max_p.y; bottom_point[3].z = max_p.z;

    for (i=0; i<4; i++)
    {
        start = top_point[i];
        end = top_point[(i+1)%4];
        SE = get_vector(start, end);
        P = get_vector_triangle_intersect_point(start, SE, T, &intersect, &k);
        if (intersect && 0 <= k && k <= 1) return 1;
    }
    for (i=0; i<4; i++)
    {
        start = bottom_point[i];
        end = bottom_point[(i+1)%4];
        SE = get_vector(start, end);
        P = get_vector_triangle_intersect_point(start, SE, T, &intersect, &k);
        if (intersect && 0 <= k && k <= 1) return 1;
    }
    for (i=0; i<4; i++)
    {
        start = top_point[i];
        end = bottom_point[i];
        SE = get_vector(start, end);
        P = get_vector_triangle_intersect_point(start, SE, T, &intersect, &k);
        if (intersect && 0 <= k && k <= 1) return 1;
    }
    return 0;
}

Point get_min_p(Triangle A)
{
    Point min_p = A.a;
    min_p.x = min_p.x<A.b.x?min_p.x:A.b.x;
    min_p.y = min_p.y<A.b.y?min_p.y:A.b.y;
    min_p.z = min_p.z<A.b.z?min_p.z:A.b.z;

    min_p.x = min_p.x<A.c.x?min_p.x:A.c.x;
    min_p.y = min_p.y<A.c.y?min_p.y:A.c.y;
    min_p.z = min_p.z<A.c.z?min_p.z:A.c.z;

    return min_p;
}

Point get_max_p(Triangle A)
{
    Point max_p = A.a;
    max_p.x = max_p.x>A.b.x?max_p.x:A.b.x;
    max_p.y = max_p.y>A.b.y?max_p.y:A.b.y;
    max_p.z = max_p.z>A.b.z?max_p.z:A.b.z;

    max_p.x = max_p.x>A.c.x?max_p.x:A.c.x;
    max_p.y = max_p.y>A.c.y?max_p.y:A.c.y;
    max_p.z = max_p.z>A.c.z?max_p.z:A.c.z;

    return max_p;
}

void divide_BVHtree(BVHTREE *parent_node, int depth)
{
    int i;
    Point box_size;
    Triangle *t;
    List *data = parent_node->data;
    if (data->length <= MAX_DATA_SIZE || depth == DEPTH_LIMIT) return;

    BVHTREE *box1 = malloc(sizeof(BVHTREE));
    BVHTREE *box2 = malloc(sizeof(BVHTREE));

    parent_node->left = box1;
    parent_node->right = box2;
    box1->left = 0;
    box1->right = 0;
    box1->min_p = parent_node->min_p;
    box1->max_p = parent_node->max_p;
    box1->data = new_list();

    box2->left = 0;
    box2->right = 0;
    box2->min_p = parent_node->min_p;
    box2->max_p = parent_node->max_p;
    box2->data = new_list();


    box_size.x = parent_node->max_p.x - parent_node->min_p.x;
    box_size.y = parent_node->max_p.y - parent_node->min_p.y;
    box_size.z = parent_node->max_p.z - parent_node->min_p.z;

    if (box_size.x > box_size.y)
    {
        if (box_size.x > box_size.z)
        {
            box1->max_p.x = (parent_node->min_p.x + parent_node->max_p.x)/2;
            box2->min_p.x = box1->max_p.x;
        }
        else
        {
            box1->max_p.z = (parent_node->min_p.z + parent_node->max_p.z)/2;
            box2->min_p.z = box1->max_p.z;
        }
    }
    else
    {
        if (box_size.y > box_size.z)
        {
            box1->max_p.y = (parent_node->min_p.y + parent_node->max_p.y)/2;
            box2->min_p.y = box1->max_p.y;
        }
        else
        {
            box1->max_p.z = (parent_node->min_p.z + parent_node->max_p.z)/2;
            box2->min_p.z = box1->max_p.z;
        }
    }

    for (i=0; i<data->length; i++)
    {
        t = list_index(data, i);
        if (is_triangle_cuboid_intersect(*t, box1->min_p, box1->max_p))
        {
            list_append(box1->data, t);
            divide_BVHtree(box1, depth+1);
        }
        if (is_triangle_cuboid_intersect(*t, box2->min_p, box2->max_p))
        {
            list_append(box2->data, t);
            divide_BVHtree(box2, depth+1);
        }
    }
    list_delete(parent_node->data);
}

BVHTREE *make_BVHtree(int triangle_num, Triangle model_data[])
{
    if (triangle_num == 0)
    {
        return 0;
    }

    BVHTREE *tree = malloc(sizeof(BVHTREE));
    int i, j;
    Triangle t;
    Point min_p;
    Point max_p;
    Point p;

    tree->data = 0;
    tree->left = 0;
    tree->right = 0;
    min_p = get_min_p(model_data[0]);
    max_p = min_p;

    for (i=1; i<triangle_num; i++)
    {
        t = model_data[i];
        p = get_min_p(t);
        min_p.x = min_p.x<p.x?min_p.x:p.x;
        min_p.y = min_p.y<p.y?min_p.y:p.y;
        min_p.z = min_p.z<p.z?min_p.z:p.z;
        max_p.x = max_p.x>p.x?max_p.x:p.x;
        max_p.y = max_p.y>p.y?max_p.y:p.y;
        max_p.z = max_p.z>p.z?max_p.z:p.z;
    }
    tree->min_p = min_p;
    tree->max_p = max_p;
    divide_BVHtree(tree, 0);
    return tree;
}