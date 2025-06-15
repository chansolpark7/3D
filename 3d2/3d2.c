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

float acos2(float x, float y)
{
    float r = sqrt(x*x+y*y);
    if (r == 0)
    {
        return 0;
    }
    else if (y > 0)
    {
        return acos(x/r)*180/PI;
    }
    else
    {
        return 360-acos(x/r)*180/PI;
    }
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
    float ex, ey;
} Camera;

typedef struct _Light{
    Point p;
    float brightness;
} Light;

typedef struct _SurfacePoint{
    Point p;
    int triangle_index;
} SurfacePoint;

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

float ccw(Point A, Point B, Point C)
{
    return (B.x-A.x) * (C.y-A.y) - (C.x-A.x) * (B.y-A.y);
}

int is_inside_triangle(Point P, Point A, Point B, Point C)
{
    float a = ccw(A, B, P);
    float b = ccw(B, C, P);
    float c = ccw(C, A, P);
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

Triangle *read_model(unsigned int *triangle_num, FILE *model_file)
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
            (&(model_data[i].a)+j)->x = x;
            (&(model_data[i].a)+j)->y = z;
            (&(model_data[i].a)+j)->z = y;

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

void reset_triangle_index(SurfacePoint *triangle_index)
{
    Point p = {0, 0, 1e9};
    for (int x=0; x<SCREEN_X; x++)
    {
        for (int y=0; y<SCREEN_Y; y++)
        {
            triangle_index[indexing(SCREEN_X, x, y)].p = p;
            triangle_index[indexing(SCREEN_X, x, y)].triangle_index = -1;
        }
    }
}

Point get_point_on_triangle(Point A, Point B, Point C, Point A3, Point B3, Point C3, float x, float y)
{
    Point vA_B_, vA_C_, vAB, vAC;
    float a, b;
    Point p;
    vA_B_.x = B.x-A.x;
    vA_B_.y = B.y-A.y;
    vA_C_.x = C.x-A.x;
    vA_C_.y = C.y-A.y;
    
    vAB.x = B3.x-A3.x;
    vAB.y = B3.y-A3.y;
    vAC.x = C3.x-A3.x;
    vAC.y = C3.y-A3.y;
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

    if (i == 0)
    {
        p.z = 1e10;
    }
    else
    {
        a = (j*vA_C_.y - k*vA_C_.x) / i;
        b = (k*vA_B_.x - j*vA_B_.y) / i;

        p.x = A3.x + a*vAB.x+b*vAC.x;
        p.y = A3.y + a*vAB.y+b*vAC.y;
        p.z = A3.z + a*vAB.z+b*vAC.z;
    }

    return p;
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

float get_brightness(Point light, Point A, Point B, Point C, Point cp)
{
    float cosx = (light.x*cp.x + light.y*cp.y + light.z*cp.z) / (v_abs(light) * v_abs(cp));
    float brightness;
    // if (cosx*9/10+0.1 < 0.1) brightness = 0.1;
    // else brightness = cosx*9/10+0.1;
    if (cosx < 0) brightness = 0;
    else brightness = cosx;
    return realitic_light(brightness);
    // return brightness;
    // return 0.9;
}

void draw_triangle(SurfacePoint *triangle_index, Point A3, Point B3, Point C3, int index)
{
    int start_x, end_x, start_y, end_y;
    int x, y, y1, y2;
    Point p;

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
            p = get_point_on_triangle(A, B, C, A3, B3, C3, x, y);
            if (triangle_index[indexing(SCREEN_X, x, y)].p.z > p.z)
            {
                triangle_index[indexing(SCREEN_X, x, y)].p = p;
                triangle_index[indexing(SCREEN_X, x, y)].triangle_index = index;
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
                p = get_point_on_triangle(A, B, C, A3, B3, C3, x, y);
                if (triangle_index[indexing(SCREEN_X, x, y)].p.z > p.z)
                {
                    triangle_index[indexing(SCREEN_X, x, y)].p = p;
                    triangle_index[indexing(SCREEN_X, x, y)].triangle_index = index;
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
            p = get_point_on_triangle(A, B, C, A3, B3, C3, x, y);
            if (triangle_index[indexing(SCREEN_X, x, y)].p.z > p.z)
            {
                triangle_index[indexing(SCREEN_X, x, y)].p = p;
                triangle_index[indexing(SCREEN_X, x, y)].triangle_index = index;
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
                p = get_point_on_triangle(A, B, C, A3, B3, C3, x, y);
                if (triangle_index[indexing(SCREEN_X, x, y)].p.z > p.z)
                {
                    triangle_index[indexing(SCREEN_X, x, y)].p = p;
                    triangle_index[indexing(SCREEN_X, x, y)].triangle_index = index;
                }
            }
        }
    }
}

void project_screen(SurfacePoint *triangle_index, int triangle_num, Triangle model_data[], Camera cmr)
{
    reset_triangle_index(triangle_index);

    int i, j;
    for (int i=0; i<triangle_num; i++)
    {
        Point A = get_3Dpos(model_data[i].a, cmr);
        Point B = get_3Dpos(model_data[i].b, cmr);
        Point C = get_3Dpos(model_data[i].c, cmr);

        if (A.z <= 0 || B.z <= 0 || C.z <= 0) return;

        draw_triangle(triangle_index, A, B, C, i);
    }
}

Color get_color(
    Color *screen,
    Point point_on_screen,
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
    float brightness = 0.1; // 환경 조명
    char is_obscured; // 1이면 빛에 가림 0이면 가려지지 않음
    Camera light_rotation_direction;
    Light original_light;
    Point rotated_light;
    Point rotated_point_on_screen;
    Triangle triangle;
    Point A, B, C;
    Point r_A, r_B, r_C;
    Point triangle_p;
    Color color;
    Point vAB, vAC;
    Point cp;

    for (i=0; i<infinite_light_num; i++)
    {
        is_obscured = 0;
        original_light = infinite_light[i];
        original_light.p = rotate_point(original_light.p, cmr);
        light_rotation_direction.ex = acos2(original_light.p.z, original_light.p.x);
        light_rotation_direction.ey = 0;
        rotated_light = rotate_point(original_light.p, light_rotation_direction);
        light_rotation_direction.ey = -acos2(rotated_light.z, rotated_light.y);
        rotated_light = rotate_point(original_light.p, light_rotation_direction);
        rotated_point_on_screen = rotate_point(point_on_screen, light_rotation_direction);

        for (j=0; j<model_triangle_num; j++)
        {
            if (target_triangle_index == j) continue;
            triangle = model_data[j];
            A = get_3Dpos(triangle.a, cmr);
            B = get_3Dpos(triangle.b, cmr);
            C = get_3Dpos(triangle.c, cmr);
            r_A = rotate_point(A, light_rotation_direction);
            r_B = rotate_point(B, light_rotation_direction);
            r_C = rotate_point(C, light_rotation_direction);
            if (is_inside_triangle(rotated_point_on_screen, r_A, r_B, r_C))
            {
                triangle_p = get_point_on_triangle(r_A, r_B, r_C, r_A, r_B, r_C, rotated_point_on_screen.x, rotated_point_on_screen.y);
                if (rotated_point_on_screen.z <= triangle_p.z)
                {
                    // printf("inside\n");
                    is_obscured = 1;
                }
            }
        }
        if (is_obscured == 0)
        {
            // printf("shadow1\n");
            // printf("added light %f\n", get_brightness(rotated_light, A, B, C)*original_light.brightness);
            A = rotate_point(model_data[target_triangle_index].a, cmr);
            B = rotate_point(model_data[target_triangle_index].b, cmr);
            C = rotate_point(model_data[target_triangle_index].c, cmr);
            vAB.x = B.x-A.x, vAB.y = B.y-A.y, vAB.z = B.z-A.z;
            vAC.x = C.x-A.x, vAC.y = C.y-A.y, vAC.z = C.z-A.z;
            cp = cross_product(vAB, vAC);
            if (cp.z > 0)
            {
                cp.x = -cp.x;
                cp.y = -cp.y;
                cp.z = -cp.z;
            }

            brightness += get_brightness(rotated_light, A, B, C, rotate_point(cp, light_rotation_direction))*original_light.brightness;
        }
    }
    // printf("%f\n", brightness);
    color.r = brightness*200;
    color.g = color.r;
    color.b = color.r;
    return color;
}

void shading(
    Color *screen,
    SurfacePoint *triangle_index,
    int triangle_num,
    Triangle *model_data,
    Camera cmr,
    int infinite_light_num,
    Light *infinite_light,
    int light_num,
    Light *light)
{
    reset_screen(screen);
    int block_size = 1;
    int i, j, k, l, m;
    int *index_array = malloc(sizeof(int)*block_size*block_size);
    Color *color_array = malloc(sizeof(Color)*block_size*block_size);
    Color background = {230, 230, 230};
    int length;
    if (SCREEN_X % block_size != 0 || SCREEN_Y % block_size != 0) return;
    for (i=0; i<SCREEN_X/block_size; i++)
    {
        for (j=0; j<SCREEN_Y/block_size; j++)
        {
            length = 0;
            for (k=0; k<block_size; k++)
            {
                for (l=0; l<block_size; l++)
                {
                    if (triangle_index[indexing(SCREEN_X, i*block_size+k, j*block_size+l)].triangle_index == -1)
                    {
                        screen[indexing(SCREEN_X, i*block_size+k, j*block_size+l)] = background;
                        continue;
                    }
                    for (m=0; m<length+1; m++)
                    {
                        if (m == length)
                        {
                            length++;
                            index_array[m] = triangle_index[indexing(SCREEN_X, i*block_size+k, j*block_size+l)].triangle_index;
                            color_array[m] = get_color(
                                screen,
                                triangle_index[indexing(SCREEN_X, i*block_size+k, j*block_size+l)].p,
                                cmr,
                                index_array[m],
                                triangle_num,
                                model_data,
                                infinite_light_num,
                                infinite_light,
                                light_num,
                                light);
                        }
                        if (triangle_index[indexing(SCREEN_X, i*block_size+k, j*block_size+l)].triangle_index == index_array[m])
                        {
                            screen[indexing(SCREEN_X, i*block_size+k, j*block_size+l)] = color_array[m];
                            break;
                        }
                    }
                }
            }
        }
    }
    free(index_array);
    free(color_array);
}

void save_screen(Color *screen, FILE *screen_file)
{
    // short t=1;
    // fwrite(&t, 2, 1, screen_file);
    // t = 600;
    // fwrite(&t, 2, 1, screen_file);
    // fwrite(&t, 2, 1, screen_file);
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
    printf("main\n");
    int i;

    SurfacePoint *triangle_index = malloc(sizeof(SurfacePoint) * SCREEN_X * SCREEN_Y);
    Color *screen = malloc(sizeof(Color) * SCREEN_X * SCREEN_Y);
    Camera cmr;
    short x = SCREEN_X;
    short y = SCREEN_Y;
    distance_to_screen = dtan(90-fov/2)*SCREEN_X/2;
    short frame_num;
    printf("variable init\n");

    printf("file open\n");
    FILE *model_file = fopen("C:/Users/chans/OneDrive/python/3D/model/cube.stl", "rb");
    FILE *movement_file = fopen("C:/Users/chans/OneDrive/python/3D/rendering data/movement.data", "rb");
    FILE *infinite_light_file = fopen("C:/Users/chans/OneDrive/python/3D/rendering data/infinite point light.data", "rt");
    FILE *light_file = fopen("C:/Users/chans/OneDrive/python/3D/rendering data/point light.data", "rt");
    FILE *screen_file = fopen("C:/Users/chans/OneDrive/python/3D/rendering data/screen.data", "wb");
    if (model_file == NULL || movement_file == NULL || infinite_light_file == NULL || light_file == NULL || screen_file == NULL)
    {
        printf("file open error\n");
        return 0;
    }

    unsigned int triangle_num;
    Triangle *model_data = read_model(&triangle_num, model_file);
    printf("triangle num : %d\n", triangle_num);
    printf("read model\n");

    int infinite_light_num;
    fscanf(infinite_light_file, "%d", &infinite_light_num);
    Light *infinite_light = malloc(sizeof(Light) * infinite_light_num);
    for (i=0; i<infinite_light_num; i++)
    {
        fscanf(infinite_light_file, "%f %f %f %f", &infinite_light[i].p.x, &infinite_light[i].p.y, &infinite_light[i].p.z, &infinite_light[i].brightness);
    }
    printf("read infinite light file\n");

    int light_num;
    fscanf(light_file, "%d", &light_num);
    Light *light = malloc(sizeof(Light) * light_num);
    for (i=0; i<light_num; i++)
    {
        fscanf(light_file, "%f %f %f %d", &light[i].p.x, &light[i].p.y, &light[i].p.z, &light[i].brightness);
    }
    printf("read light file\n");

    fread(&frame_num, 2, 1, movement_file);
    fwrite(&frame_num, 2, 1, screen_file);
    fwrite(&x, 2, 1, screen_file);
    fwrite(&y, 2, 1, screen_file);
    printf("%d\n", frame_num);
    for (i=0; i<frame_num; i++)
    {
        printf("%d\n", i);
        fseek(movement_file, 2+20*i, SEEK_SET);
        fread(&cmr, 4, 5, movement_file);
        project_screen(triangle_index, triangle_num, model_data, cmr);
        shading(screen, triangle_index, triangle_num, model_data, cmr, infinite_light_num, infinite_light, light_num, light);
        save_screen(screen, screen_file);
    }

    free(triangle_index);
    free(screen);
    free(model_data);

    fclose(model_file);
    fclose(movement_file);
    fclose(screen_file);
    fclose(infinite_light_file);
    fclose(light_file);

    printf("end\n");
}

// 삼각함수를 사용한 회전으로 그림자를 계산하는 3D 두 번째 버전