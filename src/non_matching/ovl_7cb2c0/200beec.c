/* OvlFunc_945_200beec -- NON-MATCHING, 14 differing STREAM encodings of 63.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE -- a header,
 * a diagnosis, and no compilable body -- so it had no figure, was NO CLAIM in
 * parkcheck, and was unrankable in every band.  But its own header named a
 * candidate at scratch/wbeec.c, that file still existed, and it
 * compiles.  The work had been done and left in a GITIGNORED directory instead
 * of installed, so nothing in the tree could see it.
 *
 * ref 63 / ours 63 stream encodings; 42/42 16-bit instructions -- IN PHASE, so a true DISTANCE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7cb2c0/200beec.c \
 *     asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c_c_a_c_c.s --func OvlFunc_945_200beec
 *
 * 70 of the 133 prose parks were in this state; tools/scratchfill.py found them.
 * THE FIGURE IS EVIDENCE; the prose below it is a HYPOTHESIS, and across pass
 * two a park's stated blocker has been wrong roughly 40 times in 42.  The
 * original park text is preserved underneath.
 */
extern void __CutsceneStart(void);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void OvlFunc_945_200d004(void);
extern void OvlFunc_945_200c8ac(int a, int b, int c, int d);
extern void OvlFunc_945_200c8e8(int a, int b, int c);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __MessageID(int id);
extern void __Func_8093040(int a, int b, int c);

void OvlFunc_945_200beec(void)
{
    __CutsceneStart();
    __MapActor_SetPos(0, 0, 0);
    OvlFunc_945_200d004();
    __MapActor_SetPos(0x12, 0x96 << 16, 0x24a0000);
    OvlFunc_945_200c8ac(0x9c << 16, -1, 0x86 << 18, 0x1000001);
    OvlFunc_945_200c8e8(8, 0, 0);
    __MapActor_SetSpeed(0x12, 0xcccc, 0x6666);
    __Func_80921c4(0x12, 0xa8, 0x242);
    __Func_80921c4(0x12, 0xa8, 0x22a);
    __Func_8092adc(0x12, 0x80 << 8, 0x14);
    __Func_809259c(0x12, 2);
    __MessageID(0x1e3c);
    __Func_8093040(0x12, 0, 0x14);
    OvlFunc_945_200c8e8(9, 0xc, 0);
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_945_200beec -- asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c.s
 *
 * BLOCKER: SPLIT SHIFTED BUILD -- see src/non_matching/ovl_7cb2c0/200be34.c
 *
 * 14 of 56, same length, first diff at position 8.  Byte-for-byte the same
 * residue as its sibling OvlFunc_945_200be34 in the same overlay -- same call
 * sequence with actor slot 0x12 instead of 0x10:
 *
 *     rom  mov r1,#0x96 / mov r0,#0x12 / lsl r1,#0x10 / ldr r2,=0x24a0000
 *     ours mov r1,#0x96 / lsl r1,#0x10 / ldr r2,=0x24a0000 / mov r0,#0x12
 *
 * Everything measured on 200be34 applies; the full table is in that park.  The
 * function is straight line (no labels), so the basic-block lever has nothing
 * to bite on.  Reachable in principle -- 51 of the 2987 generated .s files
 * contain the shape -- but no spelling has been found.
 *
 * Best C: scratch/wbeec.c.
 *)

*/
#endif
