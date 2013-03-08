____ = "\xFF\xFE\x0A\x20\xCA\xDD"  # stealth signature
# -*- coding: utf-8 -*-

import zlib
import marshal

b = None
__DECRYPT_PLACEHOLDER__

# exec
exec marshal.loads(zlib.decompress(b("__PAYLOAD_PLACEHOLDER__")))

____ = "\xFF\xFE\x0A\x20\xCA\xDD" # stealth signature
