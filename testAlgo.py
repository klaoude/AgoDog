from math import *

def get_vect(a, b):
	return (b[0] - a[0], b[1] - a[1])

def dot(a, b):
	return a[0]*b[0] + a[1]*b[1]

def norm(a):
	return sqrt(a[0]*a[0] + a[1]*a[1])

def calc_angle(a, b, c):
	AB = get_vect(a, b)
	AC = get_vect(a, c)
	teta = acos(dot(AB, AC) / (norm(AB) * norm(AC)))
	sign = AB[0] * AC[1] - AB[1] * AC[0]
	return teta if sign > 0 else -teta

def rotate(u, teta):
	return (u[0] * cos(teta) - u[1] * sin(teta), u[0] * sin(teta) + u[1] * cos(teta))

C = (0, 0)

B1 = (3, 1.5)
P1 = (3, 3)
B2 = (1.5, 2)

B3 = (-1, 3)
P2 = (-3, 3.5)
B4 = (-2.5, 1.5)

B5 = (-1.5, -1)
P3 = (-2.5, -2.5)
B6 = (-0.5, -2)

B7 = (1, -2)
P4 = (2.5, -2.5)
B8 = (2, -1)

print calc_angle(C, B1, P1)
print calc_angle(C, B2, P1)
print calc_angle(C, B3, P2)
print calc_angle(C, B4, P2)
print calc_angle(C, B5, P3)
print calc_angle(C, B6, P3)
print calc_angle(C, B7, P4)
print calc_angle(C, B8, P4)

U = (10, 0)
teta = -pi/4
print rotate(U, teta)