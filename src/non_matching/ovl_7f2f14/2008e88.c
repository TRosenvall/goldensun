/* OvlFunc_968_2008e88  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_a_a.s
 * Best screen: 7 instructions in disagreeing regions, of 52 (rom 52, ours 53).
 *
 * BLOCKER CLASS: basic-block placement of the default arm.
 *
 * A six-way selector where five arms set a pointer and share a tail that calls
 * __Func_808b868 and returns it; the sixth returns a different table directly.
 * The ROM lets the LAST matching arm fall through into the shared tail and puts
 * the default block after it. gcc places the default before the tail and adds a
 * branch.
 *
 * WHAT WAS TRIED
 *   1. `if (v != _AREA_ba) return L68ec; r = L6cf4;` then the tail. 8 of 52.
 *   2. The same inverted, `if (v == _AREA_ba) { r = ...; goto load; }
 *      return L68ec;`. 7 of 52 but one instruction LONGER than the ROM.
 *   3. A plain if/else-if chain with the default in the final `else` (kept
 *      below). Byte-identical to (2).
 *
 * (2) and (3) being identical is the useful part: the guard-inversion lever
 * from batch 64 moves a SHORT return block, and this default arm is not one --
 * it competes with a shared tail rather than with the body of the function.
 *
 * The area ids are solved. _AREA_b5..ba are all defined, _AREA_b6 having been
 * added in batch 67, and the pool loads reproduce exactly; the sibling
 * OvlFunc_968_200af8c uses the same six and matches.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_b5;
extern int _AREA_b6;
extern int _AREA_b7;
extern int _AREA_b8;
extern int _AREA_b9;
extern int _AREA_ba;
extern unsigned char gScript_945__0200e904[];
extern unsigned char L69c4[] __asm__(".L69c4");
extern unsigned char L6b74[] __asm__(".L6b74");
extern unsigned char L6c04[] __asm__(".L6c04");
extern unsigned char L6c64[] __asm__(".L6c64");
extern unsigned char L6cf4[] __asm__(".L6cf4");
extern unsigned char L68ec[] __asm__(".L68ec");
extern void __Func_808b868(void *p);

void *OvlFunc_968_2008e88(void)
{
    unsigned char *g;
    unsigned char *r;
    unsigned int k;
    int v;

    k = 0xe0 << 1;
    g = (unsigned char *)&gState + k;
    v = *(short *)(g + (unsigned int)0);
    if (v == (int)(&_AREA_b5))
        return gScript_945__0200e904;
    else if (v == (int)(&_AREA_b6))
        r = L69c4;
    else if (v == (int)(&_AREA_b7))
        r = L6b74;
    else if (v == (int)(&_AREA_b8))
        r = L6c04;
    else if (v == (int)(&_AREA_b9))
        r = L6c64;
    else if (v == (int)(&_AREA_ba))
        r = L6cf4;
    else
        return L68ec;
    __Func_808b868(r);
    return r;
}
