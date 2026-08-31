/* OvlFunc_945_200dca4  [overlays/rom_7cb2c0]
 *
 * Source asm: goldensun/asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_c_c_a_c.s
 *
 * BLOCKER CLASS: arg-interleave. TWO instructions of 43, and they are the same
 * two in the opposite order:
 *
 *     rom    mov r1, #0xd0 / mov r0, #0x8  / lsl r1, #0x8
 *     ours   mov r1, #0xd0 / lsl r1, #0x8  / mov r0, #0x8
 *
 * Everything else is exact: all eleven calls, every argument, every shifted
 * constant, the prologue and the interwork epilogue. 43 lines against 43.
 *
 * WHY THE BASIC-BLOCK LEVER CANNOT BE APPLIED HERE. docs/elevation.md's
 * "assign the constant where the ROM cannot keep it" retires this class by
 * naming the value in a local assigned in a DIFFERENT basic block from the
 * call, which forces gcc to rematerialise it split at the call site. That
 * lever needs a basic-block boundary to cross, and the doc is explicit that a
 * call does not create one -- only a branch does.
 *
 * THIS FUNCTION HAS NO BRANCHES AT ALL. It is a straight-line cutscene script:
 * eleven calls in sequence, no condition, no loop, no early return. There is
 * exactly one basic block, so there is no "different block" to assign the
 * constant in and the lever has nothing to bite on.
 *
 * That is worth recording as the boundary of the lever rather than as a
 * failure of this function: a straight-line function whose only defect is an
 * arg-interleave is UNREACHABLE by the documented fix, and the cutscene
 * scripts in these overlays are mostly straight-line. Do not spend screens
 * re-deriving it on the next one.
 *
 * MEASURED, all at 2 differing:
 *   `0xd0 << 8` at the call site (house style, matches other 8092adc sites)  2
 *   `0xd000` written out                                                    2
 *   naming the first argument in a local before the call                    2
 *   -fno-schedule-insns2                                                   17
 *
 * The scheduler flag is the useful negative: it makes the function WORSE, and
 * the first divergence moves from instruction 24 to instruction 7. Post-reload
 * scheduling is what makes the other ten call sites come out right, so it
 * cannot be turned off to fix this one.
 *
 * BATCH-156 UPDATE -- THE THIRD DIRECTION EXISTS, AND IT DOES NOT REACH HERE.
 *
 * This park said there was no third direction to try. There is one, found on
 * Func_80b9a70 (src/rom_b5000/rom_b8228_c_a_c_c_c_b.c): split the constant's
 * two-instruction build across two SOURCE statements and put the independent
 * work between them --
 *
 *     b = 0xd0;      a = 8;      b <<= 8;      f(a, b, ...);
 *
 * gcc keeps the halves as written and the independent mov lands in the gap.
 * That is exactly the residue here, so it was tried on this function.
 *
 * MEASURED: 2 differing, first divergence still at instruction 24, residue
 * character-for-character unchanged. The lever does not bite.
 *
 * WHY -- and this is the useful part. On Func_80b9a70 the split constant is a
 * LIVE LOCAL: `flag` is used after the sequence, in `i | flag`. Here the
 * constant is consumed immediately as a call argument and is dead at the call.
 * gcc rematerialises argument temporaries during argument fill, which discards
 * any statement structure the source imposed on them.
 *
 * So the lever is scoped to values that outlive their construction, and every
 * park in this class is an argument temporary. That is a REASON these resist,
 * replacing the earlier guess that a basic-block boundary was the missing
 * ingredient -- this function's lack of branches is not what blocks it.
 *
 * Do not re-try the statement split on an argument temporary.
 */
extern void __CutsceneStart(void);
extern void __CutsceneWait(int n);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void OvlFunc_945_200c890(int a, int b, int c, int d);
extern void OvlFunc_945_200c8e8(int a, int b, int c);

void OvlFunc_945_200dca4(void)
{
    __CutsceneStart();
    OvlFunc_945_200c8e8(0xf, 1, 0);
    OvlFunc_945_200c890(9, 0xea << 1, 0x9a << 2, 0x80 << 8);
    OvlFunc_945_200c8e8(8, 1, 0x14);
    __Func_80925cc(9, 2);
    __CutsceneWait(0x14);
    __Func_8092adc(8, 0xd0 << 8, 0x50);
    __Func_8092adc(8, 0, 0x14);
    __MapActor_DoAnim(8, 3);
    __CutsceneWait(0x14);
    OvlFunc_945_200c8e8(9, 0x15, 0);
}
