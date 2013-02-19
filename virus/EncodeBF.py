import sys
code = ''
table = range(10,260,20)
pointeur = 0
toEncode = sys.stdin.read()
for c in toEncode:
  val = ord(c)
	ra = (val/20)
	if pointeur < ra:
		code += ">"*(ra-pointeur)
	if pointeur > ra:
		code += "<"*(pointeur-ra)
	pointeur = ra
	if table[ra] < val:
		code += "+"*(val-table[ra])
	if table[ra] > val:
		code += "-"*(table[ra]-val)
	table[ra] = val
	code += "."
print code


