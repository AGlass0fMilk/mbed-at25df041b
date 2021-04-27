# Description: Generates 512 random C-style hexadecimal numbers

import random

r = []
for x in range(0, 512):
    r.append(random.randint(0, 0xFF))

for x in range(0, 512, 16):
    s = ""
    for i in range(0, 16):
        s += ('0x%X,' % (r[x+i]))
    print(s)
