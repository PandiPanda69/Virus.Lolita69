def a(c):
    n = []
    for i in range(len(c)):
        n += c[-i - 1]
    return "".join(n)


DECRYPT = """
def b(c):
    n = []
    for i in range(len(c)):
        n += c[-i - 1]
    return "".join(n)
"""
