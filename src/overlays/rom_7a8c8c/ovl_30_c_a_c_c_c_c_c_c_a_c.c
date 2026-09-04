// fakematch
/* OvlFunc_922_2009b1c  --  0x02009b1c
 *
 * From goldensun/asm/overlays/rom_7a8c8c/ovl_30_c_a_c_c_c_c_c_c_a_c.s, which
 * held this function alone, so no split was needed.
 *
 * PARKED AT 4 OF 57. The ROM fills r0 BEFORE three split builds:
 *
 *     rom   mov r1 / mov r2 / mov r3 / mov r0, #0x16 / lsl r1 / lsl r2 / lsl r3
 *     ours  mov r1 / mov r2 / mov r3 / lsl r1 / lsl r2 / lsl r3 / mov r0
 *
 * A four-register call, so all four are pinned and assigned in the ROM's order.
 *
 * The park reasoned that __CreateActor is the first thing the function does, so
 * no branch precedes it and the dominating-block lever is unavailable. That is
 * correct about that lever; a pin does not need a preceding block.
 *
 * The park also recorded that the body itself was never in doubt -- structs and
 * code came from a solved twin -- which is why the residue was exactly the four
 * instructions above and nothing else.
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
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        register int q3 __asm__("r3");
        q1 = 0xf8;
        q2 = 0x80;
        q3 = 0x98;
        q0 = 0x16;
        q1 <<= 16;
        q2 <<= 12;
        q3 <<= 16;
        act = __CreateActor(q0, q1, q2, q3);
    }
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
