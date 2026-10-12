/* Func_80c0f98 -- asm/rom_b5000/rom_bffb8_c_c_a_a.s
 *
 * NON-MATCHING, **21 of 61 stream encodings** -- batch 343 brief C.  WAS 30.
 *   ref 61 encodings / ours 61;  **59 INSTRUCTIONS AGAINST 59** (16-bit);
 *   size equal, relocations equal;  first differing index 20.  So the figure
 *   IS a distance.
 *   PINS 0 (tools/shimcount.py prints NO ROWS).  DEVICE-FREE: the one
 *   `do { } while (0)` anchor emits nothing and is not counted by shimcount,
 *   so NO pass-3 debt.  FLAG-FREE: production flags only.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_b5000/80c0f98.c \
 *     asm/rom_b5000/rom_bffb8_c_c_a_a.s --func Func_80c0f98
 *
 * SPLIT SHAPE: a split IS needed to ship this one alone.  tools/piece.py ->
 * rom_bffb8_c_c_a_a.s, **3 functions**, all three parked:
 *     Func_80c0eec  66 of 79    src/non_matching/rom_b5000/80c0eec.c
 *     Func_80c0f98  21 of 61    this file
 *     Func_80c1014  23 of 30    src/non_matching/rom_b5000/80c1014.c
 * tools/datacheck.py exits 0 silently (no data section).
 * tools/split_s.py --dry-run asm/rom_b5000/rom_bffb8_c_c_a_a.s Func_80c0f98:
 *     would write rom_bffb8_c_c_a_a_a.s (1 func, 86 lines)
 *                 rom_bffb8_c_c_a_a_b.s (1 func, 70 lines)  <- this function
 *                 rom_bffb8_c_c_a_a_c.s (1 func, 39 lines)
 *     would REMOVE rom_bffb8_c_c_a_a.s and rewrite stage1.ld
 * so the eventual install path is src/rom_b5000/rom_bffb8_c_c_a_a_b.c.  The
 * mates' 66 and 23 do NOT hold this function hostage -- split_s.py cuts
 * before/target/after, so any member cuts out alone.
 *
 * TWO STALE NUMBERS IN THE OLD HEADER, corrected:
 *   * its BLOCKER line said "**30 of 64**, LENGTH EXACT" while its own figure
 *     line said 30 of 61.  The stream is 61 and the INSTRUCTION counts are
 *     59/59.
 *   * its subject line named `asm/rom_b5000/rom_bffb8_c_c_a.s`, which DOES NOT
 *     EXIST.  The live reference is the `..._c_c_a_a.s` its recipe already used.
 *
 * AUTOSWEEP, BOTH CHEAP AXES, CLOSED ON THE OLD BODY -- a recorded negative:
 * NO flag group helps (six EXACTLY INERT at 30; -O1 49, -fno-schedule-insns2
 * 32, -ffixed-r7 30, -fno-strength-reduce 30) and ALL 22 anchor placements
 * inert or worse, with 2 of 24 producing no row (untested, not inert).
 *
 * ============== THE OLD DIAGNOSIS WAS A RENAMING.  THAT IS WHY IT
 * ============== LOOKED UNREACHABLE.  IT IS ROUTE 1, NOT A PERMUTATION.
 *
 * The old header reported ONE register pair for BOTH case bodies:
 *     rom    e->r4  mask->r2  v->r0        ours   e->r0  mask->r4  v->r1
 * Read off the two streams and THE TWO ARMS DISAGREE IN THE ROM:
 *     case 1   rom  e=r4  m=r2  v=r0      ours  e=r0  m=r4  v=r1
 *     case 2   rom  e=r4  m=r5  v=r0      ours  e=r0  m=r4  v=r1
 *
 * *** THE ROM'S `m` IS IN r2 IN CASE 1 AND r5 IN CASE 2 -- two block-local
 * quantities. ***  Ours was r4 in both because .17.lreg had it as ONE allocno,
 * `Register 37 used 12 times across 62 insns; set 4 times`, spanning both
 * arms.  Same for `v`, `Register 38 used 8 times across 27 insns`, which the
 * ROM keeps in r0 in BOTH arms by REUSING THE DEAD `p` REGISTER -- something a
 * pseudo that outlives the arm cannot do.
 *
 * So this was never a permutation to be renamed.  It is batch 342's ROUTE 1
 * (delete/split a reused local, making the value block-local), and batch 342's
 * "two is not N" says the shared name is the thing to split.
 *
 * AND IT IS A TEXTBOOK CROSSING -- ONE HALF IS EXACTLY INERT.  All sixteen
 * subsets of {m, v, e, list} crossed as per-arm names:
 *
 *     split_m    (per-arm `m` only)              29
 *     split_v    (per-arm `v` only)              30   EXACTLY INERT
 *     split_mv   (BOTH)                          22   <- shipped, + one anchor -> 21
 *     split_mve / split_mvel                     27
 *     split_e / split_el                         31   worse
 *     split_l, split_ve, split_vl, split_vel     30
 *
 * `split_mv` fixes `e` as a SIDE EFFECT -- it is r4 on both sides afterwards --
 * which is why splitting `e` explicitly is WORSE.  One anchor then takes 22 ->
 * 21.  The anchor axis was re-swept ON THE SPLIT BODY, not the parked one.
 *
 * *** A FLAT AXIS WORTH RECORDING: 145 ROWS, ALL EXACTLY 22. ***  I crossed arm
 * 1's statement order against arm 2's -- 12 x 12 = 144 variants, the only
 * constraints being `val &= 3` before `v = val << 2` and the pointer load
 * before its stores.  EVERY ROW READ EXACTLY 22, BIT-IDENTICALLY, base
 * included.  By the rule in src/rom_8a000/rom_93304_c_a_c.c -- "if an edit does
 * not move the allocator's inputs, the edit did not happen" -- the whole
 * arm-statement-order axis NEVER REACHES FLOW on this function.  Do not
 * re-screen it.  (That is the same phenomenon 8093af8's park reports for its
 * eight tail spellings, now measured on a second function with 144 rows.)
 *
 * The allocno model is checked on this function too: allocno_compare
 * (global.c:600-618) reproduces .18.greg's `;; 8 regs to allocate:
 * 35 34 40 38 37 36 39 33` for all EIGHT allocnos from the lreg refs/spans.
 *
 * ===================== WHAT THE REMAINING 21 IS, NAMED =====================
 *
 * Two clean TWO-SWAPS, both allocation order, i.e. span order:
 *   case 1, idx 20-31   ROM has `v` in r0 and the `e[5]` byte in r1;
 *                       ours the reverse.
 *   case 2, idx 35-55   ROM  list=r1 v=r0 m=r5 i=r6
 *                       ours list=r0 v=r1 m=r6 i=r5
 * NEXT: price those out of .17.lreg the way edit 2 of this batch's
 * Func_80165d8 did, and mind that `i` is referenced INSIDE the loop, so
 * REG_N_REFS counts it TWICE (flow.c:5114-5116, batch 342's correction) --
 * which is exactly the term that does not apply to the arm-local `m` and `v`.
 *
 * CARRIED FORWARD, AND STILL THE MAP.  One edit was worth 20 differences and
 * the length: USE A `switch`, NOT `if / else if`.
 *     rom    cmp r2,#1 / beq L1 / cmp r2,#2 / beq L2 / b L0
 *     ours   cmp r2,#1 / bne L1   (case 1 inlined as the fallthrough)
 * The ROM evaluates BOTH tests up front and branches to separate blocks --
 * switch-dispatch shape.  An if/else-if chain makes gcc fall through into the
 * first body instead, which costs two lines and shifts everything.  50 of 62 ->
 * 30 of 64 at the time.  docs/elevation.md notes that a wrong case count can
 * look like a register problem; this is the same warning one level up -- the
 * wrong CONTROL CONSTRUCT can too.
 *
 * ALSO CARRIED FORWARD, with a correction.  The old header says the ROM does
 * `and r5, r3` -- masking the PARAMETER IN PLACE -- where ours computed into a
 * temp, and that writing `val &= 3; v = val << 2;` to match "is byte-identical"
 * because `val` is dead and gcc folds the two forms to one rtx.  THE FOLDING
 * CLAIM IS RIGHT BUT THE CONCLUSION WAS NOT SETTLED: the body already spells it
 * in place and STILL emitted `ands r3,r5` (dest r3) where the ROM has
 * `ands r5,r3` (dest r5).  The in-place SPELLING does not buy the in-place
 * INSTRUCTION -- that is decided by whether the destination register happens to
 * equal val's, i.e. by allocation, which is the residue above.  The tell
 * remains one-directional: a ROM `mov` before a modify is evidence of two
 * names; an in-place modify is NOT evidence of one.
 *
 * WHAT IT DOES: sets a two-bit field (bits 2-3, mask -0xd) at offsets +5 and
 * +0x11, either on one object or across a null-terminated list of up to four,
 * dispatched on the low nibble of a byte at +0x54.
 *
 * ONE CONTROL FROM THIS BATCH WORTH KEEPING: the sibling target Func_80165d8
 * has the IDENTICAL `for (i = 0; i <= 3; i++)` construct and its ROM compare is
 * UNSIGNED (`bls`), worth 4 encodings there.  THIS function's loop compare at
 * index 56 is `ble` on BOTH sides -- the ROM's counter here IS signed.  So the
 * unsigned-counter tell does NOT generalise across the tree; check it per
 * function.
 */
extern char *GetBattleActor(int a);

void Func_80c0f98(int actor, int val)
{
    char *p;
    char *e;
    char **list;
    int i;
    int t;

    p = GetBattleActor(actor);
    if (p == 0)
        return;
    p = *(char **)p;
    if (p == 0)
        return;
    t = *(unsigned char *)(p + 0x54) & 0xf;
    switch (t) {
    case 1:
        {
        int m1;
        int v1;
        e = *(char **)(p + 0x50);
        m1 = -0xd;
        val &= 3;
        v1 = val << 2;
        e[5] = (e[5] & m1) | v1;
	do { } while (0);
        e[0x11] = (e[0x11] & m1) | v1;
        }
        break;
    case 2:
        {
        int m2;
        int v2;
        val &= 3;
        v2 = val << 2;
        list = *(char ***)(p + 0x50);
        m2 = -0xd;
        for (i = 0; i <= 3; i++) {
            e = *list++;
            if (e == 0)
                return;
            e[5] = (e[5] & m2) | v2;
            e[0x11] = (e[0x11] & m2) | v2;
        }
        }
        break;
    }
}
