import sys
sys.path.append("C:/Users/chans/OneDrive/python")

import modeling.stl_v3 as stl
import random
import numpy

def get_bezier(A, B, P, t):
    p1 = (A[0]*t+P[0]*(1-t), A[1]*t+P[1]*(1-t), A[2]*t+P[2]*(1-t))
    p2 = (P[0]*t+B[0]*(1-t), P[1]*t+B[1]*(1-t), P[2]*t+B[2]*(1-t))
    p = (p1[0]*t+p2[0]*(1-t), p1[1]*t+p2[1]*(1-t), p1[2]*t+p2[2]*(1-t))
    return p

t = 100
design = stl.design()
A = (0, 0, 0)
B = (20, 0, 0)
C = (20, 0, 20)
D = (0, 0, 20)
P = (10, -10, 10)
X = list(numpy.linspace(0, 1, t))
Y = list(numpy.linspace(0, 1, t))
table = []

for x in X:
    table.append([])
    for y in Y:
        p1 = get_bezier(A, B, P, x)
        p2 = get_bezier(D, C, P, x)
        
        p3 = get_bezier(p1, p2, P, y)
        table[-1].append(p3)

for x in range(t-1):
    for y in range(t-1):
        design.place_triangle(table[x][y], table[x+1][y], table[x+1][y+1])
        design.place_triangle(table[x][y], table[x+1][y+1], table[x][y+1])

design.save('C:/Users/chans/OneDrive/python/3D/model/bezier surface.stl')