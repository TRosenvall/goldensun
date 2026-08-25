/* OvlFunc_911_20080cc, the whole of goldensun/asm/overlays/rom_79e5c0/ovl_30_a_c_a_a_c_b.s.
 *
 * Total .text for this TU = 66 bytes (= 0x42). The .s is replaced outright, so
 * no linker-script change was needed.
 *
 * One per-frame step for a timed projectile: three position words take their
 * velocities, the vertical velocity decays by 0xa3d, two more words advance by
 * a fixed 0x600, and a lifetime counter at +0x64 is decremented -- reaching
 * zero deletes the actor.
 *
 * THE COUNTDOWN HAS TO BE WRITTEN AT INT WIDTH WITH THE TEST CAST BACK.
 * `if (--a->f64 == 0)` looks right and costs an extra instruction: gcc keeps
 * the value in halfword width, so the decrement becomes
 * `ldr r3, =0xffff / add r2, r3 / and r3, r2` -- a masked add rather than a
 * subtract. Written as
 *
 *      t = a->f64 - 1;            (int)
 *      a->f64 = t;
 *      if ((unsigned short)t == 0)
 *
 * gcc emits the ROM's `sub r3, #1 / strh / lsl r3, #16 / cmp r3, #0`: the
 * arithmetic at int width, the truncation only where the value is TESTED.
 *
 * That is the narrow-constant rule from batch 71 seen from the other side --
 * there an int local was needed to stop a constant narrowing, here it is needed
 * to stop the ARITHMETIC narrowing. Same cause: gcc works in the width of the
 * eventual store unless a local says otherwise.
 */

struct A {
    unsigned char pad00[8];
    int f8;
    unsigned char pad0c[4];
    int f10;
    unsigned char pad14[4];
    int f18;
    int f1c;
    unsigned char pad20[4];
    int f24;
    unsigned char pad28[4];
    int f2c;
    unsigned char pad30[0x34];
    unsigned short f64;
};

extern void __DeleteActor(struct A *a);

int OvlFunc_911_20080cc(struct A *a)
{
    int v;

    a->f8 += a->f24;
    v = a->f2c;
    a->f10 += v;
    v += 0xfffff5c3;
    a->f2c = v;
    a->f18 += 0xc0 << 3;
    a->f1c += 0xc0 << 3;
    {
        int t;
        t = a->f64 - 1;
        a->f64 = t;
        if ((unsigned short)t == 0)
            __DeleteActor(a);
    }
    return 1;
}
