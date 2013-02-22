def crypt(clear):
    n = []
    for i in range(len(clear) / 2):
        n += [clear[2 * i + 1], clear[2 * i]]
    if len(clear) % 2 == 1:
        n += [clear[-1]]
    return "".join(n)


DECRYPT = """
def decrypt(crypted):
    n = []
    for i in range(len(crypted) / 2):
        n += [crypted[2 * i + 1], crypted[2 * i]]
    if len(crypted) % 2 == 1:
        n += [crypted[-1]]
    return "".join(n)
"""
