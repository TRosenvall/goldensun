/* Cluster OvlFunc_958_2008f44..OvlFunc_958_2008f44 extracted from
 * goldensun/asm/overlays/rom_7e636c/ovl_cc0_c_a_c_a_c_c_a.s.
 *
 * Matched on the first screen with NO lever, and the reason is worth recording
 * because it cuts against a habit this corpus encourages.
 *
 * The ROM emits `mov r0` in a DIFFERENT POSITION at four of the call sites --
 * first at __Func_809280c(0xb, 0, 0), in the MIDDLE at __Func_809280c(0, 0xb,
 * 0), LAST at __MapActor_SetAnim(0, 1) and __Func_8092848(0, 0xb, 0), and first
 * again at __MapActor_SetAnim(0xb, 2).  __MapActor_SetAnim in particular is
 * called twice with opposite orders, which is exactly the shape the batch-147
 * per-call-site declaration lever exists for.
 *
 * None of it needed a lever.  gcc produces all four positions unprompted from
 * the ordinary spelling, because the argument VALUES differ at each site and
 * that is what the order follows.  Varying `mov r0` position is not by itself
 * evidence that a call needs anything doing to it -- reach for the lever when
 * the position is wrong, not when it merely varies.
 *
 * The two halfword reads are signed: `mov r3, #0xa / ldrsh r1, [r0, r3]` and
 * the same at +0x12, which is the register-offset form thumb requires for a
 * signed halfword and therefore not a residue to chase.  See the batch-150 note
 * in docs/elevation.md.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __Func_809280c(int a, int b, int c);
extern void __MapActor_SetAnim(int a, int b);
extern void __Func_8092848(int a, int b, int c);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_TravelTo(int a, int b, int c);
extern void __MapActor_WaitMovement(int a);
extern void __MapActor_SetPos(int a, int b, int c);
extern void __SetFlag(int id);

void OvlFunc_958_2008f44(void)
{
    unsigned char *e;

    __CutsceneStart();
    __Func_809280c(0xb, 0, 0);
    __Func_809280c(0, 0xb, 0);
    __MapActor_SetAnim(0, 1);
    __CutsceneWait(0xa);
    __Func_8092848(0, 0xb, 0);
    __MessageID(0x23d9);
    __ActorMessage(0xb, 0);
    __MapActor_SetAnim(0xb, 2);
    e = __MapActor_GetActor(0);
    if (e != 0)
        __MapActor_TravelTo(0xb, *(short *)(e + 0xa), *(short *)(e + 0x12));
    __MapActor_WaitMovement(0xb);
    __MapActor_SetPos(0xb, 0, 0);
    __CutsceneWait(0x14);
    __SetFlag(0x9a << 4);
    __CutsceneEnd();
}
