/* OvlFunc_931_2008904 (0x02008904) -- NON-MATCHING.
 *
 * NON-MATCHING, 4 of 221 encodings  (MEASURED, batch 331 brief A).  The park's
 * earlier figure of seven is superseded; see THE STACK ARGUMENTS BELOW.
 * Lengths are exact both ways and relocations are clean, so this IS a distance.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7b8cb0/2008904.c \
 *     asm/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_c_a_c.s --func OvlFunc_931_2008904
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 * Blocker class: stack-argument materialisation, plus an `orr` destination.
 *
 * 222 lines against 222, SEVEN differing when this was written, in two shapes.
 *
 * SHAPE 1, three lines, at the ONE call site of eight where both stack
 * arguments are distinct literals:
 *
 *     rom    mov r3, #0x14 / mov r2, #0x29 / str r3, [sp] / str r2, [sp, #4]
 *     ours   mov r3, #0x14 / str r3, [sp] / mov r3, #0x29 / str r3, [sp, #4]
 *
 * The other seven sites match, and they match for a reason worth reading: the
 * original code passes a FLAG VARIABLE (which is zero on that path) as both
 * stack arguments rather than a literal zero --
 * `__Func_8010704(0x40, 0, 0x20, 0x20, f1, f1)` inside the `else` of
 * `if (f1 != 0)`. A value already live in a register needs no move, so gcc's
 * one-register habit never shows.
 *
 * SHAPE 2, four lines, at the LAST use of each of two shared mask constants:
 *
 *     rom    orr r5, r3 / strb r5, [r0]      (third use, mask register dies)
 *     ours   orr r3, r5 / strb r3, [r0]
 *
 * The first two uses of each mask match. On the third the ROM makes the MASK's
 * register the destination, because it is dead afterwards; gcc keeps using the
 * loaded value's register. `*p = 8 | *p;` is canonicalised to `*p |= 8;` and is
 * byte-identical.
 *
 * MEASURED (rom 222 lines, all at exact length):
 *   baseline                                              222, ten
 *   `px`/`py` for the two SetPos constants assigned in
 *     the entry block (the batch-176 basic-block lever)   222, seven
 *   `*p = 8 | *p;` at the third use only                  222, seven (inert)
 *   the two masks ALSO named in the entry block           236, 232 (four locals
 *                          is too many -- gcc spills, exactly as
 *                          ovl_7b9cb4/200a6c0.c records for eight)
 *   -fno-schedule-insns / -fno-gcse / -fno-strength-reduce
 *     / -fno-defer-pop                                    222, seven (inert)
 *   -fno-schedule-insns2                                  222, fifty-two (worse)
 *
 * THE LEVER WORKED AND THEN RAN OUT, in one function. Two locals in the
 * dominating block fixed the `__MapActor_SetPos` interleave (ten to seven); adding
 * two more to fix the masks cost fourteen lines and badly broke it. That is the
 * bound recorded in ovl_7b9cb4/200a6c0.c, now measured at its edge: a
 * reading that the batch-331 section below REFUTES; do not build on it.
 *
 * WHAT IS RIGHT: the four-way flag cascade with its cross-jumped
 * `__DeleteFieldActor` tails; the named `gState` base for the halfword at
 * `0xe1 << 1`; `0xb4 << 17` and `0xa8 << 16`; the actor pointers ADVANCED IN
 * PLACE (`p = __MapActor_GetActor(0x16) + 0x59;`); and passing a flag variable
 * as a stack argument where the ROM does, which is what makes seven of the
 * eight six-argument calls come out exactly.
 *
 * NEXT: see the batch-331 section below -- the line that used to stand here,
 * claiming nothing source-level in eight probes, is REFUTED for shape one.
 *
 * ================= BATCH 331 BRIEF A =================
 *
 * THE STACK ARGUMENTS ARE SOURCE-REACHABLE.  The park's closing line, "nothing
 * source-level in eight probes", is refuted for its own shape one.  Naming the
 * two distinct stack-argument literals in locals assigned immediately before
 * the call removes that whole shape -- three of the park's seven, spelled in
 * words because that figure is dead.  Two simultaneously live pseudos is what
 * forces gcc to use two registers and materialise both before storing either;
 * with literals it reuses one register and interleaves the stores.  The body
 * above now carries this.
 *
 * AND THAT REFUTES THE PARK'S OWN SPILL BOUND.  The header recorded "two
 * dominating-block locals fit in this function and four do not", from a probe
 * that named the two MASK constants.  px, py, s5 and s6 are four named locals
 * and they cost nothing, because all four are constants that take a REG_EQUIV
 * and get rematerialised.  The dimension that probe actually varied was WHICH
 * values were named, not how many.  Do not read the old sentence as a ceiling
 * on the count.
 *
 * THE ORR DESTINATION, WITH THE DECIDING CODE NAMED.  What remains is the
 * park's shape two, twice: the last use of each shared mask, where the ROM
 * makes the mask's register the destination.  The chain, read from the
 * compiler on disk and not recalled:
 *   - arm.md:2088-2095, *thumb_iorsi3, prints "orr %0, %0, %2" with operand one
 *     constrained to match operand zero.  So the destination is always the
 *     IOR's FIRST operand and the second operand can never become it.
 *   - local-alloc.c:1121-1177 is the only place that ties them.  The percent
 *     marker on operand one does get operand two past the first skip test at
 *     local-alloc.c:1138-1144, but the second test at local-alloc.c:1149-1153
 *     throws it out -- with one alternative and only operand one requiring an
 *     inout, n_matching_alts equals n_alternatives and the operand does not
 *     itself require an inout.  The comment there says it ignores commutativity
 *     to keep things simple.  So the IOR's operand ORDER is the whole decision.
 *   - regmove.c:1139-1215 then rewrites the destination TO that first operand,
 *     which is visible in .15.regmove as a set of the same pseudo the IOR reads.
 *   - and combine.c:3492-3505 re-canonicalises the order underneath any source
 *     spelling: for a commutative code it swaps when operand zero is class 'o'
 *     or a SUBREG of class 'o' while operand one is not class 'o'.  The QImode
 *     intermediates that expand creates for a char destination make exactly
 *     that asymmetry, so the mask is pushed to position one whichever side the
 *     source writes it on.  Confirmed in the dumps: a body written mask-first
 *     reads mask-first in .00.rtl and loaded-value-first in .13.combine.
 *
 * MEASURED INERT at the figure above, all at exact length (the masks named and
 * put on the left of the OR, at the third use and at all three; the same with a
 * bare literal on the left; the load named in a local so the source order
 * survives expand; twenty-four crossings of the load temp's type constructor
 * over int, char, unsigned char, short and unsigned int against an int or
 * unsigned char mask temp, times mask-left and mask-right, times third-use-only
 * and all-three; and the OR computed int-wide through a result temp, which
 * defeats the c-typeck narrowing that creates the SUBREGs combine swaps on).
 *
 * MEASURED WORSE, with a reason worth keeping: applying any different spelling
 * to ONE of three otherwise-identical statements splits the mask pseudo and
 * costs real instructions -- the int-wide temp at the third use only, and the
 * unsigned-char mask temp at the third use only, both grew the function.  So
 * expect that shape of answer from any further third-use-only probe, and vary
 * all three statements together.
 *
 * NEXT: not the operand order and not a temp's type.  The remaining lever has
 * to change which pseudo is the IOR's first operand AFTER combine, which means
 * defeating combine.c:3492-3505's class test rather than the source order.
 */
extern unsigned char gState[];
extern char *__MapActor_GetActor(int slot);
extern int __GetFlag(int);
extern void __ClearFlag(int);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __DeleteFieldActor(int slot);
extern void __Func_8091ff0(int a);
extern void __Func_8092950(int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern int __StartTask(void *fn, int pri);
extern void OvlFunc_931_2008d08(void);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Actor_SetSpriteFlags(char *a, int f);
extern void __Func_8092b08(int a, int b);
extern void __WaitFrames(int n);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_800fe9c(void);

void OvlFunc_931_2008904(void)
{
    char *a;
    char *p;
    unsigned char *g;
    int f1;
    int f2;
    int f3;
    int px;
    int py;
    int s5;
    int s6;

    px = 0xb4 << 17;
    py = 0xa8 << 16;
    a = __MapActor_GetActor(0);
    f1 = __GetFlag(0x242);
    if (f1 != 0) {
        __CopyMapTiles(0x40, 0x20, 0, 0x20, 0x20, 0x20);
        __Func_8010704(0x40, 0x20, 0x20, 0x20, 0, 0);
        __DeleteFieldActor(0x14);
        __DeleteFieldActor(0x15);
    } else {
        f2 = __GetFlag(0x241);
        if (f2 != 0) {
            __CopyMapTiles(0x40, 0, 0, 0x20, 0x20, 0x20);
            __Func_8010704(0x40, 0, 0x20, 0x20, f1, f1);
            __DeleteFieldActor(0x11);
            __DeleteFieldActor(0x14);
            __DeleteFieldActor(0x15);
        } else {
            f3 = __GetFlag(0x90 << 2);
            if (f3 != 0) {
                __CopyMapTiles(0, 0x40, 0, 0x20, 0x20, 0x20);
                __Func_8010704(0, 0x40, 0x20, 0x20, f2, f2);
                __DeleteFieldActor(0x10);
                __DeleteFieldActor(0x11);
                __DeleteFieldActor(0x15);
            } else {
                __Func_8010704(0, 0x20, 0x20, 0x20, f3, f3);
                __DeleteFieldActor(0xf);
                __DeleteFieldActor(0x10);
                __DeleteFieldActor(0x11);
            }
        }
    }
    if (__GetFlag(0x8ff) != 0) {
        __DeleteFieldActor(0x12);
    } else {
        __Func_8091ff0(0xaa);
        __Func_8092950(0x12, 2);
        __MapActor_SetAnim(0x12, 3);
        __StartTask(OvlFunc_931_2008d08, 0xc8 << 4);
    }
    g = gState;
    if (*(short *)(g + (0xe1 << 1)) == 3)
        __ClearFlag(0x12f);
    s5 = 0x14;
    s6 = 0x29;
    __Func_8010704(0, 0x21, 4, 3, s5, s6);
    if (__GetFlag(0x906) != 0)
        __MapActor_SetPos(0x13, px, py);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x13), 0);
    __Func_8092950(0x16, 0xf);
    __Func_8092950(0x17, 0xf);
    __Func_8092950(0x18, 0xf);
    p = __MapActor_GetActor(0x16) + 0x59;
    *p |= 8;
    p = __MapActor_GetActor(0x17) + 0x59;
    *p |= 8;
    p = __MapActor_GetActor(0x18) + 0x59;
    *p |= 8;
    p = __MapActor_GetActor(0x16) + 0x23;
    *p |= 2;
    p = __MapActor_GetActor(0x17) + 0x23;
    *p |= 2;
    p = __MapActor_GetActor(0x18) + 0x23;
    *p |= 2;
    __Func_8092b08(0x16, 1);
    __Func_8092b08(0x17, 1);
    __Func_8092b08(0x18, 1);
    __WaitFrames(1);
    __CutsceneStart();
    __Func_80933f8(*(int *)(a + 8), *(int *)(a + 0xc), *(int *)(a + 0x10), 0);
    __Func_800fe9c();
    __CutsceneEnd();
    __WaitFrames(1);
}
