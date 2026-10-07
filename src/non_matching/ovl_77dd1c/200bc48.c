/*
 *
 * NON-MATCHING, 41 of 55 encodings  (MEASURED, batch 319 recipe backfill).
 *   RELOCATIONS differ, but THE SAME SYMBOLS AT A SHIFTED OFFSET -- which
 *   this project treats as a CONSEQUENCE of the length difference, not a
 *   separate blocker.  Re-classified in batch 322; the figure IS a distance.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_77dd1c/200bc48.c \
 *     asm/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_a_a_c_c_c_c_b.s --func OvlFunc_882_200bc48
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 * OvlFunc_882_200bc48 -- asm/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_a_a_c_c_c_c_b.s
 * SPLIT OUT this round; byte-neutral, verified.
 *
 * BLOCKER: gcc CSEs a constant the ROM rebuilds. 54 lines against 53 -- ONE
 * OVER, and the one is `push {r5}` with its pop, because gcc hoists
 * `0xb3 << 1` into r5 to survive the calls between the __SetFlag and the
 * __ClearFlag twenty instructions later. The ROM emits `mov r0,#0xb3 /
 * lsl r0,#1` at both sites.
 *
 * SEE the corrected section in docs/elevation.md before attacking this. The
 * shape IS reachable -- 76 matching functions in the tree do it -- but in every
 * one checked the repeated uses sit in different conditional branches, and this
 * function is straight-line in the ROM with no branch at all. So either the
 * original had control flow that the call trace does not reveal, or the
 * register pressure differs. It is NOT established that two symbols are
 * required, and one symbol used twice CSEs exactly like a literal.
 *
 * The rest of the function is exact: 22 calls, the six-times-repeated channel
 * loops written out rather than looped (a loop gives an induction variable the
 * ROM does not have), and 0x166 built as `0xb3 << 1` rather than pooled.
 *
 * ===== BATCH 334: THE EXTRA INSTRUCTION, THE PASS, AND A ONE-PIN ZERO =====
 *
 * The figure above reproduced exactly.  DIRECTION: thirty-one instructions for
 * the reference against THIRTY-TWO for us -- WE EMIT AN EXTRA ONE.  aligncmp
 * puts the whole delta in five hunks and the extra instruction is the `pop`
 * ALONE, not "push {r5} with its pop" as this header said above: `push {lr}`
 * and `push {r5, lr}` are one instruction each, so the hoist is plus two to
 * build the value, minus two because each site collapses to one `adds`, and
 * PLUS ONE for the `pop {r5}`.
 *
 * WHY THE HOIST IS UNAVOIDABLE AT SOURCE LEVEL.  0x166 is 358.
 * `arm.c:2075-2082` gives `rtx_cost (const_int, SET)` as COSTS_N_INSNS(2) for a
 * `thumb_shiftable_const`, and 358 is 0xb3 << 1, so its cost is six.
 * `calls.c:841-857` calls `copy_to_mode_reg` for any argument whose
 * `rtx_cost (value, SET) > 2` when `SMALL_REGISTER_CLASSES && *reg_parm_seen`.
 * SO THE ARGUMENT ALWAYS GETS A FRESH PSEUDO; no spelling of this value reaches
 * r0 directly.  A constant below 256 would cost zero and go straight to the hard
 * register.  CHECK THE VALUE before attacking this class.
 *
 * NAME THE PASS: this is cse1, NOT gcse.  The function has no control flow at
 * all, so it is ONE basic block; cse's `invalidate_for_call` clears HARD
 * registers at a call and leaves pseudos alone, which is why fifteen intervening
 * calls do not break the equivalence, and why this header's own survey was right
 * that all seventy-six tree functions doing the rebuild have their repeated uses
 * in different conditional branches.  A real basic block boundary is the only
 * thing that stops it, and this function has none to offer.
 *
 * THE CORPUS ANSWERED IT, AND THE ANSWER WAS IN A LANDED FILE.  Querying every
 * gcc-generated reference for a `mov rN,#imm / lsl rN,rN,#s` pair in r0-r3 whose
 * value is at or above 256, appearing twice in one function with a call between
 * and NO label or conditional branch between, returns sixty-six hits.  The
 * closest is OvlFunc_888_200987c -- `mov r0,#154 / lsl r0,r0,#1` rebuilt twice
 * across twenty-nine calls, straight-line, feeding the same callee both times.
 * Its landed source is
 *
 *     { PIN1; q0 = 0x9a << 1; __PlaySound(q0); }
 *
 * at the FIRST site with a plain literal at the second, where PIN1 is
 * `register int q0 __asm__("r0")`.  That file's own header even states the rule:
 * every one of its sixty surviving pins is a site with a SHIFTED or pooled
 * argument, which is precisely this case.
 *
 * MEASURED HERE, and it is a landing behind one pin:
 *   PIN1 on the __SetFlag site       IDENTICAL -- 156 bytes, stream exact
 *   PIN1 on both sites               IDENTICAL
 *   PIN1 on the __ClearFlag site     INERT, at the park figure
 *   the two sites spelled differently (0xb3 << 1 against 0x166)   INERT
 *   both sites spelled 0x166                                      INERT
 *
 * The __ClearFlag-only row is the mechanism's own proof: the pin works by
 * putting the constant in HARD REGISTER r0 at the FIRST site, where
 * `invalidate_for_call` then kills the equivalence; pinning the second site
 * changes nothing because the first still leaves a live pseudo.
 *
 * SO THE PARK STANDS AT ITS PIN-FREE FIGURE, with a one-pin zero recorded.
 * This is a FOR-THE-OWNER call under the prefer-pin-free policy: ONE pin, on a
 * previously pin-free SINGLE-FUNCTION piece, with a landed precedent for the
 * identical construct against the identical blocker -- materially easier than
 * Func_80979a4 in docs/owner-decisions.md, which was a fourth pin on a veneer.
 * It is NOT reachable by a symbol-table entry: 0x166 is 0xb3 << 1, so constraint
 * K of the SImode move matches ahead of the pool path and gcc can never pool it,
 * which fails the structural-impossibility test the same document sets.
 */
extern void __WaitFrames(int n);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __Func_80118c0(int n);
extern void __Func_80118a8(int n);
extern void __Func_8091200(int a, int b);
extern void __Func_8091254(int n);

void OvlFunc_882_200bc48(void)
{
    __WaitFrames(0x14);
    __SetFlag(0xb3 << 1);
    __Func_80118c0(0);
    __Func_80118c0(1);
    __Func_80118c0(2);
    __Func_80118c0(3);
    __Func_80118c0(4);
    __Func_80118c0(5);
    __Func_8091200(0x10003, 1);
    __Func_8091200(0x80 << 9, 2);
    __Func_8091254(1);
    __WaitFrames(0x78);
    __Func_8091200(0, 0);
    __Func_8091254(0x3c);
    __WaitFrames(0x3c);
    __ClearFlag(0xb3 << 1);
    __Func_80118a8(0);
    __Func_80118a8(1);
    __Func_80118a8(2);
    __Func_80118a8(3);
    __Func_80118a8(4);
    __Func_80118a8(5);
}
