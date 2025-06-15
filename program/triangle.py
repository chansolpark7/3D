import pygame

def draw_triangle(surface, A, B, C,  color):
    A, B, C = sorted([A, B, C])
    # 1 AB AC
    for x in range(A[0], B[0]):
        y1 = (B[1]-A[1])*(x-A[0])/(B[0]-A[0])+A[1]
        y2 = (C[1]-A[1])*(x-A[0])/(C[0]-A[0])+A[1]
        pygame.draw.line(surface,color, (x, y1), (x, y2), 1)
    # 2 AC BC
    for x in range(B[0], C[0]):
        y1 = (C[1]-A[1])*(x-A[0])/(C[0]-A[0])+A[1]
        y2 = (C[1]-B[1])*(x-B[0])/(C[0]-B[0])+B[1]
        pygame.draw.line(surface,color, (x, y1), (x, y2), 1)

def main():
    A = (0, 0)
    B = (0, 0)
    C = (0, 0)
    while True:
        clock.tick(60)
        pos = pygame.mouse.get_pos()
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                return
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_1:
                    A = pos
                elif event.key == pygame.K_2:
                    B = pos
                elif event.key == pygame.K_3:
                    C = pos
        screen.fill((255, 255, 255))
        pygame.draw.circle(screen, 0, A, 5)
        pygame.draw.circle(screen, 0, B, 5)
        pygame.draw.circle(screen, 0, C, 5)
        draw_triangle(screen, A, B, C, 0)
        pygame.display.flip()

if __name__ == "__main__":
    pygame.init()
    size = (500, 500)
    screen = pygame.display.set_mode(size)

    clock = pygame.time.Clock()
    main()