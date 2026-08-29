/* OvlFunc_953_2008648 -- NOT MATCHING. 6 differing of 43.
 * ref: asm/overlays/rom_7d95dc/ovl_30_c_c_c_a_a_a_c_c_a.s
 *
 * BLOCKER: straight-line arg-interleave, three sites, two positions each.
 *     rom   mov r1, #0xd0 / mov r0, #0x12 / lsl r1, #8 / mov r2, #0x14
 *     ours  mov r1, #0xd0 / lsl r1, #8    / mov r0, #0x12 / mov r2, #0x14
 * Only the three __Func_8092adc calls are wrong; the last call
 * (OvlFunc_953_2009c5c, same mov/lsl shape) comes out exactly right with
 * literals, so this is the scheduler and not the spelling.
 *
 * MEASURED, all 6 of 43: literals; `int q; q = 0xd0; q <<= 8;` per site (the
 * statement-form argument lever); `int q = 0xd0 << 8;` per site;
 * -fno-schedule-insns; -fno-rerun-cse-after-loop.  `int` return type on
 * __Func_8092adc is 9 (worse).  -fno-schedule-insns2 and -O1 are 13 (worse).
 * The function has no branch, so the basic-block lever cannot be applied.
 */
extern void __CutsceneStart(void);
extern void __Func_8092848(int, int, int);
extern void __MessageID(int);
extern void OvlFunc_953_2009c48(int);
extern void __Func_8092adc(int, int, int);
extern void __MapActor_DoAnim(int, int);
extern void OvlFunc_953_2009c5c(int, int);
extern void __CutsceneEnd(void);

void OvlFunc_953_2008648(void)
{
    __CutsceneStart();
    __Func_8092848(0x12, 0, 0x14);
    __MessageID(0x2122);
    OvlFunc_953_2009c48(0x12);
    __Func_8092adc(0x12, 0xd0 << 8, 0x14);
    __Func_8092adc(0x12, 0xb0 << 8, 0x14);
    __Func_8092adc(0x12, 0x80 << 8, 0x28);
    __Func_8092848(0x12, 0, 0x14);
    OvlFunc_953_2009c48(0x12);
    __MapActor_DoAnim(0x12, 3);
    OvlFunc_953_2009c48(0x12);
    OvlFunc_953_2009c5c(0x12, 0xa0 << 7);
    __CutsceneEnd();
}
