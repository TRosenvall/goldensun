/* OvlFunc_882_2009600  --  0x02009600
 *
 * Cut out of goldensun/asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_c_c_b.s.
 *
 * A one-shot conversation guarded by two flags.
 *
 * NEEDS BOTH CSE_CFLAGS AND THE LEVER, which is the batch-106 order. 0x836 is
 * tested at the top and set at the bottom and gcc holds it in r5 across the
 * whole body, adding a push/pop -- the flag alone is 5 of 43, the three
 * levered locals alone do not survive the r5 hoist, and together they are
 * exact.
 *
 * Drafted by a parallel screening agent and re-screened here before wiring.
 */
extern int __GetFlag(int flag);
extern void __SetFlag(int flag);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __Func_8093040(int a, int b, int c);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __CutsceneWait(int n);
extern void __ActorMessage(int a, int b);

void OvlFunc_882_2009600(void)
{
    int e;
    int w;
    int q;

    e = 0x101;
    w = 0xbf << 1;
    q = 0x26b;
    if (__GetFlag(0x836) != 0)
        return;
    if (__GetFlag(0x837) != 0)
        return;
    __CutsceneStart();
    __MessageID(0xe6c);
    __Func_8093040(0x16, 0, 0x14);
    __MapActor_Emote(0, e, 0x28);
    __Func_80921c4(0, w, q);
    __Func_809280c(0, 0x16, 0);
    __Func_80925cc(0, 2);
    __CutsceneWait(0x1e);
    __ActorMessage(0x16, 0);
    __SetFlag(0x836);
    __CutsceneEnd();
}
