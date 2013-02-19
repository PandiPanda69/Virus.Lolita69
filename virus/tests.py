# -*- coding: utf-8 -*-

import marshal
from obfuscation import obfuscate
import xor_layer

def test_xor_several():
    src="print 'XOR works several times'"
    code=marshal.dumps(compile(src, "test_xor", "exec"))
    
    #xored = obfuscate(obfuscate(code))
    for i in range(100):
        code = obfuscate(code, xor_layer)
        
    print "Obfuscation done"
    exec marshal.loads(code)
    
    
if __name__ == '__main__':
    test_xor_several()
