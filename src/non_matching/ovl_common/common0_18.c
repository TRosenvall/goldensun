/* OvlFunc_common0_18 -- NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/common/common0_a_b.s
 * Best screen: 8 differing of 41, streams the same length.
 *
 * BLOCKER CLASS: register allocation, r2 against r3, on a masked byte.
 *
 *     rom   mov r3, #0xd / ldrb r2, [r1, #9] / neg r3, r3 / and r3, r2
 *     ours  mov r2, #0xd / ldrb r3, [r1, #9] / neg r2, r2 / and r2, r3
 *
 * Same instructions, same order, the two registers exchanged, and it
 * propagates through the store and the pointer walk that follows.
 *
 * ONE LEVER WAS FOUND AND IS WORTH KEEPING: THE POSITIVE TEST. Written as an
 * early return --
 *
 *     n = __CreateActor(...);
 *     if (n == 0) return 0;
 *     ...
 *
 * -- gcc hoists the `mov r0, #0` above the test, which is the return-constant
 * hoist that parked Func_80bf37c in batch 89 and its five shape siblings. The
 * ROM keeps the zero in the else block. Turning the test around --
 *
 *     if (n != 0) { ... return n; }
 *     return 0;
 *
 * -- puts it back, and took this function from 30 differing of 40 (one
 * instruction short) to 8 of 41. That is a cheap thing to try on any function
 * whose only structural difference is a hoisted return constant, and it is the
 * first time the hoist has been defeated.
 *
 * WHAT WAS TRIED AGAINST THE REMAINING EXCHANGE, all byte-identical at 8:
 *   - `mask & p->f9` instead of `p->f9 & mask`
 *   - the mask declared after the pointer instead of before
 *   - the mask assigned before __CreateActor rather than inside the branch
 *
 * NOTE THE CONTRAST WITH src/rom_8a000/rom_8d9a4_c_a_c_c_c_c_c_c.c, which has
 * the IDENTICAL four-instruction sequence and matches. The difference is what
 * follows: there the `and` result feeds an `orr` before being stored, giving it
 * a longer live range; here it is stored immediately. That is a plausible
 * account of why the allocator splits the two functions and is the first thing
 * to test if anyone returns to this class -- which also covers
 * src/non_matching/ovl_7ed0a0/2009458.c and
 * src/non_matching/ovl_7b9cb4/200ab58.c.
 */
struct Spr { unsigned char pad00[9]; unsigned char f9; };

struct A {
    unsigned char pad00[0x50];
    struct Spr *f50;
    unsigned char pad54[1];
    unsigned char f55;
    unsigned char pad56[3];
    unsigned char f59;
};

extern struct A *__CreateActor(int a, int b, int c, int d);
extern void __Actor_SetSpriteFlags(struct A *a, int f);
extern void __Func_80929d8(struct A *a, int n);
extern void __Func_800c548(struct A *a, int n);

struct A *OvlFunc_common0_18(int a, int b, int c, int d)
{
    struct A *n;
    int mask;
    unsigned char *p;

    n = __CreateActor(d, a, b, c);
    if (n != 0) {
        mask = ~0xc;
        n->f50->f9 = n->f50->f9 & mask;
        p = &n->f55;
        *p = 0;
        p += 4;
        *p = 8;
        __Actor_SetSpriteFlags(n, 0);
        __Func_80929d8(n, 0xe);
        __Func_800c548(n, 1);
        return n;
    }
    return 0;
}
