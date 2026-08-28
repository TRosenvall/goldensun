import os, re
# ldr rA, =<symbol-ish>   /   ldr rB, =<number>   /   add rA, rB
L1 = re.compile(r'^\tldr\t(r\d+), =(\S+)$')
ADD = re.compile(r'^\tadd\t(r\d+), (r\d+)$')
hits = []
for root, _, files in os.walk('asm'):
    for fn in files:
        if not fn.endswith('.s'):
            continue
        p = os.path.join(root, fn)
        c = p.replace('asm/', 'src/', 1)[:-2] + '.c'
        if not os.path.exists(c):        # only ELEVATED TUs: we have their C
            continue
        L = open(p, errors='ignore').read().split('\n')
        for i in range(len(L) - 2):
            m1, m2, m3 = L1.match(L[i]), L1.match(L[i+1]), ADD.match(L[i+2])
            if m1 and m2 and m3 and m3.group(1) == m1.group(1) and m3.group(2) == m2.group(1):
                sym, num = m2.group(2), m1.group(2)
                # want: symbol loaded FIRST, plain number second (the ROM's order here)
                if not re.fullmatch(r'0x[0-9a-f]+|\d+', m1.group(2)) and \
                   re.fullmatch(r'0x[0-9a-f]+|\d+', m2.group(2)):
                    hits.append((c, L[i].strip(), L[i+1].strip(), L[i+2].strip()))
print(f"elevated TUs producing 'ldr =SYMBOL / ldr =CONST / add': {len(hits)}")
for c, a, b, d in hits[:10]:
    print(f"  {c}\n      {a} | {b} | {d}")
