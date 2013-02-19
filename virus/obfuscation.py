import zlib
import base64
import marshal
import no_op_layer


def obfuscate(marshalled, layer=no_op_layer):
    # compress step
    compressed = zlib.compress(marshalled)

    # crypt step
    crypted = layer.crypt(compressed)

    # B64 step
    b64 = base64.b64encode(crypted)

    # template injection step
    template = open("template.py", "r").read()
    template = template.replace("__PAYLOAD_PLACEHOLDER__", b64)
    template = template.replace("__DECRYPT_PLACEHOLDER__", layer.DECRYPT)

    # template compile step
    template_compiled = compile(template, "z", "exec")

    # marshall step
    return marshal.dumps(template_compiled)

