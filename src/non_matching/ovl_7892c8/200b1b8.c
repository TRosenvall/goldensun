/* OvlFunc_888_200b1b8 -- 0x0200b1b8  [asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_c_a_c_c.s]
 *
 * NOT MATCHING, but the long-standing diagnosis was WRONG and is now corrected.
 * Best 74 lines against the ROM's 75, 17 differing, 176 bytes against 184.
 * It sat at 79 lines and 74 differing for three batches.
 *
 * Spawns a debris actor at a randomised offset from a map actor, gives it two
 * randomised timers and a callback, and merges two flag bits into the new
 * actor's sprite byte.
 *
 * WHAT THE PARK USED TO SAY, AND WHY IT WAS WRONG. It asserted the blocker was
 * that gcc needs FIVE callee-saved registers where the ROM needs four, and that
 * the ROM covers five values with four by SHARING r5 between two values of
 * different types. The sharing observation is true, but it was never the
 * blocker: gcc shares r5 quite happily. The fifth register was going to
 * something the park never looked at.
 *
 * THE ACTUAL BLOCKER WAS AN ADDRESS HOISTED ABOVE A CALL.
 *
 *     *(short *)(n + 0x64) = (__Random() % 0xa) + 5;
 *
 * 0x64 is past the halfword store's 5-bit scaled offset range, so the address
 * needs its own `mov`/`add`. gcc forms that address BEFORE evaluating the
 * right-hand side, which contains two calls -- so the address has to survive
 * them, and it takes a callee-saved register to do it. That was the fifth.
 *
 * THE FIX IS TO NAME THE RIGHT-HAND SIDE, which forces it to be evaluated into
 * a pseudo before the store address is formed:
 *
 *     r = (__Random() % 0x3c) + 0x1e;
 *     *(short *)(n + 0x66) = r;
 *
 * Doing this to the SECOND store alone took 79 lines -> 75, the ROM's length,
 * and 74 differing -> 52. GENERALISE IT: a store whose address needs an
 * explicit add, with a call in its right-hand side, will park that address in a
 * callee-saved register unless the value is named first.
 *
 * BUT DO NOT NAME BOTH. Naming the first store's right-hand side as well frees
 * r8 ENTIRELY -- 68 lines, 7 SHORT -- because the ROM genuinely spends r8 on a
 * value held across the calls. The two stores need opposite treatment, and that
 * is the whole shape of this function.
 *
 * A MEASUREMENT IS ONLY VALID IN THE SHAPE IT WAS TAKEN IN. This park recorded
 * pinning the map actor to r6 as WORSE (80 lines, 78 differing) and cited the
 * batch 210 hazard. That was measured BEFORE the hoist was fixed. With the
 * hoist gone the same pin is worth 16 differing (42 -> 26 in combination) and
 * is load-bearing. Re-measure a rejected lever after the shape changes.
 *
 * THE r5 PINS ARE NOW INERT AND HAVE BEEN REMOVED. The old best carried
 * `register ... q __asm__("r5")` and `register int x __asm__("r5")` to force
 * the sharing. With the hoist fixed, dropping both is byte-identical. Scaffolding
 * that was load-bearing under one diagnosis is dead weight under the next.
 *
 * THE POOLED ZERO IS A LITERAL, NOT A SYMBOL -- CLOSED, DO NOT RE-OPEN.
 * The ROM loads its second zero from the pool (`ldr r2, =0x0`) and holds it in
 * r8 across two call pairs, which is exactly const.sym's stated tell for a
 * named SYMBOL. Spelling it `(int)&_CONST_0` even LOOKS better by tryc's line
 * count (16 differing against 17). It is wrong. objcmp settles it: THE
 * REFERENCE OBJECT CARRIES NO RELOCATION AT THAT POOL WORD -- its relocations
 * are the six calls and OvlFunc_888_200b144, nothing else -- while the symbol
 * spelling adds an R_ARM_ABS32 _CONST_0 the ROM does not have.
 *
 * A SYMBOL HYPOTHESIS MUST BE CHECKED AGAINST THE REFERENCE'S RELOCATIONS, NOT
 * ITS LINE COUNT. tryc normalises pool loads to `=value`, so a symbol and a
 * literal look alike to it and the score can move the wrong way. This also
 * confirms from a second angle what src/non_matching/rom_15000/801965c.c
 * already records: a pooled zero is ordinary gcc output, and no _CONST_0
 * belongs in const.sym.
 *
 * WHAT IS LEFT -- 8 bytes, and both pieces are identified:
 *
 *  - THE ROM'S `b .L3264` IS NOT A REDUNDANT BRANCH. Its literal pool is
 *    emitted INSIDE the function, between the last block and the epilogue, so
 *    the branch jumps OVER the pool. Our pool goes elsewhere, so we have no
 *    branch and are 2 bytes plus a pool word short. tryc cannot judge this --
 *    it warns "the reference keeps its literal pool INSIDE the function" -- so
 *    objcmp or make compare is the authority.
 *  - A TAIL REGISTER SWAP, 4 lines: the ROM puts the masked flag byte in r2 and
 *    the sprite byte in r1, we have them the other way round. Four spellings of
 *    the two `&` expressions were measured (fused, split, either order): 16-22
 *    differing, none swapping them.
 *
 * MEASURED THIS ROUND (differ counts, rom 75 lines):
 *
 *     name 2nd store's RHS only                     75 lines, 52
 *     ... + pin map actor to r6                     76, 42
 *     ... + assign the zero just before its use     74, 26
 *     ... + name 1st store's RHS too                74, 17   <- this file
 *     ... with the r5 pins dropped                  74, 17   (identical)
 *     ... + `(int)&_CONST_0` for the zero           74, 16   (REFUTED by objcmp)
 *     name BOTH RHSs, no r6 pin                     68, 74   (r8 freed entirely)
 *     both zeros written bare, letting CSE pool     73, 42
 *     zero assigned at the top of the function      79, 78
 *     nested positive ifs instead of early returns  74, 17   (inert)
 *     tail's two masks computed in the other order  74, 22
 *
 * NEXT: the pool placement. It is a translation-unit-level property, so the
 * remaining 8 bytes may not be reachable by spelling this function alone.
 */
extern unsigned int __Random(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern unsigned char *__CreateActor(int id, int x, int y, int z);
extern void OvlFunc_888_200b144(void);

void OvlFunc_888_200b1b8(int slot)
{
    register unsigned char *a __asm__("r6");
    unsigned char *n;
    unsigned char *q;
    int x;
    int y;
    int t;
    int v;
    int z;
    int r;
    int u;

    a = __MapActor_GetActor(slot);
    if (a == 0)
        return;
    x = *(int *)(a + 8) + ((__Random() % 0x14) << 16) - 0xa0000;
    y = *(int *)(a + 0xc) + ((0xf & __Random()) << 16) - 0x80000;
    n = __CreateActor(0x8f << 1, x, y, *(int *)(a + 0x10));
    if (n == 0)
        return;
    q = *(unsigned char **)(n + 0x50);
    n[0x55] = 0;
    u = (__Random() % 0xa) + 5;
    *(short *)(n + 0x64) = u;
    z = 0;
    r = (__Random() % 0x3c) + 0x1e;
    *(short *)(n + 0x66) = r;
    *(void **)(n + 0x6c) = OvlFunc_888_200b144;
    q[0x26] = z;
    t = 0xc;
    t &= (*(unsigned char **)(a + 0x50))[9];
    v = -13;
    v &= q[9];
    q[9] = v | t;
}
