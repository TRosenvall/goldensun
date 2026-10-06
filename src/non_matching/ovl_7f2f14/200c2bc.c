/* OvlFunc_968_200c2bc -- 0x0200c2bc,
 * asm/overlays/rom_7f2f14/ovl_30_c_c_a_c_c_c_a.s
 *
 * NON-MATCHING, 2 differing encodings of 271.  RE-DERIVED batch 328:
 * first at index 163, ref 46ca ours 4693.  Size exact, instruction count
 * exact, pool word count exact, relocations clean.  PIN COUNT: 0.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7f2f14/200c2bc.c asm/overlays/rom_7f2f14/ovl_30_c_c_a_c_c_c_a.s --func OvlFunc_968_200c2bc
 *
 * FLOOR HISTORY: 205 -> 110 -> 2.  DO NOT START OVER; starting over has cost
 * two rounds and is still the most expensive mistake available here.  The two
 * file-mates OvlFunc_968_200c048 and OvlFunc_968_200c520 landed separately in
 * batch 300, so closing this converts the last third of the .s, not the whole.
 *
 * LANDING SHAPE WHEN CLOSED: src/overlays/rom_7f2f14/ovl_30_c_c_a_c_c_c_a.c.
 * overlay.ld already names the asm/ object and MUST keep its path until then.
 * The .s carries no data; pooled symbols gState, iwram_3001ebc, iwram_3001e40
 * are already extern in the tree.  makefile_flags() empty, so plain -O2.
 *
 * ========== THE RESIDUE, in loop 2's preheader ==========
 *      ROM    mov r2,#0 | mov r8,r2 | mov r10,r9 | mov r11,r2
 *      ours   mov r2,#0 | mov r8,r2 | mov r11,r2 | mov r10,r9
 * One adjacent pair, transposed.
 *
 * ========== BATCH 328: "FLOOR, NOT A SPELLING" IS REFUTED ==========
 *
 * The park concluded "NOT ONE [of 30 variants] put the hoisted copy anywhere
 * but last" and "SO THIS IS A FLOOR, NOT A SPELLING".  A single variant puts it
 * EXACTLY where the ROM has it.  The park's loop.c reading is still correct --
 * .08.loop prints `Insn 366: regno 105 (life 21), savings 1  moved to 647`, so
 * insn 647 (`mov sl,r9`) really is a move_movables hoist emitted immediately
 * before the NOTE_INSN_LOOP_BEG -- but PLACEMENT IS NOT THE LAST WORD.
 *
 * WHAT ACTUALLY DECIDES THE OUTPUT ORDER, read from .23.sched2's block-10 table:
 *      352 (`r8 = r2`)   dep 2   prio 1   dependents: (none)
 *      355 (`fp = r2`)   dep 2   prio 1   dependents: (none)
 *      647 (`sl = r9`)   dep 0   prio 1   dependents: (none)
 * Every rung above INSN_LUID ties -- zero dependents each, equal priority -- so
 * LUID alone decides, and 647 is the LAST insn of basic block 10 and therefore
 * has the highest LUID, so it is issued last.  647 can never acquire an
 * in-block dependent, because the loop's top label opens the next block right
 * after it.  THAT is the airtight part of the floor, and it is a floor only for
 * a HOISTED copy.
 *
 * THE ESCAPE, MEASURED (scratch_elev/b328/A/tc2bc/q1.c):
 *   base + `register struct P *qq __asm__("r10");` assigned `qq = tp` BETWEEN
 *   `i = 0` and `acc = 0`, and used as the 8th argument of loop 2's
 *   OvlFunc_968_2008118 call (loop 1 keeps `tp`), emits VERBATIM
 *        mov r2,#0 | mov r8,r2 | mov sl,r9 | mov fp,r2     == THE ROM
 *   The park's 30 variants all used PSEUDO copies, which it correctly measured
 *   coalesced away (its recorded 114 class); A HARD-REGISTER LOCAL CANNOT BE
 *   COALESCED, and that is the dimension nobody varied.
 *
 *   q1 reads 4 of 271 and the first difference MOVES 163 -> 173.  The four are
 *   a pure r9 <-> r10 ROLE SWAP inside loop 2, nothing else:
 *        rom[179] mov r2,r10   ours mov r2,r9     (t.f8  store)
 *        rom[189] mov r2,r10   ours mov r2,r9     (t.fc  store)
 *        rom[197] mov r2,r10   ours mov r2,r9     (t.f22 store)
 *        rom[219] mov r2,r9    ours mov r2,r10    (8th arg -> [sp,#12])
 *   The BASE already has the ROM's roles here (checked in the base .s: stores
 *   via sl, stack argument via r9).  So this is a TRADE, not a floor: both
 *   halves are reachable, just not yet together.
 *
 * THE OPEN QUESTION IS NOW NARROW AND NAMED: keep q1's preheader order while
 * keeping the base's r9/r10 roles in loop 2.  That is one allocation question
 * about which register loop 2's struct address lands in -- NOT a loop.c
 * placement question, and not the gcse insertion-point question either.
 *
 * MEASURED IN THE PIN FAMILY, ALL WORSE THAN THE BASE's 2:
 *   q1  pin r10, copy between the inits, qq at loop-2 call           4 (first 173)
 *   q2  pin r10, copy BEFORE `i = 0`                                 6 (first 162)
 *   q3  pin r10, `qq = &t` instead of `qq = tp`                      4
 *   q4  pin r10, loop-2 stores via `qq->`, call via tp    94 of 272, +1 pool word
 *   q5  as q4 with `qq = &t`                              94 of 272, +1 pool word
 *   q6  as q4 with `&t` at the call                       94 of 272, +1 pool word
 *   q7  qq for the stores AND the call argument          108 of 272, +1 pool word
 *   q9  qq assigned and never read                                  22 (first 34)
 *   q10 pin r9 instead of r10, qq at loop-2 call                     6 (first 34)
 *   q11 pin r9, qq at BOTH call sites              249 of 281, +10 instructions
 *   q12 pin r8                                                      22 (first 34)
 *
 * STILL LOAD-BEARING in the body below (do not disturb): `t.f8 = ...` written
 * plain so GCSE manufactures the second register itself (that alone was
 * 114 -> 12); `tp = &t` assigned THIRD; `m = i + 0x1a` named before `n`; a
 * FRESH n2/s4/s5 for the second __CopyMapTiles site.
 *
 * STRUCK, and kept struck: the claim that gcse's insert_insn_end_bb appends
 * always-last.  pre_edge_insert (gcse.c:4440) uses it only for EDGE_ABNORMAL.
 * MEASURED WORSE (earlier batches): the split-condition `do` + `break` form,
 * 270 lines, 224 differing, frame grown to 0x3c by a spill.
 *
 * ========== BATCH 329 (brief F): THE PIN-FREE SIDE IS SWEPT, AND A LANDED NEAR-TWIN FOUND ==========
 *
 * RE-DERIVED: 2 differing encodings of 271, first differing index 163,
 * ref 46ca ours 4693, size 612 = 612, instruction count 236 = 236, pool word
 * count 35 = 35, relocations clean, PIN COUNT 0.  The stream differs at
 * EXACTLY indices 163 and 164.  46ca is `mov r10,r9`, 4693 is `mov r11,r2`, so
 * the residue is still the one transposed adjacent pair in loop 2's preheader
 * and nothing else.
 *
 * ----- THE LEAD THIS PARK HAS NEVER MENTIONED -----
 *
 * OvlFunc_968_200c610, in src/overlays/rom_7f2f14/ovl_30_c_c_c_c_a.c, IS A
 * LANDED NEAR-TWIN OF THIS FUNCTION.  Same overlay, same prologue down to the
 * constants (iwram_3001ebc + 0x1c0 = 0x202, CutsceneStart,
 * Actor_SetSpriteFlags(MapActor_GetActor(0), 0), Func_8092950(0, 0xf),
 * Func_8091ff0(0xaa), MapTransitionIn, WaitMapTransition, CutsceneWait(0x28),
 * PlaySound(0xa2)), the same `struct P t` on the stack with `tp = &t`, the same
 * `j = 0; i = 0; tp = &t; acc = 0;` quartet, the same inner
 * OvlFunc_968_2008118 call with 0x88 << 16 as the seventh argument and tp as
 * the eighth, the same `goto mid` re-entry, and the same CopyMapTiles tail.
 *
 * AND ITS GENERATED .s HOLDS THE PREHEADER SHAPE THIS PARK NEEDS, twice:
 *      mov r2,#16 | mov r3,#0 | add r2,r2,sp
 *      mov sl,r3  | mov r8,r3 | mov fp,r2 | mov r9,r3
 * -- the frame-address copy `mov fp,r2` sits THIRD of four, not last.  So "the
 * address copy is emitted last" is NOT a property of gcc-2.96 under this
 * invocation; it is a property of this park's copy being a move_movables HOIST
 * while that one's is an ordinary live-range copy of a pseudo that is live-in
 * to the block.  READ THAT FILE BEFORE SPENDING ANOTHER ROUND HERE.  Its
 * structural differences from this body are few and named: a `do { } while
 * (k <= 3)` inner loop instead of `while (k <= 3 && i <= 7)`, a named `base`
 * accumulated with `base += 0x80 << 11` instead of the inline
 * `(0xc0 << 14) + acc + (k << 18)`, and a named `z` for two literal-zero
 * arguments.
 *
 * CORPUS COUNT FOR THE SHAPE, so it is not mistaken for unreachable: the
 * pattern "two copies from one source register separated by one unrelated copy"
 * has 8 hits across 4,468 gcc-GENERATED .s files and 12 across 740 hand-written
 * ones.  This is NOT the zero-hit class that bounds OvlFunc_932_20082cc.  gcc
 * does emit it.
 *
 * ----- MEASURED PIN-FREE, ALL INERT OR WORSE (the park's pinned q-family stands) -----
 *
 *   a second `struct P *q` assigned `q = tp` between `i = 0` and `acc = 0`,
 *     loop 2's call taking q, loop 1 keeping tp          2, SAME two indices
 *   the same with `q = &t`                               2, SAME two indices
 *   loop 2's call taking `&t` directly                   2, SAME two indices
 *   `q = tp` beside `tp = &t`, loop 1 on q, loop 2 on tp 2, SAME two indices
 *   `q = &t` beside `tp = &t`, loop 1 on tp, loop 2 on q 2, SAME two indices
 *   q for loop 2's THREE STORES, tp for the call       105 of 271, and one
 *                                                       instruction SHORT
 *   `tp = &t` repeated at the bottom of loop 2, to give the pseudo a second
 *     set in the loop and spoil its movable status     119 of 271, 620 bytes
 *                                                       and 239 instructions
 *   `acc = 0` written BEFORE `i = 0`                     3 (idx 162,163,164)
 *   do { } while (0) between `i = 0` and `acc = 0`       5 (idx 154-156,163,164)
 *
 * The five inert rows all produced IDENTICAL CODE, which confirms the park's
 * "pseudo copies coalesce away" finding in the PIN-FREE setting and extends it:
 * making the two pointers' live ranges overlap does not stop the coalescing
 * either, in either direction.
 *
 * ----- AND ONE MECHANISM CORRECTION THAT RULES OUT THE BARRIER FAMILY -----
 *
 * A `do { } while (0)` does NOT split a basic block in sched2.  It plants a
 * reg_pending_sets_all TOTAL-ORDER ANCHOR: the first insn after its two loop
 * notes is the one sched_analyze_insn then treats it as a scheduling barrier
 * (haifa-sched.c:3714 onward, CONFIRMED IN THE COMPILER SOURCE): at :3744 it
 * gets a REG_DEP_ANTI on every reg_last_uses entry, at :3748 and :3751 a
 * dependence of type 0 -- REG_DEP_TRUE, NOT anti -- on every reg_last_sets and
 * reg_last_clobbers entry, at :3756 flush_pending_lists also clears the memory
 * lists, and at :3754 reg_pending_sets_all is set.  Then :3780-3789 assigns
 * reg_last_sets[i] = this insn FOR EVERY REGISTER i, which is exactly why the
 * next insn to write any register takes a REG_DEP_OUTPUT on it.
 * Here that is the wrong direction both ways -- an anchor at
 * `acc = 0` forces the hoist after it, and an anchor after `acc = 0` IS the
 * hoist and forces it after both inits.  For the hoist to issue before
 * `mov fp,r2` it needs the LOWER LUID, and sched2 has no instrument that
 * reaches LUID.  The reachable quantity is where loop.c's move_movables emits
 * it, and emit_insn_before(loop_start) puts a hoist last in the preheader by
 * construction.  So the open dimension is: STOP THE COPY BEING A HOIST -- which
 * is exactly what OvlFunc_968_200c610 shows is possible.
 *
 * NOTHING IN THE BODY BELOW IS CHANGED BY THIS BATCH.
 */
/* BODY REPLACED IN BATCH 300.  parkcheck caught this park's header lying about its
 * own body: the header claimed 2 of 271 while the body measured 271.  The 2-of-271
 * candidate was sitting in scratch_elev/b250/trio/final/e2bc_BEST.c, named in the
 * header but never installed -- so for fifty batches this park carried a worse body
 * than the one it described, and no tool could see the discrepancy because the park
 * had no Verify recipe either. */
/* Declarations inlined from the batch-250 scratch header hdr.h in batch 300. */
struct P {
    int f0;
    int f4;
    int f8;
    int fc;
    unsigned char pad10[0x18 - 0x10];
    unsigned short f18;
    unsigned char pad1a[0x1c - 0x1a];
    void *f1c;
    unsigned char pad20[0x22 - 0x20];
    unsigned short f22;
    unsigned char pad24[0x28 - 0x24];
};

extern char *iwram_3001ebc;
extern unsigned int iwram_3001e40;

extern unsigned int __Random(void);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(void *a, int f);
extern void __Func_8092950(int a, int b);
extern void __Func_8091ff0(int a);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __WaitFrames(int n);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8091e9c(int n);
extern void OvlFunc_968_2008118(int a, int b, int c, int d,
                                int e, int f, int g, struct P *p);


void OvlFunc_968_200c2bc(void)
{
    struct P t;
    char *p;
    unsigned int i;
    unsigned int j;
    unsigned int k;
    int acc;
    int n;
    int m;
    int n2;
    int s4;
    int s5;
    int s0;
    int s1;
    int s2;
    int s3;
    struct P *tp;
    struct P *q;

    p = iwram_3001ebc;
    *(int *)(p + (0xe0 << 1)) = 0x202;
    __CutsceneStart();
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    __Func_8092950(0, 0xf);
    __Func_8091ff0(0xaa);
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x28);
    __PlaySound(0xa2);
    j = 0;
    i = 0;
    tp = &t;
    acc = 0;
    do {
        t.f8 = (__Random() * 2 >> 16) * 0x4ccc + 0x17ffc;
        t.fc = (__Random() * 2 >> 16) * 0x4ccc + 0x17ffc;
        t.f22 = (__Random() * 0x1000 >> 16) + (0xf8 << 8);
mid:
        k = 0;
        while (k <= 3 && i <= 7) {
            OvlFunc_968_2008118(((__Random() * 7 >> 16) << 19) + (0xd8 << 18),
                                0, (0xc0 << 14) + acc + (k << 18), 0, 0, 0,
                                0x88 << 16, tp);
            k++;
        }
        __WaitFrames(3);
        if (i == 3) {
            if (j <= 2) {
                j++;
                goto mid;
            }
        }
        n = i + 3;
        s0 = 3;
        s1 = 1;
        __CopyMapTiles(0x30, n, 0x36, n, s0, s1);
        acc += 0x80 << 13;
        i++;
    } while (i <= 9);
    s2 = 5;
    s3 = 2;
    __CopyMapTiles(0x6f, 5, 0x75, 5, s2, s3);
    __CopyMapTiles(0x6f, 0xa, 0x75, 0xa, s2, s3);
    __CopyMapTiles(0x6f, 7, 0x6f, 5, s2, s3);
    __CopyMapTiles(0x6f, 7, 0x6f, 0xa, s2, s3);
    i = 0;
    acc = 0;
    do {
        t.f8 = (__Random() * 2 >> 16) * 0x4ccc + 0x17ffc;
        t.fc = (__Random() * 2 >> 16) * 0x4ccc + 0x17ffc;
        t.f22 = (__Random() * 0x1000 >> 16) + (0xf8 << 8);
        k = 0;
        while (k <= 3 && i <= 7) {
            OvlFunc_968_2008118(((__Random() * 7 >> 16) << 19) + (0xc0 << 18),
                                0, (0xc0 << 14) + acc + (k << 18), 0, 0, 0,
                                0x88 << 16, tp);
            k++;
        }
        __WaitFrames(3);
        m = i + 0x1a;
        n2 = i + 3;
        s4 = 3;
        s5 = 1;
        __CopyMapTiles(0x37, m, 0x30, n2, s4, s5);
        acc += 0x80 << 13;
        i++;
    } while (i <= 9);
    __PlaySound(0x121);
    __CutsceneWait(0x3c);
    __Func_8091e9c(0x15);
    __CutsceneEnd();
}
