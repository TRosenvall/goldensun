/* OvlFunc_943_2009684 -- NON-MATCHING, 16 differing STREAM encodings of 111.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE -- a header,
 * a diagnosis, and no compilable body -- so it had no figure, was NO CLAIM in
 * parkcheck, and was unrankable in every band.  But its own header named a
 * candidate at scratch/Y9684_F.c, that file still existed, and it
 * compiles.  The work had been done and left in a GITIGNORED directory instead
 * of installed, so nothing in the tree could see it.
 *
 * ref 111 / ours 111 stream encodings; 80/80 16-bit instructions -- IN PHASE, so a true DISTANCE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7c7b9c/2009684.c \
 *     asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_a_a_a_c.s --func OvlFunc_943_2009684
 *
 * 70 of the 133 prose parks were in this state; tools/scratchfill.py found them.
 * THE FIGURE IS EVIDENCE; the prose below it is a HYPOTHESIS, and across pass
 * two a park's stated blocker has been wrong roughly 40 times in 42.  The
 * original park text is preserved underneath.
 */
extern unsigned char gScript_943__0200c4d8[];

extern int __GetFlag(int id);
extern unsigned int __Random(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __Func_8092b08(int a, int b);

void OvlFunc_943_2009684(void)
{
    unsigned char *q;
    unsigned char *s;
    int f;
    int v;

    __Func_8092b08(0x1b, 1);
    __Func_8092b08(0x17, 1);
    __Func_8092b08(0x16, 1);
    __Func_8092b08(0x1a, 1);
    __Func_8092b08(0x18, 1);
    if (__GetFlag(0x92 << 4)) {
        __MapActor_SetPos(0x16, 0xa2 << 16, 0x29a0000);
        v = 0x80 << 8;
        *(unsigned short *)(__MapActor_GetActor(0x16) + 6) = v;
        __MapActor_SetPos(0x17, 0, 0);
        __MapActor_SetPos(0x14, 0, 0);
    }
    f = __GetFlag(0x922);
    if (f != 0) {
        __MapActor_SetPos(0x15, 0x84 << 17, 0x2be0000);
        v = 0xa0 << 7;
        *(unsigned short *)(__MapActor_GetActor(0x15) + 6) = v;
        q = __MapActor_GetActor(0x15);
        s = gScript_943__0200c4d8;
        *(short *)(q + 0x64) = __Random() % 0x5a + 0x3c;
        __MapActor_SetBehavior(0x15, s);
        __MapActor_SetPos(0x18, 0xf8 << 16, 0xaa << 18);
        q = __MapActor_GetActor(0x18);
        *(short *)(q + 0x64) = __Random() % 0x5a + 0x3c;
        __MapActor_SetBehavior(0x18, s);
        __MapActor_SetPos(0x16, 0, 0);
    } else if (__GetFlag(0x923)) {
        __MapActor_SetPos(0x14, 0xf6 << 16, 0x80 << 18);
        *(short *)(__MapActor_GetActor(0x14) + 6) = f;
    }
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_943_2009684 -- 0x02009684, asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_a.s
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
 *)

*/
#endif
