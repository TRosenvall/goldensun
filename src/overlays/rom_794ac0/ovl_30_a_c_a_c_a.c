/* Cluster OvlFunc_899_200852c..OvlFunc_899_200852c extracted from goldensun/asm/overlays/rom_794ac0/ovl_30_a_c_a_c_a.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * A one-shot conversation: check two save flags, say one of three lines, and on
 * the first pass set a flag and run a short beat.
 *
 * PARKED ON constant-CSE AND NOW BUILT WITH -fno-rerun-cse-after-loop. The flag
 * id 0x851 is read and then written on the same path; at -O2 gcc hoists it into
 * a callee-saved register, paying a push and a pop to save one pool load, and
 * the function comes out 38 instructions against the ROM's 36 -- LONGER, which
 * is what made it the clearest specimen of the class.
 *
 * -fno-rerun-cse-after-loop is the pass responsible. -fno-gcse,
 * -fno-cse-follow-jumps, -fno-cse-skip-blocks and -fno-expensive-optimizations
 * all leave the hoist untouched. See the CSE_CFLAGS block in the Makefile for
 * the caveat: this is an assumption about the original build on thin evidence,
 * and sweeping all 85 parked files with the flag matched only this one and its
 * neighbour in rom_79c738.
 *
 * The C itself is unchanged from the parked version and was always correct --
 * unlike its neighbour, whose parked C had the wrong control flow.
 */
void OvlFunc_899_200852c(void)
{
    __CutsceneStart();
    if (__GetFlag(0x856) != 0) {
        if (__GetFlag(0x851) == 0) {
            __MessageID(0x1276);
            OvlFunc_899_2008354(0x10);
            __CutsceneWait(0xa);
            OvlFunc_899_200c63c(0x10, 3, 0x14);
            __SetFlag(0x851);
        } else {
            __MessageID(0x1278);
        }
    } else {
        __MessageID(0x1250);
    }
    OvlFunc_899_2008354(0x10);
    __CutsceneEnd();
}
