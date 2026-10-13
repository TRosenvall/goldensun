/* OvlFunc_952_200be40  --  0x0200be40  [asm/overlays/rom_7d768c/ovl_30_c_a_c_a_a.s]
 *
 * NON-MATCHING, 93 of 122 encodings  (re-measured batch 347 brief B).
 *   THE FIGURE IS NOT A DISTANCE.  16-bit INSTRUCTION COUNT ref 81, ours 79 --
 *   an INSTRUCTION GAP OF -2.  SIZE ref 324 bytes, ours 328, and ours is LARGER
 *   while being two instructions SHORTER because it carries two extra pool words.
 *   122 == 122 is the STREAM length (pool words AND `bl` calls AND pads), which
 *   is what the old header read as a count.  ALIGNED residue 32 in 23 hunks.
 *   The previous header's sentence "Re-classified in batch 322; the figure IS a
 *   distance" IS FALSE and is removed; objcmp prints the instruction counts on
 *   the line directly above the figure.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7d768c/200be40.c \
 *     asm/overlays/rom_7d768c/ovl_30_c_a_c_a_a.s --func OvlFunc_952_200be40
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py \
 *     src/non_matching/ovl_7d768c/200be40.c \
 *     asm/overlays/rom_7d768c/ovl_30_c_a_c_a_a.s OvlFunc_952_200be40
 *
 * THE GAP IS TWO SITES OF 1, not one site of 2, and the second was invisible to
 * the old header because the old header was counting the stream:
 *
 *   SITE A -- THE CONSTANT HOIST.  The ROM rebuilds `mov r1,#0xa0 / lsl r1,#7`
 *     (0x5000) at both its uses and `mov r1,#0xc0 / lsl r1,#6` (0x3000) at all
 *     three of its uses.  gcc computes both once before the first call and holds
 *     them in callee-saved registers: 4 setup instructions replace 5 x 2 = 10
 *     with 5 one-instruction copies, net -1.
 *   SITE B -- THE MESSAGE BASE.  The ROM has `ldr r5, =0x2233` once and reaches
 *     the other two ids with `add r0, r5, #1` and `add r0, r5, #2`.  gcc pools
 *     0x2233, 0x2234 AND 0x2235 and loads each directly, net -1 instruction and
 *     +2 pool words -- which is the whole of the 324-vs-328 size inversion.
 *
 * SITE B IS CLOSED BY A FLAG GROUP THE PARK NEVER TRIED.  -fno-gcse
 * (GCSE_CFLAGS) takes the UNCHANGED body to an instruction gap of -1, size
 * 324/320, aligned residue 32 -> 29, and the `add r0, r5, #1` / `#2` shape comes
 * back exactly.  gcse's constant propagation is what rematerialises base+1 and
 * base+2 as pool constants.  tools/autosweep.py's flag axis finds this in one
 * command; both cheap axes are now CLOSED on this park (all eight flag groups,
 * all 39 anchor placements -- -O1 reads ndiff 48 and is a TRAP: its aligned
 * residue is 38, worse than the base 32).
 *
 * SITE A: THE PASS IS NAMED, AND IT IS NOT THE ONE docs/elevation.md NAMES.
 * elevation.md's dominance section calls this "partial redundancy elimination, a
 * different pass from the cse described above".  Measured from gcc's own dumps
 * (-da, with -fno-gcse so PRE cannot be the actor): `.00.rtl` holds FIVE separate
 * `(const_int 20480)` / `(const_int 12288)` sets, one per use site, and by
 * `.03.cse` there is ONE set of each, with the else-arm use reading
 * `(set (reg:SI 1 r1) (reg:SI 34))` plus a REG_EQUAL note -- the signature of a
 * cse substitution.  So the actor is CSE PASS 1, and PRE is not involved.
 *
 * THE GATE, with the line that decides it.  cse_main calls
 * cse_end_of_basic_block (cse.c:6534) with `flag_cse_follow_jumps` and
 * `flag_cse_skip_blocks`.  Both of its two block-extension arms -- the
 * follow-jumps arm (the target label preceded by a BARRIER) and the skip-blocks
 * arm ("detect a branch around a block of code") -- sit behind ONE shared gate:
 *
 *     && LABEL_NUSES (JUMP_LABEL (p)) == 1
 *
 * The else arm's label has exactly one use here, so cse extends its block across
 * the `bne` and commons the constant.  PROVED BY INSTRUMENT: with
 * `-fno-gcse -fno-cse-follow-jumps -fno-cse-skip-blocks` this function measures
 * 50 of 122 with INSTRUCTION COUNT AND SIZE BOTH EQUAL -- a true distance -- and
 * aligned residue 20 in 20 hunks, and the final `.s` builds `#160` twice.  Those
 * last two flags are NOT Makefile flag groups, so this is an INSTRUMENT figure
 * about the blocker, not a result; the shipped body is flag-free.
 *
 * REFUTED: "the two constants are DIFFERENT SYMBOLS in the original".  That
 * inference is recorded both here and in docs/elevation.md's dominance section
 * (and again in "A second instance of the dominance contradiction").  It cannot
 * be right: gcc-2.96 ALWAYS pools a symbol's address, and the ROM builds these
 * with `mov` + `lsl`.  A symbol would read `ldr r1, =_SYM`.  The contradiction
 * elevation.md records is not a symbol-space problem -- it is the LABEL_NUSES
 * gate above, which is a property of the CFG the original compiled, not of the
 * constants.
 *
 * MEASURED INERT OR WORSE on site A (all at production flags unless stated):
 *   -ffixed-r7, -fno-strict-aliasing, -fno-strength-reduce, -fno-rerun-loop-opt
 *     bit-identical to the base at 93.
 *   -fno-cse-follow-jumps ALONE, -fno-cse-skip-blocks ALONE: both
 *     bit-identical at 93 -- the two arms reach the else block independently, so
 *     only the PAIR closes it.  A single-flag sweep records this as a clean
 *     negative and it is not one.
 *   -fno-gcse crossed with -fno-rerun-cse-after-loop, -fno-rerun-loop-opt,
 *     -fno-caller-saves, -fno-force-mem, -fcall-saved-r4: all gap -1, aligned
 *     29-32, i.e. no better than -fno-gcse alone.
 *   PRESSURE DOES NOT WORK.  -fno-gcse -ffixed-r7 goes to +4 instructions and
 *     -fno-gcse -ffixed-r6 -ffixed-r7 to +8: denied a callee-saved register gcc
 *     SPILLS the constant to the stack, it does not rebuild it.
 *   ONE SHARED `int` FOR ALL FIVE CONSTANT ARGUMENTS (so each redefinition kills
 *     the previous equivalence): +5 at production flags, +7 under -fno-gcse,
 *     aligned 48 and 41.  One C variable is one pseudo, so it wins a callee-saved
 *     register and pays copies at every site.
 *   INVERTING THE `if` (long arm first, `!= 0`): aligned 32 -> 47.  The park's
 *     arm order is right.
 *
 * NEXT, and it is a concrete engineering target rather than a symbol-space wish:
 * make LABEL_NUSES of the else arm's entry label 2, or remove the BARRIER in
 * front of it, and cse cannot extend.  Neither has been tried from C.
 *
 * ---- retained from the previous header, still true ----
 * The message base 0x2233 with `add r0, r5, #1` and `#2` derivations IS
 * reproduced correctly by a plain `base = 0x2233;` local; under -fno-gcse that
 * part is exact.  Per-use-site named locals for the hoisted constants are
 * byte-identical to the literals, whether assigned at the top of the function or
 * immediately before their own call: gcc folds the initialisers to one rtx before
 * cse runs, so five named locals and one literal are the same input.
 * 0x2233 is NOT a .sym candidate: it cannot be built with mov+lsl, so pooling it
 * is forced and carries no information about the original spelling.
 */
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __Func_808e118(void);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __MessageID(int id);
extern void __Func_8092c40(int a, int b);
extern int __Func_8091c7c(int a, int b);
extern void __ActorMessage(int a, int b);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __MapActor_DoAnim(int a, int b);
extern void __MapActor_Emote(int a, int b, int c);

void OvlFunc_952_200be40(void)
{
    int base;

    __SetFlag(0x96c);
    __CutsceneStart();
    __Func_808e118();
    __Func_8092adc(8, 0xa0 << 7, 0);
    __Func_8092adc(9, 0xc0 << 6, 0);
    __Func_80921c4(0, 0xc8, 0x88 << 1);
    __Func_8092adc(0, 0xc0 << 8, 0);
    __CutsceneWait(0x14);
    base = 0x2233;
    __MessageID(base);
    __Func_8092c40(8, 0);
    if (__Func_8091c7c(0, 0) == 0) {
        __CutsceneWait(0x14);
        __MessageID(base + 1);
        __ActorMessage(8, 0);
    } else {
        __CutsceneWait(0x14);
        __MessageID(base + 2);
        __ActorMessage(8, 0);
        __CutsceneWait(0x14);
        __Func_8092848(8, 9, 0x3c);
        __Func_8092adc(9, 0xc0 << 6, 0);
        __CutsceneWait(0x28);
        __Func_80925cc(9, 2);
        __CutsceneWait(0x1e);
        __Func_8092848(8, 9, 0x1e);
        __MapActor_DoAnim(9, 3);
        __CutsceneWait(0x1e);
        __MapActor_Emote(8, 0x81 << 1, 0x32);
        __Func_8092adc(8, 0xa0 << 7, 0);
        __Func_8092adc(9, 0xc0 << 6, 0);
        __CutsceneWait(0x14);
        __MapActor_DoAnim(8, 4);
        __CutsceneWait(0x14);
        __ActorMessage(8, 0);
        __CutsceneWait(0xa);
        __Func_80925cc(8, 2);
        __CutsceneWait(0x14);
        __ActorMessage(8, 0);
    }
    __CutsceneEnd();
}
