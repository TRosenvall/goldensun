/* OvlFunc_932_200ab58 -- NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_c.s
 * Best screen: 7 differing of 35, streams the same length, and all seven are
 * ONE register exchange.
 *
 * BLOCKER CLASS: register allocation, r2 against r3, on a walked pointer.
 *
 *     rom   mov r2, r5 / add r2, #0x64 / mov r3, #0x1e / strh r3, [r2]
 *           add r2, #2 / mov r3, #1 / strh r3, [r2]
 *     ours  mov r3, r5 / add r3, #0x64 / mov r2, #0x1e / strh r2, [r3]
 *           add r3, #2 / mov r2, #1 / strh r2, [r3]
 *
 * Identical instructions in identical order. The ROM gives the CONSTANT the
 * preferred register (r3, first in REG_ALLOC_ORDER) and the walked pointer r2;
 * we give the pointer r3 -- which is the longer-lived value, so it is the
 * choice the allocator would be expected to make.
 *
 * ONE REAL LEVER WAS FOUND ON THE WAY, and it is worth keeping. Storing the
 * constants directly to `unsigned short` fields makes gcc pool them as
 * HALFWORDS:
 *
 *      ldrh r3, .L5        <- 0x1e loaded from the literal pool
 *
 * That is the class-1 pool trap. Assigning each value to a named `int` first
 * and storing the int forces `mov r3, #0x1e`, and took this function from 11
 * differing of 37 (two instructions long) to 7 of 35 (same length).
 *
 * WHAT WAS TRIED AGAINST THE REMAINING EXCHANGE, all byte-identical at 7:
 *   - declaration order of the pointer, the int and the actor permuted three
 *     ways
 *   - `p = (unsigned short *)((char *)a + 0x64)` instead of `&a->f64`
 *   - `*p++ = t;` instead of the separate `p += 1`
 *   - direct field writes `a->f64 = t; a->f66 = t;` with no walked pointer at
 *     all (still 7, and still the same exchange)
 *   - `short` rather than `unsigned short` fields
 *
 * This is the same class as src/non_matching/ovl_7ed0a0/2009458.c and
 * src/non_matching/rom_b0000/80b2ed8.c: the allocator picking the other member
 * of an r2/r3 pair, with nothing at the expression level reaching it.
 */
struct A {
    unsigned char pad00[0x64];
    unsigned short f64;
    unsigned short f66;
    int f68;
    void *f6c;
};

extern int iwram_3001e40;
extern int L5240[] __asm__(".L5240");
extern struct A *__CreateActor(int a, int b, int c, int d);
extern void __Actor_SetAnim(struct A *a, int anim);
extern void OvlFunc_932_200aa10(void);
extern void OvlFunc_932_200aa48(void);

void OvlFunc_932_200ab58(void)
{
    struct A *a;
    unsigned short *p;
    int t;

    if ((iwram_3001e40 & 3) == 0) {
        a = __CreateActor(0xde, L5240[0], L5240[1], L5240[2]);
        if (a != 0) {
            p = &a->f64;
            t = 0x1e;
            *p = t;
            p += 1;
            t = 1;
            *p = t;
            a->f68 = 0x14;
            OvlFunc_932_200aa10();
            a->f6c = OvlFunc_932_200aa48;
            __Actor_SetAnim(a, 1);
        }
    }
}
