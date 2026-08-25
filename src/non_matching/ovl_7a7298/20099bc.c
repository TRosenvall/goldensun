/* OvlFunc_921_20099bc  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_7a7298/ovl_30_c_c_c_c_c_c_c.s
 * Best screen: 2 instructions in disagreeing regions, of 15 (streams same length).
 *
 * BLOCKER CLASS: argument precompute -- a COMPILER DIFFERENCE, diagnosed in
 * src/non_matching/ovl_780898/2008dc0.c. Not fixable from C.
 *
 * This function is the CONFIRMING TEST for that diagnosis, which is why it is
 * parked with its own note. It has two calls:
 *
 *   __MapActor_SetSpeed(0, 0x20000, 0x1999)   one cheap constant, two expensive
 *                                             -> PREDICTED to misorder
 *   __Func_8092158(0, 0xe8, 0xcc)             three cheap constants
 *                                             -> PREDICTED to match
 *
 * Both predictions held on the first screen. The entire diff is `mov r0, #0x0`
 * sinking to last in the first call; the second call is byte-identical.
 */
extern void __CutsceneStart(void);
extern void __MapTransitionIn(void);
extern void __CutsceneEnd(void);
extern void __MapActor_SetSpeed(int a, int x, int y);
extern void __Func_8092158(int a, int b, int c);

void OvlFunc_921_20099bc(void)
{
    __CutsceneStart();
    __MapTransitionIn();
    __MapActor_SetSpeed(0, 0x20000, 0x1999);
    __Func_8092158(0, 0xe8, 0xcc);
    __CutsceneEnd();
}
