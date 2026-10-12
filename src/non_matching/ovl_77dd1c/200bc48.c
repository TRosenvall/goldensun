/* OvlFunc_882_200bc48 -- asm/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_a_a_c_c_c_c_b.s
 *
 * NON-MATCHING, 41 of 55 encodings PIN-FREE, with a ONE-PIN ZERO recorded.
 *   MEASURED batch 344 brief E; reproduces batch 319's and batch 334's figures
 *   exactly.  Gap: ref 31 16-bit instructions against ours 32 -- WE EMIT ONE
 *   EXTRA.  Size and stream length MATCH (156 bytes, 55 entries), so the 41 is
 *   partly PHASE: the position-tolerant residue is only 9 in 5 hunks.
 *   RELOCATIONS differ by the same symbols at a shifted offset.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_77dd1c/200bc48.c \
 *     asm/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_a_a_c_c_c_c_b.s --func OvlFunc_882_200bc48
 *
 * SPLIT SHAPE when it lands: none.  tools/piece.py -> 1 function, all 1 parked,
 * CONVERTS WHOLE.  tools/datacheck.py on the reference is silent.
 * Pin count 0 (tools/shimcount.py); device-free.
 *
 * ================= THE GAP, LOCATED EXACTLY (tools/aligncmp.py) =================
 *
 * Five hunks, and they are one decision:
 *
 *     ref   push {lr}            ... mov r0,#0xb3 / lsl r0,#1 ... (x2) ... pop {r0}
 *     ours  push {r5, lr} / mov r5,#0xb3 / lsl r5,#1 ... adds r0,r5,#0 (x2) ... pop {r5}
 *
 * ARITHMETIC: +2 to build the value in r5, -2 because each of the two sites
 * collapses from two instructions to one `adds`, +1 for the `pop {r5}`.  Net +1.
 * The `push` is NOT part of the gap -- `push {lr}` and `push {r5, lr}` are one
 * instruction each.  PER-REGION: pre-first-call 0, body 0, EPILOGUE +1.
 * Batch 334 had this right and this round confirms it.
 *
 * ================= THE PASS, NAMED FROM THE DUMP THIS TIME =================
 *
 * cse1.  Not reasoned -- READ OFF `-da`:
 *
 *   .00.rtl   insn 13 (set (reg:SI 32) (const_int 358))   ... insn 15 (set r0 (reg 32))
 *             insn 81 (set (reg:SI 35) (const_int 358))   ... insn 83 (set r0 (reg 35))
 *   .03.cse   insn 81 IS GONE and insn 83 reads (set (reg:SI 0 r0) (reg:SI 32))
 *
 * reg 32 therefore spans fifteen calls and must be callee-saved.
 *
 * ============== WHY NO PIN-FREE SPELLING REACHES IT -- FOUR CITED STEPS ==============
 *
 * 1. THE CONSTANT ALWAYS GETS A PSEUDO.  calls.c:849-856
 *    (`precompute_register_parameters`) calls `copy_to_mode_reg` when the value
 *    is not already a REG, `mode != BLKmode`, `rtx_cost (value, SET) > 2`, and
 *    `SMALL_REGISTER_CLASSES && *reg_parm_seen` -- all four hold in Thumb for an
 *    argument in r0.  0x166 is 358 = 0xb3 << 1, a `thumb_shiftable_const`, whose
 *    `rtx_cost (.., SET)` is COSTS_N_INSNS(2) (arm.c:2075-2082).  CHECK THE VALUE
 *    BEFORE ATTACKING THIS CLASS: a constant below 256 costs 0 and goes straight
 *    to the hard register, and then `invalidate_for_call` kills it for free.
 *
 * 2. THERE IS NO BASIC BLOCK BOUNDARY TO BUY.  The reference has ZERO branches:
 *    22 calls in a straight line, `push {lr}` to `pop {r0} / bx r0`.  cse flushes
 *    its table only in `new_basic_block` (cse.c:919), called once per extended
 *    block from `cse_basic_block`, so nothing between the two sites resets it.
 *
 * 3. CALLS DO NOT HELP.  `invalidate_for_call` (cse.c:2018-2065) loops
 *    `for (regno = 0; regno < FIRST_PSEUDO_REGISTER; regno++)` and its table scan
 *    skips any entry whose `REGNO (p->exp) >= FIRST_PSEUDO_REGISTER`.  HARD
 *    REGISTERS ONLY.  A pseudo equivalence survives every call in the function.
 *    THIS is why the pin works and why pinning the SECOND site alone does not:
 *    the pin puts the value in hard r0 at the FIRST site, which the call then
 *    invalidates.
 *
 * 4. THE COST COMPARISON CANNOT BE WON.  cse_insn picks the cheapest candidate
 *    at cse.c:5236-5251.  `src_cost = COST (const_int 358)` = `notreg_cost`, and
 *    notreg_cost is `rtx_cost (x, SET) * 2` (cse.c:740) -- so TWELVE, not six;
 *    the park header's "cost is six" dropped the `* 2`.  Against it,
 *    `src_elt_cost = elt->cost = COST (reg 32)` = 1, because COST gives a pseudo
 *    REG 1 (cse.c:509-513).  The only escape, `src_cost = -1`, needs
 *    `rtx_equal_p (src, dest)` (cse.c:5157-5159), i.e. the source to BE the
 *    destination register.  Not reachable from a constant argument.
 *
 * So the bound is: with one basic block and a value whose rtx_cost exceeds 2,
 * cse1 WILL unify the two argument pseudos, and the unified pseudo WILL be live
 * across calls.  Stated with its evidence so the next reader can check it in one
 * command each.  WHAT WOULD BREAK IT: a real block boundary between the sites,
 * which this function's reference does not offer.
 *
 * ================= MEASURED THIS ROUND, AND ALL OF IT IS A NEGATIVE =================
 *
 * TWELVE SOURCE SPELLINGS, every one gap=+1 / ALIGN 9 / 41 of 55:
 *   0xb3 << 1 at both sites (the park body) | 0x166 at both | one of each
 *   | 0xb3 * 2 vs 179 + 179 | two named int locals | one named int local
 *   | __ClearFlag prototyped `short` | `unsigned short` | both `unsigned short`
 *   | __SetFlag prototyped `short`
 * "Two is not N" does not apply to the two-named-locals row: the mode-split idea
 * it was crossed with is what mattered, and HImode does not split (0x166 fits in
 * 16 bits, so the value still travels as SImode and hashes identically).
 *
 *   volatile int local                      gap +5   WORSE (a stack slot and loads)
 *
 * ONE ROW LOOKED LIKE A WIN AND IS A WRONG PROGRAM -- recorded so nobody repeats
 * it: prototyping __ClearFlag with an `unsigned char` parameter reads gap=-1,
 * ALIGN 4, 20 of 55.  It gets there by TRUNCATING 0x166 to 0x66, which then fits
 * an 8-bit immediate and skips step 1 above.  Different value, different program.
 * A false improvement is a wrong program; check the value, not the figure.
 *
 * THE FLAG AXIS IS CLOSED, with the GAP column, which is the part autosweep.py
 * could not show.  All eight Makefile groups and thirteen probe flags
 * (-fno-cse-follow-jumps, -fno-expensive-optimizations, -fno-force-mem,
 * -fno-caller-saves, -fno-peephole, -fno-defer-pop, -fomit-frame-pointer,
 * -fno-regmove, -fno-schedule-insns, -fno-thread-jumps, -fno-delayed-branch,
 * -fno-function-cse) read gap=+1.  **autosweep.py's "-O1 37 of 55 <- BETTER" and
 * "-fno-schedule-insns2 37 <- BETTER" ARE PHASE ARTIFACTS**: both raise the
 * position-tolerant residue from 9 to 12.  No flag in gcc-2.96 disables cse1.
 * THE ANCHOR AXIS IS CLOSED: all 22 boundaries, best 39, `first` unmoved at 0.
 *
 * THE ONE-PIN ZERO REPRODUCES, device labelled as an instrument:
 *   `register int q0 __asm__("r0"); q0 = 0xb3 << 1; __SetFlag(q0);` at the FIRST
 *   site only -> 156 bytes, 55 encodings and the relocations IDENTICAL, gap 0.
 *   Measured again this round; batch 334's figure stands.
 *
 * SO THE PARK STANDS AT ITS PIN-FREE FIGURE.  Under the prefer-pin-free policy
 * this is a FOR-THE-OWNER call, unchanged from batch 334's framing: one pin, on
 * a previously pin-free single-function piece, with a landed precedent for the
 * identical construct against the identical blocker (OvlFunc_888_200987c, whose
 * source is `{ PIN1; q0 = 0x9a << 1; __PlaySound(q0); }` at the first of two
 * rebuild sites).  What batch 344 adds is that the pin-free side is now BOUNDED
 * rather than merely unsolved, and the bound has four line numbers on it.
 *
 * CORPUS BOUND, re-derived rather than inherited.  Querying the 4,625 GENERATED
 * (byte-matching-by-construction) `.s` files for a `mov rN,#imm / lsl rN,#s` pair
 * rebuilding one value >= 256 twice in an argument register with a `bl` between
 * returns 361 functions, 44 of them with `push {lr}` and no callee-saved register
 * at all.  Reading those 44 CONFIRMS the park's own survey instead of refuting
 * it: in every one the repeated sites sit in DIFFERENT basic blocks.  The closest
 * is OvlFunc_890_2008054 (src/overlays/rom_78b2ac/ovl_30_c_c_a_a_a.c), which
 * rebuilds 0x80 << 2 three times with `push {lr}` only -- and its three sites are
 * separated by a `beq`, a `bne` and a second `bne`.  Its C source is on disk and
 * needs no pin because the blocks do the work.  There is no pin-free example of
 * the straight-line case anywhere in the corpus.
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
