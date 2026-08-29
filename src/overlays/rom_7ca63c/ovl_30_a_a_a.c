/* OvlFunc_944_2008030, the whole of goldensun/asm/overlays/rom_7ca63c/ovl_30_a_a_a.s.
 *
 * Total .text for this TU = 62 bytes (= 0x3e). The .s is replaced outright, so
 * no linker-script change was needed.
 *
 * Places an actor at a camera-relative position: the two world words at the
 * head of the block behind iwram_3001e70 are rebased from the reference point
 * in .L1938 onto the screen origin in .L1930, with the vertical halved, and the
 * actor's sprite angle at +0x1e is advanced by 0x600.
 *
 * BUILT WITH -fno-strict-aliasing (see ALIAS_CFLAGS in the Makefile). Without
 * it the post-reload scheduler hoists the sprite-pointer load above the store
 * that precedes it:
 *
 *      rom    str r3, [r0, #0xc] / ldr r2, [r0, #0x50]
 *      ours   ldr r2, [r0, #0x50] / str r3, [r0, #0xc]
 *
 * `a->fc` is an int and `a->spr` is a pointer, so strict aliasing puts them in
 * different alias sets and the load is free to move. That is the same mechanism
 * as the six TUs in batch 69 and the seventh in batch 70 -- an int store
 * followed by a pointer load out of the same object.
 *
 * READING THE POINTER EARLIER IN THE SOURCE DOES NOT WORK HERE. That lever
 * fixed OvlFunc_957_200b610 in batch 71, and it was the first thing tried:
 * `s = a->spr;` before the store gives 9 differing lines rather than 2, because
 * gcc then keeps the pointer live across the whole division. Statement order
 * moves a register birth; it does not stop a scheduler that has been told the
 * two accesses cannot conflict.
 *
 * The vertical is a SIGNED halve -- `lsr r3, r2, #31 / add r2, r3 / asr r2, #1`
 * is gcc's round-toward-zero divide by two, and plain `/ 2` on an int produces
 * it. An arithmetic shift would be one instruction and is not what the ROM has.
 */

struct S {
    unsigned char pad[0x1e];
    unsigned short f1e;
};

struct A {
    unsigned char pad00[8];
    int f8;
    int fc;
    unsigned char pad10[0x40];
    struct S *spr;
};

extern int **iwram_3001e70;
extern int L1930[] __asm__(".L1930");
extern int L1938[] __asm__(".L1938");

int OvlFunc_944_2008030(struct A *a)
{
    int *p;
    int x;
    int y;
    struct S *s;

    p = *iwram_3001e70;
    x = *p;
    p++;
    y = *p;
    a->f8 = L1930[0] + (x - L1938[0]);
    a->fc = L1930[1] + (y - L1938[1]) / 2;
    s = a->spr;
    s->f1e += 0xc0 << 3;
    return 0;
}
