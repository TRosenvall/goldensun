/* OvlFunc_886_20085d4  --  0x020085d4, cut from
 * goldensun/asm/overlays/rom_786f0c/ovl_30_c_c_c_c_c_c_c_c_c_c_c.s.
 *
 * A shop counter: the attendant opens shop 0x3 when the player is inside the
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
extern void OvlFunc_886_20081e8(void);

void OvlFunc_886_20085d4(void)
{
    struct E *e;

    e = __MapActor_GetActor(0);
    if ((unsigned int)(e->f6 - 0xa001) <= 0x3ffe) {
        __Func_80b0278(3, 0x14);
    } else if (__GetFlag(0x87a)) {
        __CutsceneStart();
        __MessageID(0x1c0a);
        __ActorMessage(0x14, 0);
        __CutsceneEnd();
    } else if (__GetFlag(0x815)) {
        OvlFunc_886_20081e8();
    } else {
        __CutsceneStart();
        __MessageID(0xf55);
        __ActorMessage(0x14, 0);
        __CutsceneEnd();
    }
}
