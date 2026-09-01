/* OvlFunc_948_200938c (0x0200938c) and OvlFunc_948_200949c (0x0200949c)
 * -- NON-MATCHING.  One park for both: they are the same cutscene over
 * different slots and constants, and they fail identically.
 *
 * Blocker class: DUPLICATE-CONSTANT CSE INTO A CALLEE-SAVED REGISTER, the
 * class docs/elevation.md records as unreachable from source spelling.
 *
 * Both call __MapActor_SetSpeed twice with the SAME pooled pair. The ROM
 * reloads both constants from the pool at each site; gcc hoists them into r5
 * and r6 before the first call and copies from there:
 *
 *     rom    bl __CutsceneStart / mov r0,#0 / ldr r1,=0x1e666 / ldr r2,=0xf333
 *     ours   ldr r5,=0x1e666 / ldr r6,=0xf333 / bl __CutsceneStart
 *            / mov r0,#0 / mov r1,r5 / mov r2,r6
 *
 * 55 lines against the ROM's 52, and the tell is in the prologue exactly as
 * the doc says: `push {r5, r6, r14}` where the ROM has `push {r14}`.
 *
 * WHY THE KNOWN LEVER DOES NOT APPLY HERE, which is the useful part.
 * The recorded remedy is SEPARATE LOCALS PER USE SITE, and it has a worked
 * instance in src/overlays/rom_7e7574/ovl_9dc_c_c_a_a_a_b.c -- six locals for
 * three SetSpeed calls, all the same value. Copying that spelling here (four
 * locals for two sites) is EXACTLY INERT: 55 lines, 54 differing, byte for byte
 * the same as the plain literals.
 *
 * The difference is the guards. In the working instance three
 * `if (a != 0)` blocks sit BETWEEN the assignments and the uses, so the
 * assignments do not dominate the uses in a single block and gcc has to
 * rematerialise. Here the only guard in the function sits AFTER both SetSpeed
 * calls, so the assignments and both uses are in one straight-line region and
 * the hoist happens whatever the source calls them.
 *
 * So the per-use-site-locals lever is the dominating-block mechanism again,
 * not a property of having several names. It needs a boundary, and this shape
 * does not have one.
 *
 * FLAGS, all measured, all giving 55 lines and 54 differing unchanged:
 *   -fno-rerun-cse-after-loop      (the doc rules this out for the class)
 *   -fno-gcse
 *   -fno-cse-follow-jumps
 *
 * WHAT IS RIGHT AND SHOULD BE KEPT: everything else. The bodies below are the
 * exemplar OvlFunc_948_20095f0 with the slots and constants changed, the
 * guard's two `ldrsh` reads through a register offset, the trailing
 * __ClearFlag(0x88 << 2), and the two functions' differing PlaySound/
 * CutsceneWait order. If the class is ever cracked, both come at once.
 *
 * NEXT: nothing source-level. This is a specimen for the duplicate-constant
 * hoist in a function with NO dominating boundary, which is the narrower
 * statement of that class than the doc currently carries.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __PlaySound(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_809228c(int a, int b, int c);
extern void __ClearFlag(int id);

void OvlFunc_948_200938c(void)
{
    unsigned char *e;

    __CutsceneStart();
    __MapActor_SetSpeed(0, 0x1e666, 0xf333);
    __MapActor_SetSpeed(8, 0x1e666, 0xf333);
    __PlaySound(0xbc);
    e = __MapActor_GetActor(0);
    if (e != 0)
        __MapActor_TravelTo(8, *(short *)(e + 0xa), *(short *)(e + 0x12));
    __MapActor_WaitMovement(8);
    __Func_809228c(0, 0, 0x18);
    __CutsceneWait(4);
    __PlaySound(0xbc);
    __Func_809228c(8, 0, 0x10);
    __MapActor_WaitMovement(0);
    __MapActor_TravelTo(8, 0xb4 << 1, 0x98);
    __MapActor_WaitMovement(8);
    __CutsceneEnd();
    __ClearFlag(0x88 << 2);
}

/* --- OvlFunc_948_200949c, same shape --- */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __PlaySound(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_809228c(int a, int b, int c);
extern void __ClearFlag(int id);

void OvlFunc_948_200949c(void)
{
    unsigned char *e;

    __CutsceneStart();
    __MapActor_SetSpeed(0, 0x1b333, 0xd999);
    __MapActor_SetSpeed(9, 0x1b333, 0xd999);
    __PlaySound(0xbc);
    e = __MapActor_GetActor(0);
    if (e != 0)
        __MapActor_TravelTo(9, *(short *)(e + 0xa), *(short *)(e + 0x12));
    __MapActor_WaitMovement(9);
    __Func_809228c(0, 0, 0x18);
    __PlaySound(0xbc);
    __CutsceneWait(4);
    __Func_809228c(9, 0, 0x10);
    __MapActor_WaitMovement(0);
    __MapActor_TravelTo(9, 0xa8, 0x84 << 1);
    __MapActor_WaitMovement(9);
    __CutsceneEnd();
    __ClearFlag(0x88 << 2);
}
