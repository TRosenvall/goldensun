/* OvlFunc_952_200849c
 *
 * Cut out of goldensun/asm//overlays/rom_7d768c/ovl_30_c_a_a_c_a_b.s.
 *
 * A two-stage conversation with a one-shot branch. The first parameter really
 * is unused -- the ROM only keeps r1.
 *
 * BUILT WITH CSE_CFLAGS.
 *
 * Screened by a parallel agent; re-verified here before wiring.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __Func_8097608(void);
extern void __CutsceneWait(int n);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __Func_809280c(int slot, int a, int b);
extern void __MapActor_DoAnim(int slot, int a);
extern void __Func_8092adc(int slot, int a, int b);

void OvlFunc_952_200849c(int a, int slot)
{
    int p;
    int q;

    p = 0x80 << 1;
    q = 0x80 << 8;
    __CutsceneStart();
    __MessageID(0x2052);
    __ActorMessage(slot, 0);
    if (__GetFlag(0x968) == 0) {
        __SetFlag(0x968);
        __Func_8097608();
        __CutsceneWait(0x32);
        __MapActor_Emote(slot, p, 0x46);
        __Func_809280c(slot, 0, 0x28);
        __ActorMessage(slot, 0);
        __CutsceneWait(0x1e);
        __MapActor_DoAnim(slot, 4);
        __CutsceneWait(0x14);
        __ActorMessage(slot, 0);
        __Func_8092adc(slot, q, 0);
    }
    __CutsceneEnd();
}
