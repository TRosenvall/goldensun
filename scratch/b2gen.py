import itertools, os
HDR = '''#include "dma.h"

struct Ent {
    int f0;
    int f4;
    int f8;
    int fc;
    int f10;
    int f14;
    unsigned char pad18[0x1c - 0x18];
    short f1c;
    unsigned char pad1e[0x20 - 0x1e];
};

extern int **iwram_3001e70;
extern unsigned char Data_a001e[];

extern unsigned char *galloc_ewram(int tag, int size);
extern void DecompressLZ1(void *src, void *dst);
extern int AllocSpriteSlot(void);
extern int UploadSpriteGFX(int slot, int size, void *gfx);
extern void gfree(int tag);
extern int _Func_8011f54(int a, int b, int c);
extern int StartTask(void *f, int pri);
extern void Task_Snow(void);

void StartSnow(void)
{
'''
DECLS = {
 'p':'    unsigned char *p;',
 'g':'    unsigned char *g;',
 'e':'    struct Ent *e;',
 'i':'    unsigned int i;',
 't':'    int t;',
 'w':'    int *w;',
 'q':'    int *q;',
 'x':'    int x;',
 'y':'    int y;',
 'c1':'    int c1;',
 'c2':'    int c2;',
}
BODY = '''
    p = galloc_ewram(0x1d, 0x82 << 3);
    e = (struct Ent *)(p + 8);
    DMA3_CLEAR(p, 0x82 << 3);
    g = galloc_ewram(0xe, 0x80 << 3);
    DecompressLZ1(Data_a001e, g);
    t = AllocSpriteSlot();
    *(int *)p = t;
    *(int *)(p + 4) = UploadSpriteGFX(t, 0xc0 << 2, g);
    gfree(0xe);
    i = 0;
    do {
%(loop)s
    } while (i <= 0x1f);
%(tail)s
    StartTask(Task_Snow, 0xc8 << 4);
}
'''
def gen(name, order, loop, tail):
    s = HDR + "\n".join(DECLS[k] for k in order) + "\n" + BODY % dict(loop=loop, tail=tail)
    open("scratch/b2v/%s.c" % name, "w").write(s)

LOOPA = """        w = *iwram_3001e70;
        q = (int *)e;
        *q++ = 0;
        *q++ = 0x40000400;
        *q = 0xd4 << 8;
        x = w[0];
        y = w[2];
        e->fc = x;
        e->f14 = y;
        e->f10 = _Func_8011f54(0, x >> 16, y >> 16) << 16;
        e->f1c = (i & 0xf) + 1;
        i += 1;
        e = e + 1;"""
LOOPB = """        q = (int *)e;
        w = *iwram_3001e70;
        *q++ = 0;
        *q++ = 0x40000400;
        *q = 0xd4 << 8;
        x = w[0];
        y = w[2];
        e->fc = x;
        e->f14 = y;
        e->f10 = _Func_8011f54(0, x >> 16, y >> 16) << 16;
        e->f1c = (i & 0xf) + 1;
        i += 1;
        e = e + 1;"""
LOOPC = """        w = *iwram_3001e70;
        q = (int *)e;
        *q++ = 0;
        *q++ = 0x40000400;
        *q = 0xd4 << 8;
        e->fc = w[0];
        e->f14 = w[2];
        e->f10 = _Func_8011f54(0, w[0] >> 16, w[2] >> 16) << 16;
        e->f1c = (i & 0xf) + 1;
        i += 1;
        e = e + 1;"""
LOOPD = """        w = *iwram_3001e70;
        q = (int *)e;
        *q++ = 0;
        *q++ = 0x40000400;
        *q = 0xd4 << 8;
        x = w[0];
        y = w[2];
        e->fc = x;
        x >>= 16;
        e->f14 = y;
        y >>= 16;
        e->f10 = _Func_8011f54(0, x, y) << 16;
        e->f1c = (i & 0xf) + 1;
        i += 1;
        e = e + 1;"""
TAIL1 = """    c1 = 0xfc << 6;
    c2 = 0x1008;
    REG_BLDCNT = c1;
    REG_BLDALPHA = c2;
    REG_BLDY = 0;"""
TAIL2 = """    c1 = 0xfc << 6;
    REG_BLDCNT = c1;
    c2 = 0x1008;
    REG_BLDALPHA = c2;
    REG_BLDY = 0;"""

import sys
loops = {'A':LOOPA,'B':LOOPB,'C':LOOPC,'D':LOOPD}
tails = {'1':TAIL1,'2':TAIL2}
base = ['p','g','e','i','t','w','q','x','y','c1','c2']
n=0
for ln,lb in loops.items():
    for tn,tb in tails.items():
        for perm in itertools.permutations(['w','q','x','y']):
            order = ['p','g','e','i','t'] + list(perm) + ['c1','c2']
            gen("v_%s%s_%s" % (ln,tn,"".join(perm)), order, lb, tb)
            n+=1
print(n)
