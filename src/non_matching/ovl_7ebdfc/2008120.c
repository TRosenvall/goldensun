/* OvlFunc_961_2008120 -- NOT MATCHING. 2 of 48 lines, same length.
 *
 * Source asm: goldensun/asm/overlays/rom_7ebdfc/ovl_30_c_c_c_a.s
 *
 * TWIN: OvlFunc_961_2008194 in the same .s is instruction-identical, so this
 * park covers two functions and solving it elevates both.
 *
 * Blocker class: arg-interleave, straight-line -- the unreachable variety.
 *
 *     rom    mov r2,#0x10 / mov r0,#0 / mov r1,#0 / neg r2,r2 / bl __Func_80922c4
 *     ours   mov r2,#0x10 / neg r2,r2 / mov r0,#0 / mov r1,#0 / bl __Func_80922c4
 *
 * The ROM splits the mov/neg pair around the other two arguments. The
 * basic-block lever moves exactly this, but it needs a block that dominates the
 * call and holds none of the uses, and THIS FUNCTION HAS NO BRANCHES AT ALL --
 * so REG_BASIC_BLOCK (regno) < 0 in update_equiv_regs can never hold. See batch
 * 42's reading of local-alloc.c.
 *
 * TRIED:
 *   `n = -0x10;` assigned at the top, passed as the argument   54 lines, much
 *                                                              worse -- the
 *                                                              named local is
 *                                                              spilled
 *   __Func_80922c4's prototype withheld                        48 lines, but 4
 *
 * BATCH 94 -- WITHHOLDING THAT PROTOTYPE IS NOW THE BEST FORM, at 2 of 48, and
 * it is what the source below does. The line above recorded 4 when it was
 * written; the rest of the file has improved since and the same change now
 * lands `mov r1, #0` correctly too. What remains is one transposition:
 *
 *     rom   ... mov r0, #0 / neg r2, r2 / bl __Func_80922c4
 *     ours  ... neg r2, r2 / mov r0, #0 / bl __Func_80922c4
 *
 * Three spellings of the negative third argument were tried against it -- a
 * named `int n = -0x10`, `0 - 0x10`, and decimal `-16` -- and all three are
 * byte-identical at 2.
 *
 * This function is also the counterexample that softened batch 93's statement
 * of the prototype lever. That batch wrote it as a table: r0 EARLIER in the ROM
 * means ADD a declaration. Here the ROM puts r0 earlier and REMOVING the
 * declaration is what helps. The lever is real; its direction is not readable
 * off the ROM, so measure both.
 *                                                              instructions in
 *                                                              disagreeing
 *                                                              regions instead
 *                                                              of 2
 *
 * THREE THINGS IN THIS FUNCTION WERE FIXED AND ARE WORTH KEEPING, because the
 * body below is right about all of them and a future attempt should not undo
 * them:
 *
 *   THE TABLE POINTER IS A NAMED LOCAL. Written as the bare `.L5d0` symbol, the
 *   two `ldrsh` come out `[r3, r2]` with the OFFSET as base; the ROM has
 *   `[r2, r3]` with the table as base. Naming it swaps them. 3 of 48 to 2.
 *
 *   THE OFFSET IS A VARIABLE THAT ADVANCES. `off = idx << 2;` then `off += 2;`
 *   reproduces the ROM's `lsl r3, r1, #2 ... add r3, #2`. Indexing a
 *   two-short struct array instead is 10 of 48 and one instruction longer,
 *   because gcc folds the base in and re-materialises the 2.
 *
 *   THE TABLE ENTRIES ARE `short` READ INTO `unsigned short`. The ROM does
 *   `ldrsh` and then `lsl #16 / lsr #16`, which is a signed load zero-extended
 *   -- a short field assigned to an unsigned short variable, not a u16 field.
 *
 * BATCH 196 -- THE PIN DOES NOT REACH IT, measured in four forms. The pin
 * closed the structurally identical site in
 * src/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_a_c_b.c (also a mov/neg
 * pair split around another argument), so it was the obvious thing to try:
 *
 *     all three argument registers pinned, ROM assignment order   3 (WORSE)
 *     r0 and r2 pinned, r1 left a literal                         2
 *     r0 pinned alone                                             2
 *     all three pinned, the neg moved before the two zeros        2
 *     r1 and r2 pinned, r0 left a literal                         3 (WORSE)
 *
 * WHY IT DIFFERS FROM THE SITE THAT FELL. There the interleaved argument was
 * `mov r1, #2` -- a DISTINCT value, with the mov/neg pair on r2 to order it
 * against. Here the two interleaved arguments are r0 and r1 and BOTH RECEIVE
 * ZERO, and neither has a consumer of any kind; only r2 has the neg. So there
 * is no operation anywhere whose order the source can set to place them, which
 * is the batch-195 rule seen from its other side: mov order follows the order
 * of consuming operations, and a mov with NO consumer has nothing to follow.
 *
 * That makes this a genuinely different sub-case from the one the pin closed,
 * and the distinction is cheap to check before spending screens: does each
 * interleaved argument have an operation consuming it?
 *
 * NEXT: nothing at the source level. This is one instruction on the wrong side
 * of a mechanism that is settled.
 */
extern unsigned char *iwram_3001ebc;
extern unsigned char L5d0[] __asm__(".L5d0");
extern unsigned char L5e8[] __asm__(".L5e8");
extern void __PlaySound(int id);
extern void __Func_8010560(unsigned char *p, int a, int b);
extern void __Func_8091e9c(int a);

void OvlFunc_961_2008120(void)
{
    unsigned char *tbl;
    int idx;
    unsigned int off;
    unsigned short x;
    unsigned short y;

    idx = *(short *)(iwram_3001ebc + (0xb6 << 1));
    tbl = L5d0;
    off = idx << 2;
    x = *(short *)(tbl + off);
    off += 2;
    y = *(short *)(tbl + off);
    __PlaySound(0x9e);
    __Func_8010560(L5e8, x, y);
    __Func_80922c4(0, 0, -0x10);
    *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x10;
    __Func_8091e9c(idx);
}
