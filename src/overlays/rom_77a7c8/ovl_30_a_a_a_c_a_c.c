/* OvlFunc_881_200811c  --  0x0200811c
 *
 * Whole of goldensun/asm/overlays/rom_77a7c8/ovl_30_a_a_a_c_a_c.s: that file
 * holds this function alone, so no split and no linker edit are needed.
 *
 * A countdown on the actor's halfword at +0x64. While it is still positive the
 * actor is destroyed; otherwise the field is bumped by one.
 *
 * STATEMENT ORDER IS LOAD-BEARING -- do not "tidy" the read of `cur` down into
 * the else arm or below the test. The halfword is read twice, once unsigned
 * and once signed, and Thumb `ldrsh` has no immediate-offset form, so its zero
 * offset has to sit in a register. That register is not an allocno: it is the
 * RELOAD SCRATCH of the sign-extending load pattern, and reload simply takes
 * the lowest hard register that is free at that instruction. Reading the
 * unsigned value FIRST starts `cur`'s live range one insn earlier, which makes
 * r1 busy across the `ldrsh` and pushes the scratch out to the ROM's r4.
 * Written the other way round the whole function is still correct and still
 * 15 instructions -- it just says `mov r1, #0 / ldrsh r3, [r2, r1]`.
 */
extern void __DeleteActor(void *a);

void OvlFunc_881_200811c(void *actor)
{
    unsigned short *timer;
    int cur;

    timer = (unsigned short *)((unsigned char *)actor + 0x64);
    cur = *timer;
    if (*(short *)timer <= 0)
        *timer = cur + 1;
    else
        __DeleteActor(actor);
}
