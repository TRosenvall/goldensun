/* Cluster OvlFunc_969_2009280..OvlFunc_969_2009280 extracted from goldensun/asm/overlays/rom_7f6e64/ovl_314_c_a_c_c_a_c.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7f6e64/ovl_314_c_a_c_c_a_c_a.o and the rest of the overlay
 * in goldensun/overlays/rom_7f6e64/overlay.ld.
 *
 * UNPARKED. This was the fourth member of the pool-load-first class (batch 32),
 * where gcc emits every literal-pool argument load ahead of every `mov`:
 *
 *     rom    mov r0, r5 / ldr r1, =0xcccc / ldr r2, =0x6666
 *     ours   ldr r1, =0xcccc / ldr r2, =0x6666 / mov r0, r5
 *
 * The park recorded both callee declarations, eight return types on the
 * preceding callee, seven optimisation flags and the stack-arg-pair adjacency
 * trick, all byte-identical. None of them was the variable.
 *
 * THE FIX IS THE ARG-INTERLEAVE LEVER, which turns out to retire this class as
 * well: the two pooled values are named locals assigned in a DIFFERENT BASIC
 * BLOCK from the call -- here before the `if`, with the call inside it. That
 * stops gcc keeping them in registers, so it rematerialises at the call, and
 * the rematerialised loads land after `mov r0` instead of before it.
 *
 * The two classes were filed separately because their symptoms look unrelated
 * -- one displaces a shift, the other displaces a pool load. They are the same
 * mechanism seen twice. See reports/arg-interleave.md.
 *
 * NOT REACHABLE FOR EVERY MEMBER. The lever needs a basic-block boundary
 * between the assignment and the call. OvlFunc_882_20083cc and its two siblings
 * are straight-line and stay parked; written this way they go from two
 * differing instructions to four.
 */
extern void __Func_8092950(int slot, int n);
extern void *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(void *actor, int f);
extern void __MapActor_SetSpeed(int slot, int a, int b);

void OvlFunc_969_2009280(int slot, int on)
{
    int a;
    int b;

    a = 0xcccc;
    b = 0x6666;
    if (on) {
        __Func_8092950(slot, 0);
        __Actor_SetSpriteFlags(__MapActor_GetActor(slot), 1);
        __MapActor_SetSpeed(slot, a, b);
    } else {
        __Func_8092950(slot, 0xf);
        __Actor_SetSpriteFlags(__MapActor_GetActor(slot), 0);
    }
}
