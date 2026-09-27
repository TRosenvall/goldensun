/* Func_80a7850 -- NON-MATCHING, 183 encodings of 215 -- BUT THE 183 IS A MIRAGE AND THE
 * PARK LEADS BY REFUSING ITS OWN NUMBER.  ref 215 instructions / ours 199.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_a1000/80a7850.c \
 *     asm/rom_a1000/rom_a7380_a_c_a.s --func Func_80a7850
 *
 * NOTE THE REFERENCE PATH: this function was in rom_a7380_a_c.s until batch 285 split that
 * file to land its neighbour Func_80a7f44.  It now lives in the `_a` part.
 *
 * THE FIRST DIFFERENCE IS `sub sp, #0x1c` AGAINST `#0xc` -- THE FRAME SIZE -- and every
 * later straight-line difference is the same instruction reaching `base` at a different
 * [sp, #N].  Outside the nested loop the streams are INSTRUCTION FOR INSTRUCTION
 * IDENTICAL.  So this is ONE structural fact, not 183 defects.
 *
 * BLOCKER: the ROM spills all three row accumulators plus two inner-loop copies -- a
 * seven-word frame -- where this candidate keeps two in r9/r11 and strength-reduces the
 * message id into an induction variable, which REMOVES THE LAST USE of one accumulator
 * from the inner loop and with it the register pressure that would force the spill.
 *
 * 14 loop spellings measured and the frame NEVER left 0xc.  The `row * K` form reaches the
 * ROM's LENGTH (210 against 209) but spills `row` instead.
 *
 * ANYONE PICKING THIS UP NEEDS ONLY THE SPILL DECISION: a variant that reaches
 * `sub sp, #0x1c` is very likely the whole function.
 */
/* Func_80a7850 -- 0x080a7850, asm/rom_a1000/rom_a7380_a_c.s.  PARKED.
 *
 *   objcmp: XX SIZE ref 484 bytes, ours 452
 *           XX ENCODINGS differ in 183 place(s) (ref 215, ours 199)
 *           first at index 7: ref 4b1b ours 4b1a
 *   tryc:   rom 209 lines, ours 197, first diff at 9, 148 differ
 *
 * The 183 is a MIRAGE and the park is much closer than the number says. The
 * first difference is `sub sp, #0x1c` against `sub sp, #0xc` -- the FRAME SIZE --
 * and every later difference in the straight-line code is the same instruction
 * reaching `base` at a different stack offset (`ldr r0, [sp, #0x18]` against
 * `ldr r0, [sp, #0x8]`). Read the --full diff: outside the nested loop, the two
 * streams are instruction-for-instruction identical, calls, arguments, pool
 * loads, both halfword-store loops and the whole key-wait loop included.
 *
 * WHAT IS ALREADY SETTLED, and what it cost to establish:
 *  - The pooled small constants 0x46, 0x1e and 0x80 need NO symbol. Every one is
 *    a HImode literal store (`strh r5, [r3, #8]`), and rom_a7380_a_b.c's note
 *    explains why every halfword store of a literal pools: in the Thumb movhi
 *    pattern the CONST_INT alternative precedes the `mov` alternative, so recog
 *    never reaches the mov. Plain literals reproduce all three exactly. The two
 *    loop counters that ARE `mov`-ed (0x20 stepping 0x38, 0x82 stepping 0x20) are
 *    `int` locals, so SImode -- the same note's other half.
 *  - 0xb17, 0xb18, 0x1001, 0x45f, 0xea3 are not shiftable and pool as literals.
 *  - `extern volatile unsigned int gKeyPress;` is required and is this tree's
 *    established declaration (see rom_a1814_c_a_a_c_a_c_a_a_c_b.c).
 *
 * THE BLOCKER IS A SPILL COUNT IN THE NESTED LOOP, NOT A SOURCE SHAPE.
 * The ROM's frame is 7 words: sp+0 outgoing 5th argument, sp+4/8/0xc the three
 * row accumulators (+0x14, +7, +0x38 per row), sp+0x10/0x14 inner-loop copies of
 * two of them, sp+0x18 `base`. Ours is 3 words: the argument, ONE accumulator and
 * `base` -- the other two accumulators stay in r9/r11, and gcc additionally
 * strength-reduces the message id `aa + k + 0x45f` into an induction variable
 * (r5, `add r5,#1`), which removes the last use of `aa` from the inner loop and
 * removes the pressure that would have spilled it. The ROM keeps `aa + k` as a
 * live value in r7 across the _GetFlag call and adds the pooled 0x45f each
 * iteration.
 *
 * Fourteen loop spellings were measured and the frame size never moved off 0xc:
 * flag index inline / as its own incremented variable / as `idx + 0x30`; message
 * id inline / from a named `idx` / as a `m = 0x45f; m += idx;` accumulate (the
 * cse-associative escape recorded in rom_a7380_c_b.c); y inline `0x18 + k*8` / as
 * an incremented variable; the three accumulators as scalars, as `int acc[3]` in
 * both element orders, and as `row * 0x14` / `row * 7` / `row * 0x38`
 * expressions. Two land on the ROM's LENGTH -- `row * K` gives 210 lines against
 * 209 -- but spill `row` instead of the accumulators.
 *
 * Also measured and rejected: swapping the order in which the two halfword
 * pointers are computed. The ROM builds base+0x144 with its own
 * `mov r0,#0xa2 / lsl r0,#1` where ours derives it as `sub r1, #0xf0` from
 * base+0x234 (reload_cse_move2add, both constants landing in r1). Computing the
 * 0x144 pointer first removes the `sub` but costs 24 more differing encodings
 * elsewhere, so the derivation is a consequence of the allocation too.
 *
 * NEXT STEP FOR WHOEVER PICKS THIS UP: the question is only what makes gcc spill
 * all three accumulators. Everything outside the nested loop is already exact,
 * so a variant that reaches `sub sp, #0x1c` is very likely to be the whole
 * function.
 */
extern unsigned char *iwram_3001f2c;
extern unsigned char *iwram_3001e8c;
extern volatile unsigned int gKeyPress;
extern void Func_80a9d84(void);
extern void _PlaySound(int id);
extern void _Func_8016498(unsigned int win);
extern void _Func_80164ac(unsigned int win);
extern void _Func_801e7c0(int id, unsigned int win, int x, int y);
extern void _Func_801e41c(unsigned int win, int a, int b, int c, int d);
extern void _DrawSmallText(int id, unsigned int win, int x, int y);
extern void _Func_8019000(unsigned int win, int tile, int col, int row, int pal);
extern int _GetFlag(int id);
extern void WaitFrames(int n);

void Func_80a7850(void)
{
    unsigned char *base;
    unsigned int win;
    unsigned short *q;
    unsigned short *r;
    int x;
    int i;
    int row;
    int k;
    int aa;
    int bb;
    int cc;

    base = iwram_3001f2c;
    Func_80a9d84();
    _PlaySound(0x70);
    _Func_8016498(*(unsigned int *)(base + (0x86 << 1)));
    _Func_801e7c0(0xb17, *(unsigned int *)(base + (0x86 << 1)), 0, 0x10);
    *(unsigned char *)(*(unsigned int *)(base + 0x14) + 5) = 0xd;
    *(unsigned char *)(*(unsigned int *)(base + (0xbe << 1)) + 5) = 0xd;
    WaitFrames(1);
    win = *(unsigned int *)(base + 0x24);
    q = (unsigned short *)(base + (0x8d << 2));
    r = (unsigned short *)(base + (0xa2 << 1));
    x = 0x20;
    for (i = 3; i >= 0; i--) {
        q[0] = x;
        q[4] = 0x46;
        x += 0x38;
        r[0] = 0x1e;
        q++;
        r++;
    }
    _Func_8016498(win);
    _Func_801e41c(win, 0, 0xb, 0x1c, 0xb);
    _DrawSmallText(0xb18, *(unsigned int *)(base + (0x86 << 1)), -0x60, 0x84);
    cc = 0;
    bb = 0;
    aa = 0;
    for (row = 0; row <= 3; row++) {
        for (k = 0; k <= 6; k++) {
            if (_GetFlag(aa + 0x30 + k) != 0) {
                _Func_8019000(win, 0x1001 + row, bb + 1, k + 3, 0);
                _Func_801e7c0(aa + k + 0x45f, win, cc + 0x10, 0x18 + k * 8);
            }
        }
        cc += 0x38;
        bb += 7;
        aa += 0x14;
    }
    iwram_3001e8c[0xea3] = 1;
    while (_GetFlag(0xa8 << 1) == 0) {
        WaitFrames(1);
        if ((gKeyPress & 7) != 0)
            break;
    }
    _Func_8016498(*(unsigned int *)(base + 0x24));
    _Func_80164ac(*(unsigned int *)(base + (0x86 << 1)));
    q = (unsigned short *)(base + (0x8d << 2));
    x = 0x82;
    for (i = 3; i >= 0; i--) {
        q[0] = x;
        q[4] = 0x80;
        x += 0x20;
        q++;
    }
    *(unsigned char *)(*(unsigned int *)(base + 0x14) + 5) = 1;
    *(unsigned char *)(*(unsigned int *)(base + (0xbe << 1)) + 5) = 1;
    _PlaySound(0x71);
}
