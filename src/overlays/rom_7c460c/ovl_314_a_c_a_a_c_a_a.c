/* OvlFunc_939_20083f4  --  0x020083f4
 *
 * The whole of goldensun/asm/overlays/rom_7c460c/ovl_314_a_c_a_a_c_a_a.s, which
 * held this function and no data.
 *
 * Three map repaints, then -- if actor 8 is still around -- unhide it and set
 * two of its flag bytes, and record the scene with save bit 0x200.
 *
 * THE SECOND FLAG BYTE IS REACHED BY A `sub`, AND THAT IS WHAT PLAIN FIELD
 * WRITES GIVE. The ROM has `add r2, #0x55 / strb / sub r2, #0x32 / strb`,
 * walking one register backwards from +0x55 to +0x23 because 0x55 - 0x32 is
 * 0x23. Writing `a->f55 = 2; a->f23 = 1;` produces exactly that.
 *
 * Worth contrasting with src/non_matching/ovl_7aa430/2009bc8.c, where the ROM
 * keeps TWO independent pointer chains and the plain field writes are wrong.
 * The same C gives the derived form; which one the ROM wants has to be read off
 * the assembly, and here it wants the derived one.
 *
 * The second __MapActor_GetActor call discards its result -- it is there for
 * the side effect -- so it is written as a bare call.
 *
 * Matched on the first screen.
 */
struct A {
    unsigned char pad00[0x23];
    unsigned char f23;
    unsigned char pad24[0x55 - 0x24];
    unsigned char f55;
};

extern struct A *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Actor_SetSpriteFlags(struct A *a, int n);
extern void __SetFlag(int id);

void OvlFunc_939_20083f4(void)
{
    struct A *a;
    int f;

    a = __MapActor_GetActor(8);
    __MapActor_GetActor(0);
    f = 4;
    __Func_8010704(0x11, 4, 1, 1, 0xe, f);
    __Func_8010704(0xf, 3, 1, 1, 0xf, f);
    __Func_8010704(0xf, 3, 1, 1, 0xd, f);
    if (a != 0) {
        __Actor_SetSpriteFlags(a, 0);
        a->f55 = 2;
        a->f23 = 1;
    }
    __SetFlag(0x80 << 2);
}
