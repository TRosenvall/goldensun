/* OvlFunc_969_2008424, the whole of goldensun/asm/overlays/rom_7f6e64/ovl_314_a_a_c.s.
 *
 * Total .text for this TU = 64 bytes (= 0x40). The .s is replaced outright,
 * so no linker-script change was needed.
 *
 * Byte-identical to OvlFunc_936_20080ac in overlays/rom_7c097c; this C is that
 * file's verbatim, with only the symbol changed.
 *
 * A random idle timer: while the countdown at +0x66 is non-zero it just ticks
 * down; when it hits zero it jitters the facing halfword at +6 by a 15-bit
 * random and reloads the countdown with `(random * 80) >> 16`.
 *
 * THE SAME MEMBER IS READ SIGNED AND UNSIGNED, AND BOTH READS SURVIVE.
 * `ldrsh` for the zero test, `ldrh` for the value that gets decremented. Batch
 * 76 recorded that two reads of one member fold into one -- that is true when
 * they are the same read. These are different: `a->f66` and
 * `(unsigned short)a->f66` produce different values for a negative field, so
 * CSE cannot unify them and gcc emits both loads.
 *
 * THE ORDER OF THOSE TWO LINES DECIDES A REGISTER. Written signed-first, the
 * zero index for the `ldrsh` is born first and takes r2; the ROM has it in r1,
 * which means its index pseudo was created SECOND. Writing the unsigned read
 * first fixes it. Two instructions, and the emission order is unchanged --
 * the scheduler puts the `ldrsh` back in front either way.
 *
 * TWO MORE THINGS THAT WERE NEEDED:
 *   the countdown value is an `int` local. As `unsigned int` gcc knows it fits
 *   sixteen bits and turns `u - 1` into `ldr r1, =0xffff / add r3, r2, r1` --
 *   a masked add rather than a subtract.
 *   both exits converge on one `return 1` via a `goto`. Written as an early
 *   `return 1` inside the branch, gcc emits the `mov r0, #1` twice.
 */

struct A {
    unsigned char pad00[6];
    unsigned short f6;
    unsigned char pad08[0x5e];
    short f66;
};

extern int __Random(void);

int OvlFunc_969_2008424(struct A *a)
{
    int t;
    int u;
    int v;

    u = (unsigned short)a->f66;
    t = a->f66;
    if (t == 0) {
        a->f6 += ((unsigned int)__Random() << 15) >> 16;
        v = ((unsigned int)(__Random() * 80)) >> 16;
        a->f66 = v;
        if (v == 0)
            goto done;
        u = v;
    }
    a->f66 = u - 1;
done:
    return 1;
}
