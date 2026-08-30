import os, itertools
os.makedirs("scratch/b2w", exist_ok=True)
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
extern void StartTask(void *f, int pri);
extern void Task_Snow(void);

void StartSnow(void)
{
    unsigned char *p;
    unsigned char *g;
    struct Ent *e;
    unsigned int i;
    int t;
    int *w;
    int *q;
    int x;
    int y;
    int c1;
    int c2;
%(extra)s
    p = galloc_ewram(0x1d, 0x82 << 3);
    e = (struct Ent *)(p + 8);
%(dma)s
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

DMAS = {
 'clear': ('', '    DMA3_CLEAR(p, 0x82 << 3);'),
 'fill' : ('', '    DMA3_FILL(p, 0, 0x82 << 3);'),
 'set0' : ('    unsigned int zero;', '    zero = 0;\n    DMA3_SET(&zero, p, 0x85000104);'),
 'set1' : ('    unsigned int zero;', '    DMA3_SET(&zero, p, 0x85000104);'),
 'set2' : ('    unsigned int zero;', '    zero = 0;\n    DMA3_SET(&zero, p, 0x85000000 | ((0x82 << 3) / 4));'),
}

LOOPS = {}
LOOPS['A'] = """        w = *iwram_3001e70;
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
LOOPS['E'] = """        w = *iwram_3001e70;
        q = (int *)e;
        *q++ = 0;
        *q++ = 0x40000400;
        *q = 0xd4 << 8;
        x = w[0];
        e->fc = x;
        y = w[2];
        e->f14 = y;
        e->f10 = _Func_8011f54(0, x >> 16, y >> 16) << 16;
        e->f1c = (i & 0xf) + 1;
        i += 1;
        e = e + 1;"""
LOOPS['F'] = """        w = *iwram_3001e70;
        q = (int *)e;
        *q++ = 0;
        *q++ = 0x40000400;
        *q = 0xd4 << 8;
        y = w[2];
        x = w[0];
        e->fc = x;
        e->f14 = y;
        e->f10 = _Func_8011f54(0, x >> 16, y >> 16) << 16;
        e->f1c = (i & 0xf) + 1;
        i += 1;
        e = e + 1;"""
LOOPS['G'] = """        w = *iwram_3001e70;
        x = w[0];
        y = w[2];
        q = (int *)e;
        *q++ = 0;
        *q++ = 0x40000400;
        *q = 0xd4 << 8;
        e->fc = x;
        e->f14 = y;
        e->f10 = _Func_8011f54(0, x >> 16, y >> 16) << 16;
        e->f1c = (i & 0xf) + 1;
        i += 1;
        e = e + 1;"""
LOOPS['H'] = """        q = (int *)e;
        *q++ = 0;
        *q++ = 0x40000400;
        *q = 0xd4 << 8;
        w = *iwram_3001e70;
        x = w[0];
        y = w[2];
        e->fc = x;
        e->f14 = y;
        t = _Func_8011f54(0, x >> 16, y >> 16);
        e->f10 = t << 16;
        e->f1c = (i & 0xf) + 1;
        i += 1;
        e = e + 1;"""
LOOPS['I'] = """        w = *iwram_3001e70;
        q = (int *)e;
        *q++ = 0;
        *q++ = 0x40000400;
        *q = 0xd4 << 8;
        x = *w;
        y = *(w + 2);
        e->fc = x;
        e->f14 = y;
        e->f10 = _Func_8011f54(0, x >> 16, y >> 16) << 16;
        e->f1c = (i & 0xf) + 1;
        i += 1;
        e = e + 1;"""
LOOPS['J'] = """        w = *iwram_3001e70;
        q = (int *)e;
        *q++ = 0;
        *q++ = 0x40000400;
        *q = 0xd4 << 8;
        x = w[0];
        y = w[2];
        e->fc = x;
        e->f14 = y;
        x >>= 16;
        y >>= 16;
        e->f10 = _Func_8011f54(0, x, y) << 16;
        e->f1c = (i & 0xf) + 1;
        i += 1;
        e = e + 1;"""
LOOPS['K'] = """        w = *iwram_3001e70;
        q = (int *)e;
        *q++ = 0;
        *q++ = 0x40000400;
        *q = 0xd4 << 8;
        x = w[0];
        y = w[2];
        e->f1c = (i & 0xf) + 1;
        e->fc = x;
        e->f14 = y;
        e->f10 = _Func_8011f54(0, x >> 16, y >> 16) << 16;
        i += 1;
        e = e + 1;"""

TAILS = {}
TAILS['2'] = """    c1 = 0xfc << 6;
    REG_BLDCNT = c1;
    c2 = 0x1008;
    REG_BLDALPHA = c2;
    REG_BLDY = 0;"""
TAILS['3'] = """    c1 = 0xfc << 6;
    REG_BLDCNT = c1;
    c2 = 0x1008;
    REG_BLDALPHA = c2;
    c1 = 0;
    REG_BLDY = c1;"""
TAILS['4'] = """    {
        vu16 *r = &REG_BLDCNT;
        c1 = 0xfc << 6;
        *r++ = c1;
        c2 = 0x1008;
        *r++ = c2;
        *r = 0;
    }"""

n = 0
for dn, (extra, dma) in DMAS.items():
    for ln, lb in LOOPS.items():
        for tn, tb in TAILS.items():
            open("scratch/b2w/w_%s_%s_%s.c" % (dn, ln, tn), "w").write(
                HDR % dict(extra=extra, dma=dma, loop=lb, tail=tb))
            n += 1
print(n)
