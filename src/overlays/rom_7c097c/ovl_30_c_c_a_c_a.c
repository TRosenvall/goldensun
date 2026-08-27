/* OvlFunc_936_20082e8  --  0x020082e8
 *
 * The whole of goldensun/asm/overlays/rom_7c097c/ovl_30_c_c_a_c_a.s, which held
 * this function and no data, so the linker script's existing line for that
 * object now picks up this file's.
 *
 * A guard on the party leader's facing: the scene only plays if the leader is
 * turned away, and the last line differs depending on whether the player has
 * heard it before.
 *
 * THE FACING TEST IS DONE AT HALFWORD WIDTH. `(unsigned short)(f6 - 0x1000) >
 * 0x6000` gives the ROM's `ldr r2, =0xfffff000 / add r3, r2` -- the subtraction
 * carried out as an addition of the wrap-around value, pooled -- and the
 * `lsl #16 / cmp` against a pre-shifted 0x60000000. Doing the arithmetic in an
 * `int` would give a `sub` instead; batch 98's width rule, here wanting the
 * narrow form.
 *
 * THE CALLEE RETURNS `int`, NOT `void`. The last two instructions were a
 * rotation of `mov r0, #8` and `mov r1, #0` in front of __Func_8092c40.
 * Declaring it `void` puts r0 first; declaring it `int` puts r0 last, which is
 * the ROM. See src/rom_a1000/rom_a47b4_a_b.c, where batch 99 isolated this from
 * the parameter list -- it is the return type alone, and the r0 argument here
 * is a small CONSTANT, which the batch-93 phrasing of the rule said would not
 * respond. That caveat is withdrawn.
 */
struct A { unsigned char pad00[6]; unsigned short f6; };

extern char *iwram_3001ebc;
extern struct A *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int arg);
extern void __MapActor_DoAnim(int slot, int n);
extern void __Func_8092848(int a, int b, int c);
extern int __Func_8092c40(int a, int b);
extern int __Func_8091c7c(int a, int b);

void OvlFunc_936_20082e8(void)
{
    struct A *a;
    unsigned short *p;

    a = __MapActor_GetActor(0);
    if ((unsigned short)(a->f6 - 0x1000) > 0x6000) {
        __CutsceneStart();
        __Func_8092848(0, 8, 0);
        __CutsceneWait(0xa);
        __MessageID(0x2584);
        __Func_8092c40(8, 0);
        if (__Func_8091c7c(0, 0) == 0) {
            __MapActor_DoAnim(8, 4);
            __ActorMessage(8, 0);
        } else {
            p = (unsigned short *)(iwram_3001ebc + (0xec << 1));
            *p = *p + 1;
            __MapActor_DoAnim(8, 3);
            __ActorMessage(8, 0);
        }
        __CutsceneEnd();
    }
}
