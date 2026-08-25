/* OvlFunc_899_2008048 -- NOT MATCHING. 2 of 22 lines, same length.
 *
 * Source asm: goldensun/asm/overlays/rom_794ac0/ovl_30_a_c_a_a_a_a.s
 *
 * Blocker: COMPARISON CANONICALISATION on one of two range tests.
 *
 *     rom    cmp r3, #0xf  / blt <other>
 *     ours   cmp r3, #0xe  / ble <other>
 *
 * Equivalent, same length, and gcc-2.96 will not be talked out of it.
 *
 * WHAT GOT IT FROM 16 OF 22 TO 2, and is worth keeping: the two bounds have to
 * be SEPARATE STATEMENTS WITH A GOTO. Written as one condition --
 *
 *     if (v > 0x11 || v < 0xf)
 *
 * -- gcc fuses the pair into a single unsigned range test,
 * `sub r3,#0xf / lsl r3,#16 / cmp r3, 0x20000 / bls`, which is two instructions
 * longer and nothing like the ROM. The statement-level split is the same lever
 * that batch 53 found for the non-zero idiom: a branch in the SOURCE stops a
 * rewrite that no amount of naming reaches.
 *
 * TRIED, all 2 of 22:
 *   `v < 0xf` and the equivalent `v <= 0xe`      identical output
 *   `v > 0x11` and `v >= 0x12`                   identical output
 *   v declared `int` rather than `short`         identical output
 *
 * NEXT: nothing at the source level. gcc picks the constant for a `<`
 * comparison and both spellings arrive at the same canonical form.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned char L5cc8[] __asm__(".L5cc8");
extern unsigned char L5ab8[] __asm__(".L5ab8");
extern void __Func_808b868(unsigned char *p);

unsigned char *OvlFunc_899_2008048(void)
{
    unsigned int base;
    unsigned int off;
    short v;
    unsigned char *p;

    base = (unsigned int)&gState;
    off = 0xe1;
    off <<= 1;
    base += off;
    off = 0;
    v = *(short *)((char *)base + off);
    if (v > 0x11)
        goto other;
    if (v < 0xf)
        goto other;
    p = L5cc8;
    goto call;
other:
    p = L5ab8;
call:
    __Func_808b868(p);
    return p;
}
