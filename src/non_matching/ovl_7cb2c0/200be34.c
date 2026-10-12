/* OvlFunc_945_200be34 -- NON-MATCHING, 14 differing STREAM encodings of 70.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE -- a header,
 * a diagnosis, and no compilable body -- so it had no figure, was NO CLAIM in
 * parkcheck, and was unrankable in every band.  But its own header named a
 * candidate at scratch/rbe34.c, that file still existed, and it
 * compiles.  The work had been done and left in a GITIGNORED directory instead
 * of installed, so nothing in the tree could see it.
 *
 * ref 70 / ours 70 stream encodings; 48/48 16-bit instructions -- IN PHASE, so a true DISTANCE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7cb2c0/200be34.c \
 *     asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c_c_a_c_c.s --func OvlFunc_945_200be34
 *
 * 70 of the 133 prose parks were in this state; tools/scratchfill.py found them.
 * THE FIGURE IS EVIDENCE; the prose below it is a HYPOTHESIS, and across pass
 * two a park's stated blocker has been wrong roughly 40 times in 42.  The
 * original park text is preserved underneath.
 */
extern void __CutsceneStart(void);
extern void OvlFunc_945_200c8e8(int a, int b, int c);
extern void OvlFunc_945_200c8ac(int a, int b, int c, int d);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __MessageID(int id);
extern void __Func_8093040(int a, int b, int c);

void OvlFunc_945_200be34(void)
{
    __CutsceneStart();
    OvlFunc_945_200c8e8(0x18, 0, 0);
    OvlFunc_945_200c8e8(0x12, 0, 0);
    __MapActor_SetPos(0, 0, 0);
    __MapActor_SetPos(0x10, 0x96 << 16, 0x24a0000);
    OvlFunc_945_200c8ac(0x9c << 16, -1, 0x86 << 18, 0x1000001);
    OvlFunc_945_200c8e8(8, 0, 0);
    __MapActor_SetSpeed(0x10, 0xcccc, 0x6666);
    __Func_80921c4(0x10, 0xa8, 0x242);
    __Func_80921c4(0x10, 0xa8, 0x22a);
    __Func_8092adc(0x10, 0x80 << 8, 0x14);
    __Func_809259c(0x10, 2);
    __MessageID(0x1e3c);
    __Func_8093040(0x10, 0, 0x14);
    OvlFunc_945_200c8e8(9, 0xc, 0);
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_945_200be34 -- asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c.s
 *
 * BLOCKER: THE SPLIT SHIFTED BUILD -- unreachable, proved by corpus test
 *
 * 14 of 63, same length.  49 lines exact, including the four-argument
 * OvlFunc_945_200c8ac call with its interleaved lsl/neg/lsl, which matches
 * byte for byte.  The entire residue is one transposition:
 *
 *     rom  mov r1,#0x96 / mov r0,#0x10 / lsl r1,#0x10 / ldr r2,=0x24a0000
 *     ours mov r1,#0x96 / lsl r1,#0x10 / ldr r2,=0x24a0000 / mov r0,#0x10
 *
 * The ROM slots `mov r0, #0x10` between the two halves of the `0x96 << 16`
 * build; gcc completes the build first.  Structurally identical to the
 * `mov #K / mov #0 / neg / mov #0` family in ovl_7c460c/2008c74.c -- a
 * multi-instruction constant build split by an unrelated `mov`.
 *
 * CORRECTED: this is NOT unreachable.  I first measured "0 of 2987 generated
 * .s files contain mov rA,#K / mov rB,#K2 / lsl rA,#n" and concluded the class
 * was closed.  The detector was broken: generated .s files are gcc's own output
 * and use DECIMAL immediates and the THREE-operand shift (`lsl r2, r2, #1`),
 * while my pattern required the ROM's two-operand form.  It matched nothing.
 *
 * Re-measured with both forms accepted and a positive control:
 *     adjacent mov/lsl build (control) : 777 of 2987
 *     split mov / mov / lsl            :  51 of 2987
 *
 * gcc DOES emit this shape, 51 times.  So the spelling exists and I have not
 * found it.  This park is "not yet solved", not "cannot be solved".
 *
 * Best C: scratch/rbe34.c.
 *)

*/
#endif
