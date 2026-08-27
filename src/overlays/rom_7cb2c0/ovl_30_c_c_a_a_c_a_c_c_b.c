/* OvlFunc_945_2009280  --  0x02009280
 *
 * Cut out of goldensun/asm/overlays/rom_7cb2c0/ovl_30_c_c_a_a_c_a_c_c.s.
 *
 * Asks whether the party leader can step one tile in a given direction: look up
 * the direction's (dx, dy) pair, add it to the leader's tile coordinates, and
 * return 1 only if neither the map test nor the collision test objects.
 *
 * THE TABLE ENTRY PACKS TWO SIGNED SHORTS IN ONE WORD. `asr r2, r3, #16` takes
 * the high half and `lsl r3, #16 / asr r3, #16` the low half, so the entry is
 * read as an `int` and split with `v >> 16` and `(short)v` -- not as two
 * halfword loads. That is what makes the shift pair appear at all.
 *
 * `return 1` MUST BE THE FALL-THROUGH. Written as a nested
 * `if (a == 0) { ... if (b == 0) return 1; } return 0;` the two exits come out
 * in the opposite order and eleven positions differ. Two early
 * `if (...) return 0;` guards with a bare `return 1` at the end gives the ROM's
 * shape, because gcc cross-jumps the two zeros into one block and leaves the
 * one falling through.
 *
 * That is the same reading as batch 96's positive-test lever seen from the
 * other side: there the trailing `return 0` had to be the shared block, here it
 * is `return 1`. What decides it is which value has MORE exits reaching it --
 * two here, one there.
 *
 * The three-word scratch array is passed by pointer, so it is a plain local
 * `int t[3]` and the ROM's `mov r1, sp` is gcc taking its address.
 */
struct A {
    unsigned char pad00[0xa];
    short fa;
    unsigned char pad0c_[0];
    int fc;
    unsigned char pad10[2];
    short f12;
};

extern int L6668[] __asm__(".L6668");
extern struct A *__MapActor_GetActor(int slot);
extern int OvlFunc_945_2009144(int x, int y);
extern int __TestCollision(struct A *a, int *v);

int OvlFunc_945_2009280(int dir)
{
    struct A *a;
    int v;
    int x;
    int y;
    int t[3];

    a = __MapActor_GetActor(0);
    v = L6668[dir];
    x = a->fa + (v >> 16);
    y = a->f12 + (short)v;
    if (OvlFunc_945_2009144(x, y) != 0)
        return 0;
    t[0] = x << 16;
    t[1] = a->fc;
    t[2] = y << 16;
    if (__TestCollision(a, t) != 0)
        return 0;
    return 1;
}
