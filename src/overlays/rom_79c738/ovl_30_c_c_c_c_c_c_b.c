/* OvlFunc_909_20084ec  --  0x020084ec, cut from the head of
 * goldensun/asm/overlays/rom_79c738/ovl_30_c_c_c_c_c_c.s.
 *
 * The cut was at the head of the .s, so this file takes the first .text slot
 * and the remaining nine functions plus the data blob follow as
 * ovl_30_c_c_c_c_c_c_c.o.
 *
 * Hand over an item inside a cutscene: spawn the giver, test the recipient, and
 * on success play the accept animation and move three save bits; on failure
 * play a rejection sound and animation instead. Either way the spawned actor is
 * deleted and the cutscene ends.
 *
 * THE ARMS ARE WRITTEN IN THE ROM'S FALLTHROUGH ORDER. `cmp r0, r3 / beq .L53a`
 * with r3 built as -1 means the NOT-taken path is the success arm, so the
 * source is `if (... != -1) { success } else { failure }`. Written the other way
 * round it costs the branch condition and both arms' contents -- three
 * differences from one cause, which is the same trap batch 81 recorded on
 * OvlFunc_898_2008d78.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void *__Func_808e078(int a, int b, int c);
extern int __Func_8091a58(int a, int b);
extern void __MapActor_SetAnim(int slot, int a);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __PlaySound(int id);
extern void __DeleteActor(void *h);

void OvlFunc_909_20084ec(int slot, int b, int flag)
{
    void *h;

    __CutsceneStart();
    h = __Func_808e078(0, slot, b);
    if (__Func_8091a58(b, 0) != -1) {
        __MapActor_SetAnim(slot, 2);
        __SetFlag(0x84e);
        __SetFlag(flag);
        __ClearFlag(0x322);
        __ClearFlag(0x202);
    } else {
        __PlaySound(0x7d);
        __MapActor_SetAnim(slot, 5);
    }
    __DeleteActor(h);
    __CutsceneEnd();
}
