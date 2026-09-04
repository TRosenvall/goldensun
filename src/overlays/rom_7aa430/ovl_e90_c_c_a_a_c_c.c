// fakematch
/* OvlFunc_923_2008f48  --  0x02008f48
 *
 * From goldensun/asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_c_c.s, which held this
 * function alone, so no split was needed.
 *
 * PARKED AT 2 OF 36 ON A `precompute_register_parameters` BIND, and the park's
 * reading of the mechanism was exactly right:
 *
 *     rom    ldr r2, =0x3333 / mov r0, #0      / ldr r1, =0x6666
 *     ours   ldr r2, =0x3333 / ldr r1, =0x6666 / mov r0, #0
 *
 * calls.c:805 copies every argument whose rtx_cost exceeds 2 into a pseudo
 * before any hard register is loaded. Both 0x6666 and 0x3333 are pool loads and
 * both exceed the threshold, so gcc precomputes both and the cheap `mov r0, #0`
 * lands last. The ROM precomputed only the third argument -- and the second is
 * identical to it in shape and in arm_rtx_costs, so as the park says, NO C
 * EXPRESSION SEPARATES THEM.
 *
 * ONE PIN ON r0 CLOSES IT:
 *
 *     register int q0 __asm__("r0");
 *     q0 = 0;
 *     __MapActor_SetSpeed(q0, 0x6666, 0x3333);
 *
 * WHY THAT WORKS WHEN NO EXPRESSION COULD. The park was looking for a way to
 * make gcc treat the two pool loads differently, and there is none -- they are
 * the same cost by construction. The pin does not touch that question. It
 * takes the THIRD argument out of the precompute path entirely by naming its
 * hard register, so there is nothing left to order against and `mov r0, #0`
 * stays where the source puts it. The bind was real; it was a bind on
 * expressions, and the park generalised it to the source.
 *
 * Fourth park in three batches whose diagnosis was correct about its mechanism
 * and wrong as a conclusion -- after the dominating-branch rule, the
 * same-value rule, and the basic-block prediction in
 * src/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_a_c_b.c.
 *
 * TORN DOWN. Pinning all three argument registers matches, and pinning r0 and
 * r2 matches; both are byte-identical to this one-pin form, so neither extra
 * pin earns its place. The park's own six measured spellings -- prototypes
 * added and withheld in four combinations, a named zero, and both pool
 * constants named as locals -- all stand at 2 or worse and none is needed.
 */

extern void *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __MapActor_SetSpeed(int slot, int vx, int vz);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __Actor_SetSpriteFlags(void *a, int f);
extern void __Func_8092b08(int slot, int n);
extern void __Func_809228c(int a, int b, int c);

void OvlFunc_923_2008f48(int a)
{
    __CutsceneStart();
    __PlaySound(0xe4);
    {
        register int q0 __asm__("r0");
        q0 = 0;
        __MapActor_SetSpeed(q0, 0x6666, 0x3333);
    }
    __Func_8092b08(0, 2);
    __Func_809228c(0, 0, -8);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    __CutsceneWait(8);
    __MapActor_SetPos(0, (a << 19) + (0x80 << 12), 0);
    __CutsceneWait(0x1e);
}
