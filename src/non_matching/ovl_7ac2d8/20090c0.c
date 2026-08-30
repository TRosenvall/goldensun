/*
 * OvlFunc_924_20090c0 -- asm/overlays/rom_7ac2d8/ovl_f84_a_c_c_c_a_b.s
 * SPLIT OUT this round; byte-neutral, verified.
 *
 * BLOCKER: identical constants inside ONE basic block. 53 lines against 56 --
 * THREE SHORT.
 *
 * The ROM passes the same value as all three arguments of one call and builds
 * it three times:
 *
 *      mov r0,#0x80 / mov r1,#0x80 / mov r2,#0x80 / lsl r1,#9 / lsl r2,#9 / lsl r0,#9
 *
 * We emit one build and two copies -- `mov r2,#0x80 / lsl r2,#9 / mov r1,r2 /
 * mov r0,r2` -- because gcc CSEs identical constant rtx.
 *
 * THIS ONE IS PROPERLY CONTROLLED, unlike the claim I made about the
 * cross-call version of this shape and had to retract:
 *
 *   * ACROSS basic blocks the rebuild is routine and needs no lever: 76
 *     matching functions rebuild a constant across a call, and 31 rebuild one
 *     with no call between, and in every case checked the uses sit in
 *     different conditional branches.
 *   * WITHIN one basic block it never happens: ZERO of 3235 generated .s files
 *     build the same constant twice inside a single block.
 *   * A direct probe shows only two DISTINCT symbols avoid the CSE; a repeated
 *     literal and a repeated single symbol behave identically.
 *
 * So this is a genuine blocker rather than a spelling I have not found. It is
 * also a SMALL class -- scanning the remaining 2106 THUMB functions for the
 * same shape finds two others, Field_Carry_Target and InitWorldMap. This
 * function is not among them only because its three builds are interleaved,
 * which the detector's adjacent-pair pattern misses.
 *
 * Everything else in the function is exact, including the derived store value:
 * the ROM computes the 0x1c0 offset, adds it to the pointer, then does
 * `sub r2, #0xc0` and stores 0x100 from the same register, which the
 * `off = 0xe0 << 1; p = p + off; off -= 0xc0;` spelling reproduces.
 */
extern unsigned char *iwram_3001ebc;
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __CutsceneWait(int n);
extern void __Func_8012330(int a, int b, int c);
extern void __PlaySound(int id);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __Func_8012350(void);
extern void __Func_8091e9c(int n);

void OvlFunc_924_20090c0(void)
{
    unsigned char *p;
    int off;

    if (__GetFlag(0xc4 << 2) && __GetFlag(0x311) && __GetFlag(0x312)) {
        __SetFlag(0x876);
        __CutsceneWait(0x1e);
        __Func_8012330(0x80 << 9, 0x80 << 9, 0x80 << 9);
        __PlaySound(0x8d);
        __CutsceneWait(0x3c);
        p = iwram_3001ebc;
        off = 0xe0 << 1;
        p = p + off;
        off -= 0xc0;
        *(int *)p = off;
        __MapTransitionOut();
        __WaitMapTransition();
        __PlaySound(0x121);
        __Func_8012330(-1, -1, 0xe666);
        __Func_8012350();
        __Func_8091e9c(0xd);
    } else {
        __ClearFlag(0x876);
    }
}
