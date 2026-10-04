/* Func_8096b88 (SetEntityVisible) -- 0x08096b88.  EXACT.
 * ref: asm/rom_8a000/rom_944ec_a_c_c_a_c.s  (ONE function, no data section --
 *      grep -c thumb_func_start = 1; converts the WHOLE FILE, no split).
 * install: src/rom_8a000/rom_944ec_a_c_c_a_c.c   (path is free)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_8a000/rom_944ec_a_c_c_a_c.c \
 *     asm/rom_8a000/rom_944ec_a_c_c_a_c.s --whole
 *
 * batch 322, brief D, target 2.  PIN-FREE.  No shims, no .equ, no volatile,
 * no do{}while(0), default flags.  datacheck/split_s: not applicable.
 *
 * THE PARK'S FIGURE (7 of 48) WAS RIGHT AND ITS COUNT WAS RIGHT.  It closed in
 * TWO STEPS, each from a different source, and NEITHER step is visible on its
 * own -- the first is worth 2 and the second 4, and the second's three
 * placements measure 0 / 2 / inert.
 *
 * STEP 1 -- TWO VARIABLES AND ONE READ, FROM A LANDED SIBLING IN THIS BANK.
 * The park's central claim was that the count is read TWICE in the source:
 * `if (o[0x27] != 0) { n = o[0x27]; ...}`, because a single read is one
 * instruction short.  THAT IS HALF RIGHT AND THE WRONG HALF IS DECISIVE.
 *
 * I found the shape by scanning GENERATED asm (denominator printed: 4,418
 * generated .s files under asm/ that have a sibling .c under src/; 11 carry the ROM's
 * `mov rN, <hi> / add rN, rN, #imm / ldrb rN, [rN]` register-reuse shape, and
 * three of those are at THIS offset, #39 = 0x27).  The closest is
 * src/rom_8a000/rom_92950_a_a_c_a_a_c.c -- same bank, same subsystem, same
 * loop -- and it spells the count with **TWO VARIABLES AND A SINGLE READ**:
 *
 *     cnt = o[0x27];            <- outside the guard
 *     if (cnt != 0) { ... n = cnt; ... }
 *
 * and its generated asm has the ROM's `ldrb r3, [r3]` AND the ROM's copy
 * (`mov r0, r3` there, `adds r6, r3, #0` here).  So the copy does NOT need a
 * second read; it needs a SECOND VARIABLE.  The park's one-variable test
 * (`n = o[0x27]; if (n != 0)`) coalesces the copy away -- which is the
 * instruction it was short, measured here at 31 of 48 on 46 instructions --
 * and it never tried two.
 *
 * WHY THE DOUBLE READ COST THE REGISTER.  `.18.greg` on the park body prints
 *
 *     ;; 51 conflicts: 33 51 53 3 13
 *
 * for the `o + 0x27` address (`reg 51`), and `3` there is HARD r3.  With two
 * reads the address is live from bb3 into bb4 (`;; End of basic block 3,
 * registers live: 33 51`, and the second load in bb4 carries
 * `REG_DEAD (reg 51)`), so it overlaps `reg 53`, the loaded byte, which
 * local-alloc put in r3.  r3 is therefore unavailable and `REG_ALLOC_ORDER`
 * {3,2,1,0,...} gives it r2 -- our `mov r2, r8 / add r2, #0x27 / ldrb r3, [r2]`.
 * With one read the address dies AT the load, there is no conflict, and
 * address and value share r3: the ROM's `ldrb r3, [r3]`.  (The second load
 * does become a register copy in the end -- `reload_cse_regs` does it -- but
 * that happens AFTER allocation, too late to recover the register.)
 * This step alone: 7 -> 5.  Declaration position of `cnt` is INERT (all three
 * positions measured, 5 every time).
 *
 * STEP 2 -- AN INSN_LUID ORDER, DERIVED FROM haifa-sched.c AND THEN BUILT.
 * The last four were a sched2 permutation in the loop preheader plus the
 * reload register that follows from it:
 *
 *     rom    ldr r1,[pc] / mov r7,r8 / mov sl,r1 / add r7,#0x28 / add r6,r3,#0
 *     ours   ldr r1,[pc] / mov r7,r8 / add r6,r3,#0 / add r7,#0x28 / mov sl,r1
 *
 * `.23.sched2`'s visualisation gives the decision points exactly: at t=2 the
 * ready list is {sl=r1, r6=r3} and at t=3 {sl=r1, r6=r3, r7+=0x28}.  Reading
 * `rank_for_schedule` (haifa-sched.c) on those: `sl=r1` has NO in-block
 * dependent (its consumer is in the loop body, another block) so its PRIORITY
 * is 0 like both rivals; `insn_cost` is 1 on the `mov r7,r8` -> `add r7,#0x28`
 * edge so the CLASS rung ties all three at 3; every dependent count is 0.
 * **So all four rungs tie and INSN_LUID -- plain RTL order -- decides, and
 * lower LUID wins.**  The ROM's order requires LUID(sl=r1) < LUID(list) <
 * LUID(n).
 *
 * THAT IS WHY THE ALIAS-SET LEVER CANNOT REACH THIS ONE, and it is worth
 * saying precisely.  Batch 321's refinement is that where the residue is a
 * register copy the edge still works by RAISING THE RIVAL CHAIN'S PRIORITY.
 * Here there is no rival chain to raise: the copy's consumer is in a different
 * basic block, so no dependence of any kind -- alias set 0 or otherwise -- can
 * give `sl=r1` an in-block dependent, and priority is pinned at 0 for all
 * three insns no matter what memory is typed.  Measured, from the 5-figure
 * body: a union (alias set 0) store at q+5, 25; a union store at o+0x25,
 * exactly inert; a union read of iwram_3001e40, exactly inert; a volatile read
 * of it, exactly inert.  **The lever is live in general and it is dead when
 * the tie is settled by LUID.**
 *
 * SO I MOVED THE LUID.  The address of `iwram_3001e40` reaches the preheader
 * as a loop-invariant hoist, and loop.c emits every hoist immediately before
 * NOTE_INSN_LOOP_BEG -- i.e. ALWAYS LAST in the preheader, which is why it
 * always loses the LUID tie.  Giving the address a NAMED POINTER makes it an
 * ordinary source statement whose position I choose, and the order the
 * derivation asked for is the one that lands:
 *
 *     src = &iwram_3001e40;  then list  then n   -> 0
 *     src = &iwram_3001e40;  then n     then list -> 2
 *     list; src = &iwram_3001e40; then n          -> 2
 *     n; list; then src = &iwram_3001e40          -> 4 (exactly inert)
 *     src = &iwram_3001e40 at the top of the function -> 4 (exactly inert)
 *
 * Two of five placements are exactly inert, one lands, and the two figures
 * either side of the landing are 2.  The idx-27/28 reload register
 * (`mov r2, sl` against `mov r3, sl`) goes with it, so it was never an
 * independent residue.
 *
 * PARK CLAIMS REPRODUCED AND KEPT: the `and r3, r2` that tests bit 0 of
 * o[0x1d] reusing the +0x54 byte as the constant 1 falls out of a plain
 * `o[0x1d] & 1` (first 16 instructions match without help); the second
 * annotated argument is never read, so the C takes one parameter.
 * PARK CLAIMS REFUTED: "the count is read TWICE in the source" (two variables,
 * one read); "initialising `list` before `n` ... made it worse, 7 to 8" -- it
 * is worth -1 on the park body and worth nothing once step 2 is in, where
 * n-before-list is required (list-before-n measures 5 against 4).
 * PARK CLAIM CONFIRMED AS STILL TRUE: the struct-typed form that closed the
 * family member Func_808e0b0 does not transfer.
 */
extern unsigned int iwram_3001e40;

void Func_8096b88(unsigned char *e)
{
    unsigned char *o;
    unsigned char **list;
    unsigned char *q;
    unsigned int *src;
    int cnt;
    int n;

    if (*(unsigned char *)(e + 0x54) != 1)
        return;
    o = *(unsigned char **)(e + 0x50);
    if (o == 0)
        return;
    if (o[0x1d] & 1)
        return;
    cnt = o[0x27];
    if (cnt != 0) {
        src = &iwram_3001e40;
        list = (unsigned char **)(o + 0x28);
        n = cnt;
        do {
            q = *list++;
            q[5] = *src % 6;
            n--;
        } while (n != 0);
    }
    o[0x25] = 1;
}
