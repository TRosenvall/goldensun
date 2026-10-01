/* OvlFunc_913_2008d3c (0x02008d3c) -- NO CANDIDATE WRITTEN, NO objcmp FIGURE.
 *
 * This is a TRIAGE park. No candidate .c exists for this function, so no
 * "N of M" is claimed and none may be quoted from it. The reason is below, and
 * it is a measurement, not a shrug: this function carries 2.5x the hand-written
 * work of the second target in the same batch and 12x the first, and the brief's
 * triage had it ranked SECOND of three.
 *
 * When a candidate exists, verify it with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7a04ac/2008d3c.c \
 *     asm/overlays/rom_7a04ac/ovl_30_c_c_c_a_a_a_c.s --func OvlFunc_913_2008d3c
 *
 * SPLIT SHAPE: NONE NEEDED. asm/overlays/rom_7a04ac/ovl_30_c_c_c_a_a_a_c.s
 * holds exactly ONE function, this one. `python3 tools/datacheck.py` on it
 * reports NO data section, so deleting the .s takes no symbol with it and the
 * linker script needs no change.
 * SHIMS: none -- there is no candidate.
 * FRAME: NONE. No `sub sp` and no `mov rX, sp` in 2519 instructions, so no
 * spill slot exists and none of the aggregate-ordering or
 * declaration-order-for-spill-slots material applies.
 * 16 high-register mentions; prologue `push {r5,r6,r7}` after moving r8/r9/r10
 * down, so THREE call-saved high registers, same as the other two targets.
 *
 * ============================================================
 * THE TRIAGE CORRECTION, AND IT IS THE USEFUL PART OF THIS PARK
 * ============================================================
 * The batch brief ranked these three by high-register mentions and max pool
 * reload. Neither axis separates them, and both got the order wrong:
 *
 *   function              insns   high-reg   max reload   ACTUAL difficulty
 *   OvlFunc_899_200b6f8    1459       17          8       EASIEST  (94.4%)
 *   OvlFunc_957_20093f8    2316       19         10       middle   (94.0%)
 *   OvlFunc_913_2008d3c    2519       16         12       HARDEST  (no candidate)
 *
 * High-register mentions run 17 / 19 / 16 -- the HARDEST function has the
 * FEWEST. Max pool reload is the only column that happens to order them, and it
 * does so for the wrong reason (it tracks function length).
 *
 * THE AXIS THAT WORKS ON THIS POPULATION IS *WORK DENSITY*: the share of
 * instructions that are neither a call nor argument-fill. For a cutscene script
 * almost every instruction is `mov`/`lsl`/pooled-`ldr` feeding a `bl`, and all
 * of that is MECHANICAL -- a symbolic walk over the reference emits it. What
 * costs human effort is the remainder: memory accesses, bit manipulation and
 * branches, each of which needs a hand-written statement.
 *
 *   function              insns     bl    fill    work   work%   hand sites
 *   OvlFunc_899_200b6f8    1459    387    1016      56    3.8%        20
 *   OvlFunc_957_20093f8    2316    703    1492     121    5.2%        95
 *   OvlFunc_913_2008d3c    2519    672    1562     285   11.3%       236
 *
 *   fill = mov + lsl + pooled ldr;  work = insns - bl - fill
 *   "hand sites" is the count of reference instructions a symbolic transcriber
 *   cannot emit, i.e. the statements that must be written and understood.
 *
 * work% orders the three correctly and the hand-site count is very nearly
 * proportional to `work` (0.36 / 0.79 / 0.83 sites per work instruction -- the
 * 899 figure is lower only because its 56 work instructions include three
 * repeats of one idiom). THE INSTRUCTION COUNT IS A BAD PREDICTOR HERE: 913 is
 * only 9% longer than 957 but carries 2.4x the work instructions.
 *
 * This is the same correction batch 311 made from the other direction -- there
 * the FRAME TRIAD beat instruction count, here WORK DENSITY beats both
 * instruction count and the high-register count. The general form: on a
 * population where most instructions are mechanical, triage on the share that
 * is NOT mechanical, not on the total.
 *
 * ============================================================
 * WHAT IS ALREADY KNOWN ABOUT THIS FUNCTION, so the next pass starts here
 * ============================================================
 *  - STRUCTURE: 2519 instructions, 672 `bl` over 43 distinct callees. Control
 *    flow is much heavier than its two batch-mates: 34 `cmp`, 28 `beq`,
 *    6 `bne`, 17 `b`, 51 labels. Comparison census for THIS function (do not
 *    import either batch-mate's): 28 `beq` / 6 `bne` / 0 `blt` / 0 `ble` /
 *    0 `bgt` / 0 `bge` -- ZERO SIGNED COMPARES, so every test is an equality
 *    test and no signed-loop-shape lever applies. 957's `ble`/`bge` loop
 *    spellings would corrupt all 34 sites here.
 *  - SIX UNDEFINED `.L` SYMBOLS ARE GLOBAL VARIABLES, not labels. `.L3384`,
 *    `.L3388`, `.L338c`, `.L3390`, `.L3394` and `.L3398` are loaded as
 *    ADDRESSES (`ldr r6, =.L3394`) and then dereferenced (`str r0, [r6]`,
 *    `ldr r3, [r3]`), and NONE of them is defined anywhere in the reference
 *    file. They are RAM addresses the disassembler named. The tree already has
 *    the convention for this, in the file-mate park
 *    src/non_matching/ovl_7e3e08/200909c.c:
 *        extern unsigned char *L3f6c __asm__(".L3f6c");
 *    so these need six such declarations. `.L3394` is reached TWELVE times --
 *    more than any genuine constant in the function -- and the first use is
 *    `ldr r6, =.L3394; bl __GetFlag; str r0, [r6]`, i.e. it caches a flag
 *    result. That is the function's central state word and getting its
 *    re-read-versus-hold behaviour right is likely to dominate the residue, the
 *    way it does in the file-mate park.
 *  - FIVE SIBLING CALLEES in the same overlay -- OvlFunc_913_200a768,
 *    _200a780, _200a7c8, _200a974, _200aad8 -- plus one cross-overlay script
 *    symbol gScript_911__0200ae20, which is evidence the 911 and 913 overlays
 *    share script data and should be checked before any script symbol here is
 *    declared locally.
 *  - 43 distinct pooled constants, the largest set of the three. Message ids
 *    cluster at 0x147c/0x1488/0x1489/0x149d/0x14b4/0x14b6/0x14bf, each pooled
 *    ONCE -- so, exactly as in OvlFunc_957_20093f8 and unlike
 *    OvlFunc_899_200b6f8, THERE IS NO WALKED MESSAGE BASE here and that lever
 *    must not be imported. The screen is the pool MULTIPLICITY, not the
 *    adjacency of the values.
 *  - Three pooled words are 0x2610000 / 0x25b0000 / 0x2750000 / 0x25a0000 and
 *    two are 0x406218 / 0x405210 -- ROM and IO addresses respectively, not
 *    scalars; they want pointer spellings, not literals.
 *
 * ============================================================
 * WHAT TO DO FIRST, and what carries over from this batch
 * ============================================================
 * The two levers that both batch-mates confirmed should be applied before
 * anything subtle is attempted, because together they were worth 8-10 points:
 *   1. THE SELECTIVE PIN PASS -- pin every call carrying a literal outside
 *      0..255 or a shifted constant, filling in the reference's own order.
 *      Measured +8.1 points on OvlFunc_899_200b6f8 and +7.0 on
 *      OvlFunc_957_20093f8, and in BOTH cases the selective pass beat the
 *      blanket "pin everything" pass while using fewer pins.
 *   2. THE COMMUTATIVE-DESTINATION RULE at the `and`/`orr` sites -- there are
 *      12 `and` and 8 `orr` here. The destination is the LOADED BYTE at every
 *      site except the held constant's LAST use, where it is the constant.
 * Then the six `.L` globals, then the 28 equality tests.
 *
 * NOT TESTED AND NOT CITED: every compiler flag. This function has loops, so
 * -fno-rerun-cse-after-loop is a legitimate probe here (it is meaningless on
 * OvlFunc_899_200b6f8, which has no loop), but it has not been run and no flag
 * figure may be quoted for this function from anywhere.
 */

/* No candidate body. See the header: this park claims no figure. */
