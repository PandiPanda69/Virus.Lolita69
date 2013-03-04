# -*- coding: utf-8 -*-


def a(c):
    return "".join(chr(ord(c) ^ 0x0A) for c in c)


DECRYPT = """
def b(c):
    return "".join(chr(ord(c) ^ 0x0A) for c in c)
"""
