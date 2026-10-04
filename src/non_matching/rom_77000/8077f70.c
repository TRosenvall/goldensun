/* Func_8077f70  --  0x08077f70
 *
 * ===== BATCH 322g -- STILL A PARK, AT 9 of 120.  PIN-FREE, SHIM-FREE. =====
 *
 * NON-MATCHING, 9 of 120 encodings; counts 120/120, so this IS a distance.
 * Relocations are IDENTICAL (objcmp prints no `XX RELOCATIONS` line).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_77000/8077f70.c \
 *     asm/rom_77000/rom_77320_a_c_c.s --func Func_8077f70
 *
 * SPLIT SHAPE, if it ever lands.  asm/rom_77000/rom_77320_a_c_c.s holds THREE
 * functions -- Func_8077f70 (first), Func_807808c, Func_8078144 -- and
 * tools/datacheck.py prints nothing (no data section).
 *   python3 tools/split_s.py asm/rom_77000/rom_77320_a_c_c.s Func_8077f70
 * dry-runs as: _b.s = Func_8077f70 (1 function, 140 lines),
 *              _c.s = the other two (2 functions, 212 lines),
 *              no _a.s (the target is first), stage1.ld rewritten.
 * Installed path would be src/rom_77000/rom_77320_a_c_c_b.c.
 * PINS: 0.  SHIMS: 0.  No fakematch row needed.
 *
 * ===========================================================================
 * THE PARK'S FIGURE AND ITS DECOMPOSITION WERE BOTH WRONG.  THE RESIDUE IS
 * TWO CAUSES, 6 + 3, AND THE PARK NAMED ONLY THE SMALLER ONE.
 *
 * The header this replaces said:
 *
 *     "PARKED at 4 aligned of 123, and only TWO of the four are byte-affecting:
 *      one halfword store is emitted after the sign-extend pair instead of
 *      before it.  The other two are a label-count artefact ... worth zero
 *      bytes.  Every register in the function is the ROM's."
 *     "BLOCKER CLASS: 5, post-reload scheduling -- AND THIS IS THE FIRST ENTRY
 *      IN THAT CLASS WITH A PROOF RATHER THAN AN EXHAUSTED SEARCH."
 *
 * "4 aligned of 123" is a tryc.py --align figure.  The object-level figure is
 * 9 of 120, and SIX of the nine are a LITERAL POOL ORDERING that tryc.py
 * cannot see at all -- it normalises every PC-relative load to `=value` by
 * design (tools/objcmp.py's own docstring says so).  The park screened with
 * `tryc.py --align` and recorded "Not built", so it never looked.
 *
 *   CAUSE 1 -- POOL ORDER.  SIX of the nine.  3 pool words + the 3 `ldr`
 *   offsets that follow from them:
 *
 *       index   ROM                        ours
 *        74     .word 0x1ff                .word 0x901
 *        75     .word 0x901                .word 0x11b
 *        76     .word 0x11b                .word 0x1ff
 *         5     ldr r0,[pc,#156] -> 0xac   ldr r0,[pc,#152] -> 0xa8
 *        11     ldr r0,[pc,#140] -> 0xb0   ldr r0,[pc,#136] -> 0xac
 *        70     ldr r0,[pc,#4]   -> 0xa8   ldr r0,[pc,#12]  -> 0xb0
 *
 *   All three loads fetch the SAME THREE VALUES in both streams; only the
 *   pool's internal order differs, so all six indices are one defect.  The
 *   pool is mid-function at 0xa8 in both, behind the same `b`, same three
 *   words, same dump point.
 *
 *   *** VERIFIED OUT OF baserom.gba, NOT OUT OF THE .s. ***  The reference .s
 *   spells this region `.word 0x1ff` followed by `.pool`, which is a
 *   transcription choice; docs/elevation.md warns to read pool order from the
 *   ROM.  Done: bytes at 0x08078018 are 000001ff 00000901 0000011b.  The
 *   residue is real.
 *
 *   CAUSE 2 -- ONE HALFWORD STORE SUNK BY sched2.  THREE of the nine, indices
 *   23/24/25.  ROM: `strh r3,[r5,#0x3a] / lsl r1,#16 / asr r1,#16`.  Ours puts
 *   the store after the pair.  This is the park's whole diagnosis, and it is
 *   correct -- see below, where its numbers are reproduced from the compiler.
 *
 * ===========================================================================
 * CAUSE 1, THE DECIDING RUNG, AS ARITHMETIC.  `add_minipool_forward_ref`
 * (config/arm/arm.c:4820) keeps a minipool sorted ASCENDING by
 *     max_address = fix->address + fix->forwards,  forwards = pool_range(insn)
 * and `dump_minipool` (arm.c:4727) emits in list order.  A `-da` dump prints
 * both tables outright, in `.26.mach`:
 *
 *     ;; SImode fixup for i18;  addr  12, range (0,1020): 0x901
 *     ;; SImode fixup for i32;  addr  30, range (0,1020): 0x11b
 *     ;; SImode fixup for i273; addr 194, range (0,1020): 0x1ff
 *     ;; HImode fixup for i310; addr 230, range (0,64):   0x10
 *     ;; SImode fixup for i365; addr 296, range (0,1020): `gState'
 *     ;; Emitting minipool after insn 282; address 204
 *     ;;  Offset 0, max 1032 0x901
 *     ;;  Offset 4, max 1050 0x11b
 *     ;;  Offset 8, max 1214 0x1ff
 *
 * So the requirement is exact.  For 0x1ff to lead the pool,
 *
 *     194 + pool_range(0x1ff)  <  12 + 1020 = 1032   ==>  pool_range < 838
 *
 * and the only Thumb pool ranges in arm.md are 1020 (`*thumb_movsi_insn`),
 * 64 (`*thumb_movhi_insn`), 60 (`*thumb_zero_extendhisi2`) and 32
 * (`*thumb_movqi_insn` / the QImode extends).  OURS IS SImode, 1020.
 *
 *   > THE ROM'S MASK CONSTANT IS A NARROW-MODE OPERAND AND OURS IS SImode.
 *   > That is the whole of cause 1, and it is one bit of information.
 *
 * The `ldr r0,[pc,#4]` ENCODING is not evidence against that: a narrow fix
 * prints `ldrh rN, .LCn` and GAS assembles it as a two-byte PC-relative `ldr`
 * (docs/elevation.md records this), and MINIPOOL_FIX_SIZE still rounds the
 * entry to a full `.word`.  So the ROM's three-word pool is consistent with a
 * HImode or QImode head.
 *
 * NINE SPELLINGS MEASURED FOR THE MODE.  NONE MAKES THE FIX NARROW.  The
 * value's only consumer is an SImode `and`, and ARM's PROMOTE_MODE widens
 * every narrow LOCAL, which is docs/elevation.md's own correction ("A `short`
 * LOCAL does not give you HImode").  Measured figures, ref/ours counts shown
 * because four of them change the length:
 *
 *     base: `int mask; mask = 0x1ff;`                     120/120,  9
 *     `register short mask`                               120/120,  9  INERT,
 *          bit-identical output -- so elevation's "a `register short`
 *          declaration does give you HImode" does NOT hold at an AND site.
 *          It holds at a STORE site, which is the context it was measured in.
 *     a halfword temp for the loaded value, then `v & mask` 120/120, 9  INERT
 *     `unsigned short mask`                               120/120, 62  WORSE
 *     `register unsigned short mask`                      120/120, 62  WORSE
 *     `(unsigned short)(v & mask) != 0xf`                 120/120, 62  WORSE
 *     bare literal `& 0x1ff`, no variable                 120/122, 54  WORSE
 *     `unsigned short mask` + `& (unsigned short)0x1ff`   120/122, 54  WORSE
 *   The three WORSE-at-62 rows all push r7 as well (`b5e0` against the ROM's
 *   `b560` at index 0): widening-then-narrowing costs a register, it does not
 *   change the fix's mode.
 *
 * AND THE CONVERSE LEVER IS CONFIRMED LIVE, which is the useful dividend here.
 * Routing the OTHER narrow constant through an `int` carrier --
 * `{int ten = 0x10; *(unsigned short *)(r2 + (int)r5) = ten;}` -- widens that
 * fix from range 64 to range 1020 and MOVES ALL FOUR WORDS TO A SINGLE
 * END-OF-FUNCTION POOL (119 insns against 120, the mid-function pool and its
 * `b` gone).  So docs/elevation.md's "one such fix clamps max_address for the
 * entire pool" is reproduced here in both directions, and the mid-function
 * pool shape of this function is owed to its narrow fix, exactly as recorded.
 *
 * ONE OPEN SUB-QUESTION, STATED WITH ITS EVIDENCE RATHER THAN AS A CLAIM.
 * The ROM ALSO keeps 0x10 out of this pool (its word is a separate pool later),
 * and the exclusion test in add_minipool_forward_ref is
 *     fix->address >= minipool_vector_head->max_address - fix->fix_size
 * i.e. 230 >= 194 + R - 4, which needs R <= 40.  Combined with R < 838 that
 * would pin R = 32, QImode -- but 0x1ff does not fit in a byte, so one of the
 * two premises is wrong: most likely the ROM's 0x10 fix does not sit at
 * address 230.  DO NOT propagate "the mask is QImode"; propagate only
 * "narrow", which rests on the order inequality alone.
 *
 * ===========================================================================
 * CAUSE 2.  THE PARK WAS RIGHT, AND HERE ARE ITS NUMBERS OUT OF THE COMPILER.
 *
 * `-fsched-verbose=6` (the park's own recommended probe -- it is right about
 * that too) prints the block's dependence table.  The block is b 1 bb 0:
 *
 *     ;;      insn  code    bb   dep  prio  cost  ...  dependents
 *     ;;        61   157     0     2    38     2       101 75 68      ldrh r1,[r5,#0x34]
 *     ;;        64   157     0     2    36     2       101 459 87 72  ldrh r3,[r5,#0x36]
 *     ;;        68   180     0     3    36     2       101 87 75      strh r1,[r5,#0x38]
 *     ;;        72   180     0     3    34     2       101 459 87     strh r3,[r5,#0x3a]  <-- ours
 *     ;;        75   112     0     3    36     1       101 78         lsl  r1,#16         <-- wins
 *     ;;        78   113     0     2    35     1       101 87 81      asr  r1,#16
 *     ;;        81   112     0     3    34     1       101 87         lsl  r0,r1,#14
 *     ;;        87   240     0     6    33    32       101 460 459    bl   __divsi3
 *     ;;   Ready list (t = 40):    72  75
 *     ;;      --> scheduling insn <<<75>>> on unit core
 *
 * Priority 34 against 36, and rank_for_schedule returns on priority first.
 * The park's figures are EXACT.  Two things it did not have:
 *
 *   (a) WHY THE SIBLING STORE STAYS PUT, which is the mechanism and is
 *       transferable.  insn 68 (`strh r1,[r5,#0x38]`) carries an
 *       ANTI-dependence on insn 75, because 75 OVERWRITES the r1 that 68
 *       reads; `arm_adjust_cost` returns 0 for REG_DEP_ANTI, so 68 inherits
 *       75's priority 36 EXACTLY and ties it.  Our store 72 sources r3, which
 *       nothing in the shift chain writes, so its only path to the block end
 *       is the memory edge to the call: 1 + prio(87) = 34.  The park said this
 *       in words; the dump says it in numbers, and the "cost 0 for an anti
 *       edge" half is the part worth carrying.
 *
 *   (b) THE GAP IS EXACTLY TWO PRIORITY POINTS, AND AT A TIE OUR STORE ALREADY
 *       WINS.  Insn 72 has THREE dependents (101 459 87) against insn 75's two
 *       (101 78), and the dependent-count rung prefers more.  So this is NOT
 *       "lost at a tie-break"; it is lost at the priority rung by 2, and
 *       anything that closes those 2 closes the cause.  "Structurally
 *       impossible" overstates it -- the precise statement is: the store needs
 *       two more hops of dependence below it, or the shift chain needs two
 *       fewer, and the ROM's instruction sequence fixes both lengths.
 *
 * MEASURED FOR CAUSE 2, ALL EXACTLY INERT -- bit-identical output at 9:
 *     `*(volatile unsigned short *)((char *)r5 + 0x3a) = r3;`      9
 *     `*(volatile unsigned short *)((char *)r5 + 0x38) = r1;`      9
 *     both stores volatile                                          9
 *     the two store statements swapped in the source                9
 *     the 0x36 load made a volatile read                            9
 *   AND THE EDITS WERE VERIFIED TO HAVE HAPPENED, because a flat sweep that
 *   never reached the compiler is this project's standing trap: `mem/v` count
 *   in `.20.ce2` goes 0 -> 1 with the volatile cast and the figure does not
 *   move.  So, with evidence attached:
 *
 *   > A `volatile` MEM ADDS NO SCHEDULING DEPENDENCE THAT THE ALIAS SET DOES
 *   > NOT ALREADY GIVE, for a halfword store under gcc-2.96's sched2.  Both
 *   > stores here print as `(mem:HI (plus ...) 6)` -- ALIAS SET 6, the same one
 *   > -- so they already carry an output dependence on each other, and the
 *   > volatile bit changes neither priority nor order.
 *
 *     `short r3;` instead of `int r3;`                   120/128, 105  far worse
 *
 * ===========================================================================
 * WHAT CARRIES FORWARD.  Cause 2 needs the class crack the park asked for, and
 * now has a price on it (2 priority points).  CAUSE 1 IS THE CHEAPER HALF AND
 * IS THE ONE TO TAKE NEXT: it is one bit -- make the 0x1ff fix narrow -- and
 * the nine spellings above say the lever is not in the mask's DECLARATION.
 * Per the brief's rule for a flat sweep, the next move is the TYPE OF ITS
 * CONSUMER or the TU shape, not another cell: the mask is only ever ANDed with
 * a `u16` load and compared to 0xf, so the thing to try is a shape where that
 * whole test happens in HImode without a widen/narrow pair -- which is what
 * cost the three 62-rows a register.
 *
 * EVERYTHING ELSE IN THE PARK'S HEADER IS KEPT AND NONE OF IT WAS REFUTED:
 * the [offset] A/B result at ldrsh sites, the loop.c second-pass threshold
 * bracket of 15..17, the goto amendment, "a pooled small constant whose
 * consumer is a halfword store is blocker 1b, not a symbol", the
 * declaration-lever warning about testing it under --no-sched2, the two-shift
 * sign extension beating a `(short)` cast, and the corpus-neighbour transplant
 * out of src/rom_77000/rom_77320_c_b.c.  The body below is unchanged from the
 * park's; the figure 9 is reproduced exactly.
 *
 * -- scratch_elev/b322/G (p1_candidate.c, v1_M*.c, v1_S*.c, v1_N*.c)
 */
extern void ClearFlag(int id);
extern void SetFlag(int id);
extern void Func_8079ae8(int unit);
extern void CalcStats(int unit);
extern void *GetUnit(int unit);
extern void EquipItem(int unit);
extern unsigned char gState[];

void Func_8077f70(void)
{
    void *r5;
    unsigned char *g;
    int r0;
    int r1;
    int r2;
    int r3;
    int i;
    int mask;

    ClearFlag(0x20);
    ClearFlag(0x21);
    SetFlag(0x901);
    Func_8079ae8(5);
    CalcStats(5);
    ClearFlag(0x11b);
    SetFlag(0x11a);

    for (i = 0; i < 2; i++) {
        r5 = GetUnit(i);
        r1 = *(unsigned short *)((char *)r5 + 0x34);
        r3 = *(unsigned short *)((char *)r5 + 0x36);
        *(unsigned short *)((char *)r5 + 0x38) = r1;
        *(unsigned short *)((char *)r5 + 0x3a) = r3;
        r1 <<= 16;
        r1 >>= 16;
        r0 = r1 << 14;
        r0 /= r1;
        r3 = 0x80;
        r3 <<= 7;
        if (r0 > r3) {
            r3 = 0x80 << 7;
        } else {
            if (r0 < 0) {
                r3 = 0;
            } else {
                r3 = r0;
            }
        }
        *(short *)((char *)r5 + 0x14) = r3;
        if ((r3 << 16) != 0) {
            goto label_0x3a;
        }
        r3 = *(short *)((char *)r5 + 0x38);
        if (r3 == 0) {
            goto label_0x3a;
        }
        r3 = 1;
        *(short *)((char *)r5 + 0x14) = r3;
    label_0x3a:
        r0 = *(short *)((char *)r5 + 0x3a);
        r1 = *(short *)((char *)r5 + 0x36);
        r0 <<= 14;
        r0 /= r1;
        r3 = 0x80;
        r3 <<= 7;
        if (r0 > r3) {
            r3 = 0x80 << 7;
        } else {
            if (r0 < 0) {
                r3 = 0;
            } else {
                r3 = r0;
            }
        }
        *(short *)((char *)r5 + 0x16) = r3;
        if ((r3 << 16) != 0) {
            goto label_items;
        }
        r3 = *(short *)((char *)r5 + 0x3a);
        if (r3 == 0) {
            goto label_items;
        }
        r3 = 1;
        *(short *)((char *)r5 + 0x16) = r3;
    label_items:
        mask = 0x1ff;
        r1 = 0;
        r2 = 0xd8;
        goto items_test;
    items_next:
        r2 += 2;
        r1++;
    items_test:
        if (r1 <= 0xe) {
            if ((*(unsigned short *)(r2 + (int)r5) & mask) != 0xf) {
                goto items_next;
            }
            *(unsigned short *)(r2 + (int)r5) = 0x10;
            EquipItem(i);
        }
        Func_8079ae8(i);
        CalcStats(i);
    }

    GiveInnateMove(0, 0x8c);
    GiveInnateMove(0, 0x95);
    GiveInnateMove(1, 0x8c);
    GiveInnateMove(2, 0x8d);
    g = gState;
    *(int *)(g + 0x10) += 0x96 << 1;
}
