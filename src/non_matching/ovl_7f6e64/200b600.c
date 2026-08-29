/* OvlFunc_969_200b600 -- NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_7f6e64/ovl_314_c_a_c_c_c_c_c_c_a.s
 * Best screen: 6 differing of 42, streams the same length.
 *
 * BLOCKER CLASS: instruction scheduling around a reload.
 *
 * An orbiting actor: its position is the anchor's plus cos/sin of an angle
 * kept at +0x64, and the angle then advances by -0x800.
 *
 * TWO LEVERS GOT IT FROM 10 TO 6:
 *
 *   THE MULTIPLY'S OPERAND ORDER. `mov r2, r3 / mul r2, r0` ties the
 *   destination to (f30 + 3), not to the cosine. Thumb's `mul rd, rs` is
 *   two-operand so one input must become the destination, and the SECOND
 *   source operand is the one that does: `__cos(ang) * (a->f30 + 3)` gives the
 *   ROM, `(a->f30 + 3) * __cos(ang)` gives the reverse. Same rule as batch 96's
 *   Func_80b8f08, confirmed a second time.
 *
 *   THE TWO MIRROR STORES GO f40 BEFORE f38 IN THE SOURCE. Writing them in the
 *   ROM's apparent order (f38 then f40) makes gcc sink the f38 store past the
 *   angle update; writing f40 first keeps it adjacent. 8 to 6.
 *
 * WHAT IS LEFT is one scheduling decision. The ROM reloads `a->f8` BEFORE it
 * finishes computing f10:
 *
 *     rom   ldr r2, [r5, #8] / add r3, r0 / str r3, [r5, #0x10]
 *           str r2, [r5, #0x38] / str r3, [r5, #0x40]
 *     ours  add r3, r0 / str r3, [r5, #0x10] / str r3, [r5, #0x40]
 *           ldr r3, [r5, #8] / ... / str r3, [r5, #0x38]
 *
 * The reload itself is right and is forced -- __sin sits between the write to
 * f8 and the read, so gcc cannot reuse the value. Only WHEN it is issued
 * differs, and with it which register holds it.
 *
 * ALSO TRIED, all worse: naming the reloaded value in a local (8); hoisting the
 * sin result into a local so the f38 store precedes the f10 assignment (7);
 * naming the reload with the stores swapped (8).
 */
struct A {
    unsigned char pad00[8];
    int f8;
    unsigned char pad0c[4];
    int f10;
    unsigned char pad14[0x30 - 0x14];
    int f30;
    unsigned char pad34[4];
    int f38;
    unsigned char pad3c[4];
    int f40;
};

extern struct A *__MapActor_GetActor(int slot);
extern int __cos(int a);
extern int __sin(int a);

void OvlFunc_969_200b600(struct A *a)
{
    struct A *b;
    unsigned short *p;
    int ang;

    b = __MapActor_GetActor(0x18);
    p = (unsigned short *)((char *)a + 0x64);
    ang = *p;
    a->f8 = b->f8 + __cos(ang) * (a->f30 + 3);
    a->f10 = b->f10 + (__sin(ang) << 1);
    a->f40 = a->f10;
    a->f38 = a->f8;
    *p = *p - 0x800;
}
