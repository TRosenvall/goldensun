/* Func_80b2ffc -- 0x080b2ffc -- asm/rom_b0000/rom_b0070_c_c_a_c.s
 *
 * BLOCKER: ARGUMENT FILL ORDER. 13 of 36, one line long.
 *
 * Calls _Func_809b804 on 24 consecutive 0x48-byte records, then -- if a signed
 * byte at +0x3ab is not -1 -- passes a sprite pointer and a scaled random
 * value to _Sprite_SetColorswap.
 *
 * THE FIRST 23 INSTRUCTIONS ARE EXACT, including the whole record loop and the
 * `ldrsb` against -1. The base idiom came from the kin,
 * src/rom_b0000/rom_b0070_c_c_a_b.c: `*(unsigned char **)iwram_3001f2c` for the
 * block pointer and offsets written as shifts.
 *
 * WHAT REMAINS is the order in which the two arguments are built:
 *
 *     rom    lsl r1, r0, #3 / lsl r3, r5, #2 / lsl r2, #1 / sub r1, r0 /
 *            add r3, r2 / lsr r1, #0x10 / ldr r0, [r7, r3]
 *     ours   the offset built first, the load into r3, the multiply finished,
 *            then `mov r0, r3`
 *
 * The ROM finishes the SECOND argument (the random value, r0*7 >> 16) before
 * loading the FIRST, so its load lands straight in r0; ours loads into r3 and
 * copies, which is the extra line.
 *
 * MEASURED, both WORSE than leaving the expressions inline:
 *   the byte offset named in a local            37 lines, 13 differ  <- kept
 *   the offset inlined at the call               38 lines, 14 differ
 *   the multiply hoisted into its own local
 *     (`r = (Random() * 7) >> 16`)               37 lines, 14 differ
 *
 * Both values die at the call, so this is the argument-temporary boundary
 * recorded in docs/elevation.md -- gcc rematerialises argument temporaries
 * during fill and discards the source's statement order. Hoisting the multiply
 * is the obvious way to make it finish first and it does not work, which is
 * what that boundary predicts.
 *
 * The named-offset form is kept because it is the shortest and closest, not
 * because it fixed anything.
 */
extern unsigned char iwram_3001f2c[];
extern void _Func_809b804(void *p);
extern unsigned int Random(void);
extern void _Sprite_SetColorswap(void *sprite, unsigned int v);

void Func_80b2ffc(void)
{
    unsigned char *base;
    unsigned char *p;
    int i;
    int idx;
    int off;
    unsigned int r;

    base = *(unsigned char **)iwram_3001f2c;
    p = base + (0xec << 2);
    i = 0x17;
    do {
        _Func_809b804(p);
        i--;
        p += 0x48;
    } while (i >= 0);
    idx = *(signed char *)(base + 0x3ab);
    if (idx != -1) {
        r = Random();
        off = (idx << 2) + (0x8a << 1);
        _Sprite_SetColorswap(*(void **)(base + off), (r * 7) >> 16);
    }
}
