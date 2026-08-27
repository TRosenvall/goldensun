/* OvlFunc_common1_172c  --  shared by ovl_7db0c8, ovl_7ddb88 and ovl_7e0928
 *
 * The .s held ONLY this function and no data, so no split was needed; the .o
 * keeps its name (`asm/%.o: src/%.c`) and all three overlay linker scripts are
 * unchanged.
 *
 * A one-in-ten chance, per call, of spawning a small effect actor at a jittered
 * offset from the caller's position -- the ambient sparkle in the three
 * overlays that share this piece.
 *
 * THE POOL LOAD NEEDED THE BASIC-BLOCK LEVER. The actor id goes in r0, and gcc
 * issues its `ldr r0, =0x11d` before the three stack reads that fill r1-r3:
 *
 *     rom    ldr r1, [r6] / ldr r2, [r6, #4] / ldr r3, [r6, #8] / ldr r0, =0x11d
 *     ours   ldr r0, =0x11d / ldr r1, [r6] / ldr r2, [r6, #4] / ldr r3, [r6, #8]
 *
 * `int id = 0x11d;` at the top of the function -- a block that dominates both
 * `if`s -- rematerialises it at the call and it lands last. Five differing to
 * none. -fno-rerun-cse-after-loop does not touch it, which is consistent with
 * this being scheduling rather than CSE.
 *
 * The two randoms have to be NAMED. `__vec3_translate(__Random() << 4,
 * __Random(), v)` leaves the order of the two calls up to gcc; the ROM's first
 * result is the one that gets shifted, and only naming them says so.
 *
 * The opening test is a SIGNED RANGE, `a->f28 >= -0xff && a->f28 <= 0xff`.
 * gcc rewrites it as the unsigned `(x + 0xff) <= 0x1fe` the ROM has, and
 * builds 0x1fe as `mov r2, #0xff / lsl r2, #1` on its own.
 *
 * The `__Random() * 0x64 >> 16` scaling follows batch 96's rule (the second
 * operand becomes the multiply's destination), the same as
 * src/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_a.c.
 */
struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x28 - 0x14];
    int f28;
    unsigned char pad2c[0x55 - 0x2c];
    unsigned char f55;
};

extern unsigned char L7[] __asm__(".L7");
extern unsigned int __Random(void);
extern void __vec3_translate(int a, int b, int *v);
extern struct Actor *__CreateActor(int id, int x, int y, int z);
extern void __Actor_SetSpriteFlags(struct Actor *a, int f);
extern void __Actor_SetScript(struct Actor *a, unsigned char *s);
extern void __Actor_SetAnim(struct Actor *a, int n);

void OvlFunc_common1_172c(struct Actor *a)
{
    int v[3];
    struct Actor *q;
    unsigned int r1;
    unsigned int r2;
    int id;

    id = 0x11d;
    if (a->f28 >= -0xff && a->f28 <= 0xff)
        a->f55 = 0;
    if (__Random() * 0x64 >> 16 <= 9) {
        v[0] = a->f8;
        v[1] = a->fc;
        v[2] = a->f10;
        r1 = __Random();
        r2 = __Random();
        __vec3_translate(r1 << 4, r2, v);
        q = __CreateActor(id, v[0], v[1], v[2]);
        if (q != 0) {
            q->f55 = 0;
            __Actor_SetSpriteFlags(q, 0);
            __Actor_SetScript(q, L7);
            __Actor_SetAnim(q, 1);
            __Actor_SetAnim(q, 0);
        }
    }
}
