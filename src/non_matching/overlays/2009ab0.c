/* OvlFunc_959_2009ab0  --  0x02009ab0, asm/overlays/rom_7e7574/ovl_9dc_c_a_c_c_a_a_a_a.s
 *
 * Source asm: goldensun/asm/overlays/rom_7e7574/ovl_9dc_c_a_c_c_a_a_a_a.s
 *
 * BLOCKER CLASS: a pool load scheduled too early. Second member, after
 * OvlFunc_883_20091d8, and the shape is identical.
 * Status: 41 lines against 41, 7 differing, all of them one instruction sliding
 * across three calls.
 *
 *     rom    ... bl __MapActor_SetBehavior / ldr r5, =0x240d / mov r0, r5
 *     ours   ldr r5, =0x240d / ... three calls ... / mov r0, r5
 *
 * The message id is loaded once and reused as `id` and `id + 1`, which is
 * right: written as two literals gcc emits two pool entries and the function
 * comes out shorter than the ROM. `id++` in place of `id + 1` makes no
 * difference, and neither does assigning `id` after the first `__MessageID`.
 *
 * Measured: -fno-schedule-insns 7, -fno-gcse 7. gcc hoists the load because
 * nothing stops it -- the value has no dependencies and r5 is callee-saved
 * either way -- and turning the schedulers off does not expose the ROM's order.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int n);
extern void __MapActor_SetBehavior(int slot, int b);
extern void __MapActor_SetIdle(int slot);
extern void __MapActor_SetAnim(int slot, int a);
extern void __MapActor_Emote(int slot, int e, int n);
extern void __MapTransitionOut(void);
extern void __Func_809228c(int a, int b, int c);
extern void __Func_8091e9c(int n);

void OvlFunc_959_2009ab0(void)
{
    int id;

    __CutsceneStart();
    __Func_809228c(9, 0, 0);
    __MapActor_SetBehavior(9, 1);
    __MapActor_SetIdle(9);
    __MapActor_SetAnim(9, 0);
    __MapActor_SetBehavior(0, 1);
    id = 0x240d;
    __MessageID(id);
    __ActorMessage(9, 0);
    __MapActor_Emote(0, 0x81 << 1, 0x3c);
    __MessageID(id + 1);
    __ActorMessage(9, 0);
    __Func_8091e9c(0x3c);
    __MapTransitionOut();
    __CutsceneEnd();
}
