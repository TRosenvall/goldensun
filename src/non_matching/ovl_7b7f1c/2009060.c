/* OvlFunc_930_2009060  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_c_c_c.s
 * Best screen: 11 instructions in disagreeing regions, of 25 (streams same length).
 *
 * BLOCKER CLASS: register allocation, plus operand order on the ORR.
 *
 * The function is a two-armed read-modify-write on actor byte 0x23: one arm
 * ORs in 2, the other ANDs with 0xfd, and both join at a single strb. Each ROM
 * arm recomputes `a + 0x23` rather than hoisting it, which the source below
 * reproduces by assigning `p` inside each arm.
 *
 * WHAT WAS TRIED
 *
 *  1. `v = 2 | t;` / `v = 0xfd & t;` -- the read-modify-write spelling that
 *     fixed Field_Growth_Target in batch 57.  11 of 25.  gcc emits
 *     `mov r3, #0x2 / orr r2, r3`; the ROM has `mov r3, #0x2 / orr r3, r2`,
 *     i.e. the CONSTANT is the destination.
 *  2. `v = 2; v |= t;` -- compound assignment onto the constant, the form that
 *     makes the constant the destination elsewhere.  WORSE, 13 of 25.  It
 *     fixes nothing here and perturbs the surrounding allocation.
 *
 * The residue after either attempt is that the ROM walks the pointer in r1
 * while gcc uses r0, and every dependent instruction inherits the difference.
 * That is allocation, not spelling: r0 holds the incoming parameter and gcc
 * has no reason to move it.  Consistent with the note in
 * src/non_matching/ovl_7ed0a0/2009458.c, the fully-spelled read-modify-write
 * is not a general lever -- it helps in one function and hurts in another.
 */
extern void *__MapActor_GetActor(int slot);

void OvlFunc_930_2009060(void *actor)
{
    unsigned char *a;
    unsigned char *p;
    int t;
    int v;

    a = (unsigned char *)actor;
    if (*(int *)((unsigned char *)__MapActor_GetActor(0) + 0xc) > *(int *)(a + 0xc)) {
        p = a; p += 0x23; t = *p; v = 2 | t;
    } else {
        p = a; p += 0x23; t = *p; v = 0xfd & t;
    }
    *p = v;
}
