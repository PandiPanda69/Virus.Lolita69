import sys
import random
random.seed

L,C,k,T,p,x,kde,br,bullshit=["k[1]*(x(c)/20-p)","k[0]*(p-x(c)/20)","k[2]*(x(c)-T[x(c)/20])","k[3]*(T[x(c)/20]-x(c))"] ,["p < x(c)/20","p > x(c)/20","T[x(c)/20] < x(c)","T[x(c)/20] > x(c)"] ,"".join([chr(i) for i in random.sample(range(256),5)]), range(10,260,20),0,ord ,compile("""k += "".join([br(L[i]) for i in range(4) if br(C[i])])+k[4]; T[x(c)/20] = x(c); p = x(c)/20""",'a','exec'),eval,sys.stdin.read

for c in bullshit():
	br(kde)
print k
