/* OvlFunc_886_20084dc  --  0x020084dc, cut from
 * goldensun/asm/overlays/rom_786f0c/ovl_30_c_c_c_c_c_c_c_c_c_c_a.s.
 *
 * A shop counter: the attendant opens shop 0x1 when the player is inside the
 * facing arc, and speaks otherwise, with the line chosen by two save bits.
 *
 * THE ARC TEST IS ONE UNSIGNED COMPARE ON A FULL WORD, and getting the width
 * right is the whole trick. The ROM writes
 *
 *     ldrh r3, [r0, #6] / add r3, r2   @ r2 = 0xffff5fff
 *     cmp r3, r2                       @ r2 = 0x3ffe
 *     bhi ...
 *
 * with no `lsl #16 / lsr #16` pair, so the subtraction wraps in 32 bits and the
 * comparison is unsigned over the whole word -- `(unsigned int)(f6 - 0xa001) <=
 * 0x3ffe`. The `(unsigned short)` form used elsewhere in the tree for arc tests
 * would add the shift pair and is wrong here; the absence of those two
 * instructions is what says which one to write.
 *
 * Both constants are past the eight-bit immediate range, so their pool loads
 * carry no information and no symbol is invented for them.
 *
 * Every arm is written in the ROM's fallthrough order.
 */

struct E { unsigned char pad00[6]; unsigned short f6; };

extern struct E *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __ActorMessage(int slot, int n);
extern void __Func_80b0278(int shop, int slot);
extern void __Func_8093054(int slot, int n);

void OvlFunc_886_20084dc(void)
{
    struct E *e;

    e = __MapActor_GetActor(0);
    if ((unsigned int)(e->f6 - 0xa001) <= 0x3ffe) {
        __Func_80b0278(1, 0x15);
    } else {
        __CutsceneStart();
        if (__GetFlag(0x87a)) {
            __MessageID(0x1c06);
            __Func_8093054(0x15, 0);
        } else {
            if (__GetFlag(0x815))
                __MessageID(0x11a2);
            else
                __MessageID(0xf53);
            __ActorMessage(0x15, 0);
        }
        __CutsceneEnd();
    }
}
