// fakematch
/* OvlFunc_959_2009528 -- EXACT under the tree's PRODUCTION flags, no Makefile row.
 * 296 bytes, 133 encodings and 13 relocations identical; --whole also OK.
 *
 * Supersedes the park's "132 of 133, exact only under CSE_CFLAGS".  The one-actor twin
 * of OvlFunc_959_2009150 takes the identical one-line shim: a hard-register pin on the
 * FIRST (dominating) use of the flag id 0x214, which is what stops cse2 commoning it
 * into a callee-saved register and adding the extra push.  See the mechanism write-up
 * in ovl_9dc_c_a_a_a_a_c_a.c (2009150) -- it is NOT path extension, cse1 commons the
 * pair too, and gcse's cprop is what restores the ROM's second literal.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/overlays/rom_7e7574/ovl_9dc_c_a_a_c_a_c.c \
 *     asm/overlays/rom_7e7574/ovl_9dc_c_a_a_c_a_c.s --whole
 *
 * SHIMS: one `register ... __asm__` declaration.  Zero `__asm__(".equ ...)` lines.
 * Needs a fakematch.txt row:
 *   OvlFunc_959_2009528  src/overlays/rom_7e7574/ovl_9dc_c_a_a_c_a_c.c
 *
 * The park's COMPILER CRASH NOTE still applies to the `off = 0x93 << 2` spelling and is
 * unrelated to the shim; the per-arm `g = gState` locals stay.
 */
struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x5b - 0x14];
    unsigned char f5b;
};

struct S {
    unsigned char pad00[0x18];
    int f18;
    int f1c;
    int f20;
    int f24;
};

extern unsigned char gState[];
extern unsigned char *iwram_3001e70[];
extern volatile int iwram_3001e40;
extern struct Actor *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern unsigned int OvlFunc_959_20094cc(void);
extern int OvlFunc_959_2009918(int slot);
extern unsigned int OvlFunc_959_20098e4(unsigned int slot);

void OvlFunc_959_2009528(void)
{
    struct Actor *a;
    struct S *s;
    unsigned char *b;
    unsigned char *g;
    int v;
    unsigned short *p;
    int w;

    a = __MapActor_GetActor(0x12);
    s = (struct S *)(iwram_3001e70[0] + (0xb2 << 1));
    b = iwram_3001e70[0x13];
    if (iwram_3001e40 & 1) {
        s->f18 = 1;
        s->f1c = 1;
    } else {
        s->f18 = -1;
        s->f1c = -1;
    }
    if (__GetFlag(0x83 << 1) != 0 || *(short *)(b + (0xbf << 1)) != 0
        || *(short *)(b + (0xc0 << 1)) != 0) {
        a->f5b = 1;
        return;
    }
    {
        register int k1 __asm__("r0") = 0x85 << 2;
        if (__GetFlag(k1) != 0)
            return;
    }
    a->f5b = 0;
    if (__GetFlag(0x85 << 2) == 0 && a->f5b == 0) {
        s->f20 = (0xbc << 18) - a->f8;
        s->f24 = (0xf8 << 17) - a->f10;
    }
    if (OvlFunc_959_20094cc() != 0)
        return;
    if (OvlFunc_959_2009918(0x12) != 0) {
        w = 0x2092;
        g = gState;
        if (*(short *)(g + (0x93 << 2)) != 0) {
            *(unsigned short *)(b + (0xbf << 1)) = w;
            return;
        }
    } else {
        g = gState;
        if (*(short *)(g + (0x93 << 2)) != 0)
            goto check;
    }
    if (OvlFunc_959_20098e4(0x12) != 0) {
        __SetFlag(0x215);
        __SetFlag(0x85 << 2);
    }
check:
    if (__GetFlag(0x85 << 2) != 0) {
        p = (unsigned short *)(b + (0xc1 << 1));
        v = 0x5d;
        *p = v;
    }
}
