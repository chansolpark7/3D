# 왼손 좌표계 사용

# | y     /z
# |     /
# |   /
# | /
# --------- x

import sys
sys.path.append("C:/Users/chans/OneDrive/python")
import os

import pygame
import math
import time

import modeling.stl_v3 as stl
import random

def cos(x):
    return math.cos(math.radians(x))

def sin(x):
    return math.sin(math.radians(x))

def tan(x):
    return math.tan(math.radians(x))

def distance(A):
    return (A[0]**2 + A[1]**2 + A[2]**2)**0.5

def cross_product(A, B):
    return (A[1]*B[2]-A[2]*B[1], A[2]*B[0]-A[0]*B[2], A[0]*B[1]-A[1]*B[0])

def get_cos(A, B):
    # |A||B|cos x = A1B1+A2B2+A3B3
    # cos x = (A1B1+A2B2+A3B3)/(|A||B|)
    return (A[0]*B[0]+A[1]*B[1]+A[2]*B[2])/(distance(A)*distance(B))

def get_2Dpos(size, x, y, z):
    px = x*distance_to_screen/z
    py = y*distance_to_screen/z
    return (size[0]/2+px, size[1]/2-py)

def get_3Dpos(x, y, z):
    cx = cos(eye_x_degree)
    sx = sin(eye_x_degree)
    cy = cos(eye_y_degree)
    sy = sin(eye_y_degree)
    x, y, z = (cx*x-sx*z, y, cx*z+sx*x)
    return (x, cy*y+sy*z, cy*z-sy*y)

def draw_line(surface, size, color, start_pos, end_pos):
    x1, y1, z1 = start_pos
    x2, y2, z2 = end_pos

    x1, y1, z1 = get_3Dpos(x1-my_x, y1-my_y, z1-my_z)
    x2, y2, z2 = get_3Dpos(x2-my_x, y2-my_y, z2-my_z)

    if z1 <= 0:
        return
    if z2 <= 0:
        return
    
    # x1, y1, z1 = (x1-my_x, y1-my_y, z1-my_z)
    # x2, y2, z2 = (x2-my_x, y2-my_y, z2-my_z)

    p1 = get_2Dpos(size, x1, y1, z1)
    p2 = get_2Dpos(size, x2, y2, z2)
    # pygame.draw.line(surface, color, p1, p2, 1)
    pygame.draw.line(surface, color, p1, p2, 1)

def draw_triangle(surface, size, triangle):
    x1, y1, z1 = triangle[0]
    x2, y2, z2 = triangle[1]
    x3, y3, z3 = triangle[2]

    x1, y1, z1 = get_3Dpos(x1-my_x, y1-my_y, z1-my_z)
    x2, y2, z2 = get_3Dpos(x2-my_x, y2-my_y, z2-my_z)
    x3, y3, z3 = get_3Dpos(x3-my_x, y3-my_y, z3-my_z)

    if z1 <= 0:
        return
    if z2 <= 0:
        return
    if z3 <= 0:
        return

    p1 = get_2Dpos(size, x1, y1, z1)
    p2 = get_2Dpos(size, x2, y2, z2)
    p3 = get_2Dpos(size, x3, y3, z3)

    # normal = cross_product(triangle[0], triangle[1])
    # light = (-1, -1, -1)

    # cos_value = get_cos(normal, light)
    # if cos_value < 0:
    #     color = [50, 50, 50]
    # else:
    #     color = [255*cos_value]*3

    color = [255, 255, 255]

    pygame.draw.polygon(surface, color, (p1, p2, p3), 1)

def move(key):
    global my_x
    global my_y
    global my_z

    global eye_x_degree
    global eye_y_degree

    if key[pygame.K_w]:
        my_x += sin(eye_x_degree) * speed
        my_z += cos(eye_x_degree) * speed
    if key[pygame.K_a]:
        my_x -= cos(eye_x_degree) * speed
        my_z += sin(eye_x_degree) * speed
    if key[pygame.K_s]:
        my_x -= sin(eye_x_degree) * speed
        my_z -= cos(eye_x_degree) * speed
    if key[pygame.K_d]:
        my_x += cos(eye_x_degree) * speed
        my_z -= sin(eye_x_degree) * speed
    if key[pygame.K_SPACE]:
        my_y += speed
    if key[pygame.K_LSHIFT]:
        my_y -= speed
    
    if key[pygame.K_UP]:
        eye_y_degree -= 1
    if key[pygame.K_LEFT]:
        eye_x_degree -= 1
    if key[pygame.K_DOWN]:
        eye_y_degree += 1
    if key[pygame.K_RIGHT]:
        eye_x_degree += 1

def make_data():
    design = stl.design()
    triangles = []
    p_x = (random.random(), random.random())
    p_y = (random.random(), random.random())
    p_z = (random.random(), random.random())
    min_p = (min(p_x), min(p_y), min(p_z))
    max_p = (max(p_x), max(p_y), max(p_z))
    v1 = (min_p[0], min_p[1], min_p[2])
    v2 = (max_p[0], min_p[1], min_p[2])
    v3 = (max_p[0], min_p[1], max_p[2])
    v4 = (min_p[0], min_p[1], max_p[2])
    v5 = (min_p[0], max_p[1], min_p[2])
    v6 = (max_p[0], max_p[1], min_p[2])
    v7 = (max_p[0], max_p[1], max_p[2])
    v8 = (min_p[0], max_p[1], max_p[2])
    stl.design.place_rect(design, v1, v2, v3, v4)
    stl.design.place_rect(design, v5, v6, v7, v8)
    stl.design.place_rect(design, v1, v2, v6, v5)
    stl.design.place_rect(design, v2, v3, v7, v6)
    stl.design.place_rect(design, v3, v4, v8, v7)
    stl.design.place_rect(design, v4, v1, v5, v8)
    S = (0, 0, 0)
    V = (random.random(), random.random(), random.random())
    length = (V[0]**2 + V[1]**2 + V[2]**2)**0.5
    P = (S[0]+V[0]/length*2, S[1]+V[1]/length*2, S[2]+V[2]/length*2)
    stl.design.place_triangle(design, S, P, P)
    for i in design.data:
        triangles.append(i)

    print(*min_p, *max_p, *V)
    return triangles

def open_obj():
    path = "C:/Users/chans/OneDrive/python/3D/model/tinker.obj"
    with open(path, 'rt') as file:
        point = []
        triangle = []
        while True:
            string = file.readline()
            if not string:
                break
            if string[0] == 'v':
                x, y, z = map(float, string[2:].split())
                point.append((x, z, y))
            elif string[0] == 'f':
                a, b, c = map(int, string[2:].split())
                triangle.append((point[a-1], point[b-1], point[c-1]))
    return triangle

def main():
    global fov
    global distance_to_screen

    global eye_x_degree
    global eye_y_degree

    # triangles = make_data()
    triangles = open_obj()
    # print(triangles)

    # lines = set()
    # for v1, v2, v3 in design.data:
    #     lines.add((tuple(v1), tuple(v2)))
    #     lines.add((tuple(v2), tuple(v3)))
    #     lines.add((tuple(v1), tuple(v3)))
    
    print(len(triangles))

    while True:
        clock.tick(60)
    
        key = pygame.key.get_pressed()

        move(key)
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                return
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_ESCAPE:
                    triangles = make_data()
                elif event.key == pygame.K_TAB:
                    img_file_path = "C:/Users/chans/OneDrive/python/3D/img"
                    t = time.localtime()
                    file_name = str(t.tm_year)
                    file_name += str(t.tm_mon).zfill(2)
                    file_name += str(t.tm_mday).zfill(2)
                    file_name += str(t.tm_hour).zfill(2)
                    file_name += str(t.tm_min).zfill(2)
                    file_name += str(t.tm_sec).zfill(2)
                    file_name += ".png"
                    pygame.image.save(screen, os.path.join(img_file_path, file_name))
                    print(f'saved {file_name}')
            elif event.type == pygame.MOUSEWHEEL:
                fov += event.y
                distance_to_screen = tan(90-fov/2)*size[0]/2
                print(fov)

        screen.fill((0, 0, 0))
        for triangle in triangles:
            # draw_line(screen, size, (255, 255, 255), line[0], line[1])
            draw_triangle(screen, size, triangle)
        pygame.display.flip()

if __name__ == "__main__":
    pygame.init()
    size = [540, 540]
    screen = pygame.display.set_mode(size)
    clock = pygame.time.Clock()

    fov = 60
    distance_to_screen = tan(90-fov/2)*size[0]/2 # 스크린 사이즈의 x값을 기준으로한 스크린 까지의 거리

    my_x = 0
    my_y = 0
    my_z = 0
    speed = 0.1

    eye_x_degree = 0
    eye_y_degree = 0

    main()