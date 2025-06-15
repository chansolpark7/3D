import os
import sys

base_path = "C:/Users/chans/OneDrive/python/3D"

if "make_model" in sys.argv:
    os.system(f"python -u \"{base_path}/program/makemodel.py\"")

if "make_data" in sys.argv:
    os.system(f"python -u \"{base_path}/program/makedata.py\"")

version = 3
c_file_path = f"{base_path}/3d{version}/"
os.system(f"gcc -o {c_file_path}3d{version} {c_file_path}3d{version}.c {c_file_path}includes/geometry.c")
os.system(f"{c_file_path}3d3")

os.system(f"python -u {base_path}/program/viewer.py")