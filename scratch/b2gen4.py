import os, itertools
os.makedirs("scratch/b2y", exist_ok=True)
src = open("scratch/b2w/w_fill_F_2.c").read()
head, rest = src.split("    do {\n", 1)
body, tail = rest.split("    } while (i <= 0x1f);\n", 1)

STMS = {
 's0': ["        q = (int *)e;", "        *q++ = 0;", "        *q++ = 0x40000400;", "        *q = 0xd4 << 8;"],
 's1': ["        q = (int *)e;", "        *q++ = 0;", "        *q++ = 0x40000400;", "        *q++ = 0xd4 << 8;"],
 's2': ["        q = (int *)e;", "        *q++ = 0;", "        q[0] = 0x40000400;", "        q[1] = 0xd4 << 8;"],
 's3': ["        q = (int *)e;", "        *q++ = 0;", "        *q++ = 0x40000400;", "        *q = 0xd4 << 8;", "        q = 0;"],
}
LOADS = {
 'lxly': ["        x = w[0];", "        y = w[2];", "        e->fc = x;", "        e->f14 = y;"],
 'lylx': ["        y = w[2];", "        x = w[0];", "        e->fc = x;", "        e->f14 = y;"],
 'lylx2': ["        y = w[2];", "        x = w[0];", "        e->f14 = y;", "        e->fc = x;"],
}
W = "        w = *iwram_3001e70;"
CALL = "        e->f10 = _Func_8011f54(0, x >> 16, y >> 16) << 16;"
TAILB = ["        e->f1c = (i & 0xf) + 1;", "        i += 1;", "        e = e + 1;"]

n=0
for sn, st in STMS.items():
    for ln, ld in LOADS.items():
        for pos in range(3):
            if pos==0: b=[W]+st+ld
            elif pos==1: b=st+[W]+ld
            else: b=[W]+st[:1]+[st[1]]+st[2:]+ld
            b=b+[CALL]+TAILB
            open("scratch/b2y/y_%s_%s_%d.c"%(sn,ln,pos),"w").write(head+"    do {\n"+"\n".join(b)+"\n    } while (i <= 0x1f);\n"+tail)
            n+=1
print(n)
