/* Cluster OvlFunc_896_200c3bc..OvlFunc_896_200c3bc extracted from goldensun/asm/overlays/rom_78ef88/ovl_314_c_c_c_c_c.s.
 *
 * Total .text for this TU = 224 bytes (= 0x00e0).
 * Preserves the original ROM layout when slotted before
 * asm/overlays/rom_78ef88/ovl_314_c_c_c_c_c_c.o in goldensun/overlays/rom_78ef88/overlay.ld.
 * The target was the FIRST of three functions, so there is no _a part; the
 * other two and the trailing .data travel together in _c.
 *
 * Spawns a burst of particles from actor 0xe over 32 frames, every other frame.
 *
 * THE STRUCT SIZE IS LOAD-BEARING AND IS THE ONLY THING THAT WAS WRONG.
 * `struct P` must be exactly 0x28 bytes.  The ROM's frame is `sub sp, #0x38`,
 * of which 0x10 is the outgoing stack-argument area for the eight-argument
 * OvlFunc_common0_10c call, leaving 0x28 for this local.  Pad it to 0x40 and
 * the frame becomes 0x50 and nothing else changes -- that single line was the
 * entire difference on the first screen, 2 of 97.
 *
 * So when a screen differs only in `sub sp, #N`, do not go looking at the body:
 * subtract the outgoing argument area from the ROM's frame and make the local
 * aggregate exactly that size.
 *
 * Everything else reproduced unaided, which is worth recording because the
 * function looked expensive: the eight-argument call with four words spilled to
 * [sp], the 16.16 fixed-point masking written as `>> 16 << 16`, __Random
 * declared UNSIGNED so the shifts come out `lsr` (see the division-helper note
 * in docs/elevation.md for the same signedness question), and the do/while whose
 * counter lives in a high register.
 */
struct P {
    int f0;
    int f4;
    int f8;
    int fc;
    unsigned char pad10[8];
    short f18;
    unsigned char pad1a[0xe];
};

extern unsigned int __Random(void);
extern void __PlaySound(int id);
extern void __CutsceneWait(int n);
extern int *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(int *actor, int f);
extern void __Func_8092950(int a, int b);
extern void OvlFunc_common0_10c(int a, int b, int c, int d, int e, int f, int g, struct P *p);

void OvlFunc_896_200c3bc(void)
{
    struct P p;
    int *a;
    unsigned int i;
    int t;
    int x, y;

    a = __MapActor_GetActor(0xe);
    __PlaySound(0xbe);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0xe), 0);
    p.f0 = 1;
    p.f4 = 5;
    p.f18 = 0x8e << 1;
    p.f8 = 0x6666;
    p.fc = 0xc0 << 10;
    i = 0;
    do {
        __CutsceneWait(1);
        t = 1 & i;
        if (t == 0) {
            x = a[2] + ((__Random() * 24) >> 16 << 16);
            x += 0xfff40000;
            y = a[3] + ((__Random() << 5) >> 16 << 16);
            y += 0x80 << 14;
            OvlFunc_common0_10c(x, y, a[4], 0, 0xfffc0000, t, 0xd8 << 13, &p);
        }
        if (i == 0x14)
            __Func_8092950(0xe, 0x80 << 1);
        i++;
    } while (i <= 0x1f);
    __Func_8092950(0xe, 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0xe), 1);
}
