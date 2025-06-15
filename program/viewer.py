import pygame
import time
import os

def read1():
    return int.from_bytes(screen_file.read(1), 'little')

def read2():
    return int.from_bytes(screen_file.read(2), 'little')

def draw(frame):
    screen_file.seek(6+3*surface_x*surface_y*frame)
    for y in range(surface_x):
        surface_array[:, y] = [int.from_bytes(screen_file.read(3), 'big') for i in range(surface_x)]
    pygame.transform.smoothscale(surface, (screen_x, screen_y), screen)

def main():
    frame = 0
    update = True
    while True:
        clock.tick(30)
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                return
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_LEFT:
                    update = True
                    frame = (frame-1)%frame_num
                    print(frame)
                elif event.key == pygame.K_RIGHT:
                    update = True
                    frame = (frame+1)%frame_num
                    print(frame)
                elif event.key == pygame.K_TAB:
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

        if update:
            update = False
            draw(frame)
        pygame.display.flip()

if __name__ == "__main__":
    setting_file_path = "C:/Users/chans/OneDrive/python/3D/rendering data/setting.txt"
    screen_file_path = None
    img_file_path = None
    with open(setting_file_path, 'rt', encoding='utf-8') as setting_file:
        strings = setting_file.readlines()
        for string in strings:
            index = string.find(' ')
            var = string[:index]
            if var == "screen_file":
                screen_file_path = string[index+1:].rstrip().strip('`')
            elif var == "img_path":
                img_file_path = string[index+1:].rstrip().strip('`')

    if screen_file_path == None or img_file_path == None:
        print("No value found in setting")
        exit()

    screen_file = open(screen_file_path, "rb")
    frame_num = read2()
    surface_x = read2()
    surface_y = read2()
    screen_x = 600
    screen_y = 600
    print(surface_x, surface_y)

    pygame.init()
    surface = pygame.Surface((surface_x, surface_y))
    screen = pygame.display.set_mode((screen_x, screen_y))
    surface_array = pygame.PixelArray(surface)
    clock = pygame.time.Clock()

    print(frame_num)
    main()