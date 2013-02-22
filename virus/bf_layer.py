import random


L,C,k,T,p,x,kde,br=["k[1]*(x(c)/20-p)","k[0]*(p-x(c)/20)","k[2]*(x(c)-T[x(c)/20])","k[3]*(T[x(c)/20]-x(c))"] ,["p < x(c)/20","p > x(c)/20","T[x(c)/20] < x(c)","T[x(c)/20] > x(c)"] ,"".join([chr(i) for i in random.sample(range(256),5)]), range(10,260,20),0,ord ,compile("""k += "".join([br(L[i]) for i in range(4) if br(C[i])])+k[4]; T[x(c)/20] = x(c); p = x(c)/20""",'a','exec'),eval
loc = locals()
def a(c):
	for o in c:
		loc['c'] = o
		br(kde,globals(),loc)
	return k

DECRYPT = """
loc = locals()
w,t,p,h ="", range(10,260,20),0,["p-=1","p+=1","t[p]+=1","t[p]-=1","w+='%c'%t[p]"]
k=[compile(x,'a','exec') for x in h]
def b(d):
	loc["c"] = d
	[eval(k[i],globals(),loc)  for o in c[5:] for i in range(5)if o==c[i]]	
	return w
"""

