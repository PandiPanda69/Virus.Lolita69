import sys

table = range(10,260,20)
ptr = 0
code = sys.stdin.read()
for opcode in code:
  if opcode == "<":
		ptr -= 1
	if opcode == ">":
		ptr += 1
	if opcode == "+":
		table[ptr] = table[ptr] + 1
	if opcode == "-":
		table[ptr] = table[ptr] - 1
	if opcode == ".":
		sys.stdout.write("%c"%table[ptr])
		
