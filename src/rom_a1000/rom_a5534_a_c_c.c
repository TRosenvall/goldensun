/* Cluster Func_80a5614..Func_80a56c8 extracted from goldensun/asm/rom_a1000/rom_a5534_a_c.s.
 *
 * Total .text for this TU = 364 bytes (= 0x16c), being 180 + 184.
 * The .s held exactly these two functions and BOTH are elevated, so the whole file
 * becomes one translation unit and its stage1.ld line is untouched -- NO SPLIT.
 *
 * Never attempted before batch 274. No pins, no flags. Each function was verified on its
 * own against the reference (81 and 84 encodings, every call relocation at an identical
 * offset) before being combined; the combined unit is gated on make compare. _MSG_75 and
 * _MSG_182 are new in message.sym under the same argument as _MSG_ad0 and _MSG_c20.
 *
 * TWO LEVERS, and the first is worth 54 instructions.
 *
 * 1. THE FIELD IS DEREFERENCED TWICE, NOT CACHED. `v = *p; if (v != 0) f((v & 0x1ff) + M)`
 *    is 55 differing; `if (*p != 0) f((*p & 0x1ff) + M)` is 1. Named-temp,
 *    separate-mask, operand-order and `unsigned short`-temp spellings ALL measured
 *    identically at 55, so it is the dereference COUNT and nothing else. The redundant
 *    `mov rX, rY` the ROM carries between the load and its `cmp` is what a second textual
 *    deref produces. Same tell landed Func_80a9e48 in this batch on its first screen.
 *
 * 2. THE `bls` GUARD AND THE GIV PLACEMENT ARE IN TENSION, and an explicit `if` plus
 *    `do/while` is what satisfies both. A `for` or `while` gives the right guard but puts
 *    the cursor init BEFORE it; a statement before the loop plus `if (count != 0)` gives
 *    the right placement but the wrong branch. `i = 0; if (count > i) { p = ...;
 *    do { ... } while (count > i); }` gives both -- 26 to 13 to 1 on that single change.
 *
 *    The branch detail matters: `cmp rX, #0 / bls` for an unsigned count needs the guard
 *    written as `count > i` with `i` just set to 0. `count != 0` gives `beq`, and so do
 *    `count > 0` and `count > 0u` -- gcc folds the constant forms to EQ. This is the
 *    duplicated loop-exit test surviving unsimplified.
 */
extern unsigned char *iwram_3001f2c;
extern int _MSG_75;
extern int _MSG_182;
extern void _Func_8016498(unsigned int win);
extern void WaitFrames(int n);
extern void _Func_801e7c0(int msg, unsigned int win, int x, int y);
extern void _Func_801e41c(unsigned int win, int a, int b, int c, int e);
extern void Func_80a2268(unsigned int win, int a, int b, int c, int d, int e);
extern void Func_80a2324(int count, int first, unsigned int win, int x, int y);
extern void Func_80a21b0(unsigned int win, int total, int perPage, int page, int col);

int Func_80a5614(int a0, int a1, int *d)
{
    unsigned char *state;
    int ofs;
    int i;

    state = iwram_3001f2c;
    d[6] = d[2] * 5 + d[4];
    _Func_8016498(*(unsigned int *)(state + 0x2c));
    WaitFrames(1);
    ofs = d[6] * 2 + 0xe4 * 2;
    if (*(unsigned short *)((int)state + ofs) != 0)
        _Func_801e7c0((*(unsigned short *)((int)state + ofs) & 0x1ff) + (int)&_MSG_75,
                      *(unsigned int *)(state + 0x2c), 0, 0);
    for (i = 0; i <= 4; i++) {
        if (i == d[4])
            Func_80a2268(*(unsigned int *)(state + 0x20), 1, i * 2 + 1, 0xe, 1, 0xe);
        else
            Func_80a2268(*(unsigned int *)(state + 0x20), 1, i * 2 + 1, 0xe, 1, 0xf);
    }
    WaitFrames(1);
    return 1;
}

int Func_80a56c8(unsigned int win, int a1, int *d)
{
    unsigned char *state;
    unsigned short *p;
    int first;
    unsigned char count;
    unsigned char i;

    state = iwram_3001f2c;
    _Func_8016498(win);
    _Func_801e41c(win, 0, 0xb, 0x10, 0xb);
    first = d[2] * 5;
    count = d[5] - first;
    if (count > 5)
        count = 5;
    Func_80a2324(5, first, win, 0x74, 0x22);
    Func_80a21b0(win, d[5], 5, d[2], 0xf);
    i = 0;
    if (count > i) {
        p = (unsigned short *)(state + first * 2 + 0xe4 * 2);
        do {
            _Func_801e7c0((*p & 0x1ff) + (int)&_MSG_182,
                          *(unsigned int *)(state + 0x20), 0x18, i * 16 + 8);
            p++;
            i++;
        } while (count > i);
    }
    return 1;
}
