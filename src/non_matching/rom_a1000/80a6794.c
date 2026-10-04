/* Func_80a6794  --  NON-MATCHING, 14 of 102 encodings
 *                   (ref 102, ours 102 -- COUNT EQUAL, so the figure IS a
 *                    distance.  SIZE EXACT.  RELOCATIONS CLEAN.)
 *
 *   RE-MEASURED batch 322, brief I.  The parks 14 stands and is ONE cause.
 *   Its ARITHMETIC DOES NOT: the park prices `box` at 6 references and
 *   floor_log2 2.  It is 8 references and floor_log2 3, and the real margin is
 *   11.4 percent, not the landslide the park implies.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_a1000/80a6794.c \
 *     asm/rom_a1000/rom_a5534_c_c_a_c.s --func Func_80a6794
 *
 * SPLIT SHAPE.  asm/rom_a1000/rom_a5534_c_c_a_c.s holds TWO functions
 * (Func_80a6614, Func_80a6794) and tools/datacheck.py reports no data section.
 * tools/split_s.py --dry-run asm/rom_a1000/rom_a5534_c_c_a_c.s Func_80a6794:
 *     would write asm/rom_a1000/rom_a5534_c_c_a_c_a.s  (1 function, 176 lines)
 *     would write asm/rom_a1000/rom_a5534_c_c_a_c_b.s  (1 function, 110 lines)
 *     would REMOVE asm/rom_a1000/rom_a5534_c_c_a_c.s, rewrite stage1.ld
 * So a landing installs at src/rom_a1000/rom_a5534_c_c_a_c_b.c with NO exports.
 * PINS: 0.
 *
 * ------------------------------------------------------------------------
 * THE 14, PER INDEX -- ONE CAUSE, CONFIRMED.  It is a genuine EXCHANGE, not a
 * one-way slip: ten indices have the ROM saying `sl` where we say `r8`
 * (7, 8, 26, 27, 32, 35, 39, 47, 55, 74) and four the mirror (43, 52, 66, 84).
 * Every other instruction, both loop preheaders and all five argument fills,
 * matches.  `g` is r10 in the ROM and r8 in ours; `box` is r8 in the ROM and
 * r10 in ours.
 *
 * (The parks prose has this BACKWARDS -- it reads "`g` and `box` are in r10 and
 * r8 where the ROM has them in r8 and r10", which contradicts its own greg
 * quote.  Index 7 is `mov sl, r3` in the ROM, immediately after loading the
 * global, and index 43 is `mov r8, r0`, the _CreateUIBox result.  The
 * MEASUREMENTS in the park are right; one sentence is inverted.)
 *
 * ------------------------------------------------------------------------
 * THE RUNG, PRICED.  `.18.greg`: `;; 9 regs to allocate: 37 36 49 57 39 53 32 34 38`
 * -- GLOBAL allocnos, so the denominator is `allocno[].live_length`, which is
 * exactly `.17.lreg` "across N insns".  `global.c:605`:
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
 * Those nine values reproduce greg published order EXACTLY, in that sequence.
 * That is the proof the formula and the denominator are the right ones.
 *
 * THE PARK IS WRONG ABOUT `box`.  It says "6 references, floor_log2 2", giving
 * 2*6 = 12 against g 30 and making this look hopeless.  `.17.lreg` says
 * `Register 34 used 8 times across 56 insns` -- floor_log2(8) = 3, so 3*8 = 24.
 * The eight comes from the six source references PLUS the two argument fills
 * inside the two `do` loops being counted at loop weight.  THE MARGIN IS
 * 4838 AGAINST 4285, i.e. 11.4 PERCENT, NOT A FACTOR OF ANYTHING.
 *
 * FOUR WAYS TO FLIP IT, all exact:
 *     n_refs(g)       10 -> 8       (24/62 = 3871 < 4285)   -- 9 is NOT enough
 *                                     (27/62 = 4354 still wins)
 *     n_refs(box)      8 -> 10      (30/56 = 5357 > 4838)   -- 9 is NOT enough
 *                                     (27/56 = 4821, loses by 17)
 *     live_length(box) 56 -> <= 48  (24/48 = 5000 > 4838)
 *     live_length(g)   62 -> >= 71  (30/71 = 4225 < 4285)   -- 70 TIES, and the
 *                                     tie-break `v1 - v2` on allocno number
 *                                     still puts 32 before 34, so 70 loses.
 *
 * ------------------------------------------------------------------------
 * A BOUND, WITH EVIDENCE -- A SECOND NAME CANNOT CHANGE REG_N_REFS.
 *
 * The obvious attack on n_refs(g) is to route some of gs uses through a copy:
 * `gb = g;` then `gb[0x110] = z;` and so on.  It does not work, and the reason
 * matters for every allocation park in this bank.  THREE such edits --
 *   `gb = g` carrying the four byte stores      (g refs 10 -> 7 intended)
 *   `gl = g` carrying the two loop preheaders   (g refs 10 -> 9 intended)
 *   `box2 = box` carrying the two loop uses     (box refs 8 -> 10 intended)
 * -- all measure 14, and `.17.lreg` shows allocator inputs BIT-IDENTICAL to this
 * body (32: 10/62, 34: 8/56, 36: 10/28, 37: 18/64, 39: 14/52 in all four).
 * Copy propagation rewrites every use back onto the original pseudo long before
 * `.17.lreg`, so the second name never exists by the time REG_N_REFS is counted.
 * SO: THE REF-COUNT ROUTE NEEDS GENUINELY DIFFERENT ADDRESS ARITHMETIC, which
 * changes the instruction stream.  Do not re-run the copy trick.
 *
 * That also means the flat rows below are NOT evidence against the declaration
 * lever; they are the batch-321 "screen allocation edits by .17.lreg, not by the
 * figure" rule firing.  Checked that way, every one of them is an edit that
 * never reached the allocator.
 *
 * MEASURED AND EXACTLY INERT AT 14 (allocator inputs verified identical):
 *   gb = g for the four byte stores
 *   gl = g for the two loop preheaders
 *   box2 = box for the two loop uses
 *   g[0x110] / g[0x112] spelled as literals instead of 0x88 << 1 / 0x89 << 1
 *   `two` dropped and 2 written as a literal in all three places
 *   `box` declared before `g`
 *
 * MEASURED AND WORSE:
 *   the second loop continuing from where p left off, no `g + 0x68`
 *                                                 99 differing, size -16
 *       -- this is the only edit that really does cut a `g` reference, and it
 *          costs the whole second preheader.  The ROM re-derives `g + 0x68`
 *          from `g`, so the shape is not available.
 *   `*(void **)(g + 0x20) = box` moved after the first byte store     17
 *   `r[5] = 0xd` after the g+0x44 store instead of before            17
 *   `n = 0x18` before `i = 8` in the second preheader                 16
 *       -- confirms the parks own 17 -> 14 finding from the other direction:
 *          the ROM wants `i` named before `n`, and this body has it right.
 *
 * CARRIED OVER FROM THE PARK AND NOT RE-TESTED: `g` retyped as a full
 * `struct St *` with named fields is byte-identical to `unsigned char *` plus
 * hand offsets.  Consistent with the bound above -- the typed form produces the
 * same addressing, hence the same REG_N_REFS.
 *
 * NEXT, CONCRETELY: this needs box live_length cut by 8 insns, or g live_length
 * grown by 9, with the instruction stream unchanged.  Both are REORDERING
 * questions, not spelling questions, and neither is reachable from the edits
 * tried here.  `box` is born at the _CreateUIBox result and dies in the second
 * loop; `g` is born at the top and dies in the second loop preheader, so their
 * ranges nest and the 8-insn gap between them is structural.  A brief with
 * budget should look at whether the two `do` loops can be given a different
 * BLOCK shape -- that is the only thing left that moves a live_length without
 * moving an instruction.
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
