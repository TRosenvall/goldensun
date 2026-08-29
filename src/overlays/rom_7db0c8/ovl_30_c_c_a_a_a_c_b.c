/* Cluster OvlFunc_954_2008178..OvlFunc_954_2008178 extracted from goldensun/asm/overlays/rom_7db0c8/ovl_30_c_c_a_a_a_c.s.
 *
 * Slotted between ovl_30_c_c_a_a_a_c_a.o and the rest of the overlay.
 *
 * A spin-wait with a timeout, written as an un-rotated `do`/`while` with an
 * early test before it -- the ROM checks once, branches past the loop, and
 * enters the body directly thereafter.
 *
 * THE COUNTER INCREMENT GOES AFTER THE __WaitFrames CALL, not before it. The
 * ROM has `mov r0, #1 / add r5, #1 / bl __WaitFrames` -- the increment
 * SCHEDULED INTO the argument slot -- and writing `i++;` first puts the add
 * ahead of the `mov r0`, which is 2 of 22. Writing it after lets gcc schedule
 * it into the slot itself.
 *
 * That is the same swap tried on OvlFunc_956_20081c8 in batch 56, where it
 * improved the count but could not close the function because a pre-header load
 * merge remained. Here there is nothing else in the way, so the construct is
 * confirmed reachable rather than merely helpful.
 */
extern void __WaitFrames(int n);
extern int L441c __asm__(".L441c");

void OvlFunc_954_2008178(void)
{
    int i;
    int v;

    __WaitFrames(0xa);
    v = L441c;
    i = 0;
    if (v == 0x16)
        return;
    do {
        __WaitFrames(1);
        i++;
        if (i > 0x77)
            return;
        v = L441c;
    } while (v != 0x16);
}
