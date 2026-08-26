/* OvlFunc_940_200808c  --  0x0200808c, cut from
 * goldensun/asm/overlays/rom_7c5974/ovl_30_c_c_a_c_a.s.
 *
 * The inn counter: the attendant opens inn 8 when the player is inside the
 * facing arc AND the save bit is set, and speaks otherwise, with the line
 * chosen by the same bit.
 *
 * Two rules, and both are about not writing what the ROM's shape does not say.
 *
 *   THE ARC TEST IS UNSIGNED OVER THE WHOLE WORD. There is no
 *   `lsl #16 / lsr #16` pair around the subtraction, so it wraps in 32 bits:
 *   `(unsigned int)(f6 - 0xa001) <= 0x3ffe`. Same reading as the two shop
 *   counters in batch 84 -- the absence of two instructions is the signal.
 *
 *   THE COMPOUND CONDITION IS NESTED, NOT `&&`. The ROM tests the arc and the
 *   save bit with two separate `beq .Lb4`, both to the same target; an `&&`
 *   fuses into one comparison. Nesting the second `if` inside the first gives
 *   the ROM's pair. Same rule as OvlFunc_899_2008310's park.
 *
 * `__GetFlag(0x941)` really is called twice -- once for the arc gate and once
 * after the cutscene starts -- and both calls are in the source.
 */
struct E { unsigned char pad00[6]; unsigned short f6; };

extern struct E *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __Func_80b3284(int inn, int slot);
extern void __Func_8093054(int slot, int n);

void OvlFunc_940_200808c(void)
{
    struct E *e;

    e = __MapActor_GetActor(0);
    if ((unsigned int)(e->f6 - 0xa001) <= 0x3ffe) {
        if (__GetFlag(0x941)) {
            __Func_80b3284(8, 0x11);
            return;
        }
    }
    __CutsceneStart();
    if (__GetFlag(0x941)) {
        __MessageID(0x24fb);
        __Func_8093054(0x11, 0);
    } else {
        __MessageID(0x1bd0);
        __Func_8093054(0x11, 0);
    }
    __CutsceneEnd();
}
