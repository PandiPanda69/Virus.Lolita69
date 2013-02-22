import byteplay
from sys import argv
import marshal

print "Disassembling %s"%argv[1]
f = open(argv[1], "r")
f.read(8) # skip header
data = byteplay.Code.from_code(marshal.loads(f.read()))
f.close()

for instruction in data.code:
    print instruction
