from math import *
import numpy as np
import matplotlib.pyplot as plt

def get_vect(a, b):
	return (b[0] - a[0], b[1] - a[1])

def dot(a, b):
	return a[0]*b[0] + a[1]*b[1]

def norm(a):
	return sqrt(a[0]*a[0] + a[1]*a[1])

def dist(a, b):
	return sqrt((b[0] - a[0]) * (b[0] - a[0]) + (b[1] - a[1])*(b [1] - a[1]))


def calc_angle(a, b, c):
	AB = get_vect(a, b)
	AC = get_vect(a, c)
	teta = acos(dot(AB, AC) / (norm(AB) * norm(AC)))
	sign = AB[0] * AC[1] - AB[1] * AC[0]
	return teta if sign > 0 else -teta

def rotate(u, teta):
	return (u[0] * cos(teta) - u[1] * sin(teta), u[0] * sin(teta) + u[1] * cos(teta))

def target(brebie):
	dest = (0, 0)
	
	rb = dist(brebie, dest)
	ab = tan(brebie[1] - dest[1] / brebie[0] - dest[0])

	print "rb = " + str(rb) + " ab = " + str(ab)

	ret = ((rb+3)*cos(ab), (rb+3)*sin(ab))

	x = [0]
	y = [0]

	x.append(brebie[0])
	x.append(ret[0])

	y.append(brebie[1])
	y.append(ret[1])

	plt.plot(x, y, 'ro')
	plt.axis([-10, 10, -10, 10])
	plt.show()
	
	return ret

B = (-1, 1)
print target(B)

"""
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
"""

"""
[Bot-Purple] Trajet viewer !
(0, 0) -> (4500, 3000) -> (4500, 3000) -> (4500, 3000) -> (4500, 3000) -> (4500, 3000) -> (4529, 3026) -> (4529, 3026) -> (4559, 3053) -> (4559, 3053) -> (4589, 3080) -> (4589, 3080) -> (4589, 3080) -> (4618, 3107) -> (4618, 3107) -> (4648, 3133) -> (4648, 3133) -> (4682, 3155) -> (4682, 3155) -> (4715, 3177) -> 
-------------------------------------
[Bot-Purple] sorted Trajet viewer !
(4529, 3026) -> (4559, 3053) -> (4589, 3080) -> (4618, 3107) -> (4648, 3133) -> (4682, 3155) -> 
-------------------------------------
[Bot-Purple] Direction = (153, 129) !
"""

"""
def calc_dir(deplacement):
	None

def show_dir(dep):
	x = []
	y = []
	for vec in dep:
		x.append(vec[0] - 4500)
		y.append(vec[1] - 3000)
	plt.plot(x, y, 'ro')
	plt.axis([-100, 100, -100, 100])
	plt.show()


move = [(4500, 3000), (4506, 3039), (4513, 3078), (4520, 3118), (4527, 3157), (4534, 3197), (4541, 3236)]

show_dir(move)
"""