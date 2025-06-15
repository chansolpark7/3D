import sys
sys.path.append("C:/Users/chans/OneDrive/python")

import modeling.stl_v3 as stl

model = stl.design()
# v1 = [-10, 10, -10]
# v2 = [-10, 10, 10]
# v3 = [10, 10, -10]
# v4 = [10, 10, 10]
# v5 = [10, -10, -10]
# v6 = [10, -10, 10]
# # v7 = [-10, -10, -10]
# # v8 = [-10, -10, 10]
# # model.place_rect(v1, v2, v4, v3)
# # model.place_rect(v3, v4, v6, v5)
# # model.place_rect(v5, v6, v8, v7)
# # model.place_rect(v7, v8, v2, v1)
# # model.place_rect([-100, 11, -100], [-100, 11, 100], [100, 11, 100], [100, 11, -100])
# model.place_rect(v1, v2, v4, v3)
# model.place_rect(v3, v4, v6, v5)

try:
    while True:
        x1, y1, z1 = map(float, input('p1 : ').split())
        x2, y2, z2 = map(float, input('p2 : ').split())
        x3, y3, z3 = map(float, input('p3 : ').split())
        model.place_triangle((x1, y1, z1), (x2, y2, z2), (x3, y3, z3))
except:
    pass

setting_file_path = "C:/Users/chans/OneDrive/python/3D/rendering data/setting.txt"
model_file_path = None
with open(setting_file_path, 'rt', encoding='utf-8') as setting_file:
    strings = setting_file.readlines()
    for string in strings:
        index = string.find(' ')
        var = string[:index]
        if var == "model_file":
            model_file_path = string[index+1:].rstrip()

if model_file_path == None:
    print("No value found in setting")
    exit()

answer = input(f"save {model_file_path}? (y/n) ")
if answer != 'y':
    exit()

try:
    model.save(model_file_path)
except Exception as reason:
    print("save error")
    print(reason)