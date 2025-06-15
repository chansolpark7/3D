#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "includes/geometry.h"
#include "includes/screen.h"
#include "includes/project_screen.h"
#include "includes/shading.h"

#define PI 3.14159265358979323846
#define STRING_LENGTH 200

int fov;
int screen_x;
int screen_y;
char model_file_path[STRING_LENGTH];
char movement_file_path[STRING_LENGTH];
char light_file_path[STRING_LENGTH];
char screen_file_path[STRING_LENGTH];
Color background_color;

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
            (&(model_data[i].a)+j)->x = x;
            (&(model_data[i].a)+j)->y = z;
            (&(model_data[i].a)+j)->z = y;

        }
        fseek(model_file, 2, SEEK_CUR);
    }
    return model_data;
}

void save_triangle_screen(SurfacePoint *triangle_index, FILE *screen_file)
{
    int index;
    unsigned char color;
    for (int j=0; j<screen_y; j++)
    {
        for (int i=0; i<screen_x; i++)
        {
            index = indexing(screen_x, i, j);
            color = triangle_index[index].triangle_index*30;
            // color = (unsigned char)(triangle_index[index].depth/5);
            fwrite(&color, 1, 1, screen_file);
            fwrite(&color, 1, 1, screen_file);
            fwrite(&color, 1, 1, screen_file);
        }
    }
}

void save_screen(Color *screen, FILE *screen_file)
{
    int index;
    for (int j=0; j<screen_y; j++)
    {
        for (int i=0; i<screen_x; i++)
        {
            index = indexing(screen_x, i, j);
            fwrite(&screen[index].r, 1, 1, screen_file);
            fwrite(&screen[index].g, 1, 1, screen_file);
            fwrite(&screen[index].b, 1, 1, screen_file);
        }
    }
}

int parsing(char string[], char *command[])
{
    int index = 0;
    int num = 0;
    while (string[index] != 0)
    {
        if (string[index] == ' ')
        {
            string[index] = 0;
            index++;
        }
        else if (string[index] == '`')
        {
            string[index] = 0;
            index++;
            command[num] = string+index;
            num++;
            while (string[index] !=0 && string[index] != '`') index++;
            string[index] = 0;
            index++;
        }
        else
        {
            command[num] = string+index;
            num++;
            while (string[index] != 0 && string[index] != ' ') index++;
        }
    }

    return num;
}

int main()
{
    printf("start\n");
    int i, length;
    clock_t t;

    FILE *setting_file = fopen("C:/Users/chans/OneDrive/python/3D/rendering data/setting.txt", "rt");
    char string[STRING_LENGTH];
    char *command[STRING_LENGTH];
    int num;

    while (1)
    {
        if (fgets(string, STRING_LENGTH, setting_file) == NULL) break;
        length = strlen(string);
        if (string[length-1] == '\n') string[length-1] = 0;
        num = parsing(string, command);

        if (strcmp(command[0], "fov") == 0)
        {
            if (num != 2)
            {
                printf("setting value error1\n");
                return 1;
            }
            fov = atoi(command[1]);
        }
        else if (strcmp(command[0], "screen_size") == 0)
        {
            if (num != 3)
            {
                printf("setting value error2\n");
                return 1;
            }
            screen_x = atoi(command[1]);
            screen_y = atoi(command[2]);
        }
        else if (strcmp(command[0], "model_file") == 0)
        {
            if (num != 2)
            {
                printf("setting value error3\n");
                return 1;
            }
            strcpy(model_file_path, command[1]);
        }
        else if (strcmp(command[0], "movement_file") == 0)
        {
            if (num != 2)
            {
                printf("setting value error4\n");
                return 1;
            }
            strcpy(movement_file_path, command[1]);
        }
        else if (strcmp(command[0], "light_file") == 0)
        {
            if (num != 2)
            {
                printf("setting value error5\n");
                return 1;
            }
            strcpy(light_file_path, command[1]);
        }
        else if (strcmp(command[0], "screen_file") == 0)
        {
            if (num != 2)
            {
                printf("setting value error6\n");
                return 1;
            }
            strcpy(screen_file_path, command[1]);
        }
        else if (strcmp(command[0], "background_color") == 0)
        {
            if (num != 4)
            {
                printf("setting value error7\n");
                return 1;
            }
            background_color.r = atoi(command[1]);
            background_color.g = atoi(command[2]);
            background_color.b = atoi(command[3]);
        }
    }

    if (fov == 0 || screen_x == 0 || screen_y == 0 || model_file_path[0] == 0 ||
        movement_file_path[0] == 0 || light_file_path[0] == 0 || screen_file_path[0] == 0)
    {
        printf("No value found in setting\n");
        return 1;
    }

    SurfacePoint *triangle_index = malloc(sizeof(SurfacePoint) * screen_x * screen_y);
    Point *screen_point = malloc(sizeof(Point) * screen_x * screen_y);
    Color *screen = malloc(sizeof(Color) * screen_x * screen_y);
    Camera cmr;
    short x = screen_x;
    short y = screen_y;
    float distance_to_screen = tan((90-fov/2)*PI/180)*screen_x/2;
    short frame_num;

    FILE *model_file = fopen(model_file_path, "rb");
    FILE *movement_file = fopen(movement_file_path, "rb");
    FILE *light_file = fopen(light_file_path, "rt");
    FILE *screen_file = fopen(screen_file_path, "wb");
    if (model_file == NULL || movement_file == NULL || light_file == NULL || screen_file == NULL)
    {
        printf("file open error\n");
        return 1;
    }

    int triangle_num;
    Triangle *model_data = read_model(&triangle_num, model_file);
    printf("triangle num : %d\n", triangle_num);

    printf("BVHtree\n");
    t = clock();
    BVHTREE *model_bvhtree = make_BVHtree(triangle_num, model_data);
    printf("%.2fs\n", (float)(clock()-t)/1000);
    
    if (model_bvhtree == NULL)
    {
        printf("model BVH tree error\n");
        return 1;
    }

    int directional_light_num = 0, point_light_num = 0, spot_light_num = 0;
    int d_index = 0, p_index = 0, s_index = 0;
    while (1)
    {
        if (fgets(string, STRING_LENGTH, light_file) == NULL) break;
        num = parsing(string, command);
        if (strcmp(command[0], "#") == 0) continue;
        else if (strcmp(command[0], "d") == 0) directional_light_num++;
        else if (strcmp(command[0], "p") == 0) point_light_num++;
        else if (strcmp(command[0], "s") == 0) spot_light_num++;
    }
    fseek(light_file, 0, SEEK_SET);
    Brightness environmental_light;
    Light *directional_light = malloc(sizeof(Light) * directional_light_num);
    Light *point_light = malloc(sizeof(Light) * point_light_num);
    Light *spot_light = malloc(sizeof(Light) * spot_light_num);
    Vector *spot_light_vector = malloc(sizeof(Vector) * spot_light_num);
    float *spot_light_degree = malloc(sizeof(float) * spot_light_num);
    while (1)
    {
        if (fgets(string, STRING_LENGTH, light_file) == NULL) break;
        num = parsing(string, command);
        if (strcmp(command[0], "#") == 0) continue;
        else if (strcmp(command[0], "e") == 0)
        {
            if (num != 4)
            {
                printf("light value error\n");
                return 1;
            }
            environmental_light.r = atof(command[1]);
            environmental_light.g = atof(command[2]);
            environmental_light.b = atof(command[3]);
        }
        else if (strcmp(command[0], "d") == 0)
        {
            if (num != 7)
            {
                printf("light value error\n");
                return 1;
            }
            directional_light[d_index].p.x = atof(command[1]);
            directional_light[d_index].p.y = atof(command[2]);
            directional_light[d_index].p.z = atof(command[3]);
            directional_light[d_index].brightness.r = atof(command[4]);
            directional_light[d_index].brightness.g = atof(command[5]);
            directional_light[d_index].brightness.b = atof(command[6]);
            d_index++;
        }
        else if (strcmp(command[0], "p") == 0)
        {
            if (num != 7)
            {
                printf("light value error\n");
                return 1;
            }
            point_light[p_index].p.x = atof(command[1]);
            point_light[p_index].p.y = atof(command[2]);
            point_light[p_index].p.z = atof(command[3]);
            point_light[p_index].brightness.r = atof(command[4]);
            point_light[p_index].brightness.g = atof(command[5]);
            point_light[p_index].brightness.b = atof(command[6]);
            p_index++;
        }
        else if (strcmp(command[0], "s") == 0)
        {
            if (num != 11)
            {
                printf("light value error\n");
                return 1;
            }
            spot_light[i].p.x = atof(command[1]);
            spot_light[i].p.y = atof(command[2]);
            spot_light[i].p.z = atof(command[3]);
            spot_light_vector[i].x = atof(command[4]);
            spot_light_vector[i].y = atof(command[5]);
            spot_light_vector[i].z = atof(command[6]);
            spot_light_degree[i] = atof(command[7]);
            spot_light[i].brightness.r = atof(command[8]);
            spot_light[i].brightness.g = atof(command[9]);
            spot_light[i].brightness.b = atof(command[10]);
            s_index++;
        }
    }

    fread(&frame_num, 2, 1, movement_file);
    fwrite(&frame_num, 2, 1, screen_file);
    fwrite(&x, 2, 1, screen_file);
    fwrite(&y, 2, 1, screen_file);
    printf("frame num %d\n", frame_num);
    for (i=0; i<frame_num; i++)
    {
        printf("%d / %d\n\n", i+1, frame_num);
        fseek(movement_file, 2+20*i, SEEK_SET);
        fread(&cmr, 4, 5, movement_file);

        printf("project screen\n");
        t = clock();
        project_screen(
            triangle_index,
            screen_point,
            model_data,
            model_bvhtree,
            cmr,
            screen_x,
            screen_y,
            distance_to_screen);
        printf("%.2fs\n\n", (double)(clock()-t)/1000);
        // save_triangle_screen(triangle_index, screen_file);

        printf("shading\n");
        t = clock();
        shading(
            screen,
            triangle_index,
            screen_point,
            triangle_num,
            model_data,
            model_bvhtree,
            cmr,
            environmental_light,
            directional_light_num,
            directional_light,
            point_light_num,
            point_light,
            spot_light_num,
            spot_light,
            spot_light_vector,
            spot_light_degree,
            screen_x,
            screen_y,
            background_color);
        printf("%.2fs\n\n", (double)(clock()-t)/1000);

        save_screen(screen, screen_file);
    }

    free(triangle_index);
    free(screen_point);
    free(screen);
    free(model_data);
    delete_BVHtree(model_bvhtree);
    free(directional_light);
    free(point_light);
    free(spot_light);
    free(spot_light_vector);
    free(spot_light_degree);

    fclose(setting_file);
    fclose(model_file);
    fclose(movement_file);
    fclose(light_file);
    fclose(screen_file);

    printf("end\n");

    return 0;
}

// BVH까지 적용하여 렌더링을 최적화시킨 버전
// 다양한 조명과 색상을 지원하는 버전

// 20230606
// 3D 네 번째 버전 3d4 개발 시작

// 20230608
// BVH tree 알고리즘 구현
// project_screen에 적용하였지만 검은줄이 나타나는 현상 발생

// 20230827
// BVH 버그 수정
// 시간 복잡도 O(N^3) 1095.2s -> O(N^2 log N) 2.5s

// 20240111
// 다시 프로젝트 시작
// BVH TREE 직육면체 반직선 교차 판별에서 부동소수점 오차로 k가 아주 작은 음수일 경우 제대로 판단하지 못하는 버그 수정(BVH는 애매한 경우는 계산해도 됨)

// 20240114
// shading - point light - BVH tree 버그 수정
// spot light 추가
// cielab 색공간 코드 수정

// 20240125
// BVH tree의 DEPTH_LIMIT, MIN_DATA_SIZE 값 수정 후 삼각형 166306개 분할 10.67초, 투영 6.51초, 셰이딩 18.18초 확인

// 20240126
// BVH tree 동적 할당 크기 정확히 계산, 메모리 해제 기능 추가
// setting 파일 읽는 파서 제작
// light 파일 구조 변경, 파서 적용
// 3d4 버전 개발 종료