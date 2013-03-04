# -*- coding: utf-8 -*-

import random


def a(c):
    k = chr(random.randint(0, 255))
    return k + "".join(chr(ord(c) ^ ord(k)) for c in c)


DECRYPT = """
def b(c):
    k = c[0]

    return "".join(chr(ord(c) ^ ord(k)) for c in c[1:])
"""
