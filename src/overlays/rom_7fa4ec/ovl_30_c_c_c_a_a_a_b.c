/* OvlFunc_970_2008100  --  0x02008100
 *
 * Cut out of goldensun/asm/overlays/rom_7fa4ec/ovl_30_c_c_c_a_a_a.s.
 *
 * A per-frame jitter for a hovering actor: while the counter at +0x64 is
 * non-zero, tick it down and nudge the actor by a random amount; when it
 * reaches zero, and only once, switch the actor to its landing animation.
 *
 * THREE WIDTH DECISIONS, and all three were needed.
 *
 *   THE COUNTER IS READ TWICE, SIGNED AND UNSIGNED. The ROM does
 *   `ldrsh r1, [r2, r0]` for the test and `ldrh r3, [r2]` for the value it
 *   decrements, both before the branch. Two reads of the same halfword through
 *   two types is what produces that.
 *
 *   THE DECREMENT MUST HAPPEN AT `int` WIDTH. `*(unsigned short *)p =
 *   *(unsigned short *)p - 1`, `(*p)--` and `*p -= 1` all give
 *   `ldr r1, =0xffff / add r3, r1` -- the wrap-around addend, pooled. Reading
 *   the halfword into an `int` and storing `h - 1` gives the ROM's
 *   `sub r3, #1`. Same rule as
 *   src/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_a_b.c.
 *
 *   THE 0x14 NEEDS A NAMED `int` AND A POINTER COMPUTED FIRST. Stored straight
 *   into a `short *`, gcc pools it as a halfword (the class-1 pool trap).
 *   Naming it `int t` fixes the pool but then emits the constant BEFORE the
 *   address; computing the pointer into its own local first puts them back in
 *   the ROM's order. Both halves are needed -- either alone leaves two
 *   instructions transposed.
 *
 * The zero stored at +0x66 in the else branch is written as the loaded counter
 * value rather than a literal, matching the ROM's `strh r1`, but that arm only
 * runs when the counter IS zero so it is not a source signal either way.
 */
struct A {
    unsigned char pad00[8];
    int f8;
    int fc;
};

extern unsigned char gScript_970__020094c4[];
extern unsigned int __Random(void);
extern void __Actor_SetAnim(struct A *a, int n);
extern void __Actor_SetScript(struct A *a, unsigned char *s);

void OvlFunc_970_2008100(struct A *a)
{
    short *p;
    short *q;
    short *w;
    int t;
    int v;
    int h;
    int r;

    p = (short *)((char *)a + 0x64);
    v = *p;
    h = *(unsigned short *)p;
    if (v != 0) {
        *(unsigned short *)p = h - 1;
        r = __Random();
        a->f8 += r - __Random();
        a->fc += 0xcccc;
    } else {
        q = (short *)((char *)a + 0x66);
        if (*q != 0) {
            *q = v;
            __Actor_SetAnim(a, 1);
            w = (short *)((char *)a + 0x5e);
            t = 0x14;
            *w = t;
            __Actor_SetScript(a, gScript_970__020094c4);
        }
    }
}
