/* Cluster OvlFunc_964_2009038..OvlFunc_964_2009038 extracted from goldensun/asm/overlays/rom_7ed0a0/ovl_30_a_a_c_c_a_a_c.s.
 *
 * The .s held this function and nothing else, so the C replaces the whole
 * translation unit and the linker script is untouched.
 *
 * Waits up to 0x3c frames for the actor's field at +0xc to fall to or below
 * the one at +0x14, then commits.
 *
 * THE LEVER IS THE LOOP SHAPE. The ROM's continue-test is the BACK EDGE:
 *
 *      bgt .L103e      <- conditional jumps BACKWARD to the loop head
 *      b   .L1056      <- unconditional falls out
 *
 * Written as a forward goto -- `if (w > v) goto loop; goto join;` -- gcc
 * inverts it and emits `ble join / b loop`, the conditional going FORWARD. That
 * is the only difference between this function at 2 of 26 and at an exact
 * match. A `do { ... } while (w > v);` puts the condition on the back edge,
 * which is what the ROM has.
 *
 * The `if (i == 0) goto zero;` sits INSIDE the do/while, because the ROM tests
 * the counter at the top of each iteration and the value test at the bottom.
 * `w` is genuinely uninitialised on the path where the counter runs out, and
 * that path leaves through `zero:` before reading it -- which is what the ROM
 * does too.
 *
 * An earlier attempt with a plain `while` loop sat at 9 of 26: gcc merged the
 * two `ldr r2, [r5, #0x14]` loads, the in-loop one and the counter-expired one.
 * Spelling the two paths out separately keeps both.
 *
 * At the join the constant 0x80 << 24 is built BEFORE the store of `v`,
 * matching the ROM's `mov r3, #0x80 / lsl r3, #24 / str r2, ... / str r3, ...`.
 */
extern void __WaitFrames(int n);

void OvlFunc_964_2009038(void *actor)
{
    unsigned char *a;
    int i;
    int v;
    int w;
    int z;
    int k;

    a = (unsigned char *)actor;
    i = 0x3c;
    do {
        if (i == 0)
            goto zero;
        __WaitFrames(1);
        w = *(int *)(a + 0xc);
        v = *(int *)(a + 0x14);
        i--;
    } while (w > v);
    goto join;
zero:
    v = *(int *)(a + 0x14);
join:
    z = 0;
    *(int *)(a + 0x28) = z;
    k = 0x80 << 24;
    *(int *)(a + 0xc) = v;
    *(int *)(a + 0x3c) = k;
}
