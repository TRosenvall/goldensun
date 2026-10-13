/* Func_8028574 (RunSubScreenLoop) -- NON-MATCHING, 136 encodings of 139 differ.
 *
 * 0x08028574, the ONLY function now in asm/rom_15000/rom_23178_a_a_a_a_c_c_a_c_a.s.
 * ours 137 encodings / 125 16-bit instructions against ref 139 / 127, pool
 * 11/11, 296 bytes against 300 -- TWO INSTRUCTIONS SHORT, so the 136 is NOT a
 * distance.  Honest figure: 97 instructions in disagreeing regions of 144
 * (tryc --align); aligncmp raw 97.  0 pins, 0 devices (shimcount).
 * One const.sym reference (`_CONST_1f`), already present -- see below.
 *
 * (The claim line is first on purpose: parkcheck reads the FIRST `N encodings of
 *  M` in the header, and the ladders below are full of `N of M` strings.)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_15000/8028574.c \
 *     asm/rom_15000/rom_23178_a_a_a_a_c_c_a_c_a.s --func Func_8028574
 * Rank variants on:
 *   ... tools/tryc.py src/non_matching/rom_15000/8028574.c \
 *       --ref asm/rom_15000/rom_23178_a_a_a_a_c_c_a_c_a.s --align
 *
 * SPLIT SHAPE: NONE NEEDED ANY MORE.  The old header said this .s holds two
 * functions and needs an `_a`/`_b` two-way text split.  That is STALE: the piece
 * has been re-split since, datacheck.py exits silent, and
 * `split_s.py ... Func_8028574 --dry-run` now says "holds only Func_8028574 and
 * no data; convert it directly, no split needed".  Func_80286a0 lives elsewhere.
 *
 * ================= BATCH 348 (brief D): THE PER-REGION SPLIT =================
 * d_regions.py, running total, net -2:
 *   prologue `mov r7, r8 / push {r7}`                 ref 2, ours 0      -2
 *   inner-loop preheader `mov r3,#0x8e / add r3,r8 /
 *     mov r9,r3`  (t = u + 0x8e kept in r9)           ref 3, ours 1      -2
 *   the ~30-instruction CONFIRM-ARM block relocation  ref 34, ours 30    -4
 *     (difflib splits it +26 / -30)
 *   gKeyRepeat `& 0x80` test                          ref 4, ours 5      +1
 *   epilogue (one fewer high register to restore)                        -1
 * So the old header's diagnosis -- THE ROM SPENDS A FOURTH CALLEE-SAVED HIGH
 * REGISTER, and keeps `t` at the INNER preheader where we compute it inline --
 * is CONFIRMED by the split, and it is where the next round should aim.
 *
 * ---- THE BLOCK-ORDER LEVER IS INERT HERE: FOUR LAYOUTS, ALL BIT-IDENTICAL ----
 * The old finding #2 reasons from "gcc-2.96 has no block-reordering pass, so RTL
 * order is emission order".  I wrote the ROM's layout out in full -- the
 * gKeyPress&1 CONFIRM BLOCK LAST, after both key-repeat arms, with the two `||`
 * pairs split into explicit `goto down` / `goto chkup` / `goto up` / `continue`
 * exactly as the ROM branches (bne .L28620 / beq .L2863e / bne .L28652 /
 * beq .L285e6) -- plus two intermediate layouts, and measured all four:
 *
 *   as-is (this body)                     125 insns / 11 pool / 296 / align 97
 *   confirm block moved to the end        125 / 11 / 296 / 97   BIT-IDENTICAL
 *   only the two `||` tests made gotos    125 / 11 / 296 / 97   BIT-IDENTICAL
 *   both (the ROM's full layout)          125 / 11 / 296 / 97   BIT-IDENTICAL
 *
 * EVERY ONE BYTE-IDENTICAL.  jump.c normalises the emission order for this
 * function whatever the source says, so the 30-instruction block relocation in
 * the aligned diff IS NOT REACHABLE THROUGH SOURCE LAYOUT.  That does not refute
 * finding #2's historical measurement on the cancel block -- it bounds it: the
 * sentence "that placement is not reachable from two `if` bodies in the inner
 * loop however they are spelled" is false in the stronger sense that the gotos
 * do not change the output at all.  DO NOT SPEND ANOTHER ROUND ON LAYOUT.
 *
 * ---- `_CONST_1f` IS LOAD-BEARING, AND ITS EXTRA RELOCATION IS THE DOCUMENTED
 *      PHANTOM.  TESTED, NOT ASSUMED ----
 *   (int)&_CONST_1f    125 insns / pool 11 (= ref) / 296 / align 97 / tryc 97
 *   plain 0x1f         125 insns / pool 10 (-1)    / 292 / align 98 / tryc 99
 * The reference's eleventh 32-bit entry is the `ldr r3, =0x1f` feeding the
 * three-operand `add r0, r2, r3`, and gcc will not pool 31 from a plain literal
 * (Thumb `add Rd, Rn, #imm3` cannot hold 31, so it builds it with `mov`
 * instead).  objcmp reports ref 10 relocations against our 11; the extra one is
 * exactly docs/elevation.md's const.sym phantom class -- a disassembly shows a
 * resolved symbol as a bare word, so the reference's pooled eight-bit-movable 31
 * implies a relocation existed.  The adjudicator at docs/elevation.md:29412-29420
 * applies only "when relocation parity is already exact", which is not this case.
 *
 * ================= WHAT MOVED THE NEEDLE (all previously measured) ==========
 * 1. THE INNER LOOP'S CURSOR ACCESSES MUST BE WRITTEN INLINE, IN BOTH ARMS.
 *    `*(short *)((unsigned char *)u + 0x8c) = *(short *)(...) - 1;` in place of
 *    `*c = *c - 1;`, same in the increment arm.  That is what produces the ROM's
 *    `mov r6, r10` at the INNER LOOP PREHEADER: loop.c hoists the invariant
 *    address out of the inner loop into a new pseudo, and the cse rerun rewrites
 *    that pseudo's initialiser to `= c`, giving a copy whose uses are all inside
 *    the loop.  A named `p = c` cannot do this.  ALL-OR-NOTHING: inlining ONE arm
 *    is byte-identical to inlining neither (cse merges the two addresses).
 * 2. The cancel path as a `goto` into a label between the two arms of the
 *    `if (*m != 0)`.  Historically 121 -> 105, 136 -> 101, 113 -> 97.
 * 3. `volatile` on gKeyPress and gKeyRepeat is load-bearing and is a real
 *    qualifier: the ROM re-reads each one per test (ldr r2,[r7] three times from
 *    the CSE'd address in r7, ldr r2,[r1] four times for gKeyRepeat), and the
 *    interrupt handler writes the key state.
 * 4. `*c = 0;` and NOT a fresh constant in the increment arm's wrap: the ROM
 *    writes `strh r5, [r2]` reusing the zero the failed `gKeyRepeat & 0x40` left
 *    in r5.  Same lever the landed Func_802938c in this bank documents.
 *
 * ===================== MEASURED INERT, WITH NUMBERS =====================
 * ALL 24 PERMUTATIONS of the four pointer declarations (u, c, m, t) are
 * BYTE-IDENTICAL -- same count, same size, same aligned figure.  Declaration
 * order only decides an EXACT TIE and there is no tie here.
 * Also inert: `short *p = c;` before the inner loop (coalesced); inlining the
 * cursor in one arm only; and now the four block layouts above.
 * WORSE: `t = u + 0x8e` inline WITHOUT the inline cursor accesses -- gcc then
 * hoists the address out of the OUTER loop into the prologue.
 *
 * ==================== WHAT THE NEXT ATTEMPT SHOULD AIM AT ====================
 * The two attributable sites are the PROLOGUE's fourth high-register save (-2)
 * and the INNER PREHEADER's `t = u + 0x8e` in r9 (-2); they are the same
 * question, because the fourth high register is what r9 is for.  batch 292's
 * move_movables finding (loop.c:1803, the cut between m->lifetime 3 and 4) is
 * the knob: something that gives that address a lifetime of 3 should leave it at
 * the inner preheader.  Half-measures on `t` are cse-merged and inert, so it has
 * to be reached through the LIFETIME, not through spelling -- and NOT through
 * block layout, which is now a measured dead end.
 *
 * NO SHIMS: no `register ... __asm__`, no `__asm__ ("")`, no per-file flags, no
 * fakematch row.
 */
struct Ui {
    unsigned char pad0[0x78];
    void *box;
};

extern unsigned char *iwram_3001f38;
extern int _CONST_1f;
extern volatile unsigned int gKeyPress;
extern volatile unsigned int gKeyRepeat;

extern void Func_8016478(void *w);
extern void Func_801e7c0(int id, void *w, int a, int y);
extern void WaitFrames(int n);
extern void _PlaySound(int id);

int Func_8028574(int start)
{
    struct Ui *u;
    short *c;
    short *m;
    int id;
    int k;

    u = (struct Ui *)iwram_3001f38;
    c = (short *)((unsigned char *)u + 0x8c);
    m = (short *)((unsigned char *)u + 0x92);
    *c = start;
    for (;;) {
        Func_8016478(u->box);
        if (*m != 0) {
            id = *m + *c;
            goto draw;
        }
        goto def;
    cancel:
        _PlaySound(0x71);
        return -1;
    def:
        k = *c + 0x84;
        id = *((unsigned char *)u + k) + (int)&_CONST_1f;
    draw:
        Func_801e7c0(id, u->box, 0, 0);
        for (;;) {
            WaitFrames(1);
            if (gKeyPress & 1) {
                _PlaySound(0x70);
                return *c;
            }
            if (gKeyPress & 2)
                goto cancel;
            if (gKeyPress & 8)
                goto cancel;
            if ((gKeyRepeat & 0x20) || (gKeyRepeat & 0x40)) {
                _PlaySound(0x6f);
                *(short *)((unsigned char *)u + 0x8c) = *(short *)((unsigned char *)u + 0x8c) - 1;
                if (*(short *)((unsigned char *)u + 0x8c) < 0)
                    *c = *(short *)((unsigned char *)u + 0x8e) - 1;
                break;
            }
            if ((gKeyRepeat & 0x10) || (gKeyRepeat & 0x80)) {
                _PlaySound(0x6f);
                *(short *)((unsigned char *)u + 0x8c) = *(short *)((unsigned char *)u + 0x8c) + 1;
                if (*(short *)((unsigned char *)u + 0x8c) >= *(short *)((unsigned char *)u + 0x8e))
                    *c = 0;
                break;
            }
        }
    }
}
