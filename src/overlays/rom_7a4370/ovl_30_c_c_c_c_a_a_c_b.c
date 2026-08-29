/* OvlFunc_917_200952c  --  0x0200952c
 *
 * Cut out of goldensun/asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_c.s.
 *
 * The per-frame task for one falling-debris actor: advance it along an arc for
 * 0x78 frames, then release its sprite slot and delete it.
 *
 * THE MAIN PATH IS THE `if` BODY, NOT AN EARLY RETURN. Written as
 * `if (n > 0x77) { cleanup; return; }` followed by the arc code, gcc inverts
 * the test and lays the cleanup block first -- 42 differing of 55. Written as
 * `if (n <= 0x77) { arc } else { cleanup }` it is exact. The ROM tells you
 * which way round: its `bgt` skips FORWARD over the arc code, so the arc is the
 * fallthrough and therefore the `if` body.
 *
 * The step is `translate(n << 16, ((n * 3) << 8) + d, v)` -- the `n * 3` is the
 * ROMs `lsl r1, r2, #1 / add r1, r2`, which is gccs own strength reduction, not
 * something written as a shift.
 *
 * The frame counter at +0x64 is read as a SIGNED short for the comparison and
 * incremented as an UNSIGNED one; both spellings appear in the ROM (`ldrsh`
 * then `ldrh`) and both are needed.
 */
struct Actor {
    unsigned char pad00[8];
    int x;
    int y;
    int z;
    unsigned char pad14[0x18 - 0x14];
    int f18;
    int f1c;
    unsigned char pad20[0x38 - 0x20];
    int f38;
    int f3c;
    int f40;
    unsigned char pad44[0x50 - 0x44];
    unsigned char *f50;
};

extern void __vec3_translate(int a, int b, int *v);
extern void __Func_8003f3c(int n);
extern void __DeleteActor(struct Actor *a);

void OvlFunc_917_200952c(struct Actor *a)
{
    int v[3];
    short *t;
    int n;
    int d;

    t = (short *)((char *)a + 0x64);
    n = *t;
    if (n <= 0x77) {
    v[0] = a->f38;
    v[1] = a->f3c;
    v[2] = a->f40;
    d = *(short *)((char *)a + 0x66);
    __vec3_translate(n << 16, ((n * 3) << 8) + d, v);
    a->x = v[0];
    a->y = v[1];
    a->z = v[2];
    a->f18 += 0x147;
    a->f1c += 0x147;
    *(unsigned short *)t = *(unsigned short *)t + 1;
    } else {
        __Func_8003f3c(a->f50[0x1c]);
        __DeleteActor(a);
    }
}
