# 왼손 좌표계 사용

# | y     /z
# |     /
# |   /
# | /
# --------- x

import sys
sys.path.append("C:/Users/chans/OneDrive/python")

import pygame
import math
import modeling.stl_v2 as stl
import struct
import random

def cos(x):
    return math.cos(math.radians(x))

def sin(x):
    return math.sin(math.radians(x))

def tan(x):
    return math.tan(math.radians(x))

def distance(A):
    return (A[0]**2 + A[1]**2 + A[2]**2)**0.5

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
        eye_y_degree -= rotation_speed
    if key[pygame.K_LEFT]:
        eye_x_degree -= rotation_speed
    if key[pygame.K_DOWN]:
        eye_y_degree += rotation_speed
    if key[pygame.K_RIGHT]:
        eye_x_degree += rotation_speed

def main():
    global fov
    global distance_to_screen

    global eye_x_degree
    global eye_y_degree

    triangles = []
    for v1, v2, v3 in design.data:
        triangles.append((v1, v2, v3))
    
    print(len(triangles))

    movement_data = []

    while True:
        clock.tick(60)
    
        key = pygame.key.get_pressed()

        move(key)
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                return movement_data
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_TAB:
                    movement_data.append([my_x, my_y, my_z, eye_x_degree, eye_y_degree])
                    print(len(movement_data))
            elif event.type == pygame.MOUSEWHEEL:
                fov += event.y
                distance_to_screen = tan(90-fov/2)*size[0]/2

        screen.fill((0, 0, 0))
        for triangle in triangles:
            draw_triangle(screen, size, triangle)
        pygame.display.flip()

if __name__ == "__main__":
    fov = None
    screen_x = None
    screen_y = None
    speed = None
    rotation_speed = None
    model_file_path = None
    movement_file_path = None

    setting_file_path = "C:/Users/chans/OneDrive/python/3D/rendering data/setting.txt"
    with open(setting_file_path, 'rt', encoding='utf-8') as setting_file:
        strings = setting_file.readlines()
        for string in strings:
            index = string.find(' ')
            var = string[:index]
            value = string[index+1:].rstrip()
            if var == "fov":
                fov = int(value)
            elif var == "screen_x":
                screen_x = int(value)
            elif var == "screen_y":
                screen_y = int(value)
            elif var == "camera_speed":
                speed = int(value)
            elif var == "camera_rotation_speed":
                rotation_speed = int(value)
            elif var == "model_file":
                model_file_path = value
            elif var == "movement_file":
                movement_file_path = value
    
    if (fov == None or
        screen_x == None or
        screen_y == None or
        speed == None or
        rotation_speed == None or
        model_file_path == None or
        movement_file_path == None):
        print("No value found in setting")
        exit()

    distance_to_screen = tan(90-fov/2)*screen_x/2

    my_x = 0
    my_y = 0
    my_z = 0

    eye_x_degree = 0
    eye_y_degree = 0

    design = stl.design()
    design.open_design(model_file_path)
    if len(design.data) > 3000:
        design.data = random.sample(design.data, 3000)

    pygame.init()
    size = (screen_x, screen_y)
    screen = pygame.display.set_mode(size)
    clock = pygame.time.Clock()

    movement = main()
    print(len(movement))
    with open(movement_file_path, "wb") as file:
        file.write(int.to_bytes(len(movement), 2, 'little'))
        for x, y, z, ex, ey in movement:
            file.write(struct.pack('f', x))
            file.write(struct.pack('f', y))
            file.write(struct.pack('f', z))
            file.write(struct.pack('f', ex))
            file.write(struct.pack('f', ey))