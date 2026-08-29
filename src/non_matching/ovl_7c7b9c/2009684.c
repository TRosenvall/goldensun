/* OvlFunc_943_2009684 -- 0x02009684, asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_a.s
 *
 * 109 of 109 lines, 17 differing -- of which TWO are not real: `bl __umodsi3`
 * against the ROM's `bl _umodsi3_RAM` is the linker alias this overlay already
 * carries (`__umodsi3 = _umodsi3_RAM;` in overlay.ld, line 71).  So 15 real.
 * Candidate: scratch/Y9684_F.c.
 *
 * TWO FINDINGS WORTH REUSING, both of which moved the count a long way:
 *
 *   1. THE DIVISION HELPER NAMES THE SIGNEDNESS.  The first transcription used
 *      `__Random() % 0x5a` with __Random declared `int`, which emits __modsi3.
 *      The ROM calls the UNSIGNED helper.  Declaring `extern unsigned int
 *      __Random(void);` took the screen from 55 differing to 17 in one edit.
 *      Read which of divsi3/udivsi3/modsi3/umodsi3 the ROM calls and let that
 *      decide the operand types -- it is a free signedness oracle and it is
 *      cheaper than reasoning about the values.
 *
 *   2. THE HALFWORD STORES NEED AN INT INTERMEDIATE, not an unsigned short
 *      destination.  `*(unsigned short *)p = 0x80 << 8` POOLS the constant
 *      (`ldr r3, =0x8000`) where the ROM builds `mov r3,#0x80 / lsl r3,#8`;
 *      routing it through an int local builds it.  That is the opposite of what
 *      the HImode note in docs/elevation.md predicts for a sign-extending value,
 *      and it is a second case where that rule's two remedies are not
 *      interchangeable.
 *
 * BLOCKER: the int intermediate wins the build and loses the register.
 *      rom   bl GetActor / mov r3,#0x80 / lsl r3,#8 / strh r3,[r0,#6]
 *      ours  mov r5,#0x80 / lsl r5,#8 / bl GetActor / strh r5,[r0,#6]
 * The ROM computes the value AFTER the call, so it lives in scratch r3.  A named
 * int is assigned before the call, so it must survive it and gcc gives it
 * callee-saved r5.  Naming the POINTER instead moves the problem rather than
 * solving it -- then the pointer takes r5 and the store becomes `strh r3,[r5,#6]`.
 * Something has to cross the call, and the ROM has nothing crossing it.
 *
 * TRIED: unsigned short destination with the shift inline (64, pooled); signed
 * short the same (64); an `unsigned short *` pointer local (64); one shared int
 * local and two separate ones (17 both).
 *
 * ALSO LEFT: the argument-setup interleave at the last __MapActor_SetPos, and
 * the placement of the gScript pool load relative to `mov r5, r0`.  Those are
 * downstream of the register question and are not worth attacking until it moves.
 */
