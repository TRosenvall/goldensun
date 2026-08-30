/* OvlFunc_922_2009b1c  [overlays/rom_7a8c8c]
 *
 * Source asm: goldensun/asm/overlays/rom_7a8c8c/ovl_30_c_a_c_c_c_c_c_c_a.s
 *
 * BLOCKER CLASS: arg-interleave at an UNGUARDED site. 4 of 57, one site.
 *
 *     rom    mov r0, #0x16 / lsl r1, #0x10 / lsl r2, #0xc / lsl r3, #0x10
 *     ours   lsl r1, #0x10 / lsl r2, #0xc  / lsl r3, #0x10 / mov r0, #0x16
 *
 * The ROM wants r0 filled BEFORE three split builds. __CreateActor is the
 * first thing the function does, so no branch precedes it and the
 * dominating-block lever is unavailable -- the same boundary as 200cf44 above.
 *
 * THE BODY IS NOT IN QUESTION. Structs and code come from the solved twin
 * src/overlays/rom_7987ac/ovl_30_c_c_c_b.c; only the __CreateActor arity, the
 * __LoadItemIcon literal and the trailing store differ.
 *
 * ONE LEVER DID FIRE, and it is worth keeping: leaving __UploadSpriteGFX
 * UNDECLARED removes three of the seven differing lines. That is the
 * documented "no prototype moves `mov r0` LATER" direction, and here it is
 * wanted at that site. It is retained in the C below.
 *
 * MEASURED: full prototypes 7; __UploadSpriteGFX undeclared 4; naming the
 * three shifted arguments at the top 4; __CreateActor undeclared 4; naming the
 * kind 4; --no-rerun-cse 4; --no-sched2 23.
 */
struct S {
    unsigned char pad00[5];
    unsigned char f5_lo : 5;
    unsigned char f5_b5 : 1;
    unsigned char f5_hi : 2;
    unsigned char pad06[3];
    unsigned char f9;
    unsigned char pad0a[0x12];
    unsigned char f1c;
    unsigned char pad1d[9];
    unsigned char f26;
    unsigned char f27;
};

struct A {
    unsigned char pad00[0x50];
    struct S *spr;
    unsigned char pad54;
    unsigned char f55;
    unsigned char pad56[6];
    unsigned char f5c;
};

extern struct A *__CreateActor(int kind, int x, int y, int z);
extern void *__galloc_iwram(int tag, int size);
extern void __LoadItemIcon(int id);

extern void __gfree(int tag);
extern struct A *gSlot __asm__(".L2488");

void OvlFunc_922_2009b1c(void)
{
    struct A *act;
    struct S *s;
    void *buf;
    int z;

    z = 0;
    act = __CreateActor(0x16, 0xf8 << 16, 0x80 << 12, 0x98 << 16);
    if (act == 0)
        return;
    s = act->spr;
    s->f26 = z;
    s->f27 = z;
    s->f5_b5 = 0;
    s->f9 &= 0xf;
    act->f55 = z;
    act->f5c = 1;
    buf = __galloc_iwram(0x11, 0xc1 << 3);
    __LoadItemIcon(0xe6);
    __UploadSpriteGFX(s->f1c, 0x80, (char *)buf + (0x80 << 3));
    __gfree(0x11);
    gSlot = act;
}
