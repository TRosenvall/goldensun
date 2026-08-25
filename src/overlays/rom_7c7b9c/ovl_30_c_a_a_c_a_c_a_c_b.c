/* Cluster OvlFunc_943_200b150..OvlFunc_943_200b150 extracted from goldensun/asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c.s.
 *
 * Slotted between ovl_30_c_a_a_c_a_c_a_c_a.o and the rest of the overlay.
 *
 * A SWITCH AND AN UN-ROTATED LOOP, and both selectors are UNSIGNED.
 *
 * The switch lowers to gcc`s balanced tree -- `cmp #1 / beq` then `cmp #1 / bcc`
 * then 2 and 3 -- and the `bcc` is what says the argument is unsigned. Writing
 * it as a `switch` reproduces the tree directly; an if/else chain would not.
 *
 * THE LOOP COUNTER IS UNSIGNED TOO, and that was the last instruction to fall:
 * `while (i <= 8)` on a signed `int` gives `ble` where the ROM has `bls`. One
 * of 44. The loop is a `do`/`while` because the ROM enters the body directly
 * rather than jumping to the test first.
 *
 * Both flag ids and the returned slot walk together inside the loop, so all
 * three increments are in the source.
 *
 * Near-twin of ovl_7cb2c0/ovl_30_c_c_c_c_c_c_a_c_c_a_c_b.c, which takes a
 * second argument choosing the starting slot.
 */
extern int __GetFlag(int id);

int OvlFunc_943_200b150(unsigned int which)
{
    int base;
    int slot;
    unsigned int i;

    base = 0;
    slot = 0x17;
    switch (which) {
    case 0:
        base = 0x92c;
        break;
    case 1:
        base = 0x935;
        break;
    case 2:
        base = 0x917;
        break;
    case 3:
        base = 0x99 << 4;
        break;
    }
    i = 0;
    do {
        if (__GetFlag(base))
            return slot;
        i++;
        base++;
        slot++;
    } while (i <= 8);
    return 0;
}
