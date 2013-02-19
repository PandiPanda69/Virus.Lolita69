# -*- coding: utf-8 -*-

import random


def crypt(clear):
    key = chr(random.randint(0,255))
    crypted = "".join(chr(ord(c) ^ ord(key)) for c in clear)

    return key + crypted

    
DECRYPT = """
def decrypt(crypted):
    key = crypted[0]

    return "".join(chr(ord(c) ^ ord(key)) for c in crypted[1:])
"""
