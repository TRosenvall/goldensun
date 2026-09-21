/* PrepareSaveHeader (0x0801f818) -- NON-MATCHING, 69 differing of 181.
 * Blocker class: a CSE base-canonicalisation choice, plus reload_cse_move2add.
 * Never attempted before batch 277.
 *
 * asm/rom_15000/rom_1de5c_c_c_c_c_a_a_c_a_c_a_c.s (1 function, so landing needs NO split).
 *
 * THE RECORD LAYOUT IS FULLY RECOVERED, AND THE BASE IS PROVED RATHER THAN GUESSED. The
 * header base is `ewram_2000000 - 0x10`, and the evidence is a COST the ROM pays: the four
 * djinn-count `strb`s each spend a `mov` + `add` pair, which only happens because the biased
 * base pushes offsets 0x18-0x1b past Thumb's 5-bit `strb` range. An unbiased base would have
 * used `strb r0,[r7,#0x18]` and saved four instructions. A ROM paying for an awkward base is
 * evidence about the base.
 *
 * Fields relative to that base: name[12] @0x10, u8 @0x1c, u8 @0x1d, s16 (location name)
 * @0x1e, s32 @0x20, s32 @0x24, four djinn counts @0x28-0x2b, a -1-terminated id array @0x2c,
 * u8 @0x31 (a flag counter incremented in memory), u8 @0x32, u8 @0x33, u8 @0x34, u8 @0x35,
 * s16 @0x36, s32 checksum @0x3c. `gState` = 0x02000240.
 *
 * FOUR LEVERS LANDED, three isolated with direct A/B probes:
 *
 * 1. `sym + K` IS POOLED AS ONE RELOCATION UNLESS A NAMED POINTER WALKS TO IT.
 *    `*(int *)(ewram_2001000 + 0x100) = v` pools `ewram_2001000+256`; `e = ewram_2001000;
 *    e += 0x100; *(int *)e = v;` gives the ROM's `ldr =sym / mov #0x80 / lsl #1 / add`. A
 *    named `int` offset works too. `((int *)sym)[0x40]` and `sym + 0x80*2` both fold.
 * 2. TWO CONSECUTIVE WORD STORES AT `sym+0` AND `sym+4` COME OUT AS `stmia r3!, {r0}` UNLESS
 *    THE BASE IS A NAMED POINTER ASSIGNED AFTER THE CALL. Assigning it before the call puts
 *    it in a callee-saved register; `t = c(); g = gState; *(int*)g = t; ... *(int*)(g+4) = v;`
 *    reproduces the ROM's `ldr r2,=gState / str r0,[r2] / str r3,[r2,#4]`.
 * 3. THE `-1` ARRAY TERMINATOR NEEDS A `signed char` TARGET. `unsigned char` folds it to
 *    `mov #0xff`; `((signed char *)h)[...] = -1` gives the ROM's `mov #1 / neg / mov` triple.
 *    80 -> 76.
 * 4. THE 968-ITERATION CHECKSUM LOOP MUST BE A `goto` LOOP WITH A *NAMED* BOUND.
 *    `for (k = 0; k < 0x3c8; k++) sum += *q++;` is reversed by check_dbra_loop into
 *    `sub/cmp #0/bne`. `n = 0x3c8; k = 0; goto tst; top: ...; tst: if (k < n) goto top;`
 *    gives the ROM's `mov #0xf2 / lsl #2 / b / add r5,#1 / cmp r5,r1 / blt` exactly. A plain
 *    `while`, a `do/while`, `k != 0x3c8`, an `unsigned` counter with `<=`, indexing `q[k]`,
 *    and a non-goto named bound were all wrong in different ways. 76 -> 69.
 *
 * THE REMAINING 69, ITEMISED:
 *
 * (a) Our second `gState` pseudo holds `gState+4` where the ROM holds the BARE symbol, so
 *     five offsets are 4 low and the built 0x1c0 becomes 0x1bc. CSE CANONICALISES ON THE
 *     GROUP'S FIRST ADDRESS EXPRESSION, and the ROM's group leads with the bare symbol.
 *     Routing the whole group through a named pointer fixes the base but costs r8 (99
 *     against 69).
 * (b) reload_cse_move2add derives 0x1f4 from the live 0x100 pseudo (`add r0,#0xf4`) where the
 *     ROM builds `0xfa << 1` independently. A 10-way permutation of the opening statements
 *     did not separate them. Note this is the move2add SPACING lever with the sign reversed:
 *     usually you want move2add to fire, here you need it not to.
 * (c) buf/cnt swapped across r5/r6; the `-1` store using a computed address rather than a
 *     register offset; and the 0x205/0x206/0x20f destinations chained by move2add through one
 *     register where the ROM rotates r3/r1/r2 -- all downstream of (a).
 *
 * NEXT: (a). Make the group's FIRST address expression the bare symbol without paying r8 for
 * it. Everything else in the residue is downstream.
 */
extern unsigned char ewram_2000000[];
extern unsigned char ewram_2001000[];
extern unsigned char gState[];
extern unsigned char gFlags[];
extern int iwram_3001c9c;
extern unsigned char iwram_3001d08;

extern int _Func_8077cb8(void);
extern unsigned char *_GetUnit(int id);
extern int _GetLocationName(int x, int y);
extern int _GetNumDjinn(int e);
extern void _Func_80796c4(unsigned short *buf);
extern int _GetFlag(int n);

int PrepareSaveHeader(void)
{
    unsigned char *d;
    unsigned char *h;
    unsigned char *u;
    unsigned char *s;
    unsigned char *dst;
    unsigned char *g;
    unsigned char *e;
    int *q;
    int sum;
    int v;
    int t;
    int i;
    int cnt;
    int k;
    int n;
    unsigned short buf[14];

    sum = 0;
    d = ewram_2000000;
    t = _Func_8077cb8();
    g = gState;
    *(int *)g = t;
    v = iwram_3001c9c;
    e = ewram_2001000;
    e += 0x100;
    *(int *)(g + 4) = v;
    *(int *)e = v;
    g[0x22a] = iwram_3001d08;
    u = _GetUnit(*(int *)(g + 0x1f4));
    h = d - 0x10;
    s = u;
    dst = d;
    i = 11;
    do {
        *dst++ = *s++;
    } while (--i >= 0);
    h[0x1c] = u[0xf];
    *(int *)(h + 0x20) = *(int *)(gState + 4);
    *(short *)(h + 0x1e) = _GetLocationName(*(short *)(gState + 0x1c0),
                                           *(short *)(gState + 0x1c2));
    h[0x1d] = u[0x129];
    *(int *)(h + 0x24) = *(int *)(gState + 0x10);
    h[0x28] = _GetNumDjinn(0);
    h[0x29] = _GetNumDjinn(1);
    h[0x2a] = _GetNumDjinn(2);
    h[0x2b] = _GetNumDjinn(3);
    cnt = 0;
    _Func_80796c4(buf);
    while (cnt < 4) {
        if (buf[cnt] == 0xff)
            break;
        h[0x2c + cnt] = buf[cnt];
        cnt++;
    }
    ((signed char *)h)[0x2c + cnt] = -1;
    h[0x34] = gState[0x205];
    h[0x35] = gState[0x206];
    h[0x31] = gState[0x20f];
    h[0x32] = 0;
    for (k = 0x30; k <= 0x7f; k++) {
        if (_GetFlag(k) != 0)
            h[0x32]++;
    }
    h[0x33] = _GetFlag(0x20) != 0;
    *(short *)(h + 0x36) = *(int *)gState;
    q = (int *)gFlags;
    n = 0x3c8;
    k = 0;
    goto tst;
top:
    sum += *q++;
    k++;
tst:
    if (k < n)
        goto top;
    *(int *)(h + 0x3c) = sum;
    return sum;
}
