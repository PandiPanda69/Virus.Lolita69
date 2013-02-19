import sys
import random
random.seed
k = "".join([chr(i) for i in random.sample(range(256),5)])
T = range(10,260,20)
p = 0
x = ord
for c in sys.stdin.read():
	if p < x(c)/20:
		k += k[1]*(x(c)/20-p)
	if p > x(c)/20:
		k += k[0]*(p-x(c)/20)
	p = x(c)/20
	if T[x(c)/20] < x(c):
		k += k[2]*(x(c)-T[x(c)/20])
	if T[x(c)/20] > x(c):
		k += k[3]*(T[x(c)/20]-x(c))
	T[x(c)/20] = x(c)
	k += k[4]
print k



