/* Func_8028574 (RunSubScreenLoop) -- 0x08028574, FIRST of the TWO functions in
 * NON-MATCHING, 136 encodings of 139.  NOT a distance (ref 300 bytes / 139 encodings against ours 296 / 137, two instructions short).
 * READ `--align` INSTEAD: 97 instructions in disagreeing regions of 144.
 *
 * (The claim line is first on purpose: parkcheck reads the FIRST `N encodings of M` in
 *  the header, and a drop ladder below is full of `N of M` strings whose earliest is the
 *  ladder's worst rung.)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_15000/8028574.c \
 *     asm/rom_15000/rom_23178_a_a_a_a_c_c_a_c_a.s --func Func_8028574
 * Distance while iterating (the number that ranks variants here):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/tryc.py src/non_matching/rom_15000/8028574.c \
 *     --ref asm/rom_15000/rom_23178_a_a_a_a_c_c_a_c_a.s --align
 * asm/rom_15000/rom_23178_a_a_a_a_c_c_a_c_a.s.
 *
 * THE SPLIT.  grep -ci func_start says 2: Func_8028574 at 0x08028574 (lines 7-155)
 * and Func_80286a0 at 0x080286a0 (lines 156-end).  The second is ALREADY PARKED at
 * src/non_matching/rom_15000/80286a0.c (4 of 85, three real), so it stays in
 * assembly and this is the `_a` / `_b` two-way text split.  datacheck.py exits 0
 * with no data section and no EXPORTS line: NO label crosses the boundary in either
 * direction and no export is required.
 *
 * NOT MATCHING: 137 instructions against the reference's 139, 296 bytes against
 * 300, so objcmp's 136-of-139 IS NOT A DISTANCE.  The honest figure is
 * tryc --align: 97 instructions in disagreeing regions, of 144.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_15000/8028574.c \
 *     asm/rom_15000/rom_23178_a_a_a_a_c_c_a_c_a.s --func Func_8028574
 *
 * Every shared idiom is the sibling park's, not a guess: struct Ui, iwram_3001f38,
 * the `short *c` / `short *m` pair at +0x8c / +0x92, `k = *c + 0x84` as a named
 * local, and `(int)&_CONST_1f` for the pooled 0x1f -- _CONST_1f ALREADY EXISTS in
 * const.sym, so NO .sym edit is needed and the `=0x1f` vs `=_CONST_1f` line in any
 * diff is the documented const.sym phantom-relocation class.
 *
 * ============================ THE BLOCKER ============================
 * REGISTER ALLOCATION, and specifically that the ROM spends a FOURTH callee-saved
 * HIGH register where we spend three.
 *
 *   ROM :  r8 = u   r9 = t (u+0x8e)   r10 = c   r11 = m
 *          r6 = a LOW COPY of c   r7 = &gKeyPress   r5 = the gKeyRepeat & 0x40 value
 *   ours:  r8 = c   r9 = t   r10 = u   r11 = m        (and no low copy of c)
 *
 * Because the ROM keeps `c` HIGH, every signed read of it needs a low copy first --
 * `mov r2, r10 / mov r1, #0 / ldrsh r3, [r2, r1]` at three sites, `mov r1, r10` and
 * `mov r2, r10` at the two wrap stores -- and the fourth high register costs
 * `mov r7, r8 / push {r7}` in the prologue and one more register in the epilogue's
 * pop/restore.  That is the whole 10-instruction gap of the first candidate, and it
 * is also the r8<->r10 swap that makes most of the 97 differing positions.
 *
 * The seventh long-lived quantity that forces `c` high is the LOW COPY ITSELF, and
 * it is circular from the outside: `short *p = c;` written before the inner loop is
 * COALESCED AWAY (byte-identical to not writing it, measured), because p and c hold
 * the same value with overlapping ranges and copy propagation rewrites p's uses.
 *
 * ================= WHAT MOVED THE NEEDLE (all measured) =================
 * First candidate 129 instructions / 121 aligned.
 *
 * 1. THE INNER LOOP'S CURSOR ACCESSES MUST BE WRITTEN INLINE, IN BOTH ARMS.
 *    `*(short *)((unsigned char *)u + 0x8c) = *(short *)(...) - 1;` in place of
 *    `*c = *c - 1;`, and the same in the increment arm.  This is what produces the
 *    ROM's `mov r6, r10` at the INNER LOOP PREHEADER: loop.c hoists the invariant
 *    address out of the inner loop into a new pseudo, and the cse rerun then
 *    rewrites that pseudo's initialiser to `= c`, giving a copy that survives
 *    because its uses are all inside the loop.  A named `p = c` cannot do this.
 *    IT IS ALL-OR-NOTHING: inlining ONE arm is byte-identical to inlining neither
 *    (cse merges the two addresses), and inlining both takes the count from 129 to
 *    141 with `t` a local, or to 137 with `t` inline.  Four combinations measured,
 *    only the two both-arms ones differ from the baseline at all.
 *
 * 2. THE CANCEL PATH IS A `goto` INTO A LABEL BETWEEN THE TWO ARMS OF THE
 *    `if (*m != 0)`.  121 -> 105 on the first base, 136 -> 101 and 113 -> 97 on the
 *    other two -- it improves EVERY base.  The reference lays the shared
 *    `_PlaySound(0x71); return -1;` block out at 0x080285b4, i.e. BETWEEN the
 *    `*m != 0` arm and the `*m == 0` arm, and it ends in `b <epilogue>` rather than
 *    falling through.  gcc-2.96 has no block-reordering pass, so RTL order is
 *    emission order and that placement is not reachable from two `if` bodies in the
 *    inner loop however they are spelled; cross-jumping merges them where they are
 *    written.  Writing the arms as `goto draw;` / `goto def;` with `cancel:` and
 *    `def:` labels between them puts it exactly where the ROM has it.
 *
 * 3. `volatile` on gKeyPress and gKeyRepeat is load-bearing: the ROM re-reads each
 *    one per test (`ldr r2, [r7]` three times from the CSE'd address in r7, and
 *    `ldr r2, [r1]` four times for gKeyRepeat).  It is also a real qualifier -- the
 *    interrupt handler writes the key state.
 *
 * 4. `*c = 0;` and NOT a fresh constant in the increment arm's wrap: the ROM writes
 *    `strh r5, [r2]` reusing the zero that the failed `gKeyRepeat & 0x40` left in
 *    r5.  That falls out of the plain `*c = 0;` spelling -- the same lever the
 *    landed Func_802938c in this bank documents.
 *
 * ===================== MEASURED INERT, WITH NUMBERS =====================
 * ALL 24 PERMUTATIONS OF THE FOUR POINTER DECLARATIONS (u, c, m, t) ARE
 * BYTE-IDENTICAL at 141 instructions / 101 aligned.  Not "similar" -- every one of
 * the 24 reports the same instruction count, the same size and the same 101.  This
 * is the cleanest confirmation in the corpus of docs/elevation.md's rule that
 * declaration order only decides an EXACT TIE, and here there is no tie: the
 * r8<->r10 assignment is forced by live lengths, so permuting the source cannot
 * touch it.
 * Also inert: `short *p = c;` before the inner loop (coalesced, byte-identical);
 * inlining the cursor in one arm only (byte-identical).
 * WORSE: writing `t = u + 0x8e` inline WITHOUT the inline cursor accesses -- 127
 * instructions, 125 aligned, because gcc then hoists the address out of the OUTER
 * loop into the prologue where the ROM keeps it at the inner preheader.
 *
 * ==================== WHAT THE NEXT ATTEMPT SHOULD AIM AT ====================
 * The count is BRACKETED, which is the useful part: with both cursor arms inline
 * the same source is 141 with `t` a local and 137 with `t` inline, and the target
 * is 139.  The 4-instruction step is loop.c's invariant motion on `u + 0x8e`
 * choosing the outer preheader (prologue) or the inner one, and batch 292's
 * move_movables finding (loop.c:1803, the cut between m->lifetime 3 and 4) is the
 * knob: something that gives that address a lifetime of 3 should leave it at the
 * inner preheader while keeping the local, which is the ROM's shape and should be
 * worth exactly the missing two instructions.  Half-measures on `t` are cse-merged
 * and inert, so it has to be reached through the lifetime, not through spelling.
 *
 * NO SHIMS: no `register ... __asm__`, no `__asm__ ("")`, no per-file flags, no
 * fakematch row.  One const.sym reference, already present.
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
