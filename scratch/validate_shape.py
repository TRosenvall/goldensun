import os, re
L1 = re.compile(r'^\tldr\t(r\d+), =(\S+)$')
ADD = re.compile(r'^\tadd\t(r\d+), (r\d+)$')
NUM = re.compile(r'0x[0-9a-fA-F]+|\d+')

elevated = scanned = anypat = symfirst = 0
for root, _, files in os.walk('asm'):
    for fn in files:
        if not fn.endswith('.s'): continue
        p = os.path.join(root, fn)
        c = p.replace('asm/', 'src/', 1)[:-2] + '.c'
        has_c = os.path.exists(c)
        elevated += has_c
        scanned += 1
        L = open(p, errors='ignore').read().split('\n')
        for i in range(len(L) - 2):
            m1, m2, m3 = L1.match(L[i]), L1.match(L[i+1]), ADD.match(L[i+2])
            if m1 and m2 and m3 and m3.group(1) == m1.group(1) and m3.group(2) == m2.group(1):
                anypat += 1
                if not NUM.fullmatch(m1.group(2)) and NUM.fullmatch(m2.group(2)):
                    symfirst += 1
print(f".s files scanned                 : {scanned}")
print(f"  of which have a mirrored .c    : {elevated}")
print(f"two-ldr+add triples anywhere     : {anypat}")
print(f"  ...with SYMBOL first, num 2nd  : {symfirst}")
# control: the park's own function must contain the shape
t = open('scratch/OvlFunc_923_20091b4.s', errors='ignore').read().split('\n')
found = any(L1.match(t[i]) and L1.match(t[i+1]) and ADD.match(t[i+2]) for i in range(len(t)-2))
print(f"control: shape present in OvlFunc_923_20091b4.s : {found}")
