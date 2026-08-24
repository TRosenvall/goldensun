/* Cluster OvlFunc_884_20085e8..OvlFunc_884_20085e8 extracted from goldensun/asm/overlays/rom_784360/ovl_30_c_a_a_a_c_c_a_c.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_784360/ovl_30_c_a_a_a_c_c_a_c_a.o and the rest of the
 * overlay in goldensun/overlays/rom_784360/overlay.ld.
 *
 * A two-way talk on save bit 0x840.
 *
 * ONE MESSAGE ID IS A SYMBOL AND THE OTHER IS A LITERAL, and the ROM says so
 * directly -- `ldr r0, =_MSG_eb0` against `ldr r0, =0xeb1`. _MSG_eb0 was
 * already in message.sym, put there by an earlier batch for a different
 * function. So one arm is written `(int)(&_MSG_eb0)` and the other `0xeb1`,
 * which looks inconsistent and is what the ROM has.
 *
 * The flag is `0x84 << 4`, not the literal 0x840: the ROM builds it with
 * `mov r0,#0x84 / lsl r0,#4` rather than pooling it.
 */
extern int _MSG_eb0;
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_8092848(int a, int b, int c);
extern int __GetFlag(int id);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);

void OvlFunc_884_20085e8(void)
{
    __CutsceneStart();
    __Func_8092848(0x10, 0, 0xa);
    if (__GetFlag(0x84 << 4)) {
        __MessageID(0xeb1);
        __ActorMessage(0x10, 0);
    } else {
        __MessageID((int)(&_MSG_eb0));
        __ActorMessage(0x10, 0);
    }
    __CutsceneEnd();
}
