/* Func_80bf440  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/rom_b5000/rom_bbb0c_a_c_c_a.s
 * Best screen: 2 instructions in disagreeing regions, of 31 (rom 31, ours 30).
 *
 * BLOCKER CLASS: an elided copy -- `ldrb r2, [r5] / mov r3, r2` where gcc emits
 * `ldrb r3, [r5]` alone.
 *
 * Identical to src/non_matching/rom_b5000/80bf3bc.c with two constants changed
 * (offset 0x13b for 0x139, timeout 0x28 for 0x3c). It reached 2 of 31 on the
 * first screen by copying that source, including its `return 1;`-per-path
 * spelling.
 *
 * THE FAMILY IS FOUR FUNCTIONS, all in this .s and all blocked by the same two
 * instructions: Func_80bf54c (4 of 19), Func_80bf574 (8 of 25),
 * Func_80bf3bc (2 of 31) and this one. The analysis lives in 80bf3bc.c --
 * read that one rather than these.
 */
extern unsigned char *_GetUnit(void);
extern int Func_80bf208(int a, int b, int c);

int Func_80bf440(int who)
{
    unsigned char *p;
    unsigned char *q;
    unsigned int off;
    int w;
    int v;
    int t;
    int n;
    int z;

    w = who;
    p = _GetUnit();
    off = 0x13b;
    q = p + off;
    v = *q;
    t = v;
    if (t == 0)
        goto zero;
    t = t + 0xff;
    *q = t;
    t = t << 24;
    if (t == 0)
        return 1;
    n = *q;
    if (Func_80bf208(w, n, 0x28) == 0)
        goto zero;
    z = 0;
    *q = z;
    return 1;
zero:
    return 0;
}
