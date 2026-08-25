/* OvlFunc_937_20080e4  --  0x020080e4, asm/overlays/rom_7c3044/ovl_30_c_c_a.s
 *
 * BLOCKER CLASS: the signed lower-bound floor, and nothing else.
 * Status: 38 lines against the ROM's 38, TWO differing:
 *
 *      rom    cmp r3, #0x9 / blt
 *      ours   cmp r3, #0x8 / ble
 *
 * WHAT IT DOES
 * Picks one of four script tables by area, with the 0x64 arm further split on
 * the sub-area word: 9..0xf or 0x11 take one table, everything else another.
 *
 * THIS IS THE THIRD FUNCTION TO LAND EXACTLY ON THE FLOOR. docs/elevation.md
 * names OvlFunc_899_2008048 and Func_80a3ce4; this one joins them, and it is
 * the cleanest example so far because every other difference is gone.
 * gcc-2.96 canonicalises every signed lower-bound test to `cmp #(K-1) / ble`
 * and the ROM's compiler does not. `v < 9`, `v <= 8`, `9 > v` and
 * `!(v >= 9)` all produce the same two instructions.
 *
 * TWO LEVERS DID WORK HERE and are kept in the source:
 *
 *   1. THE STRUCT-MEMBER READ. `gState.area` rather than pointer arithmetic is
 *      what gives the ROM's `add r3, r1, r0 / mov r0, #0 / ldrsh r3, [r3, r0]`.
 *      Thumb `ldrsh` has no immediate form, so it needs an index register
 *      either way, and written as arithmetic gcc makes the index carry the
 *      offset instead of a zero. Same lever as OvlFunc_922_2009a34.
 *
 *   2. THE goto SPLIT. Written as plain returns per arm this was 23 of 38 --
 *      gcc placed the Lc88 return before the 0x11 test and inverted the
 *      middle branch to `bgt`. Routing both outcomes through labels puts the
 *      shared returns after the tests, which is the ROM's layout, and takes it
 *      to 2. That is the compound-condition lever from docs/elevation.md
 *      applied to the RESULT rather than to the condition.
 *
 * So the diagnosis is narrow and confident: the two levers account for
 * everything except the canonicalisation, which no spelling reaches.
 */

typedef struct {
    unsigned char pad[0x1c0];
    short area;
    short sub;
} GlobalState;

extern GlobalState gState;
extern int _AREA_64;
extern int _AREA_65;
extern unsigned char La3c[] __asm__(".La3c");
extern unsigned char La48[] __asm__(".La48");
extern unsigned char Lc88[] __asm__(".Lc88");
extern unsigned char Leb0[] __asm__(".Leb0");

void *OvlFunc_937_20080e4(void)
{
    int v;

    if (gState.area == (int)(&_AREA_64)) {
        v = gState.sub;
        if (v < 9)
            goto other;
        if (v <= 0xf)
            goto hit;
        if (v != 0x11)
            goto other;
    hit:
        return Lc88;
    other:
        return La48;
    }
    if (gState.area == (int)(&_AREA_65))
        return Leb0;
    return La3c;
}
