/* Func_801c34c -- PARK HOLDS AT 8 of 69.  Batch 328 brief D.
 * THE PARK'S OWN "NAMED NEXT MOVE" IS NOW MEASURED AND REFUTED, AND THE
 * RESIDUE IS BOUNDED BY AN IDENTITY BETWEEN TWO PRIORITIES.
 *
 *   8 differing encodings of 69.  ref 69 encodings, ours 69 -- EQUAL.
 *   SIZE EQUAL (156 bytes).  RELOCATIONS IDENTICAL.  POOL IDENTICAL.
 *   Memory profile ldr=7 ldrsh=2 str=7 strh=1 = the reference's.
 *   First differing index 2.  PINS 0.  Figure independently re-derived.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_15000/801c34c.c asm/rom_15000/rom_1aeec_c_a_a_a_a_a_c_c_c_c.s --func Func_801c34c
 *
 * ============ REFUTED: THE PARK'S NAMED NEXT MOVE, WITH A NUMBER ============
 *
 * The park said: "A source change that closes A must move `mov r1,#0xe1` above
 * the first `ldrsh` IN THE PRE-SCHED RTL while leaving the emitted order
 * alone."  Done, by naming the two halfword reads as locals in reverse order:
 *
 *   int a2 = *(short *)(g + (0xe1 << 1));
 *   int a1 = *(short *)(g + (0xe0 << 1));
 *   id = _GetLocationName(a1, a2);          ->  22 of 69, counts equal, no MEM
 *
 * The CONTROL (a1 then a2) is 8, EXACTLY INERT -- so the 22 is the reversal,
 * not the naming.  Reversing the pre-sched order of the two reads costs 14
 * encodings.
 *
 * ============ THE BOUND: prio(21) = prio(212) + 1 IDENTICALLY ============
 *
 * Residue A is indices 2-9, an identical multiset in a different sched2 order.
 * The ROM issues 10, 212, 12, 21, 15, 200, 57, 60, 201; we issue
 * 10, 21, 200, 212, 12, 201, 15, 57, 60.  Priorities re-read from
 * `.23.sched2`'s Region Dependences table (the park's numbers reproduce
 * exactly):
 *   10:182  21:181  200:181  212:180  12:180  15:180  201:180
 *   57:179  60:179  28:179  32:178  202:178  203:177  38:176  42:175  47:174
 * The ROM needs 212 scheduled BEFORE 21.  Priority is rank_for_schedule's
 * FIRST rung, so no class / dependent-count / INSN_LUID reordering can reach
 * it -- the LUID lever cannot touch this residue, as the park already said.
 *
 * WHAT IS NEW IS THAT prio(21) AND prio(212) MOVE TOGETHER.  Three edges, all
 * read out of `.19.flow2` and `arm.c`:
 *   * insn 21 is `ldr r2,=gState`, a CONSTANT-POOL LOAD, so its result_ready
 *     cost is 2, and its only high consumer is insn 28 `add r3,r2,r1`:
 *         prio(21) = prio(28) + 2.
 *     `arm_adjust_cost` (arm.c:2416-2453) cannot reduce it: insn 28's SET_SRC
 *     is a PLUS, not a MEM, so the load-after-store rung (:2434-2449) does not
 *     apply, and the CALL_INSN rung (:2430-2432) needs a call consumer.
 *   * insn 57 is `str r3,[sp,#16]` (`w = 8`) and ANTI-depends on insn 28,
 *     which writes r3.  `arm_adjust_cost` returns 0 for REG_DEP_ANTI
 *     (arm.c:2425-2427), so prio(57) = prio(28).
 *   * insn 212 is `sub sp,#20` and 57 TRUE-depends on it, cost 1:
 *         prio(212) = prio(57) + 1 = prio(28) + 1.
 *   => prio(21) = prio(212) + 1 FOR EVERY VALUE OF prio(28).
 *
 * AND prio(28) CANNOT DROP BELOW 179 EITHER, which closes the other door:
 *   insn 32 is `*thumb_extendhisi2_insn`, a PARALLEL carrying
 *   `(clobber (reg:SI 1 r1))` -- that clobber is the zero offset register the
 *   ROM emits as `mov r1,#0`, and it is where the park's "insn 32 clobbers r1"
 *   comes from.  It gives an OUTPUT dependence 32 -> 202 (`mov r1,#0xe1`), so
 *   prio(32) >= prio(202), and 28 ANTI-depends on 202 as well, so
 *   prio(28) >= prio(202) = 178.  prio(202) = prio(47) + 4 through the second
 *   argument's own chain 202 -> 203 -> 38 -> 42 -> 47 (mov, lsl, add, ldrsh,
 *   call) -- which the ROM emits INSTRUCTION FOR INSTRUCTION, so that chain is
 *   not a free variable.
 *
 *   >> BOUND, with its evidence attached: insn 21 strictly outranks insn 212
 *      in every body where gState's pool load feeds the first index add and the
 *      frame store of `w` anti-depends on that add's destination register.
 *      Both facts are visible in `.19.flow2`.  WHAT WOULD RETIRE IT: a body in
 *      which insn 21 is NOT READY at the cycle 212 must issue -- i.e. the
 *      gState pool load acquires an incoming dependence -- or one in which the
 *      first index add does not consume a pool load.  Neither is reachable by
 *      reordering the opening, which is what every measured row varies. <<
 *
 * ============ MEASURED THIS BATCH.  CROSSED, NOT ONE AT A TIME ============
 * All against the base at 8 (tools/crossfire.py, depth 2, two runs):
 *   reversed named reads (the park's named move)                 22
 *   named reads in ROM order (control)                            8  INERT
 *   `((short *)g)[0xe0]` / `[0xe1]` array indices                 8  INERT
 *   `w`/`h` moved after `g = &gState`                             8  INERT
 *   st->f230 written as *(void **)((char *)st + (0x8c << 2))      8  INERT
 *   the previous three crossed, pairwise                          8  INERT
 *   `g` declared `short *`                                        9
 *   the same crossed with each inert row above                    9
 *   a comma-sequenced duplicate call (instrument)                 70  at 72 insns
 *
 * THE LANDED MODULE-MATE'S LEVER DOES NOT TRANSFER, and this is worth the row.
 * src/overlays/rom_7bdeb0/ovl_169c_a_a_a_c.c is a LANDED file that reads
 * `gState + 0x1c0` through the SAME `mov #0xe0 / lsl #1` pair and its header
 * says the offset "is written as a named local built in statement form, not
 * folded into either access.  Folded, both become addressing-mode constants
 * and the shared r2 disappears."  Transplanted here, in both the two-variable
 * and the one-reused-variable form:
 *   o1 = 0xe0; o1 <<= 1; o2 = 0xe1; o2 <<= 1;                    66  at 65 insns
 *   one `o` reassigned between the two reads                      66  at 63 insns
 * Both LOSE instructions -- the statement form FOLDS here, where in the
 * overlay it did not, because here the two offsets are used ONCE each and
 * there is no second consumer to keep them alive.  The park's folded
 * `(0xe0 << 1)` is correct.
 *
 * STATUS: OPEN at 8.  Residue B (the pooled HImode 0x5a) stays closed by the
 * typed `Blk`.  Residue A is now bounded by the priority identity above
 * rather than by "nothing we tried worked".
 */
/* Func_801c34c -- asm/rom_15000/rom_1aeec_c_a_a_a_a_a_c_c_c_c.s   (PARK)
 *
 * NON-MATCHING, 8 of 69 encodings.  Was 17.  COUNT NOW EQUAL (ref 69, ours 69),
 * SIZE EQUAL (156 bytes), RELOCATIONS IDENTICAL, POOL IDENTICAL (4 words, the
 * ROM's order).  So unlike the previous figure, THIS ONE IS A DISTANCE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/801c34c.c \
 *     asm/rom_15000/rom_1aeec_c_a_a_a_a_a_c_c_c_c.s --func Func_801c34c
 *
 * PINS: 0.
 *
 * ================= THE OLD 17 WAS TWO CAUSES, 9 AND 8 =================
 *
 * RESIDUE B -- the pooled HImode 0x5a -- IS CLOSED.  It was worth 9 of the 17
 * and it owned the whole count difference and the whole relocation difference:
 * 2 real instructions (idx 54, 55), 4 pool words (65, 67, 68, 69) and 3
 * pure pc-offset consequences (1, 19, 58).  The extra pool word landed FIRST
 * because *thumb_movhi_insn's load alternative has pool_range 64 against
 * movsi's 1020, so a HImode fix sorts ahead of every SImode one.
 *
 * THE FIX CAME FROM A LANDED SIBLING IN THIS BANK, NOT FROM A SPELLING.
 * src/rom_15000/rom_15e8c_c_a_c_a_a_a.c (Func_80173ac, five halfword stores of
 * small literals through iwram_3001e8c) says it outright: a constant stored to
 * a `short` through a cast pool is pooled as a halfword, and "declared as
 * struct members at their real offsets [...] the stored constants come out as
 * immediates".  Giving iwram_3001ebc a type with the two touched fields as
 * members (`void *f230` at 0x230, `short f234` at 0x234) is worth all 9.
 * Three spellings reach 8: typed pointer (shipped), `((Blk *)st)->f234`, and
 * the member declared `unsigned short`.  The park's own best remedy for B, an
 * int local beside the store, is worth only 3 of the 9 (14), and an int local
 * declared at the top is worth 0 (17).
 *
 * The old header's "-fno-schedule-insns2 gives the ROM's PRE-SCHEDULER order
 * EXACTLY" is REFUTED on this body: it reads 31.  (It is not a prologue-only
 * probe either -- sched2 is what produces the other 59 correct encodings.)
 *
 * ============ RESIDUE A -- 8 of 69, AND THE NUMBER THAT DECIDES IT ============
 *
 * Indices 2-9, all real instructions, identical multiset, pure sched2 order:
 *     idx   ref                      ours
 *       2   sub  sp,#20              ldr  r2,=gState
 *       3   ldr  r6,[r3,#0]          movs r1,#0xe0
 *       4   ldr  r2,=gState          sub  sp,#20
 *       5   movs r3,#8               ldr  r6,[r3,#0]
 *       6   movs r1,#0xe0            lsls r1,r1,#1
 *       7   str  r3,[sp,#16]         movs r3,#8
 *       8   str  r3,[sp,#12]         str  r3,[sp,#16]
 *       9   lsls r1,r1,#1            str  r3,[sp,#12]
 *
 * From -da -fsched-verbose=6 (x.c.23.sched2): basic block 0 is the WHOLE
 * function, insn 182..218, with no branch anywhere, and the opening ready list
 * is {10, 21, 200, 212}:
 *     10  ldr r3,=iwram_3001ebc  prio 182  cost 2
 *     21  ldr r2,=gState         prio 181  cost 2
 *     200 mov r1,#0xe0           prio 181  cost 1
 *     212 sub sp,#20             prio 180  cost 1
 *     12  ldr r6,[r3]            prio 180  cost 2
 *     57/60 str r3,[sp,#16/#12]  prio 179  cost 2
 * The ROM's order is 10, 212, 12, 21, 15, 200, 57, 60, 201, which needs 21 and
 * 200 to rank BELOW 212.  181 > 180 is a strict priority win, so
 * rank_for_schedule's lower rungs -- class-vs-last-scheduled, depend_count,
 * INSN_LUID -- are never reached.  THE LUID LEVER CANNOT TOUCH THIS RESIDUE.
 *
 * Where the 181s come from:
 *     prio(21)  = prio(28) + 2        prio(200) = prio(201) + 1 = prio(28) + 2
 *     prio(28)  = prio(32) + 1 = 179
 *     prio(32)  = 178, THROUGH THE OUTPUT DEPENDENCE 32 -> 202 (insn 32's
 *                 `ldrsh` clobbers r1; insn 202 `mov r1,#0xe1` rewrites it) --
 *                 not through the call
 *     prio(212) = prio(57) + 1 = 180
 *     prio(57)  = 179, THROUGH THE ANTI DEPENDENCE 57 -> 28 (57 reads r3, 28
 *                 rewrites it)
 *
 * So ONE quantity decides it: prio(28).  At 178 instead of 179, both 21 and 200
 * drop to 180 and the four-way tie goes to the lower rungs.  prio(28) is one
 * greater than prio(32), and prio(32) is pinned by the output dependence on the
 * SECOND argument's index constant.  A source change that closes A must move
 * `mov r1,#0xe1` above the first `ldrsh` IN THE PRE-SCHED RTL while leaving the
 * emitted order alone -- which is the shape the old header's "halfword reads
 * hoisted into locals" (19) was reaching for from the wrong end.
 *
 * ============== MEASURED INERT FOR A -- CROSSED, NOT ONE AT A TIME ==============
 * g-form (5) x w/h-form (3) x placement (4), plus all 24 permutations of the
 * four opening statements.  All against the B-fixed base at 8:
 *     all 4 g-locals x all 3 in-prologue placements, `w=8; h=8;`
 *                                                      8, BIT-IDENTICAL
 *     any g-form with w/h moved after the call         18, RELOCDIFF
 *     `&gState` inlined with no local                  66, dsize -12
 *     the 24 opening permutations:  four read 7, four 8, eight 9, eight 15
 *
 * THE FOUR 7s ARE A FALSE IMPROVEMENT AND ARE RECORDED AS ONE.  Every 7 has
 * `h = 8` before `w = 8`.  &w is sp+16 and &h is sp+12 (from the matching
 * `add r1,sp,#16` / `add r2,sp,#12` argument block), so the ROM stores sp+16
 * then sp+12 -- w then h.  The 7 variants store them in the OPPOSITE ORDER and
 * gain a line only because one of the two mis-ordered stores happens to land on
 * the ROM's other store.  The structurally correct body is the 8.
 */
typedef struct { unsigned char b[704]; } GlobalState;
extern GlobalState gState;
extern int _GetLocationName(int a, int b);
extern void TextBox(int id, int *a, int *b, int *c, int *d);
extern void *CreateUIBox(int a, int b, int c, int d, int e);
extern void DrawSmallText(int id, void *w, int x, int y);
extern int StartTask(void *f, int prio);
extern void Func_801c3e8(void);

typedef struct { unsigned char pad[0x230]; void *f230; short f234; } Blk;
extern Blk *iwram_3001ebc;

void Func_801c34c(void)
{
    Blk *st;
    unsigned char *g;
    int w, h, tw, th;
    int id;
    void *box;
    st = iwram_3001ebc;
    w = 8;
    h = 8;
    g = (unsigned char *)&gState;
    id = _GetLocationName(*(short *)(g + (0xe0 << 1)), *(short *)(g + (0xe1 << 1)));
    id += 0x99b;
    TextBox(id, &w, &h, &tw, &th);
    w = (0x1e - tw) >> 1;
    h = (0xa - th) >> 1;
    box = CreateUIBox(w, h, tw, th, 2);
    st->f230 = box;
    DrawSmallText(id, box, 0, 0);
    st->f234 = 0x5a;
    StartTask(Func_801c3e8, 0xc8 << 4);
}
