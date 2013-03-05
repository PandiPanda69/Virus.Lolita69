__py_version__ = "\xFF\xFE\x0A\x20\xCA\xDD" # haha bullshit, this is a stealth signature
# -*- coding: utf-8 -*-

import zlib
import marshal

b = None
__DECRYPT_PLACEHOLDER__

# exec
exec marshal.loads(zlib.decompress(b("__PAYLOAD_PLACEHOLDER__")))

__py_version__ = "\xFF\xFE\x0A\x20\xCA\xDD" # haha bullshit, this is a stealth signature
