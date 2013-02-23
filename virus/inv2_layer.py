def a(c):
    n = []
    for i in range(len(c) / 2):
        n += [c[2 * i + 1], c[2 * i]]
    if len(c) % 2 == 1:
        n += [c[-1]]
    return "".join(n)


DECRYPT = """
def b(c):
    n = []
    for i in range(len(c) / 2):
        n += [c[2 * i + 1], c[2 * i]]
    if len(c) % 2 == 1:
        n += [c[-1]]
    return "".join(n)
"""
