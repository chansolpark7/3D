setting_file_path = "C:/Users/chans/OneDrive/python/3D/rendering data/setting.txt"
with open(setting_file_path, 'rt', encoding='utf-8') as setting_file:
    data = setting_file.readlines()

print(data)