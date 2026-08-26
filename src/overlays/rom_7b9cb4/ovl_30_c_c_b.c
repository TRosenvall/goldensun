/* OvlFunc_932_200ba44  --  0x0200ba44, cut from
 * goldensun/asm/overlays/rom_7b9cb4/ovl_30_c_c.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7b9cb4/ovl_30_c_c_a.o and
 * asm/overlays/rom_7b9cb4/ovl_30_c_c_c.o in
 * goldensun/overlays/rom_7b9cb4/overlay.ld.
 *
 * Clears two halfwords in the overlay's own storage and starts a task.
 *
 * `ldr r2, =0` IS NOT THE POOL TELL HERE. Both stores are to `unsigned short`
 * globals, so the zero is a HImode constant and gcc pools it -- `ldr r2, =0x0`
 * with the pool entry a full word, which is what the ROM has. Batch 83 met the
 * same thing from the other side. No symbol is needed and none is invented.
 *
 * The task priority is a NAMED LOCAL, and that is what fixes the argument
 * order: written as a literal, gcc emits `ldr r0, =OvlFunc_932_200b9c8` before
 * `lsl r1, #4`, where the ROM has them the other way round. Leaving __StartTask
 * implicit, or giving it an `int` return type, also works -- all three were
 * measured -- and the local is preferred because it keeps the declaration.
 *
 * The two storage cells are reached with gcc's asm-label extension rather than
 * by renaming them, per docs/elevation.md: they are `.L5260` and `.L525c` in
 * the overlay's own .s and nothing else has to change.
 */
extern unsigned short L5260 __asm__(".L5260");
extern unsigned short L525c __asm__(".L525c");
extern void OvlFunc_932_200b9c8(void);
extern void __StartTask(void *fn, int prio);

void OvlFunc_932_200ba44(void)
{
    int pr;

    L5260 = 0;
    L525c = 0;
    pr = 0xc8 << 4;
    __StartTask(OvlFunc_932_200b9c8, pr);
}
