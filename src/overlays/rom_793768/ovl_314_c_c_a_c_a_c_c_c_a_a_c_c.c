/* Cluster OvlFunc_898_200885c..OvlFunc_898_200885c, 112 bytes of .text.
 *
 * One of three byte-identical siblings in overlay rom_793768, differing only
 * in the actor slot and the message id. From the branch-over-pool class; the
 * pool needed no help.
 *
 * DELETING THE POINTER LOCAL IS WHAT MATCHED IT. Holding `&a->f64` in an
 * explicit `unsigned short *` gets 44 lines against 44 with SIX differing,
 * and all six are the same ordering shape twice over -- gcc hoists the
 * `ldrh` above the `mov r8` that saves the field, where the ROM defers it.
 * Writing `a->f64 |= 2;` and `a->f64 &= 1;` directly is exact. Same lever as
 * Func_80c1084 in batch 155: a named local used only to hold an address can
 * cost the ordering, and removing it is the fix rather than respelling it.
 *
 * Measured and negative, all still 6: the constant on the left of the
 * operator; narrow `unsigned short` named constants; `int` named constants;
 * the saved field as `int`. Assigning the pointer before reading the field is
 * worse (13).
 *
 * The field at +6 is SIGNED: the ROM reads it with a REGISTER-offset `ldrsh`
 * and writes it back with an immediate-offset `strh`, which is the documented
 * signedness tell.
 */
struct A {
    unsigned char pad00[6];
    short f6;
    unsigned char pad08[0x5c];
    unsigned short f64;
};

extern struct A *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __WaitFrames(int n);
extern void OvlFunc_898_200973c(int a, int b, int c);
extern void OvlFunc_898_2009724(int a, int b);

void OvlFunc_898_200885c(void)
{
    struct A *a;
    short saved;

    a = __MapActor_GetActor(0xf);
    saved = a->f6;
    a->f64 |= 2;
    __CutsceneStart();
    __MessageID(0x122d);
    __MapActor_SetAnim(0xf, 0);
    OvlFunc_898_200973c(0xf, 0, 2);
    OvlFunc_898_2009724(0xf, 0xa);
    a->f6 = saved;
    __WaitFrames(1);
    __CutsceneEnd();
    a->f64 &= 1;
}
