import marshal
from zlib import compress
import imp

import xor_layer
import no_op_layer
import inv2_layer
import invAll_layer
import bf_layer
import xor_light_layer
from minify import minify


def to_hex(s):
    return "\\x" + "\\x".join(c.encode("hex") for c in s)


def obfuscate(marshalled, layer=no_op_layer):
    # compress step
    compressed = compress(marshalled, 9)

    # crypt step
    crypted = layer.a(compressed)

    # hex step
    hexed = to_hex(crypted)

    # template injection step
    template = open("template.py", "r").read()
    template = template.replace("__PAYLOAD_PLACEHOLDER__", hexed)
    template = template.replace("__DECRYPT_PLACEHOLDER__", layer.DECRYPT)

    # template compile step
    if layer == bf_layer:  # minify fails on this heavily obfuscated py :(
        template_compiled = compile(template, "", "exec")
    else:
        template_compiled = compile(minify(template), "", "exec")

    # marshall step
    return marshal.dumps(template_compiled)


def obfuscate_light(marshalled, layer):
    # crypt step
    crypted = layer.a(marshalled)

    hexed = to_hex(crypted)

    # template injection step
    template = open("template_light.py", "r").read()
    template = template.replace("__PAYLOAD_PLACEHOLDER__", hexed)
    template = template.replace("__DECRYPT_PLACEHOLDER__", layer.DECRYPT)

    template_compiled = compile(minify(template), "", "exec")

    # marshall step
    return marshal.dumps(template_compiled)


f = open("builder.py", "r")
builder = f.read()
f.close()

payload_f = open("payload.py", "r")
builder = builder.replace("__PAYLOAD_PY__", to_hex(payload_f.read()))
payload_f.close()

byteplay_f = open("byteplay.py", "r")
builder = builder.replace("__BYTEPLAY_PY__", to_hex(byteplay_f.read()))
byteplay_f.close()

template_f = open("template.py", "r")
builder = builder.replace("__TEMPLATE_PY__", to_hex(template_f.read()))
template_f.close()

xor_layer_f = open("xor_layer.py", "r")
builder = builder.replace("__XOR_LAYER_PY__", to_hex(xor_layer_f.read()))
xor_layer_f.close()

payload = marshal.dumps(compile(minify(builder), "", "exec"))

print "Ajout des BF layers"
for i in range(1):
    payload = obfuscate(payload, bf_layer)

print "Ajout des invAll layers"
for i in range(3):
    payload = obfuscate(payload, invAll_layer)

print "Ajout des XOR layers"
for i in range(5):
    payload = obfuscate(payload, xor_layer)

print "Ajout des inv2 layers"
for i in range(5):
    payload = obfuscate(payload, inv2_layer)

print "Ajout du XOR light layer"
payload = obfuscate_light(payload, xor_light_layer)

lolita_f = open("lolita.py", "r")
lolita = lolita_f.read()
lolita_f.close()

payload_hex = to_hex(payload)

lolita = lolita.replace("__FINAL_PLACEHOLDER__", payload_hex)
lolita = marshal.dumps(compile(minify(lolita), "", "exec"))

lolita_f = open("lolita.final.pyc", "wb")

lolita_f.write(imp.get_magic() + ")\xb9\x1bQ")
lolita_f.write(lolita)
lolita_f.close()
