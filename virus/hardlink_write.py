# pour tester


import os
from random import randint

DATA = "START ABC OWNED XYZ END"
size = len(DATA)
offset = 0

#print "size=%d" % (size,)

os.link("/tmp/hosts", "/tmp/hardlink")
f = open("/tmp/hardlink","a")
while(offset < size):
    size_chunk = randint(1, 1+(size-offset)/2)
    #print "taille %d"%size_chunk
    #print "size_chunk=%d, [%d:%d]"%(size_chunk, offset, offset+size_chunk)
    f.write(DATA[offset:offset+size_chunk])
    f.flush()
    offset+=size_chunk
    #break
    

f.close()
