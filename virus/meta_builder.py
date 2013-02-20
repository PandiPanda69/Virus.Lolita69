from base64 import b64encode

f = open("builder.py", "r")
builder = f.read()
f.close()


payload_f = open("payload.py", "r")
builder = builder.replace("__PAYLOAD_PY__", b64encode(payload_f.read()))
builder = builder.replace("__PAYLOAD_PLACE_HOLDER__", b64encode(payload_f.read()))
payload_f.close()

byteplay_f = open("byteplay.py", "r")
builder = builder.replace("__BYTEPLAY_PY__", b64encode(byteplay_f.read()))
byteplay_f.close()

template_f = open("template.py", "r")
builder = builder.replace("__TEMPLATE_PY__", b64encode(template_f.read()))
template_f.close()

injector_f = open("injector.py", "r")
builder = builder.replace("__INJECTOR_PY__", b64encode(injector_f.read()))
injector_f.close()

f = open("builder_generated.py", "w")
f.write(builder)
f.close()
