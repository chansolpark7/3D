#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "includes/geometry.h"
#include "includes/screen.h"
#include "includes/project_screen.h"
#include "includes/shading.h"

#define PI 3.14159265358979323846

int fov;
int screen_x;
int screen_y;
char model_file_path[200];
char movement_file_path[200];
char light_file_path[200];
char screen_file_path[200];
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

    FILE *setting_file = fopen("C:/Users/chans/OneDrive/python/3D/rendering data/setting.txt", "rt");
    char string[200];
    char *command[200];
    int num;

    while (1)
    {
        if (fgets(string, 200, setting_file) == NULL) break;
        
        num = parsing(string, command);

        if (strcmp(command[0], "fov") == 0)
        {
            fov = atoi(command[1]);
        }
        else if (strcmp(command[0], "screen_size") == 0)
        {
            screen_x = atoi(command[1]);
            screen_y = atoi(command[2]);
        }
        else if (strcmp(command[0], "model_file") == 0)
        {
            strcpy(model_file_path, command[1]);
            length = strlen(model_file_path);
            if (model_file_path[length-1] == '\n') model_file_path[length-1] = 0;
        }
        else if (strcmp(command[0], "movement_file") == 0)
        {
            strcpy(movement_file_path, command[1]);
            length = strlen(movement_file_path);
            if (movement_file_path[length-1] == '\n') movement_file_path[length-1] = 0;
        }
        else if (strcmp(command[0], "light_file") == 0)
        {
            strcpy(light_file_path, command[1]);
            length = strlen(light_file_path);
            if (light_file_path[length-1] == '\n') light_file_path[length-1] = 0;
        }
        else if (strcmp(command[0], "screen_file") == 0)
        {
            strcpy(screen_file_path, command[1]);
            length = strlen(screen_file_path);
            if (screen_file_path[length-1] == '\n') screen_file_path[length-1] = 0;
        }
        else if (strcmp(command[0], "background_color") == 0)
        {
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

    int infinite_light_num;
    fscanf(light_file, "%d", &infinite_light_num);
    Light *infinite_light = malloc(sizeof(Light) * infinite_light_num);
    for (i=0; i<infinite_light_num; i++)
    {
        fscanf(light_file, "%f %f %f %f %f %f",
        &infinite_light[i].p.x, &infinite_light[i].p.y, &infinite_light[i].p.z,
        &infinite_light[i].brightness.r, &infinite_light[i].brightness.g, &infinite_light[i].brightness.b);
    }

    int light_num;
    fscanf(light_file, "%d", &light_num);
    Light *light = malloc(sizeof(Light) * light_num);
    for (i=0; i<light_num; i++)
    {
        fscanf(light_file, "%f %f %f %f %f %f",
        &light[i].p.x, &light[i].p.y, &light[i].p.z,
        &light[i].brightness.r, &light[i].brightness.g, &light[i].brightness.b);
    }

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

        printf("project screen\n");
        project_screen(
            triangle_index,
            screen_point,
            triangle_num,
            model_data,
            cmr,
            screen_x,
            screen_y,
            distance_to_screen);

        printf("shading\n");
        shading(
            screen,
            triangle_index,
            screen_point,
            triangle_num,
            model_data,
            cmr,
            infinite_light_num,
            infinite_light,
            light_num,
            light,
            screen_x,
            screen_y,
            background_color);

        save_screen(screen, screen_file);
    }

    free(triangle_index);
    free(screen_point);
    free(screen);
    free(model_data);

    fclose(setting_file);
    fclose(model_file);
    fclose(movement_file);
    fclose(light_file);
    fclose(screen_file);

    printf("end\n");

    return 0;
}

// 벡터를 사용해서 그림자를 계산하는 버전

// 20230319
// 3D 세 번째 버전 3d3 개발 시작

// 20230324
// c언어에서의 byte order과 파이썬 프로그램에서 사용하는 byte order과 달라서 color 값이 잘못 전달되는 문제를 해결해야 함

// 20230327
// is_in_triangle 함수에서 AB, BC, CA 벡터의 z값을 초기화하지 않은 실수 수정
// Camera cmr_ = {x, y, z, ex, ey} 순서를 {ex, ey, x, y, z}로 잘못 초기화한 실수 수정
// screen에 삼각형 투영 과정에서 cmr 벡터 회전 순서를 x회전, y회전에서 y회전, x회전으로 변경

// 20230328
// get_color 함수 제작
// 필요 없는 함수 제거
// 반복되는 연산 함수로 제작
// 가독성 향상
// 여러 버그 수정
// infinite point light 완성

// 20230407
// 다양한 역할을 하는 함수들을 헤더 파일과 c 파일로 나눠서 따로 관리하는 방법 공부

// 20230520
// 파잍 분리 시작
// geometry 헤더 파일 분리

// 20230521
// setting 파일 생성
// 3D 버전 별로 관리
// 모델, 프로그램, 렌더링 정보, 기타 파일 분리해서 보관
// 파일 경로 구조화
// 모든 프로그램을 한번에 실행해주는 자동화 프로그램 제작
// 삼각형과 직육면체 교차 검사 알고리즘 구상

// 20230524
// setting 파일로 렌더링 상수를 저장하는 구조 완성

// 20230525
// 헤더 파일 분리 완료

// 20230605
// BVH 알고리즘 제작 시작
// list 자료구조 완성

// 20230606
// 파일 정리 완료
// 3d3 버전 개발 종료

/////
// 알 수 없는 오류가 발생하면 triangle num unsigned로 바꾸기
// 오류 발생시 project_screen에서 k>=0을 k>0으로 바꾸기

/////
/*
실행 명령어
cd C:\Users\chans\OneDrive\python\3D\3d3
gcc -o 3d3 3d3.c geometry.o list.o project_screen.o screen.o shading.o
*/