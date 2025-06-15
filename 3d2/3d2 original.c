#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SCREEN_X 600
#define SCREEN_Y 600
#define swap(a, b, tmp) {tmp=a; a=b; b=tmp;}
#define PI 3.14159265358979323846

int fov = 60;
float distance_to_screen;

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

int min_n(int a, int b)
{
    if (a < b) return a;
    else return b;
}

int max_n(int a, int b)
{
    if (a > b) return a;
    else return b;
}

typedef struct _Point{
    float x;
    float y;
    float z;
} Point;

typedef struct _Triangle{
    Point a;
    Point b;
    Point c;
} Triangle;

typedef struct _Color{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} Color;

typedef struct _Camera{
    float x, y, z;
    int ex, ey;
} Camera;

int indexing(int max_w, int x, int y)
{
    return x + max_w*y;
}

Point translate_point(Point A, Camera cmr)
{
    A.x -= cmr.x;
    A.y -= cmr.y;
    A.z -= cmr.z;
    return A;
}

Point rotate_point(Point A, Camera cmr)
{
    float x, y, z;
    float cx = dcos(cmr.ex);
    float sx = dsin(cmr.ex);
    float cy = dcos(cmr.ey);
    float sy = dsin(cmr.ey);

    x = cx*A.x-sx*A.z;
    y = A.y;
    z = cx*A.z+sx*A.x;
    A.x = x;
    A.y = cy*y+sy*z;
    A.z = cy*z-sy*y;
    return A;
}

double ccw(Point A, Point B, Point C)
{
    // 모든 점을 회전시키고 나서 x, y, z 중에서 어떤 2개를 사용해야 하는지 생각하기
    // 회전을 어떤 축이 기준이 되게 만들지 생각하기
    return (B.x-A.x) * (C.y-A.y) - (C.x-A.x) * (B.y-A.y);
}

int is_inside_triangle(Point A, Triangle B)
{
    double a = ccw(B.a, A, B.b);
    double b = ccw(B.b, A, B.c);
    double c = ccw(B.c, A, B.a);
    if ((a > 0 && b > 0 && c > 0) || (a < 0 && b < 0 && c < 0)) return 1;
    else return 0;
}

Point get_3Dpos(Point A, Camera cmr)
{
    A = translate_point(A, cmr);
    return rotate_point(A, cmr);
}

Point get_2Dpos(Point A)
{
    Point B;
    B.x = (int)(A.x*distance_to_screen/A.z+SCREEN_X/2);
    B.y = (int)(-A.y*distance_to_screen/A.z+SCREEN_Y/2);
    return B;
}

Triangle *read_model(int *triangle_num, FILE *model_file)
{
    fseek(model_file, 80, SEEK_SET);
    fread(triangle_num, 4, 1, model_file);
    Triangle *model_data = malloc(sizeof(Triangle)*(*triangle_num));
    float x, y, z;
    for (int i=0; i<*triangle_num; i++)
    {
        fseek(model_file, 12, SEEK_CUR);
        for (int j=0; j<3; j++)
        {
            fread(&x, 4, 1, model_file);
            fread(&y, 4, 1, model_file);
            fread(&z, 4, 1, model_file);
            (*(&(model_data[i].a)+j)).x = x;
            (*(&(model_data[i].a)+j)).y = z;
            (*(&(model_data[i].a)+j)).z = y;

        }
        fseek(model_file, 2, SEEK_CUR);
    }
    return model_data;
}

void reset_screen(Color *screen)
{
    for (int x=0; x<SCREEN_X; x++)
    {
        for (int y=0; y<SCREEN_Y; y++)
        {
            screen[indexing(SCREEN_X, x, y)].r = 200;
            screen[indexing(SCREEN_X, x, y)].g = 200;
            screen[indexing(SCREEN_X, x, y)].b = 200;
        }
    }
}

void reset_depth_table(double *depth_table)
{
    for (int x=0; x<SCREEN_X; x++)
    {
        for (int y=0; y<SCREEN_Y; y++)
        {
            depth_table[indexing(SCREEN_X, x, y)] = 1e9;
        }
    }
}

double get_depth(Point A, Point B, Point C, Point A3, Point B3, Point C3, int x, int y)
{
    Point vA_B_, vA_C_, vAB, vAC;
    double a, b, z;
    vA_B_.x = B.x-A.x;
    vA_B_.y = B.y-A.y;
    vA_C_.x = C.x-A.x;
    vA_C_.y = C.y-A.y;
    // vAB.x = B3.x-A3.x;
    // vAB.y = B3.y-A3.y;
    // vAC.x = C3.x-A3.x;
    // vAC.y = C3.y-A3.y;
    vAB.z = B3.z-A3.z;
    vAC.z = C3.z-A3.z;

    // a*vA_B_.x+b*vA_C_.x = P.x-A.x;
    // a*vA_B_.y+b*vA_C_.y = P.y-A.y;

    // a*vA_B_.x*vA_B_.y+b*vA_C_.x*vA_B_.y = (P.x-A.x)*vA_B_.y;
    // a*vA_B_.x*vA_B_.y+b*vA_B_.x*vA_C_.y = (P.y-A.y)*vA_B_.x;
    // -> b = ((P.x-A.x)*vA_B_.y - (P.y-A.y)*vA_B_.x) / (vA_C_.x*vA_B_.y - vA_B_.x*vA_C_.y);

    // a*vA_B_.x*vA_C_.y + b*vA_C_.x*vA_C_.y = (P.x-A.x)*vA_C_.y;
    // a*vA_B_.y*vA_C_.x + b*vA_C_.x*vA_C_.y = (P.y-A.y)*vA_C_.x;
    // -> a = ((P.x-A.x)*vA_C_.y - (P.y-A.y)*vA_C_.x) / (vA_B_.x*vA_C_.y - vA_B_.y*vA_C_.x)

    double i = vA_B_.x*vA_C_.y - vA_B_.y*vA_C_.x;
    double j = x-A.x;
    double k = y-A.y;

    a = (j*vA_C_.y - k*vA_C_.x) / i;
    b = (k*vA_B_.x - j*vA_B_.y) / i;

    z = A3.z + a*vAB.z+b*vAC.z;

    return z;
}

Point cross_product(Point u, Point v)
{
    Point V;
    V.x = u.y*v.z-u.z*v.y;
    V.y = u.z*v.x-u.x*v.z;
    V.z = u.x*v.y-u.y*v.x;
    return V;
}

float v_abs(Point A)
{
    return sqrt(pow(A.x, 2) + pow(A.y, 2) + pow(A.z, 2));
}

float realitic_light(float brightness)
{
    float a = 6.0/29;
    if (brightness > pow(a, 3)) return 1.16*pow(brightness, 1.0/3) - 0.16;
    else return 1.16*(brightness/pow(a, 2)/3 + 4.0/29) - 0.16;
}

float get_brightness(Point light, Point A, Point B, Point C)
{
    Point vAB = {B.x-A.x, B.y-A.y, B.z-A.z};
    Point vAC = {C.x-A.x, C.y-A.y, C.z-A.z};
    Point cp = cross_product(vAB, vAC);
    float cosx = (light.x*cp.x + light.y*cp.y + light.z*cp.z) / (v_abs(light) * v_abs(cp));
    float brightness;
    // if (cosx*9/10+0.1 < 0.1) brightness = 0.1;
    // else brightness = cosx*9/10+0.1;
    if (cosx < 0) brightness = 0;
    else brightness = cosx;
    return realitic_light(brightness);
    // return brightness;
}

void draw_triangle(Color *screen, double *depth_table, Point A3, Point B3, Point C3, Color clr, Point light)
{
    int start_x, end_x, start_y, end_y;
    int x, y, y1, y2;
    double z;
    float brightness = get_brightness(light, A3, B3, C3);

    Point A = get_2Dpos(A3);
    Point B = get_2Dpos(B3);
    Point C = get_2Dpos(C3);
    Point temp;
    if (A.x < B.x)
    {
        if (A.x < C.x)
        {
            if (B.x > C.x)
            {
                swap(B, C, temp);
                swap(B3, C3, temp);
            }
        }
        else
        {
            swap(B, C, temp);
            swap(A, B, temp);
            swap(B3, C3, temp);
            swap(A3, B3, temp);
        }
    }
    else
    {
        if (B.x < C.x)
        {
            if (A.x < C.x)
            {
                swap(A, B, temp);
                swap(A3, B3, temp);
            }
            else
            {
                swap(A, B, temp);
                swap(B, C, temp);
                swap(A3, B3, temp);
                swap(B3, C3, temp);
            }
        }
        else
        {
            swap(A, C, temp);
            swap(A3, C3, temp);
        }
    }

    clr.r = (char)(brightness*clr.r);
    clr.g = (char)(brightness*clr.g);
    clr.b = (char)(brightness*clr.b);

    // 1
    start_x = min_n(max_n(min_n(A.x, B.x), 0), SCREEN_X);
    end_x = min_n(max_n(max_n(A.x, B.x), 0), SCREEN_X-1);
    if (start_x == end_x)
    {
        x = start_x;
        start_y = min_n(max_n(min_n(A.y, B.y), 0), SCREEN_Y);
        end_y = min_n(max_n(max_n(A.y, B.y), 0), SCREEN_Y-1);
        for (y=start_y; y<=end_y; y++)
        {
            z = get_depth(A, B, C, A3, B3, C3, x, y);
            if (depth_table[indexing(SCREEN_X, x, y)] > z)
            {
                depth_table[indexing(SCREEN_X, x, y)] = z;
                screen[indexing(SCREEN_X, x, y)] = clr;
            }
        }
    }
    else
    {
        for (x=start_x; x<=end_x; x++)
        {
            y1 = (B.y-A.y)*(x-A.x)/(B.x-A.x)+A.y;
            y2 = (C.y-A.y)*(x-A.x)/(C.x-A.x)+A.y;
            start_y = min_n(max_n(min_n(y1, y2), 0), SCREEN_Y);
            end_y = min_n(max_n(max_n(y1, y2), 0), SCREEN_Y-1);
            for (y=start_y; y<=end_y; y++)
            {
                z = get_depth(A, B, C, A3, B3, C3, x, y);
                if (depth_table[indexing(SCREEN_X, x, y)] > z)
                {
                    depth_table[indexing(SCREEN_X, x, y)] = z;
                    screen[indexing(SCREEN_X, x, y)] = clr;
                }
            }
        }
    }

    // 2
    start_x = min_n(max_n(min_n(B.x, C.x), 0), SCREEN_X);
    end_x = min_n(max_n(max_n(B.x, C.x), 0), SCREEN_X-1);
    if (start_x == end_x)
    {
        x = start_x;
        start_y = min_n(max_n(min_n(B.y, C.y), 0), SCREEN_Y);
        end_y = min_n(max_n(max_n(B.y, C.y), 0), SCREEN_Y-1);
        for (y=start_y; y<=end_y; y++)
        {
            z = get_depth(A, B, C, A3, B3, C3, x, y);
            if (depth_table[indexing(SCREEN_X, x, y)] > z)
            {
                depth_table[indexing(SCREEN_X, x, y)] = z;
                screen[indexing(SCREEN_X, x, y)] = clr;
            }
        }
    }
    else
    {
        for (x=start_x; x<=end_x; x++)
        {
            y1 = (C.y-A.y)*(x-A.x)/(C.x-A.x)+A.y;
            y2 = (C.y-B.y)*(x-B.x)/(C.x-B.x)+B.y;
            start_y = min_n(max_n(min_n(y1, y2), 0), SCREEN_Y);
            end_y = min_n(max_n(max_n(y1, y2), 0), SCREEN_Y-1);
            for (y=start_y; y<=end_y; y++)
            {
                z = get_depth(A, B, C, A3, B3, C3, x, y);
                if (depth_table[indexing(SCREEN_X, x, y)] > z)
                {
                    depth_table[indexing(SCREEN_X, x, y)] = z;
                    screen[indexing(SCREEN_X, x, y)] = clr;
                }
            }
        }
    }
}

void draw_screen(Color *screen, double *depth_table, int triangle_num, Triangle model_data[], FILE *movement_file, int frame)
{
    reset_screen(screen);
    reset_depth_table(depth_table);

    Color c = {230, 230, 230};
    Camera cmr;
    Point light = {-1, -1, 0.5};
    fseek(movement_file, 2+20*frame, SEEK_SET);
    fread(&cmr, 4, 5, movement_file);
    light = rotate_point(light, cmr);
    for (int i=0; i<triangle_num; i++)
    {
        Point A = get_3Dpos(model_data[i].a, cmr);
        Point B = get_3Dpos(model_data[i].b, cmr);
        Point C = get_3Dpos(model_data[i].c, cmr);

        if (A.z <= 0 || B.z <= 0 || C.z <= 0) return;

        draw_triangle(screen, depth_table, A, B, C, c, light);
    }
}

void save_screen(Color *screen, FILE *screen_file)
{
    for (int j=0; j<SCREEN_Y; j++)
    {
        for (int i=0; i<SCREEN_X; i++)
        {
            fwrite(&screen[indexing(SCREEN_X, i, j)], 1, 3, screen_file);
        }
    }
}

int main()
{
    Color *screen = malloc(sizeof(Color) * SCREEN_X * SCREEN_Y);
    double *depth_table = malloc(sizeof(double) * SCREEN_X * SCREEN_Y);
    short x = SCREEN_X;
    short y = SCREEN_Y;
    distance_to_screen = dtan(90-fov/2)*SCREEN_X/2;
    short frame_num;

    FILE *model_file = fopen("sphere.stl", "rb");
    FILE *movement_file = fopen("movement.data", "rb");
    FILE *screen_file = fopen("screen.data", "wb");

    int triangle_num;
    Triangle *model_data = read_model(&triangle_num, model_file);

    fread(&frame_num, 2, 1, movement_file);
    fwrite(&frame_num, 2, 1, screen_file);
    fwrite(&x, 2, 1, screen_file);
    fwrite(&y, 2, 1, screen_file);
    printf("%d\n", frame_num);
    for (int i=0; i<frame_num; i++)
    {
        printf("%d\n", i);
        draw_screen(screen, depth_table, triangle_num, model_data, movement_file, i);
        save_screen(screen, screen_file);
    }
    free(model_data);
    free(screen);
    free(depth_table);
    fclose(model_file);
    fclose(movement_file);
    fclose(screen_file);
    printf("end\n");
}