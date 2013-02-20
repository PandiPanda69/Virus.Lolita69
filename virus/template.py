# -*- coding: utf-8 -*-
signature = "PY_2.6"

import zlib
import base64
import marshal

__DECRYPT_PLACEHOLDER__

payload = "__PAYLOAD_PLACEHOLDER__"

# de-b64 step
deb64 = base64.b64decode(payload)

# de-crypt step
decrypt = decrypt(deb64)

# de-compress step
decompress = zlib.decompress(decrypt)

# de-marshal step
demarshal = marshal.loads(decompress)

# exec
exec demarshal

signature = "PY_2.6"
