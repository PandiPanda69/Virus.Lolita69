# pour tester


import os
from random import randint
from uuid import uuid4 as uuid

DATA = "START ABC OWNED XYZ END"
size = len(DATA)
offset = 0

#print "size=%d" % (size,)

while(offset < size):
    hardlink = "/tmp/%s" % uuid()
    os.link("/tmp/hosts", hardlink)
    f = open(hardlink,"a")
    size_chunk = randint(1, 1+(size-offset)/2)
    #print "taille %d"%size_chunk
    #print "size_chunk=%d, [%d:%d]"%(size_chunk, offset, offset+size_chunk)
    f.write(DATA[offset:offset+size_chunk])
    f.flush()
    offset+=size_chunk
    f.close()
    os.unlink(hardlink)
    #break
    


