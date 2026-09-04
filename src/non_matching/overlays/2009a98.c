/* OvlFunc_943_2009a98 -- 0x02009a98,
 * asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_a_a_c.s
 *
 * Resets a cutscene stage: blank the camera target, park eight actors at the
 * origin, then place two of them at fixed spots with fixed facings and hand off
 * to OvlFunc_943_2009c14.
 *
 * 28 of 75. Candidate below.
 *
 * THE HEADLINE RESULT IS THAT THE `-1` TRIPLE IS NOT UNBROKEN AFTER ALL.
 * tools/pickable.py rejects any function with three or more `neg` on the
 * strength of batch 148, which recorded the shape as an unbroken class:
 *
 *     rom     mov r0,#1 / mov r1,#1 / mov r2,#1 / neg r1 / neg r2 / neg r0
 *     plain   mov r6,#1 / neg r6,r6 / mov r1,r6 / mov r2,r6 / mov r0,r6
 *
 * gcc builds -1 once and copies it, because the three arguments want the same
 * value and cse has no reason to rebuild. Plain C measures a total mismatch --
 * 75 of 75, with the prologue widened to `push {r5, r6, lr}` against the ROM's
 * bare `push {r14}`.
 *
 * PINNING THE FOUR ARGUMENT REGISTERS AND NEGATING IN PLACE reproduces the
 * triple exactly and takes it to 27, with the negations landing in the ROM's
 * own order. So the class is reachable by the fakematch idiom; it was recorded
 * as unbroken because the idiom had not been tried on it, not because it
 * resists. pickable.py's rejection should be read as "expensive", not
 * "impossible" -- and the entry in batch 148 wants amending.
 *
 * WHAT STILL BLOCKS IT: 28 of 75, and the residue is a second hoist plus an
 * ordering. 0xe8 is used twice -- as `0xe8 << 16` for a position and again for
 * a camera argument -- and gcc parks it in callee-saved r5, which is what keeps
 * the prologue one register wider than the ROM's. Pinning that site too holds
 * the line count at 75 but does not clear the residue, and the first
 * disagreement moves from instruction 0 to instruction 2 rather than
 * disappearing.
 *
 * TRIED:
 *   a  plain C throughout                                       75 of 75
 *   b  the four -1 arguments pinned, negated in place           27
 *   c  b, plus the 0xe8 site pinned with a two-step shift       28, 75 lines
 *   d  c, with the pinned values assigned rather than
 *      initialised, to move the `mov r0, #1`                    28
 * c and d tie exactly, which is the usual sign that the remaining lever is not
 * in the spelling.
 *
 * The honest reading is that this needs a further round with the teardown
 * discipline applied properly -- b is better than c on the count, so the second
 * pin may be actively wrong rather than merely insufficient, and that was not
 * chased.
 */

extern void __CutsceneStart(void);
extern void __WaitFrames(int n);
extern void __MapActor_SetPos(int slot, int x, int z);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_800fe9c(void);
extern void OvlFunc_943_2009c14(int a, int b);

void OvlFunc_943_2009a98(void)
{
    unsigned char *a;

    __CutsceneStart();
    {
        register int p0 __asm__("r0") = 1;
        register int p1 __asm__("r1") = 1;
        register int p2 __asm__("r2") = 1;
        register int p3 __asm__("r3") = 0;
        p1 = -p1;
        p2 = -p2;
        p0 = -p0;
        __Func_80933f8(p0, p1, p2, p3);
    }
    __WaitFrames(1);
    __MapActor_SetPos(0x14, 0, 0);
    __MapActor_SetPos(0x16, 0, 0);
    __MapActor_SetPos(0x18, 0, 0);
    __MapActor_SetPos(0x19, 0, 0);
    __MapActor_SetPos(0x1a, 0, 0);
    __MapActor_SetPos(0x1b, 0, 0);
    __MapActor_SetPos(0, 0, 0);
    __MapActor_SetPos(0x17, 0, 0);
    a = __MapActor_GetActor(0x17);
    *(short *)(a + 6) = 0xc0 << 6;
    {
        register int q1 __asm__("r1") = 0xe8;
        register int q2 __asm__("r2") = 0x28a0000;
        register int q0 __asm__("r0") = 0x15;
        q1 <<= 16;
        __MapActor_SetPos(q0, q1, q2);
    }
    a = __MapActor_GetActor(0x15);
    *(short *)(a + 6) = 0xb0 << 8;
    __Func_80933f8(0xe8 << 16, -1, 0x9f << 18, 0);
    __Func_800fe9c();
    __WaitFrames(1);
    OvlFunc_943_2009c14(0x17, 0x15);
}
