def crypt(clear):
    n = []
    for i in range(len(clear)):
        n += clear[-i - 1]
    return "".join(n)


DECRYPT = """
def decrypt(crypted):
    n = []
    for i in range(len(crypted)):
        n += crypted[-i - 1]
    return "".join(n)
"""
