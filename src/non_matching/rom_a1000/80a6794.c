/* Func_80a6794  --  PARK, 14 differing encodings of 102.
 *
 * RE-MEASURED batch 325, brief F.  ref 224 bytes / 102 encodings / 102
 * instruction rows; ours 224 bytes / 102 encodings / 102 instruction rows;
 * RELOCATIONS IDENTICAL.  So the figure IS a distance, not a misalignment.
 * PINS: 0.  DEVICES: 0.  FLAG GROUPS: 0.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_a1000/80a6794.c asm/rom_a1000/rom_a5534_c_c_a_c.s --func Func_80a6794
 *
 * SPLIT SHAPE.  asm/rom_a1000/rom_a5534_c_c_a_c.s holds TWO functions
 * (Func_80a6614, Func_80a6794) and tools/datacheck.py reports no data section.
 * tools/split_s.py --dry-run asm/rom_a1000/rom_a5534_c_c_a_c.s Func_80a6794:
 *     would write asm/rom_a1000/rom_a5534_c_c_a_c_a.s  (1 function, 176 lines)
 *     would write asm/rom_a1000/rom_a5534_c_c_a_c_b.s  (1 function, 110 lines)
 *     would REMOVE asm/rom_a1000/rom_a5534_c_c_a_c.s, rewrite stage1.ld
 * So a landing installs at src/rom_a1000/rom_a5534_c_c_a_c_b.c with NO exports.
 *
 * ------------------------------------------------------------------------
 * THE 14, PER INDEX -- ONE CAUSE, and batch 325 PROVED it is the only one.
 * It is a genuine EXCHANGE: ten indices have the ROM saying `sl` where we say
 * `r8` (7, 8, 26, 27, 32, 35, 39, 47, 55, 74) and four the mirror (43, 52, 66,
 * 84).  Every other instruction, both loop pre-headers and all five argument
 * fills, matches.  `g` is r10 in the ROM and r8 in ours; `box` is r8 in the ROM
 * and r10 in ours.  The park prose before batch 322 had this sentence inverted;
 * its measurements were right.
 *
 * ===== PROVED WITH AN INSTRUMENT (batch 325): THE ORDER IS THE WHOLE 14 =====
 * scratch_elev/b325/F/work/s7.c appends ONE `*(void **)(g + 0x20) = box;` after
 * both loops.  That is a DEVICE -- three extra instructions, 232 bytes against
 * 224, 18 of 102 as a figure -- but it moves `.17.lreg` to g 11/75 = 4400 and
 * box 9/57 = 4736, which flips greg's published order to
 * `37 36 49 57 39 53 34 32 38` and its dispositions to **32 in 10, 34 in 8 --
 * the ROM's assignment**.  Its diff is then ONLY the three added instructions
 * and the 4-byte pool shift: ALL 14 REGISTER DIFFERENCES GO TO ZERO.  There is
 * no second cause hiding behind the first.
 *
 * ------------------------------------------------------------------------
 * THE RUNG, PRICED.  `.18.greg`: `;; 9 regs to allocate: 37 36 49 57 39 53 32 34 38`
 * -- GLOBAL allocnos, so the denominator is `allocno[].live_length`, which is
 * exactly `.17.lreg` "across N insns".  `global.c:607` (allocno_compare), with
 * a real `(double)` division and no frequency and no loop-depth term:
 *
 *     pri = floor_log2(n_refs) * n_refs / live_length * 10000 * size
 *
 *     allocno  role     n_refs  live_length  priority
 *       37                 18       64        11250
 *       36                 10       28        10714
 *       49                  2        2        10000
 *       57                  2        2        10000   (tie, broken by number)
 *       39                 14       52         8076
 *       53                  2        3         6666
 *       32     g           10       62         4838   <-- first, takes r8
 *       34     box          8       56         4285   <-- then r10
 *       38                  6       56         2142
 *
 * Those nine values reproduce greg's published order EXACTLY, in that sequence.
 *
 * WHY r8 BEFORE r10, so nobody re-derives it.  `REG_ALLOC_ORDER` (arm.h:989) runs
 * `... 6, 7, 8, 10, 9, 11, ...` -- r8 precedes r10, so whichever of the pair is
 * allocated FIRST takes r8.  `find_reg`'s pass 0 (global.c:1015) ors in
 * `~regs_used_so_far`, and `regs_used_so_far` starts as
 * `regs_ever_live | call_used_regs` (global.c:385-397): r8..r11 are in neither
 * before greg, so **pass 0 can never reach a hi register here** and the choice is
 * pass 1's plain REG_ALLOC_ORDER walk.  `hard_reg_copy_preferences` and
 * `hard_reg_preferences` cannot redirect it either -- g's only copy preference is
 * r0, which `AND_COMPL_HARD_REG_SET (…, used)` has already removed because r0 is
 * call-used and g crosses 6 calls.  **So the register identities are not a lever;
 * only the allocation order is.**
 *
 * ------------------------------------------------------------------------
 * THE FOUR ROUTES, PRICED AGAINST WHAT SOURCE CAN REACH (batch 325).
 *
 * `REG_N_REFS (regno) += pbi->bb->loop_depth + 1` (flow.c:4948), which is why
 * box's six source references count 8 (its two in-loop ones count double) and
 * g's ten count 10.  `loop_depth` is recomputed from REAL CFG BACK EDGES by
 * `flow_loops_find` / `find_loop_nodes_find` (flow.c:7295-7322), NOT from
 * front-end loop notes.  **So a `do { } while (0)` wrapper cannot raise
 * loop_depth** -- there is no back edge.  That closes the cheapest-looking route
 * before anyone spends a round on it.
 *
 * Targets, re-derived (one correction to the earlier header):
 *   g 10 refs: needs live_length >= 71.  70 TIES at 4285 and the tie-break
 *              `v1 - v2` on allocno number still puts 32 first, so 70 loses.
 *   g  9 refs: needs live_length >= 64.
 *   box 8 refs: needs live_length <= 49.  (The earlier header said <= 48; 49
 *              gives 4897, which already beats 4838.)
 *   box 9 refs: needs live_length <= 55.  57 gives 4736 and 56 gives 4821 --
 *              BOTH lose.  This is the near miss.
 *
 * MEASURED RANGE ACROSS 14 BODY SHAPES, every one screened on `.17.lreg` and not
 * on the figure: **g's live_length only ever reaches 60..63, and box's 55..57.**
 *   park body                                14   g 10/62=4838  box 8/56=4285
 *   both loops `for`                         18   g 10/61        box 8/56
 *   both loops `while`                       14   g 10/62        box 8/56
 *   `box` declared first                     14   g 10/62        box 8/56
 *       -- the pseudo NUMBERS swap and greg's order swaps with them; the
 *          assignment is unchanged, so declaration order is not a lever
 *   `p =` last in each pre-header            59   SIZE 228  g 10/63  box 8/56
 *   `p =` FIRST in each pre-header           20   g 10/60=5000   box 8/56
 *   `i = 8` last in pre-header 2             18   g 10/61        box 8/56
 *   y written as 0x60 + i * 0x10 in-loop     14   g 10/62        box 8/56
 *   `p[i]` instead of `*p++`                 18   g 10/63        box 8/56
 *   second loop reuses `i`                   37 RELOC  g 10/62  box 8/55=4363
 *   both loops index `g` directly            18   g 10/63        box 8/56
 *   one base indexed i = 0..15               47   SIZE 228  g 10/63  box 8/57
 *   only loop 2 indexes `g`                  24   g 10/63        box 8/56
 *   only loop 1 indexes `g`                  24   g 10/62        box 8/56
 *
 * NEW BOUND, WITH ITS EVIDENCE: **indexing `g` inside a loop does NOT add
 * references to `g`.**  Four of the shapes above move one or both loop bodies to
 * `((void **)(g + 0x48))[i]`, a reference to `g` at loop_depth 1 that should be
 * worth +2 each.  It is worth ZERO: `strength_reduce` replaces it with a giv
 * before `.17.lreg` is written, and all four read g at 10 refs.  The one route
 * that could plausibly reach the +9 is closed by the very pass that produces the
 * ROM's `stmia r6!` in the first place.
 *
 * WHY THE LENGTHS ARE STRUCTURAL.  `box`'s last reference is in loop 2's body,
 * so box is live on loop 2's back edge and therefore across ALL of loop 2
 * whatever the order inside the body: its range is [birth at `_CreateUIBox`, end
 * of loop 2] and reordering cannot shorten it.  `g`'s last reference is in loop
 * 2's PRE-HEADER, so its range ends there, and that pre-header is 4 insns -- the
 * entire reordering budget, which is the 60..63.  Growing g past 71 needs a
 * reference to `g` at or after loop 2, and every such reference costs
 * instructions (the s7 instrument: +3) or is a device (a trailing
 * `g[0x110] = g[0x110];` is deleted after greg and reads g 11/75 = 4400, still
 * short of the 78 that 11 refs would need).
 *
 * ------------------------------------------------------------------------
 * A BOUND CARRIED FORWARD -- A SECOND NAME CANNOT CHANGE REG_N_REFS.
 * THREE such edits -- `gb = g` carrying the four byte stores, `gl = g` carrying
 * the two loop pre-headers, `box2 = box` carrying the two loop uses -- all
 * measure 14, with `.17.lreg` showing allocator inputs BIT-IDENTICAL to this body
 * (32: 10/62, 34: 8/56, 36: 10/28, 37: 18/64, 39: 14/52 in all four).  Copy
 * propagation rewrites every use back onto the original pseudo long before
 * `.17.lreg`.  Do not re-run the copy trick.
 *
 * MEASURED AND EXACTLY INERT AT 14 (allocator inputs verified identical):
 *   gb = g for the four byte stores; gl = g for the two loop pre-headers;
 *   box2 = box for the two loop uses; g[0x110] / g[0x112] spelled as literals
 *   instead of 0x88 << 1 / 0x89 << 1; `two` dropped and 2 written as a literal in
 *   all three places; `box` declared before `g`.
 *
 * MEASURED AND WORSE:
 *   the second loop continuing from where p left off, no `g + 0x68`
 *                                                 99 differing, size -16
 *       -- the only edit that really does cut a `g` reference, and it costs the
 *          whole second pre-header.  The ROM re-derives `g + 0x68` from `g`.
 *   `*(void **)(g + 0x20) = box` moved after the first byte store     17
 *   `r[5] = 0xd` after the g+0x44 store instead of before            17
 *   `n = 0x18` before `i = 8` in the second pre-header                16
 *       -- confirms the 17 -> 14 finding from the other direction: the ROM wants
 *          `i` named before `n`, and this body has it right.
 *
 * CARRIED OVER AND NOT RE-TESTED: `g` retyped as a full `struct St *` with named
 * fields is byte-identical to `unsigned char *` plus hand offsets.
 *
 * NEXT, CONCRETELY.  The question is no longer which shape but whether C can
 * reference `g` at or after loop 2, or reference `box` a ninth time while
 * shortening its range, at ZERO instruction cost.  Fourteen shapes say no and the
 * two live ranges are structural for the reasons above.  The remaining candidates
 * are outside this function: whether the ROM's object was built from a TU whose
 * neighbours changed the pseudo numbering (the tie-break is `v1 - v2`), or whether
 * one of the nine allocnos can be removed entirely so the ordering changes around
 * it.  A plain reordering search is exhausted.
 */
extern unsigned char *iwram_3001f2c;
extern void *Func_80a1814(void *g);
extern void Func_80a1870(void *q, int a, int b, int c, int d);
extern void *_CreateUIBox(int a, int b, int c, int d, int e);
extern char *Func_80a1778(void *box, int b, int c);
extern void _Func_801ec6c(int a, int b, int c, void *box, int e, int f);
extern void *_Func_801eb64(int a, int b, void *box, int y, int n);

void Func_80a6794(void)
{
    unsigned char *g;
    void *q2;
    void *box;
    char *r;
    void **p;
    int i;
    int n;
    int y;
    int z;
    int two;

    g = iwram_3001f2c;
    q2 = Func_80a1814(g);
    z = 0;
    Func_80a1870(q2, 2, 2, 8, z);
    two = 2;
    box = _CreateUIBox(0, 5, 0x1e, 0xf, two);
    *(void **)(g + 0x20) = box;
    g[0x88 << 1] = z;
    g[0x111] = z;
    g[0x89 << 1] = 8;
    g[0x113] = two;
    r = Func_80a1778(box, 0, 4);
    r[5] = 0xd;
    *(char **)(g + 0x44) = r;
    _Func_801ec6c(0, 0, 0, box, z, z);
    i = z;
    n = 8;
    p = (void **)(g + 0x48);
    y = 0x60;
    do {
        *p++ = _Func_801eb64(4, i, box, y, n);
        i++;
        y += 0x10;
    } while (i <= 7);
    i = 8;
    n = 0x18;
    p = (void **)(g + 0x68);
    y = 0x60;
    do {
        *p++ = _Func_801eb64(4, i, box, y, n);
        i++;
        y += 0x10;
    } while (i <= 0xf);
}
