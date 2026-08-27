/* OvlFunc_927_200a004 -- NOT MATCHING. 38 differing of 43 (ours 44 lines).
 * ref: asm/overlays/rom_7b4558/ovl_30_c_c_c_a_a.s
 *
 * BLOCKER: same class as OvlFunc_953_200a5f0 -- constant hoisted across calls
 * in a straight-line function.  0xba<<18 and 0xfc<<17 are each used twice (the
 * __Func_80933f8 call and the final __MapActor_SetPos); gcc builds both into
 * r5/r6 before the first call and adds a push the ROM does not have.
 *
 * CONTROL: with the final SetPos given distinct constants the function is
 * 5 of 43 and 43 lines -- everything matches except the last argument block,
 * where the ROM has `mov r1 / mov r2 / mov r0 / lsl r1 / lsl r2` and gcc has
 * r0 last.  Note the SECOND call (__Func_80933f8) reproduces the identical
 * interleave with plain literals and no help at all.
 *
 * MEASURED: -fno-rerun-cse-after-loop is byte-identical (38).  All four
 * combinations of int/void return type on __MapActor_SetPos and __SetFlag are
 * 5 of 43 on the control -- the return-type lever does not reach this rotation.
 * Straight-line, so the basic-block lever is unreachable.
 */
extern void __CutsceneStart(void);
extern void OvlFunc_927_2008ea8(int, int);
extern void __Func_80933f8(int, int, int, int);
extern void OvlFunc_927_2008d90(int, int, int, int);
extern void OvlFunc_927_2008e18(int);
extern void __Func_8092950(int, int);
extern void *__MapActor_GetActor(int);
extern void __Actor_SetSpriteFlags(void *, int);
extern void __CutsceneWait(int);
extern void __SetFlag(int);
extern void __MapActor_SetPos(int, int, int);
extern void __CutsceneEnd(void);

void OvlFunc_927_200a004(void)
{
    __CutsceneStart();
    OvlFunc_927_2008ea8(0x12, 1);
    __Func_80933f8(0xba << 18, -1, 0xfc << 17, 1);
    OvlFunc_927_2008d90(0x12, 0xba << 2, 0xfc << 1, 0x90 << 12);
    OvlFunc_927_2008e18(0x12);
    __Func_8092950(0x12, 0xf);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x12), 0);
    __CutsceneWait(0x1e);
    __SetFlag(0x30a);
    __MapActor_SetPos(0x16, 0xba << 18, 0xfc << 17);
    __CutsceneEnd();
}
