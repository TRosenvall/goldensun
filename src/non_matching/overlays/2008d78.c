/* OvlFunc_898_2008d78  --  0x02008d78, asm/overlays/rom_793768/ovl_314_c_c_c_a_a_c_a.s
 * OvlFunc_901_2008864  --  0x02008864, asm/overlays/rom_797990/ovl_314_c_c_a_a_c_c_a_c_c_a_a.s
 *
 * Source asm: goldensun/asm/overlays/rom_793768/ovl_314_c_c_c_a_a_c_a.s
 *
 * BLOCKER CLASS: commutative-operand canonicalisation.
 * Status: 32 lines against 32 and 24 against 24, TWO differing in each, and
 * both are the same two lines.
 *
 * The cutscene bookend: set bit 1 of the actor's flag halfword at +0x64, run a
 * cutscene, then clear the halfword on the way out. Four occurrences of this
 * prologue survive in the tree.
 *
 * THE WHOLE REMAINING DIFFERENCE:
 *
 *     rom    ldrh r2, [r0]  /  ldr r3, =2   /  orr r3, r2
 *     ours   ldr r2, =2     /  ldrh r3, [r0] /  orr r3, r2
 *
 * `orr rd, rs` is `rd |= rs`, so the ROM makes the CONSTANT the destination and
 * ORs the memory value into it; we make the memory value the destination. That
 * is the operand order of the source expression surviving into the register
 * allocation -- the ROM's tree had the constant first. gcc canonicalises a
 * commutative operator to put the constant second and there is no spelling that
 * undoes it.
 *
 * TRIED AND MEASURED, all 2 of 32:
 *
 *   *p |= 2                                  2
 *   *p = 2 | *p                              2
 *   *p = *p | 2                              2
 *   v = *p; *p = v | 2   (v unsigned short)  2
 *   v = *p; *p = v | 2   (v int)             2
 *   v = *p; *p = 2 | v                       2
 *   t2 = 2 assigned before the call, *p |= t2  2   (propagated)
 *   *p |= (unsigned short)2                  2
 *   t2 = 2, *p = t2 | *p                     8   worse, and 33 lines
 *
 * Flags: -fno-schedule-insns, -fno-schedule-insns2, -fno-gcse and
 * -fno-strict-aliasing all leave it at 2; -O1 gives 4. It is not the scheduler
 * -- turning both scheduling passes off changes nothing -- which places it in
 * the expander, where the canonical form is fixed.
 *
 * THREE THINGS WERE SOLVED GETTING HERE and all three are reusable:
 *
 *   TAKE THE ADDRESS, DO NOT KEEP THE STRUCT. `a = GetActor(0xf); a->f64 |= 2;`
 *   gives `mov r1, r0 / add r1, #0x64`, two instructions, because gcc keeps the
 *   actor pointer alive. `p = &GetActor(0xf)->f64; *p |= 2;` gives the ROM's
 *   single in-place `add r0, #0x64`. Offset 0x64 is past the halfword immediate
 *   range, so the address has to be materialised either way -- the question is
 *   only whether the base survives.
 *
 *   THE STORED ZERO IS A VARIABLE, NOT A LITERAL. `*p = 0` on a `u16` pools the
 *   zero as a HImode constant -- `ldr r3, =0x0`, which is real and wrong. The
 *   ROM has `mov r5, #0` with r5 pushed, the signature of a pseudo created
 *   before the calls and therefore given a callee-saved register. Declaring
 *   `int z = 0;` at the top and storing `*p = z;` reproduces it exactly,
 *   push included. Same pattern as OvlFunc_899_200c698 in batch 78.
 *
 *   THE ROM FALLS THROUGH TO THE FIRST MESSAGE. `cmp r0, #0 / bne` means the
 *   not-taken arm is the one written first, so the test is `== 0` with 0x123d
 *   in the `if` -- writing it the other way round swaps both message ids and
 *   the branch condition, three differences that look like three problems.
 *
 * Both functions are believed correct apart from the two lines above.
 */

struct A { unsigned char pad00[0x64]; unsigned short f64; };

extern struct A *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void OvlFunc_898_2008938(int slot);

void OvlFunc_898_2008d78(void)
{
    unsigned short *p;

    p = &__MapActor_GetActor(0xf)->f64;
    *p |= 2;
    __CutsceneStart();
    if (__GetFlag(0x855) == 0)
        __MessageID(0x123d);
    else
        __MessageID(0x134b);
    OvlFunc_898_2008938(0xf);
    __CutsceneEnd();
    p = &__MapActor_GetActor(0xf)->f64;
    *p &= 1;
}


/* --- the twin shape, same blocker, 2 of 24 --- */
#if 0
struct A { unsigned char pad00[0x64]; unsigned short f64; };

extern struct A *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void OvlFunc_901_20084b4(int slot);

void OvlFunc_901_2008864(void)
{
    unsigned short *p;
    int z;

    z = 0;
    p = &__MapActor_GetActor(0xf)->f64;
    *p |= 2;
    __CutsceneStart();
    __MessageID(0x1cc1);
    OvlFunc_901_20084b4(0xf);
    __CutsceneEnd();
    p = &__MapActor_GetActor(0xf)->f64;
    *p = z;
}
#endif
