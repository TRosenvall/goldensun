/* Cluster OvlFunc_945_2008670..OvlFunc_945_2008670 extracted from goldensun/asm/overlays/rom_7cb2c0/ovl_30_c_c_a_a_a_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7cb2c0/ovl_30_c_c_a_a_a_a_a.o and the rest of the overlay in
 * goldensun/overlays/rom_7cb2c0/overlay.ld.
 *
 * A four-way talk on two save flags and a yes/no answer.
 *
 * `v = 0xd0 << 8;` at the top is the basic-block lever -- the call that uses it
 * is three branches deep. And `__Func_8092c40` is left UNDECLARED, which is the
 * subtractive form of the declaration lever: declared, gcc fills its r0 first
 * and the ROM fills r1 first. Both are in docs/elevation.md.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_808e118(void);
extern int __GetFlag(int id);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern int __Func_8091c7c(int a, int b);
extern void OvlFunc_945_2009f3c(void);
extern void __Func_809259c(int a, int b);
extern void __Func_8092adc(int a, int b, int c);

void OvlFunc_945_2008670(void)
{
    int v;

    v = 0xd0 << 8;
    __CutsceneStart();
    __Func_808e118();
    if (__GetFlag(0x921)) {
        __MessageID(0x1dd4);
        __ActorMessage(0xa, 0);
    } else if (__GetFlag(0x922)) {
        __MessageID(0x1d91);
        __Func_8092c40(0xa, 0);
        if (!__Func_8091c7c(0, 0)) {
            OvlFunc_945_2009f3c();
        } else {
            __Func_809259c(0xa, 2);
            __ActorMessage(0xa, 0);
            __Func_8092adc(0xa, v, 0);
        }
    } else {
        __MessageID(0x1d31);
        __ActorMessage(0xa, 0);
    }
    __CutsceneEnd();
}
