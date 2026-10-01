/* Func_808bec0 -- RECON, NO CANDIDATE BODY, THEREFORE NO FIGURE.
 *
 * NO `NON-MATCHING, N of M` LINE AND NO `Verify with:` RECIPE, AND THIS TIME
 * THAT IS AN HONEST STATEMENT OF STATE RATHER THAN AN APPEAL TO A BLOCKER:
 * there is no .c body here to measure, because the function is 470 instructions
 * of ground-up reconstruction and this round did not write it.  parkcheck will
 * report this file UNCHECKABLE and that is correct.  DO NOT COPY THE PREVIOUS
 * PARK'S FRAMING -- that file declined a figure on the authority of the
 * `.call_via` structural-blocker section of docs/elevation.md, and that section
 * IS RETRACTED IN FULL further down the same document.  The function is
 * ACTIONABLE.  The only reason there is no number is that nobody has written
 * the body yet.
 *
 * Reference: asm/rom_8a000/rom_8ba38_a_a_a_c_a_c_a_a.s, 470 instructions.
 *
 * ============ THE VENEER: ONE SITE, AND IT IS A SOLVED SHAPE ============
 * Screened on the anchored pattern `^[[:space:]]*\.call_via` over the function's
 * own block: EXACTLY ONE site, and ZERO ordinary `bl _call_via_rN` beside it.
 *
 *     strh r0, [r3] / mov r3, #0xd8 / ldr r2, [sp, #0xc] / lsl r3, #1 /
 *     add r3, r8 / ldr r4, =Func_8000888 / ldr r0, [r3] / ldr r1, [r2, #0x30] /
 *     .call_via r4
 *
 * That is the EXACT shape `Func_8097a10` landed BYTE-EXACT on: one site, one
 * known callee, bound register r4.  The working template is
 * src/rom_8a000/rom_97384_c_c_a_b.c; for a single site, BIND THE SYMBOL INSIDE
 * THE HELPER (`register int (*_f)(int,int) __asm__("r4") = Func_8000888;`) rather
 * than passing it as a parameter -- passing it costs a `mov` the ROM does not
 * have.  Clobbers: `"memory"` and `"r12"` always; DROP `"lr"` (the ARM callee
 * returns through r12 and never writes lr); decide `"r2"` and `"r3"`
 * INDEPENDENTLY by reading whether the ROM parks a call-crossing value there.
 * Here the ROM loads BOTH arguments immediately before the veneer
 * (`ldr r0,[r3] / ldr r1,[r2,#0x30]`) and r2 holds a pointer reloaded from
 * sp+0xc for that very load, so start WITHOUT "r2" and measure.
 *
 * The veneer reproduces byte-exactly at object level (docs/elevation.md,
 * "CONFIRMED TWICE"): the `mov ip, pc`, the `.align` fill halfword and the pool
 * offset for the callee word all agree; the only residue ever seen there is
 * which register holds the callee, and binding it fixes that.  SO THE VENEER IS
 * NOT PART OF THIS FUNCTION'S DISTANCE AT ALL -- it is four bytes of already
 * solved shape, and whatever this function's residue turns out to be, it will be
 * the other 466 instructions.
 *
 * ============ SPLIT SHAPE, DRY-RUN VERIFIED THIS BATCH ============
 *   python3 tools/split_s.py --dry-run asm/rom_8a000/rom_8ba38_a_a_a_c_a_c_a_a.s \
 *       Func_808bec0
 * reports a clean TWO-WAY TAIL split:
 *     _a.s  1 function, 113 lines  (CheckSpecialExits, 0x0808bde0)
 *     _b.s  1 function, 519 lines  (Func_808bec0, 0x0808bec0)  <- the target
 * and rewrites stage1.ld.  tools/datacheck.py is silent: no code/data cut.
 * The target becomes src/rom_8a000/rom_8ba38_a_a_a_c_a_c_a_a_b.c.
 * CheckSpecialExits is CALLED BY the target, so link order must be preserved --
 * which the split does by construction.  ALWAYS --dry-run first; split_s.py
 * deletes a tracked .s.  Gate `make compare` on the split BEFORE writing any .c.
 *
 * ============ THE SPILL MAP *IS* THE DECLARATION LIST ============
 * Frame `sub sp, #0x2c` = 44 bytes, fully accounted:
 *     sp+0x1c .. sp+0x2b   16 bytes -- the short[8] party HP snapshot (AGGREGATE)
 *     sp+0x18  arg2 (`str r2, [sp, #0x18]` in the prologue)
 *     sp+0x14  sp+0x10  sp+0xc  sp+8  sp+4   five more word scalars
 *     sp+0     r4's spill slot, NOT A DECLARED LOCAL (see below)
 * Sorted DESCENDING the scalars give the declaration order directly:
 * arg2 first among the spilled ones, then the five locals; the single aggregate
 * sits highest, which is the "aggregates reversed" rule with one member.
 * arg0, arg1 and arg3 are never spilled (arg3 goes straight to r9), so they are
 * invisible in the map yet still numbered -- the map constrains the ORDER of the
 * spilled six, not the full list.
 *
 * ============ TWO FACTS A FUTURE ATTEMPT MUST NOT CHASE ============
 * 1. `-fcall-used-r4` IS SET FOR THIS FILE, so r4 is CALL-CLOBBERED and the ROM
 *    spills it around calls as `str r4, [sp] / bl ... / ldr r4, [sp]`, using the
 *    outgoing-argument word at sp+0 because no call in that region passes a
 *    fifth argument.  NOT A SOURCE FACT.
 *    CORRECTION TO THE PREVIOUS PARK, WHICH SAID "SIX SUCH PAIRS": there are
 *    FOUR.  `grep -c 'str	r4, \[sp\]'` = 4 and `grep -c 'ldr	r4, \[sp\]'` = 4
 *    over the function's own block.  A header caught miscounting again -- count
 *    it yourself before you build on it.
 * 2. THE DIVISIONS ARE ROUNDING PAIRS, NOT SHIFTS.  `if (v < 0) v += mask;
 *    v >>= shift` with (0x1fffff, 21), (0xfffff, 20), (0xffff, 16) and a
 *    `lsr #31 / add / asr #1` -- i.e. plain `/ 0x200000`, `/ 0x100000`,
 *    `/ 0x10000` and `/ 2` on SIGNED ints.  Write the divisions; do not write
 *    the shifts.  And do NOT hand-write the rounding pair in order to name the
 *    constant: on OvlFunc_923_200a030 every hand-written spelling cost 6
 *    encodings over the compiler's own division, and the reason is now known --
 *    the rounding constant is a CONST_INT operand of *thumb_addsi3 that RELOAD
 *    materialises, so there is no pseudo to name and nothing to place.
 *
 * ============ THE BODY, as traced; unchanged from the previous park ============
 * Four arguments (origin a, x b, range c, z d).  Snapshots every party member's
 * HP into the short[8] at sp+0x1c from `_GetUnit(gState[0x1f8 + i])->f38` over
 * `_GetPartySize()`; picks a collision cell either from `ewram_2020000` on a
 * 32x32 grid (`(b >> 21) & 0x1f` plus `((d >> 21) & 0x1f) << 5`, when
 * scene->f19e == 3) or from a 128-wide grid off `*(iwram_3001ebc - 0x4c)`
 * indexed `(b >> 20) + ((d >> 20) << 7)`; rotates scene->f1b8 into scene->f1bc;
 * calls CheckSpecialExits(b, c, d) when the cell's byte +2 is non-zero.  The
 * rest is damage/poison bookkeeping on gState at 0x22c/0x22e/0x230/0x232/0x23e/
 * 0x244, finishing with a second party sweep that fills scene->f188[] with the
 * ids of members whose f38 <= 0.  Callees, by count: MapActor_Surprise x3,
 * _GetUnit x2, then one each of _PlaySound, _GetPartySize, _GetFlag,
 * _Func_80bf5a8, UpdatePoison, Func_8091858, Func_808c30c, Func_808c2dc,
 * Func_808b048, Func_808b02c, CheckSpecialExits.
 *
 * NEXT: write the body.  It is an ordinary reconstruction with one solved
 * four-byte veneer in the middle, a clean two-way tail split and a spill map
 * that already hands you the order of six of the declarations.
 */
