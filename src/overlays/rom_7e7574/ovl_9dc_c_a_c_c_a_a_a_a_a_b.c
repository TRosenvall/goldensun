/* OvlFunc_959_2009ab0
 *
 * Cut out of goldensun/asm//overlays/rom_7e7574/ovl_9dc_c_a_c_c_a_a_a_a_a_b.s.
 *
 * A two-line exchange that walks the message id forward with `id++`.
 *
 * THE MESSAGE ID IS A SYMBOL. `id = 0x240d;` puts `ldr r5, =0x240d` two calls
 * earlier than the ROM regardless of where the assignment is written, and no
 * flag moves it; `id = (int)&_MSG_240d;` lands it exactly. A pool load of a
 * SYMBOL is not hoisted where a pool load of an int constant is -- a second,
 * independent tell for the symbol class, and it fires on values too large for
 * `mov` where the small constant pooled tell says nothing.
 *
 * `_MSG_240d` was added to message.sym in its own commit.
 *
 * Drafted by a parallel screening agent; re-screened here before wiring.
 */
extern int _MSG_240d;
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_809228c(int a, int b, int c);
extern void __MapActor_SetBehavior(int slot, int b);
extern void __MapActor_SetIdle(int slot);
extern void __MapActor_SetAnim(int slot, int n);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int n);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __Func_8091e9c(int n);
extern void __MapTransitionOut(void);

void OvlFunc_959_2009ab0(void)
{
    int id;

    __CutsceneStart();
    __Func_809228c(9, 0, 0);
    __MapActor_SetBehavior(9, 1);
    __MapActor_SetIdle(9);
    __MapActor_SetAnim(9, 0);
    __MapActor_SetBehavior(0, 1);
    id = (int)&_MSG_240d;
    __MessageID(id);
    __ActorMessage(9, 0);
    __MapActor_Emote(0, 0x81 << 1, 0x3c);
    id++;
    __MessageID(id);
    __ActorMessage(9, 0);
    __Func_8091e9c(0x3c);
    __MapTransitionOut();
    __CutsceneEnd();
}
