/* OvlFunc_common1_588 -- MATCHING, AT -O1.
 *
 * A common-overlay routine: runs __Func_8019908 on its int argument, reads the
 * area id halfword at gState+0xe0*2, picks one of three message ids from it and
 * shows the message on the actor passed in.
 *
 * ***** THIS TU REQUIRES O1_CFLAGS.  Rule to add (EXPLICIT, never a pattern --
 * ***** the Makefile records FIVE mis-scoped -O1 wildcards that each captured a
 * ***** split product that merely shared a prefix):
 *
 *   asm/overlays/common/common1_a_a_a_a_c_a_b.o: src/overlays/common/common1_a_a_a_a_c_a_b.c
 *   	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
 *   	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
 *   	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
 *
 * Scope is as tight as it can be: after the split this TU holds ONE function,
 * so the flag cannot reach a sibling.  O1_CFLAGS already exists (Makefile:190)
 * and no wildcard in the Makefile touches the common1_a stem -- the only
 * `common1` entry is `$(call common_orig_deps, common1_c, ...)`, an orig.bin
 * dependency on a different stem.
 *
 * Verify with (AFTER the Makefile rule exists, so objcmp picks the flag up and
 * prints `(built with: -O1)`):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/common/common1_a_a_a_a_c_a_b.c \
 *     asm/overlays/common/common1_a_a_a_a_c_a_b.s --whole
 *   (before the split and the rule, the measured figure is reproduced with
 *      OBJCMP_EXTRA=-O1 ... --func OvlFunc_common1_588 against
 *      asm/overlays/common/common1_a_a_a_a_c_a.s
 *    -- 27/27 instructions, 37/37 stream, 92/92 bytes, and the ONLY two
 *    differing encodings are the two area pool words described below.)
 *
 * PINS: none.  shimcount.py reports nothing.
 *
 * THE TWO AREA POOL WORDS ARE PHANTOM, and this is the batch-332 park's own
 * finding, re-verified: `_AREA_8f` and `_AREA_90` are real area.sym entries
 * defined BY VALUE, so they emit no bytes and resolve at link, while the
 * reference is a DISASSEMBLY in which the values were already baked in -- so
 * our object carries R_ARM_ABS32 placeholders where the reference carries
 * 0x8f and 0x90.  objcmp's relocation list cannot see past that; `make compare`
 * can, and the precedent is already in the tree: the LANDED, byte-matching
 * src/overlays/rom_7b4558/ovl_30_c_c_c_a_b.c compares _AREA_44/45/46 exactly
 * this way, and batch 336 established the same for _AREA_97.
 *
 * SOLVED AND NOT TO BE RE-DERIVED (from the park, which was right): the two
 * area comparisons are SYMBOLS, not integers.  `ldr r3, =0x8f` for a value that
 * `cmp Rn, #imm8` would cover is the pooled-constant tell, and the halfword at
 * gState+0x1c0 is the area id by area.sym's own header.  THIS FUNCTION IS WHERE
 * _AREA_8f AND _AREA_90 WERE IDENTIFIED -- a value-based reading would have
 * called them _FILE_8f / _FILE_90, which is what file_table.sym calls them.
 * See the batch-67 block in area.sym.
 *
 * SPLIT -- AND ovl_common IS NOT LOCAL.  datacheck.py on the reference is CLEAN
 * (exit 0, no output).  split_s.py --dry-run cuts the .s THREE ways and rewrites
 * THREE overlay linker scripts, because asm/overlays/common/ is linked into
 * three overlays:
 *     common1_a_a_a_a_c_a_a.s   1 fn,  79 lines  (OvlFunc_common1_4cc)
 *     common1_a_a_a_a_c_a_b.s   1 fn,  36 lines  (this one)
 *     common1_a_a_a_a_c_a_c.s   1 fn, 280 lines  (OvlFunc_common1_5e4, parked 232)
 *     overlays/rom_7db0c8/overlay.ld, overlays/rom_7ddb88/overlay.ld,
 *     overlays/rom_7e0928/overlay.ld all rewritten
 * ALL THREE overlays have to stay green, not one.
 * TWO PARK RECIPES MUST BE REPOINTED at the products: 
 *   src/non_matching/ovl_common/4cc.c:1         -> ..._c_a_a.s
 *   src/non_matching/ovl_common/common1_5e4.c:8 -> ..._c_a_c.s
 *
 * ================= BATCH 337 BRIEF D: HOW IT CLOSED =================
 *
 * MEASURED BASELINE: 9 of 37 stream, instructions 27 / 27, size 92 / 92, so the
 * figure was a distance.  aligncmp -v resolves it into exactly three causes:
 * five insns where the reference builds the 0xe0 shift in r1 and lands the
 * halfword in r2 while ours used r2 and r0; the two `cmp` that follow from
 * that; and the two phantom area words.  7 real + 2 phantom, exactly as the
 * park claimed.  The park's mechanism was right too -- if-conversion,
 * find_if_case_1 at ifcvt.c:1651 -- and its five structural negatives all
 * reproduce.
 *
 * THE AXIS THE PARK NEVER SWEPT WAS THE FLAG AXIS, and the labelled corpus
 * handed it over.  Censusing every `.s` in asm/ for the shape this function
 * needs -- `cmp` / `bne` / `ldr rX, <pool>` / `b <join>` -- finds 394 hits in
 * 162 of the 4,527 generated files and 73 in 34 of the 704 hand-written ones,
 * so the shape is thoroughly reachable and the park's "not source-reachable"
 * was only ever true AT -O2.  Narrowing to the hits whose then-block pool word
 * is a PLAIN NUMBER rather than a symbol leaves 15, and the `.c` beside one of
 * them -- src/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.c, OvlFunc_939_20087f4 --
 * names this park's blocker in its own header and says how it closed:
 * "parked in batch 32 on SPECULATIVE LITERAL HOIST: for a two-way pick between
 * two pooled constants feeding one call, gcc at -O2 loads the fall-through
 * constant BEFORE the compare and conditionally overwrites it ... -O1 alone
 * matches it."  That file also names the general failure, which is this one:
 * "Flags and source forms were swept as two separate axes and the product was
 * never covered."
 *
 * MEASURED, -O2 against -O1 (encodings differing):
 *      body                                            -O2    -O1
 *      the park body (compared-variable reuse)           9      9
 *      TWO SEPARATE LOCALS (`v` and `id`)               25      2  <-- the pick
 *      the same, no `p = a` alias                       25      2
 *      the same, `short v`                              25      2
 *      the sibling's spelling, two locals               25      2  <-- shipped
 *      a third parameter reused as the id               25      2
 *      default assigned first, two arms                 27     26
 *      `volatile int id` (a MEM in each arm)            37     39
 *      parameter `b` reused as the id                   25      6
 *      the same via explicit gotos                      25      6
 * The 2 is both phantom area words; the instruction stream is IDENTICAL.
 * Note the inversion: the park's own body is the ONLY one that does not improve
 * at -O1, so a flag sweep run against the parked body alone would have found
 * nothing.  THE FLAG AND THE SOURCE FORM HAD TO MOVE TOGETHER.
 *
 * THE SHIPPED SPELLING is the one src/overlays/rom_7b4558/ovl_30_c_c_c_a_b.c
 * uses -- a landed matching file doing the identical gState+0xe0*2 halfword
 * read against _AREA_* symbols -- so the two read as one family.  The park's own
 * address spelling (`k = 0xe0 << 1; g = (unsigned char *)&gState + k;`) ties it
 * exactly, at 2 either way.  Keep the arithmetic spelled out: the park and the
 * sibling both record that folding it makes the +0x1c0 vanish into the pool
 * constant.
 *
 * TWO THINGS ADDED TO THE PARK'S GATE LIST, both from the compiler on disk:
 *   * count_bb_insns (ifcvt.c:111-126) counts only INSN and CALL_INSN, so the
 *     arm's `b <join>` is NOT counted.  With BRANCH_COST 1 (arm.h:2413) the size
 *     gate at ifcvt.c:1683 needs a TWO-insn arm and the ROM's arm is one insn,
 *     so that gate is UNREACHABLE here rather than merely hard.
 *   * dead_or_predicable's non-predicated path fails if `merge_set` holds any
 *     HARD register, under `SMALL_REGISTER_CLASSES && ! reload_completed`
 *     (ifcvt.c:1958-1970), and SMALL_REGISTER_CLASSES is TARGET_THUMB
 *     (arm.h:1061).  So a hard-register local in the arm refuses the conversion.
 *     That is the pinned route; it is not needed and was not measured, because
 *     -O1 lands this pin-free.
 *
 * AND ONE HYPOTHESIS OF MINE, MEASURED AND REFUTED, recorded so it is not
 * re-derived: reusing the PARAMETER `b` as the message id does NOT trip the
 * TEST_SET half of the liveness gate.  The reasoning looked sound -- the call
 * does not split a basic block, so the prologue, the load and the compare are
 * all in block 0, and assign_parms' copy sets b's pseudo there.  But TEST_SET
 * is the registers set between `earliest` and the end of the block, with
 * `earliest` coming from noce_get_condition (jump, &earliest) at
 * ifcvt.c:1937-1939, and that walks back only as far as the condition's own
 * operands; the prologue copy is above it.  Measured 25 at -O2, and the
 * `.14.ce` dump says `2 IF blocks converted`.  The `.14.ce` dump is the
 * instrument for this whole class and nobody had used it here: it prints
 * `IF-CASE-1 found, start N, then M`, then `Conversion succeeded.` or nothing,
 * and a final count -- the park body reads `0 IF blocks converted`, two
 * separate locals read `2`.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_8f;
extern int _AREA_90;
extern void __Func_8019908(int a, int b);
extern void __MessageID(int id);
extern void __ActorMessage(void *a, int n);

void OvlFunc_common1_588(void *a, int b)
{
    unsigned int base;
    unsigned int off;
    short v;
    int id;

    __Func_8019908(b, 5);
    base = (unsigned int)&gState;
    off = 0xe0;
    off <<= 1;
    base += off;
    off = 0;
    v = *(short *)((char *)base + off);
    if (v == (int)(&_AREA_8f))
        id = 0x2076;
    else if (v == (int)(&_AREA_90))
        id = 0x2078;
    else
        id = 0x207a;
    __MessageID(id + 1);
    __ActorMessage(a, 0);
}
