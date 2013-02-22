from base64 import b64encode

f = open("builder.py", "r")
builder = f.read()
f.close()


payload_f = open("payload.py", "r")
builder = builder.replace("__PAYLOAD_PY__", b64encode(payload_f.read()))
payload_f.close()

byteplay_f = open("byteplay.py", "r")
builder = builder.replace("__BYTEPLAY_PY__", b64encode(byteplay_f.read()))
byteplay_f.close()

template_f = open("template.py", "r")
builder = builder.replace("__TEMPLATE_PY__", b64encode(template_f.read()))
template_f.close()

xor_layer_f = open("xor_layer.py", "r")
builder = builder.replace("__XOR_LAYER_PY__", b64encode(xor_layer_f.read()))
xor_layer_f.close()

f = open("builder_generated.py", "w")
f.write(builder)
f.close()
