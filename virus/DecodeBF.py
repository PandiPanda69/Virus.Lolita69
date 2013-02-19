import sys

t,p,c,h = range(10,260,20),0,sys.stdin.read(),["p-=1","p+=1","t[p]+=1","t[p]-=1","sys.stdout.write('%c'%t[p])"]
k=[compile(x,'a','exec') for x in h]
[r for r in[eval(k[i])  for o in c[5:] for i in range(5)if o==c[i]] for r in c[5:]]
		
