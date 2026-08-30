import os, itertools
os.makedirs("scratch/b2x", exist_ok=True)
src = open("scratch/b2w/w_fill_F_2.c").read()
head, rest = src.split("    do {\n", 1)
body, tail = rest.split("    } while (i <= 0x1f);\n", 1)

STM = ["        q = (int *)e;",
       "        *q++ = 0;",
       "        *q++ = 0x40000400;",
       "        *q = 0xd4 << 8;"]
LX = "        x = w[0];"
LY = "        y = w[2];"
SX = "        e->fc = x;"
SY = "        e->f14 = y;"
CALL = "        e->f10 = _Func_8011f54(0, x >> 16, y >> 16) << 16;"
TAILB = ["        e->f1c = (i & 0xf) + 1;",
         "        i += 1;",
         "        e = e + 1;"]
W = "        w = *iwram_3001e70;"

seqs = []
for order in itertools.permutations([('lx',LX),('ly',LY),('sx',SX),('sy',SY)]):
    names = [o[0] for o in order]
    if names.index('lx') > names.index('sx'): continue
    if names.index('ly') > names.index('sy'): continue
    seqs.append((names, [o[1] for o in order]))

n=0
for names, lines in seqs:
    for pos in range(3):
        # pos: 0 = W then STM then lines; 1 = W then lines then STM; 2 = STM then W then lines
        if pos==0: b = [W] + STM + lines
        elif pos==1: b = [W] + lines + STM
        else: b = STM + [W] + lines
        b = b + [CALL] + TAILB
        name = "x_%s_%d" % ("".join(names), pos)
        open("scratch/b2x/%s.c" % name, "w").write(head + "    do {\n" + "\n".join(b) + "\n    } while (i <= 0x1f);\n" + tail)
        n+=1
print(n)
