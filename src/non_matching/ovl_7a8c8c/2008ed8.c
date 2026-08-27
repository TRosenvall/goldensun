/* OvlFunc_922_2008ed8 -- NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_7a8c8c/ovl_30_c_a_c_c_c_a_c_c_c_c_a_a.s
 * Best screen: 8 differing of 43, streams the same length.
 *
 * BLOCKER CLASS: register allocation -- the sprite pointer lands in r0 where
 * the ROM has r1, and the +0x55 address is computed after the flag store rather
 * than before it.
 *
 * THREE LEVERS FROM THE TREE ALL APPLY HERE AND ALL WERE NEEDED to get this
 * far, which is why the file is worth keeping even unmatched:
 *
 *   the mask is a named `int` (`mask = ~0xc`), so it is built 32-bit with
 *   `mov r3, #0xd / neg r3, r3` rather than narrowed to `mov r3, #0xf3`
 *   -- batch 92, src/rom_8a000/rom_8d9a4_c_a_c_c_c_c_c_c.c
 *
 *   the OR'd 2 is a named `unsigned char`, which puts the CONSTANT in the
 *   destination of the `orr` -- batch 97,
 *   src/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_a_a_c_b.c
 *
 *   the null test is written positive, `if (n != 0) { ... return n; }
 *   return 0;`, so the return constant is not hoisted above it -- batch 96,
 *   src/non_matching/ovl_common/common0_18.c
 *
 * WHAT WAS TRIED AGAINST THE RESIDUE:
 *   - the +0x55 write through a named pointer computed before the flag store
 *     (the batch-97 two-pointer lever): WORSE, 45 lines and 32 differing,
 *     because it forces r12 into use
 *   - the sprite dereferenced inline instead of through a local (44 lines, 30)
 *   - the +0x55 store moved ahead of the flag store in the source (43 lines, 11)
 *
 * The plain field write below is the best of the four. This is the same family
 * as src/non_matching/ovl_common/common0_18.c, which has the identical masked
 * byte and is parked on the identical exchange -- except that THIS one also has
 * the `| 4` that makes src/rom_8a000/rom_8d9a4_c_a_c_c_c_c_c_c.c match, so the
 * "does the and feed an orr" theory recorded in docs/elevation.md does NOT
 * explain the split. That theory should be treated as refuted.
 */
struct Spr { unsigned char pad00[9]; unsigned char f9; };

struct A {
    unsigned char pad00[0x23];
    unsigned char f23;
    unsigned char pad24[0x50 - 0x24];
    struct Spr *f50;
    unsigned char pad54[1];
    unsigned char f55;
};

extern struct A *__CreateActor(int a, int b, int c, int d);
extern void __Actor_SetSpriteFlags(struct A *a, int f);
extern void __Func_80929d8(struct A *a, int n);

struct A *OvlFunc_922_2008ed8(int a, int b, int c, int d)
{
    struct A *n;
    struct Spr *s;
    unsigned char *q23;
    unsigned char two;
    int mask;

    n = __CreateActor(d, a, b, c);
    if (n != 0) {
        s = n->f50;
        mask = ~0xc;
        s->f9 = (mask & s->f9) | 4;
        n->f55 = 0;
        __Actor_SetSpriteFlags(n, 0);
        __Func_80929d8(n, 0xf);
        q23 = &n->f23;
        two = 2;
        *q23 = two | *q23;
        return n;
    }
    return 0;
}
