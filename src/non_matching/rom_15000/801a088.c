/* DrawInventoryIcon -- 0x0801a088, sole function of
 * asm/rom_15000/rom_19ebc_a_c_c_c_a_a_c.s (grep -ci func_start = 1), so it
 * converts WHOLE-FILE; no data section (datacheck), no split needed.
 *
 * NON-MATCHING, 197 of 252 encodings  (batch 346 brief A).
 *   INSTRUCTION COUNTS NOW EQUAL: ref 234, ours 234.  SIZE EQUAL: 540/540.
 *   ALIGNED RESIDUE 57 of 252 in 27 hunks (tools/aligncmp.py).
 *   Was: 105 of 252, ref 234 against OURS 233, aligned 74 in 41 hunks.
 *
 * *** THE FIGURE WENT UP AND THE BODY GOT BETTER.  READ THIS FIRST. ***
 * The parked body read 105 and was ONE INSTRUCTION SHORT, with a `.short
 * 0x0000` pad making the byte size agree -- so 105 was a positional count on an
 * out-of-phase stream.  This body has equal length AND equal byte size, and its
 * ALIGNED residue is 57 in 27 hunks against the park's 74 in 41.  By every
 * measure that is not phase-sensitive it is closer; the positional figure rose
 * only because the register rotation now runs the whole length instead of
 * stopping at the shortfall.  Rank this by aligncmp (batch 292), not by 197.
 * The parked body is kept as p2/base.c in this batch's scratch.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/801a088.c \
 *     asm/rom_15000/rom_19ebc_a_c_c_c_a_a_c.s --func DrawInventoryIcon
 *
 * *** THE PREVIOUS HEADER'S CENTRAL CLAIM IS REFUTED. ***  It said: "The ROM
 * spills `id` and keeps `k` in r10; we spill `k` and keep `id`.  THAT ONE CHOICE
 * IS THE WHOLE 4 BYTES ... 2 instructions x 2 sites = the 4 bytes, and nothing
 * else differs in length anywhere in the function."  Measured on the parked
 * body: the byte sizes were EQUAL (540/540, objcmp prints no SIZE line), the
 * instruction gap was ONE, not two, and there were FIVE unbalanced sites, not
 * two.  Region by region on the parked body (16-bit instructions):
 *
 *   region                                              gap
 *   prologue + `_GetItemInfo` + the p == 0 guard          0  (a +3/-3 pair:
 *       ROM spills `id` with str/ldr where we keep it in fp with two movs)
 *   the six icon blocks (flags 1, uncond, 8, 0x10, 0x20)  0  (every ROM
 *       `ldr r2,[sp,#8]` is answered by our `mov r2, fp`: 1 for 1)
 *   the `flags & 2` block                                -2
 *   the `flags & 4` block                                +4
 *   the `k % 10` and `k / 10` blocks                     -3  (-4, +2, -1)
 *   tail                                                  0
 *                                                        -1
 * The `k % 10` region alone is -3 and has nothing to do with the id/k spill.
 * The park's "nothing else differs in length anywhere" was the costly half of
 * the mistake: it told three batches not to look there.
 *
 * WHAT THIS BATCH CHANGED, one line: `k = 0;` MOVED DOWN from the top of the
 * function to immediately before the unconditional
 * `*(int *)(p + 0x604) = L29ee4[...]` store.  Semantically identical -- nothing
 * reads `k` before that point and `k = 0` still dominates every use.
 *
 * WHY IT WORKS, with the arithmetic.  `allocno_compare` (global.c:598-621) is
 * `floor_log2(n_refs)*n_refs/live_length*10000*size`, no frequency term, in-loop
 * refs doubled (flow.c:5114-5116), ties on ASCENDING allocno number.  From
 * `.17.lreg` + `.18.greg` line 4 on the PARKED body:
 *   reg 34  `p`      24 refs / 139 insns  -> 0.690
 *   reg 33  (id)      7 refs / 112 insns  -> 0.125
 *   reg 32  (info)    7 refs / 116 insns  -> 0.121
 *   reg 35  `k`       9 refs / 258 insns  -> 0.105   <-- LAST, so it spills
 * greg order: `197 34 115 125 66 177 73 184 69 180 60 33 32 155 [35] 156 36 37`.
 * `k`'s live_length is 258 -- essentially the whole function -- purely because
 * `k = 0;` sat on line 2.  It needs live_length < 216 to pass reg 33 at 0.125.
 * Moving the definition is the brief's allocation route 2 and it is enough:
 * `k` now gets a HIGH REGISTER (r9) instead of a stack slot, which is the ROM's
 * class, and both halves of the `flags & 2` / `flags & 4` imbalance collapse.
 * The previous header said "WHAT WOULD CLOSE IT: anything that makes `k`
 * outrank `id` in allocno_compare.  Declaration order is NOT enough."  Both
 * halves were right; it just never tried moving the definition.
 *
 * WHICH POSITION, measured -- the lever is sharply position-dependent and three
 * of the eight placements are SEMANTICALLY INVALID, so do not sweep this blind:
 *   before the unconditional 0x604 store   197 of 252, 234/234, aligned 57 / 27  <- THIS
 *   before `LoadIcon(p, slot)`             193 of 252, 234/234, aligned 59 / 34
 *   before `if ((flags & 8) ...)`          200 of 252, 234/234, aligned 74 / 42
 *   after the p == 0 guard                 200 of 252, 234/234, aligned 84 / 48
 *   before `if (flags & 1)`                200 of 252, 234/234, aligned 84 / 48
 *   before `if ((flags & 0x10) ...)`       200 of 252, 234/234, aligned 98 / 40
 *   before `if ((flags & 0x20) ...)`       ours 232 (2 short), aligned 83 / 54
 *   before `if (flags & 2)`                ours 232 (2 short), aligned 121 / 39
 *   INVALID -- do not use: before `if (flags & 4)` (kills the `flags & 2`
 *     result) and before `if (k != 0 ...)` (kills both).  Both compile and both
 *     read "better" on hunks; they are WRONG PROGRAMS.  Noted because they are
 *     exactly what a blind placement sweep would promote.
 *
 * WHAT IS LEFT, 27 hunks and all of it register choice.  The nine unbalanced
 * sites now sum to ZERO: -1, +2, +1, -1, -2, +4, -4, +2, -1.  `k` is in r9
 * where the ROM has it in sl, and the ROM pays `mov sl,r3 / movs r3,#1 /
 * add sl,r3` where we pay `adds r3,#1 / mov r9,r3` -- same count, different
 * register, so the remaining work is which HIGH register `k` gets, not whether
 * it gets one.
 *
 * BOTH CHEAP AXES CLOSED, on the parked body AND on this one (autosweep.py):
 *   parked: flags 8/8 inert or worse (-fno-strict-aliasing 101 of 252 with
 *           `first` unmoved -- phase; -O1 212; -ffixed-r7 215).  anchor 46/46
 *           inert -- NOTE the first run was CAPPED at 40 and reported the axis
 *           as "not closed"; the full 46 run is the one to cite.
 *   this body: flags 8/8 inert (-fno-gcse, -fno-strength-reduce,
 *           -fno-rerun-cse-after-loop and -fno-rerun-loop-opt all read exactly
 *           197, i.e. no effect at all).  anchor 46/46 inert, best 194 with
 *           `first` unmoved at 15 on all thirteen lower rows -- all phase.
 *
 * LOAD-BEARING CONSTRUCTS (each measured by dropping it singly; carried
 * forward, all still true):
 *  - THE BLOCK-LOCAL int CARRIER FOR THE HALFWORD 2 IS THE BIGGEST LEVER HERE:
 *    writing `*(short *)(p + 0x600) = 2;` pools the constant
 *    (`ldr r2,=0x2`, because gcc-2.96 has no immediate alternative for an
 *    HImode constant) where the ROM has `mov r2,#2`.  Wrapping each pair of
 *    halfword stores in its own `{ int t = 2; ... }` block gives the ROM's
 *    `mov`.  Measured: literal 2 -> 208 of 258; ONE shared `int v = 2;` for all
 *    six sites -> 221 of 254 (gcc hoists the single carrier into r8 and reuses
 *    it, where the ROM re-materialises 2 per block); block-local carriers ->
 *    111 of 252.  The scope is the whole lever, not the carrier.
 *  - `unsigned int id`, not `int id`: the ROM shifts with `lsr r3,#0xb` and a
 *    signed operand gives `asr`.
 *  - The cached-vs-recomputed pointer pattern needs NO source distinction.  The
 *    three stores are written identically in all six blocks; cse.c works on
 *    extended basic blocks, so the `flags & 8` block (single predecessor, in
 *    the unconditional block's extended BB) reuses r9/r8/r7 while the
 *    `flags & 0x10` and `flags & 0x20` blocks (reached through a join)
 *    recompute -- exactly the ROM.  Do not try to spell this difference.
 *
 * CONTRADICTS docs/web-session-286-289.md section 5: that handoff says
 * "A libcall with a constant divisor means the divisor was a variable at
 * expand."  NOT TRUE ON THUMB-1.  Here `k % 10` and `k / 10` written with
 * PLAIN LITERAL 10 emit `mov r1,#0xa / bl __modsi3` and `mov r1,#0xa /
 * bl __divsi3` byte-for-byte the same as routing them through a variable.
 * thumb-1 has no high-part multiply, so expand_divmod has no shift/multiply
 * path to take and falls back to the libcall even for a constant divisor.
 *
 * INERT: all 24 permutations of the four local declarations (107..113 on an
 * older body; it never flips the id/k spill choice); routing the divisor through
 * a variable; naming `k / 10` in a local.
 *
 * The four .L tables live in asm/rom_15000/rom_19ebc_c_c.s and are ALREADY
 * .global there (lines 5-9), so NO new export and no .s change is needed --
 * only gcc's asm-label extension, which is a name binding, not a shim.
 * No pins, no "+r" barriers, no volatile, no DMA3_SET, no .equ.
 *
 * NEXT: which high register `k` lands in.  It is r9 here and sl in the ROM, and
 * the three remaining +2/-2 and +4/-4 pairs are the knock-on.  The lever class
 * is the same one that worked above -- live_length, not declaration order -- so
 * the thing to measure is `info`'s and `id`'s live ranges (regs 32 and 33, both
 * at ~0.12 and both ahead of `k` before this change), since whichever of them
 * `k` now displaces decides the rest of the rotation.
 * Do NOT re-screen: the flag axis, the anchor axis (46 of 46, not the capped
 * 40), declaration order, or the halfword-carrier scoping.
 */
extern unsigned char *iwram_3001e94;

extern unsigned char *_GetItemInfo(int id);
extern int LoadIcon(unsigned char *p, int n);

extern int L29a10[] __asm__(".L29a10");
extern int L29ee4[] __asm__(".L29ee4");
extern int L29acc[] __asm__(".L29acc");
extern int L29b68[] __asm__(".L29b68");

int DrawInventoryIcon(unsigned int id, int flags)
{
    unsigned char *p;
    int k;
    int slot;
    unsigned char *info;

    slot = 0;
    info = _GetItemInfo(id & 0x1ff);
    p = iwram_3001e94;
    if (p == 0)
        return -1;
    if (flags & 1) {
        *(int *)(p + 0x604) = L29a10[2];
        {
            int t = 2;
            *(short *)(p + 0x600) = t;
            *(short *)(p + 0x602) = t;
        }
        LoadIcon(p, 0);
        slot = 1;
    }
    k = 0;
    *(int *)(p + 0x604) = L29ee4[*(unsigned short *)(info + 6)];
    {
        int t = 2;
        *(short *)(p + 0x600) = t;
        *(short *)(p + 0x602) = t;
    }
    LoadIcon(p, slot);
    if ((flags & 8) && (id & 0x400)) {
        *(int *)(p + 0x604) = L29acc[1];
        {
            int t = 2;
            *(short *)(p + 0x600) = t;
            *(short *)(p + 0x602) = t;
        }
        LoadIcon(p, 1);
    }
    if ((flags & 0x10) && (id & 0x200)) {
        *(int *)(p + 0x604) = L29acc[0];
        {
            int t = 2;
            *(short *)(p + 0x600) = t;
            *(short *)(p + 0x602) = t;
        }
        LoadIcon(p, 1);
    }
    if ((flags & 0x20) && (id & 0x200) && (info[3] & 1) && (info[3] & 2)) {
        *(int *)(p + 0x604) = L29acc[2];
        {
            int t = 2;
            *(short *)(p + 0x600) = t;
            *(short *)(p + 0x602) = t;
        }
        LoadIcon(p, 1);
    }
    if (flags & 2) {
        k = ((id & 0xf800) >> 11) + 1;
        if (k <= 1)
            k = 0;
    }
    if (flags & 4) {
        k = ((id & 0xf800) >> 11) + 1;
    }
    if (k != 0 && k <= 0x1e) {

        *(int *)(p + 0x604) = L29b68[k % 10];
        {
            int t = 2;
            *(short *)(p + 0x600) = t;
            *(short *)(p + 0x602) = t;
        }
        LoadIcon(p, 1);

        if (k / 10 != 0) {
            *(int *)(p + 0x604) = L29b68[k / 10 + 9];
            {
                int t = 2;
                *(short *)(p + 0x600) = t;
                *(short *)(p + 0x602) = t;
            }
            LoadIcon(p, 1);
        }
    }
    return 0x100;
}
