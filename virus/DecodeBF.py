import sys

table = range(10,260,20)
ptr = 0
code = sys.stdin.read()
key = code[:5]
code = code[5:]

for opcode in code:
	if opcode == key[0]:
		ptr -= 1
	if opcode == key[1]:
		ptr += 1
	if opcode == key[2]:
		table[ptr] = table[ptr] + 1
	if opcode == key[3]:
		table[ptr] = table[ptr] - 1
	if opcode == key[4]:
		sys.stdout.write("%c"%table[ptr])
		
