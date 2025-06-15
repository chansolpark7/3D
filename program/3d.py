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
import time

import modeling.stl_v3 as stl
from patheasier import path

import threading

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

def add_data():
    global points
    points = []
    while True:
        try:
            string = input('com : ')
            if string == 'end':
                print('input end')
                break
            elif string == '-':
                lock.acquire()
                print(design.data.pop())
                lock.release()
            elif string == '.':
                print(points.pop())
            elif string == 'print_p':
                print(points)
            elif string == 'print_t':
                print(design.data)
            else:
                if len(string.split()) == 3:
                    x, y, z = map(float, string.split())
                    points.append((x, y, z))
                elif len(string.split()) == 4:
                    m, v1, v2, v3 = map(int, string.split())
                    lock.acquire()
                    design.place_triangle(points[v1], points[v2], points[v3])
                    lock.release()
                elif len(string.split()) == 5:
                    v1, v2, v3, v4 = map(int, string.split())
                elif len(string.split()) == 9:
                    x1, y1, z1, x2, y2, z2, x3, y3, z3 = map(float, string.split())
                    lock.acquire()
                    design.place_triangle((x1, y1, z1), (x2, y2, z2), (x3, y3, z3))
                    lock.release()
                else:
                    print('unknown command')
        except Exception as reason:
            print(reason)
            lock.release()



def main():
    global fov
    global distance_to_screen

    global eye_x_degree
    global eye_y_degree

    global points

    selected = []

    # lines = set()
    # for v1, v2, v3 in design.data:
    #     lines.add((tuple(v1), tuple(v2)))
    #     lines.add((tuple(v2), tuple(v3)))
    #     lines.add((tuple(v1), tuple(v3)))
    

    while True:
        clock.tick(60)
    
        key = pygame.key.get_pressed()

        move(key)
        pos = pygame.mouse.get_pos()
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                return
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_ESCAPE:
                    if len(selected) == 3:
                        try:
                            lock.acquire()
                            design.place_triangle(points[selected[0]], points[selected[1]], points[selected[2]])
                            lock.release()
                            selected = []
                        except Exception as reason:
                            print(reason)
                            lock.release()
                    elif len(selected) == 5:
                        try:
                            lock.acquire()
                            design.place_bezier_surface(
                                points[selected[0]],
                                points[selected[1]],
                                points[selected[2]],
                                points[selected[3]],
                                points[selected[4]],
                                6)
                            lock.release()
                            selected = []
                        except Exception as reason:
                            print(reason)
                            lock.release()
                            
            elif event.type == pygame.MOUSEWHEEL:
                fov += event.y
                distance_to_screen = tan(90-fov/2)*size[0]/2
                print(fov)
            elif event.type == pygame.MOUSEBUTTONDOWN:
                for i in range(len(points)):
                    x, y, z = points[i]
                    x, y, z = get_3Dpos(x-my_x, y-my_y, z-my_z)
                    if z <= 0:
                        continue
                    x, y = get_2Dpos(size, x, y, z)
                    if ((x-pos[0])**2 + (y-pos[1])**2)**0.5 <= 8:
                        if i in selected:
                            selected.remove(i)
                        else:
                            if len(selected) < 6:
                                selected.append(i)

        screen.fill((0, 0, 0))

        lock.acquire()
        for triangle in design.data:
            # draw_line(screen, size, (255, 255, 255), line[0], line[1])
            draw_triangle(screen, size, triangle)
        for i in range(len(points)):
            x, y, z = points[i]
            x, y, z = get_3Dpos(x-my_x, y-my_y, z-my_z)
            if z <= 0:
                continue
            x, y = get_2Dpos(size, x, y, z)
            if i in selected:
                pygame.draw.circle(screen, (255, 0, 0), (x, y), 3)
            else:
                pygame.draw.circle(screen, (255, 255, 255), (x, y), 3)
        lock.release()
        pygame.display.flip()

def test():
    tick = 0
    t = time.time()
    while True:
        tick += 1
        if time.time()-t > 1:
            print(tick)
            t = time.time()
            tick = 0

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                return
        
        screen.fill(0)
        for i in range(400):
            pygame.draw.line(screen, (255, 255, 255), (0, 0), (540, 540), 1)
            # pygame.draw.aaline(screen, (255, 255, 255), (0, 0), (540, 540), True)
        pygame.display.flip()

if __name__ == "__main__":
    lock = threading.Lock()
    pygame.init()
    size = [540, 540]
    screen = pygame.display.set_mode(size)
    clock = pygame.time.Clock()

    fov = 60
    distance_to_screen = tan(90-fov/2)*size[0]/2 # 스크린 사이즈의 x값을 기준으로한 스크린 까지의 거리

    my_x = 0
    my_y = 0
    my_z = 0
    speed = 2

    eye_x_degree = 0
    eye_y_degree = 0

    design = stl.design()
    # design.place_sphere(0, 0, 200, 100, 2)
    # for x in range(1, 4):
    #     for y in range(1, 4):
    #         for z in range(1, 4):
    #             if x!=1 and y!=1 and z!=1:
    #                 continue
    #             design.place_cube(x*100, y*100, z*100, 30)
    base_path = str(path(__file__)-2)
    # design.open_design(base_path + '/model/bay view.stl')
    design = stl.design()
    design.data = design.data[:2000]
    # print(design.data[:10])

    points = []
    t = threading.Thread(target=add_data)
    t.start()

    main()
    # test()
    if input('save?') == 'y':
        design.save('C:/Users/chans/OneDrive/python/3D/model/bay view2.stl')

# 제작 이유
# 가상 공간 구현을 위해