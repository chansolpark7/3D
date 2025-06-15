import pygame

def draw_bezier_curve(surface, A, B, C):
    Ax, Ay = A
    Bx, By = B
    Cx, Cy = C
    f = lambda a, b, c, t: ((1-t)**2)*a + 2*t*(1-t)*b + (t**2)*c

    points = [(f(Ax, Bx, Cx, i/100), f(Ay, By, Cy, i/100)) for i in range(100)]
    pygame.draw.aalines(surface, (255, 0, 0), False, (A, B, C), 4)
    pygame.draw.aalines(surface, (0, 255, 0), False, points, 4)

l = []
l2 = []
i = 0
file = open('music player/graph file.txt', 'rt', encoding='utf-8')
while True:
    a = file.readline().rstrip()
    if a == '':
        break
    x, y = map(float, a.split())
    l.append((int(x), int(y*10)))
    i += 1

i = 0
while True:
    a = file.readline().rstrip()
    if a == '':
        break
    x, y = map(float, a.split())
    l2.append((int(x), int(y*10)))
    i += 1

pygame.init()
size = (600, 600)
s_size = (500, 500)
screen = pygame.display.set_mode(size)
surface = pygame.Surface(s_size)
clock = pygame.time.Clock()
surface.fill(0)
A = (0, 0)
B = (0, 0)
C = (0, 0)
for pos in l:
    surface.set_at(pos, (255, 0, 0))

for pos in l2:
    surface.set_at(pos, (0, 255, 0))

# a = -0.25
# for x in range(200):
#     x = x/200
#     surface.set_at((int(400*x), int(400*((a*a-a)/(x-a)+a))), (255, 0, 0))
while True:
    clock.tick(60)
    pos = pygame.mouse.get_pos()
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            exit()
        elif event.type == pygame.KEYDOWN:
            if event.key == pygame.K_1:
                A = pos
            elif event.key == pygame.K_2:
                B = pos
            elif event.key == pygame.K_3:
                C = pos
            elif event.key == pygame.K_SPACE:
                print(A, B, C)
    screen.fill((255, 255, 255))
    screen.blit(surface, (50, 50))
    draw_bezier_curve(screen, A, B, C)
    pygame.display.flip()