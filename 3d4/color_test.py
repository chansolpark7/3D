import pygame

pygame.init()
size = (1024, 600)
screen = pygame.display.set_mode(size)
for i in range(256):
    pygame.draw.rect(screen, (i, i, i), (i*4, 0, 4, 600))
pygame.display.flip()

while True:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            exit()