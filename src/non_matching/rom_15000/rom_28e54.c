/* MEASURED FIGURE, backfilled in batch 324 (this park carried none).
 *
 *   30 differing encodings of 35.  SIZE DIFFERS (ref 35, ours 32 -- three short).
 *
 * First diff is at INDEX 0 and it is the PROLOGUE PUSH SET: ref b5e0
 * (`push {r4,r5,r6,r7,lr}`) against ours b560 (`push {r5,r6,lr}`).  So this is
 * not 30 problems -- it is a register-allocation difference visible in the very
 * first encoding, with the whole body shifted behind it.  Two callee-saved
 * registers the ROM uses are going unused here.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_15000/rom_28e54.c asm/rom_15000/rom_23178_a_a_c_c_a.s --func YesNoMenu2
 *
 * The figure is EVIDENCE.  Everything below it is a HYPOTHESIS, and across
 * pass two a park's diagnosis has been wrong roughly 40 times in 42.
 *
 * ----------------------------------------------------------------------------
 * BATCH 332.  THE DIRECTION OF THE GAP: the reference is TWO 16-bit
 * instructions LONGER than ours, so WE ARE MISSING TWO.  The leading block's
 * "three short" counts a third entry that is the missing POOL WORD, a 32-bit
 * entry, not an instruction -- read the instruction column and the stream
 * column separately here.
 *
 * AND THE LAST PARAGRAPH OF THIS PARK IS WRONG.  It says that with the literal
 * in place the instruction stream is the same length and only the register
 * shuffling differs.  It is not the same length: the literal body is two
 * instructions short.  That superseded reading should not be built on.
 *
 * TWO LEVERS, each worth exactly one instruction.  With both, every instruction
 * is identical and the only differing encoding left is the pool word itself,
 * holding an unresolved placeholder -- the phantom-relocation class const.sym's
 * own note on its a1 entry describes.
 *
 * LEVER ONE -- local-alloc CAN NEVER HAND OUT r7 IN THUMB.  local-alloc.c:1978
 * says so in its own comment ("Don't use the frame pointer reg in local-alloc
 * ... It can move only regs made by global-alloc") and sets
 * HARD_FRAME_POINTER_REGNUM in its `used` set at :1990; config/arm/arm.h:899
 * makes that r11 under the ARM target and r7 under Thumb (:898).  The guard
 * above it compares the fake FRAME_POINTER_REGNUM against that ternary in the
 * preprocessor, so it is always taken.
 *
 * The ROM spends r5, r6, r7, r8.  Once a fourth long-lived value exists here,
 * gcc spends r5, r6, r8 and r10 -- TWO high registers -- which costs one extra
 * move-and-push at entry and one extra move at exit.  That is the gap.
 *
 * The source lever is to REUSE THE THIRD PARAMETER AS THE RESULT VARIABLE
 * rather than declaring a separate one, so that single pseudo spans basic
 * blocks, is therefore not block-local, and is allocated by GLOBAL alloc --
 * whose find_reg scans REG_ALLOC_ORDER (arm.h:989) and does reach r7.  The
 * ROM's own r7 is shared between the third argument and the return value, which
 * is the tell for this.  This is a general lever and it is cheap to check: in
 * any Thumb function where the ROM uses r7 for a value, that value must be one
 * global-alloc can see.
 *
 * LEVER TWO -- THE POOL TELL SURVIVES, AND THIS PARK'S "TWO READINGS" RESOLVE
 * IN FAVOUR OF THE FIRST.  The exception the second reading was groping for is
 * already written down, in const.sym's header: gcc does pool a small constant
 * that meets a HALFWORD expression.  It does not apply.  The operand is an
 * SImode call argument -- the landed callee in
 * src/rom_15000/rom_23178_a_a_a_a_c_c_c_c.c declares all four parameters int --
 * so there is no HImode path.  Checked anyway with an unsigned short local, a
 * short local and an unsigned short cast; all three build the value with a
 * move.
 *
 * CRITERION ONE IS MET IN ITS STRONG FORM, the same form the owner accepted for
 * message.sym's 0x182 entry.  CONST_OK_FOR_THUMB_LETTER (arm.h:1096) makes
 * letter I accept anything under 256, which is alternative ONE of
 * *thumb_movsi_insn (arm.md:3838); the pool path is alternative SIX and recog
 * takes the first match.  An SImode const_int of this value therefore CANNOT
 * reach the pool, so the pool word says the source named a symbol.
 *
 * AND THE INTERNAL CONTROL IS IN THIS VERY .s FILE.  The sibling function at
 * 0x08028df4 also parks a long-lived flag in r8, and the ROM BUILDS both of its
 * values there with a move into a low register followed by a move to r8.  Same
 * bank, same translation unit, same HIGH-register destination, one built and
 * one pooled.  That refutes outright the competing explanation that a
 * high-register destination forces the pool, because demonstrably it does not.
 *
 * CRITERION TWO, MEASURED.  EIGHT literal spellings, every one leaving the
 * function two instructions short: the bare literal inline, the same in
 * decimal, a product that folds to it, an int local, a const int local, an
 * unsigned short local, a short local, and an unsigned short cast.  A ninth, a
 * volatile int local, is far worse -- seven instructions too many.
 *
 * ALSO MEASURED, and it is the opposite of what const.sym's 0x1f note records
 * for a different function: the symbol used INLINE at the call site reaches the
 * pool but does NOT keep the value in a register, so it stays two instructions
 * short.  THE NAMED LOCAL IS REQUIRED HERE.  Both facts are real; they are
 * different functions and the note should not be read as a rule.
 *
 * SO WHAT THIS PARK NEEDS is a const.sym entry named by value for 0x24.  THAT
 * IS AN OWNER DECISION and batch 332 did not make it; the evidence above is
 * assembled for it and nothing has been added to const.sym.  Without the symbol
 * the best literal body is this file's own.
 *
 * SPILLOVER FOR THE SIBLING PARK, src/non_matching/rom_15000/8028df4.c.  It
 * records a five-way permutation and says "nothing in the source chooses which
 * of five equally-long live ranges gets the high register".  Something does:
 * whichever pseudo spans a basic block is the ONLY one that can get r7, per the
 * local-alloc reading above -- and that park's own measured table puts its flag
 * variable in r7 precisely because the flag is the one assigned in two blocks.
 * That park should be re-attacked with this rather than with further
 * declaration-order probes.
 */

/* YesNoMenu2  [rom_15000]  --  0x08028e54
 *
 * Source asm: goldensun/asm/rom_15000/rom_23178_a_a_c_c_a.s
 *
 * Blocker: THE POOL TELL, with the namespace unidentified.
 *
 * The ROM loads 0x24 from the literal pool at the top of the function and keeps
 * it in r8 across three calls:
 *
 *     ldr r3, =0x24 / ... / mov r8, r3 / ... / mov r3, r8 / bl Func_80288a8
 *
 * 0x24 fits in an eight-bit `mov`, and gcc never pools what it can `mov`. So
 * the operand was a SYMBOL REFERENCE in the original source -- the same tell
 * that identified the area ids, the message ids and the file ids. Written as a
 * literal, gcc materialises it at the call site and the register allocation of
 * the whole prologue shifts with it -- a figure of sixteen of thirty-four was
 * recorded here and is SUPERSEDED; see the batch-332 block above.
 *
 * WHAT IS MISSING IS THE NAMESPACE, not the mechanism. This tree defines such
 * operands by value in a .sym -- `_AREA_35 = 0x35;` and so on -- which emits no
 * bytes and asserts nothing beyond the value. Doing that here would need a name,
 * and there is not enough evidence for one:
 *
 *   * Func_80288a8 has exactly ONE caller in the whole ROM, this function, so
 *     the parameter cannot be triangulated from other call sites the way
 *     __Func_8091f90's area id was (two elevated files pass _AREA_51 and
 *     _AREA_4d to it).
 *   * only one other place in the ROM pools 0x24 --
 *     asm/overlays/rom_79e5c0/ovl_30_c_a_a_a_a.s:17 -- and whether it is the
 *     same kind of value is unknown.
 *
 * THAT READ WAS DONE and it did not settle it. Func_80288a8 takes four
 * arguments and stores three of them as HALFWORDS into the UI structure at
 * [iwram_3001f38]:
 *
 *     arg2 + 2  ->  +0x90     (a count; also passed to CreateUIBox)
 *     arg3      ->  +0x92     <- the pooled 0x24
 *     arg1      ->  +0x94
 *     arg0      ->  scaled by 8 into a per-row field in a loop
 *
 * So 0x24 is a sixteen-bit field of a menu box. 36 is a plausible pixel width,
 * and a pixel width is not the sort of thing that is a symbol -- which leaves
 * the pool tell and the semantics pointing in opposite directions, and that is
 * the interesting part of this park rather than a detail.
 *
 * TWO READINGS, and I cannot separate them:
 *
 *   1. it IS a symbol -- some UI metric constant defined in a header the
 *      decompilation has not reconstructed -- and the namespace is simply
 *      unknown.
 *   2. the pool tell has an exception nobody here has characterised. Every
 *      confirmed instance so far has been an ID compared or dispatched on;
 *      none has been a value STORED into a struct field. That may matter.
 *
 * Reading 2 would be worth knowing generally, because the pool tell has been
 * load-bearing for the area, message and file namespaces and this tree has
 * assumed it is unconditional.
 *
 * Inventing a namespace for a single function would be worse than leaving this
 * parked, and this tree has been burned once by adopting names on thin
 * evidence -- see the area-id discussion in HANDOFF.md.
 *
 * Everything else about the translation is believed right; with the literal in
 * place the instruction stream is the same length and the only differences are
 * where 0x24 is built and the register shuffling that follows from it.
 */
extern void Func_80284dc(void);
extern void AddMenuBarOption(int a);
extern void Func_80288a8(int a, int b, int c, int d);
extern int Func_8028574(int a);
extern void Func_802851c(void);

int YesNoMenu2(int a, int b, int c)
{
    int k;
    int r;

    k = 0x24;
    Func_80284dc();
    AddMenuBarOption(5);
    AddMenuBarOption(6);
    Func_80288a8(a, b, 3, k);
    r = Func_8028574(c);
    Func_802851c();
    if (r == -1)
        r = 1;
    return r;
}
