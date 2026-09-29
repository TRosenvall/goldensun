/* Func_80aafb8 -- DrawDjinnGrid, 0x080aafb8, 253 ROM instructions.
 * NON-MATCHING, 230 of 264 encodings differ.
 * Size 580 against the ROM's 572 (+8) and 269 encodings against 264 (+5), so
 * 230 is NOT a true distance.  tools/aligncmp.py reads 162 aligned-equal of
 * 264, 146 differing in 53 hunks.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_a1000/80aafb8.c \
 *     asm/rom_a1000/rom_aa538_c_c_a_c_c.s --func Func_80aafb8
 *
 * THE SPLIT.  asm/rom_a1000/rom_aa538_c_c_a_c_c.s holds THREE functions
 * (Func_80aafb8, Func_80ab1f4 at 0x080ab1f4, Func_80ab21c at 0x080ab21c -- the
 * last already parked as src/non_matching/rom_a1000/80ab21c.c) and NO data
 * section; datacheck is clean, so the split is text-only and needs NO `.global`.
 *
 * SHIMS: ZERO.
 *
 * ============================================================
 * THE FRAME AND THE SLOT MAP ARE EXACT, AND THAT WAS THE WHOLE FIRST JOB.
 *
 *   `sub sp, #0x34` = 0x34, and all 13 slots (0x00, 0x04 .. 0x30) are present
 *   with the ROM's roles.  Read top-down, the ROM PUBLISHES ITS DECLARATION
 *   ORDER: 0x30 = the parameter, then 0x2c = j, 0x28 = i, 0x24 = k, 0x20 = the
 *   iwram pointer, 0x1c = col.  Everything at 0x18 and below is a
 *   COMPILER-CREATED induction variable and must NOT be a declared local:
 *   0x18/0x14 are the k-loop copies of i*7 and i*0x38, 0x10 is 0xa0+i, 0x0c is
 *   i*0x38, 0x08 is i*7, 0x04 is a + i*0x14, and 0x00 is the outgoing fifth
 *   argument.
 *
 *   THE MEASUREMENT THAT SETTLED IT, and it is the reusable part:
 *     inner pointer written `(unsigned short *)(a + i*0x14 + j*2)` inline
 *         ....... frame 0x30, 12 slots, 147 aligned -- gcc does NOT strength-
 *         reduce on j and rebuilds `((i*5)*2 + j)*2 + a` every iteration (8
 *         instructions where the ROM has one `add r5, #2`)
 *     a DECLARED `unsigned char *row` stepped by 0x14
 *         ....... frame 0x30, 160 aligned -- `row` is a DECLARED local, so it
 *         takes a HIGH slot (0x18) and displaces one of the ROM's temps
 *     `p = (unsigned short *)(a + i * 0x14);` at the top of the k-loop body,
 *     with `p++` at the bottom of the j-loop body
 *         ....... frame 0x34, ALL 13 SLOTS, 162 aligned  <- this file
 *   The third form gives gcc exactly two givs to build: the i-giv `a + i*0x14`
 *   (slot 0x04) and the j-giv in r5.  `p` itself never needs a slot because the
 *   ROM keeps it in r5.
 *
 * REPRODUCED: r5 = p, r6 = 0x1f, r7 = 0xf00, r8 = the found flag, r9 = the
 * iwram_3001f2c state, r10 = the col*8+0x10 giv, r11 = 0xe0 -- the ROM's whole
 * hi/lo register assignment inside the inner loop; the five `*p` RE-READS (r4
 * is call-used in this tree, so the halfword is reloaded after every call and
 * must be written out at each use, not cached); `((v&0xe0)>>5)*20 + (v&0x1f) +
 * 0x45f` with the ROM's `lsl/add/lsl` shape; the `k <= 3` mid loop.
 *
 * THE TWO iwram NAMES ARE ONE POOL WORD PLUS A `sub`, taken straight from
 * src/non_matching/rom_a1000/80ab314.c item 2 and correct first try:
 *   `*(unsigned char **)((int)&iwram_3001f2c - 0xa0)` for the ONE early read
 *   (the ROM's `ldr r3,=iwram_3001f2c / ldr r0,[r3] / sub r3,#0xa0 / ldr r3,[r3]`)
 *   and the plain `iwram_3001e8c` extern for the later 0xea3 store.
 *
 * ============================================================
 * THE RESIDUE: +5 INSTRUCTIONS, ALL IN THE THREE LOOP GUARDS AND PREHEADERS.
 *
 *  - LOOP 1's GUARD.  ROM `ldrb r3,[r3] / cmp r3,#0 / beq`; ours materialises
 *    the zero and compares registers, `movs rN,#0 / cmp rN,r3 / bge` (+1).  The
 *    ROM's compiler folded `0 < (unsigned char)n` to `n != 0`; ours did not,
 *    even though loops 2 and 3 -- with the SAME bound expression -- use the
 *    register form in BOTH streams.  So this is not a type question.
 *  - THE k-LOOP PREHEADER.  Ours emits an extra `lsls`/`adds` pair rebuilding
 *    `a + i*0x14` where the ROM reloads slot 0x04.
 *  - THE COUNT READ.  ROM `ldrsb r3,[r2,r0]` with r2 = slot 0x10 (the index
 *    0xa0+i) and r0 = slot 0x30 (`a`) -- TWO spilled pseudos.  Ours folds them
 *    into ONE pointer pseudo and indexes with a zero register.  Both are reg+reg
 *    (Thumb `ldrsb` has no immediate form); the difference is which operand is
 *    the giv.
 *
 * MEASURED INERT -- both produce BYTE-IDENTICAL output to this file (162):
 *   the count read as `*(signed char *)(0xa0 + i + (int)a)`;
 *   a separate `b = a;` base for the row pointer, leaving `a` only in the count.
 * By this notebook's own rule, that means the residue is in neither.
 *
 * NEXT, if reopened: the question is narrow and answerable -- what source form
 * makes gcc reduce `a[0xa0 + i]` to an INDEX giv (a DEST_REG giv on 0xa0+i,
 * leaving `a` as the base) rather than a DEST_ADDR giv on the whole address?
 * The ROM has the former and every spelling tried here gives the latter.
 */
extern unsigned char *iwram_3001f2c;
extern unsigned char *iwram_3001e8c;

extern int Func_80ac8fc(unsigned char *p, int v, int c);
extern void _Func_8016498(int win);
extern void _Func_801e7c0(int id, int win, int x, int y);
extern void _SetTextColor(int c);
extern int _Func_807a1f8(int a, int b, int c);
extern int _Func_807a2bc(int a, int b, int c);
extern void _Func_8019000(int win, int id, int x, int y, int e);
extern void _Func_801e41c(int win, int a, int b, int c, int d);

void Func_80aafb8(unsigned char *a)
{
    int j;
    int i;
    int k;
    unsigned char *g;
    int col;
    unsigned char *st;
    unsigned short *p;
    int flag;

    st = iwram_3001f2c;
    g = *(unsigned char **)((int)&iwram_3001f2c - 0xa0);
    g[0xea6] = 1;
    for (j = 0; j < st[0x219]; j++)
        a[0xa0 + j] = Func_80ac8fc(a + j * 0x14,
                                   *(unsigned short *)(st + 0x208 + j * 2), -1);
    _Func_8016498(*(int *)(st + 0x30));
    _Func_801e7c0(0xbad, *(int *)(st + 0x30), 0, 0x50);
    for (i = 0; i < st[0x219]; i++) {
        col = 0;
        for (k = 0; k < 4; k++) {
            p = (unsigned short *)(a + i * 0x14);
            for (j = 0; j < ((signed char *)a)[0xa0 + i]; j++) {
                if (k == ((*p & 0xe0) >> 5)) {
                    if ((*p & 0x8000) == 0)
                        _SetTextColor(2);
                    flag = 0;
                    if (_Func_807a1f8((*p & 0xf00) >> 8, (*p & 0xe0) >> 5, *p & 0x1f) != 0)
                        flag = 1;
                    else if (_Func_807a2bc((*p & 0xf00) >> 8, (*p & 0xe0) >> 5, *p & 0x1f) != 0)
                        flag = 1;
                    if (flag == 0)
                        _SetTextColor(4);
                    _Func_8019000(*(int *)(st + 0x30), ((*p & 0xe0) >> 5) + 0x5001,
                                  i * 7 + 1, col + 2, 0);
                    _Func_801e7c0(((*p & 0xe0) >> 5) * 20 + (*p & 0x1f) + 0x45f,
                                  *(int *)(st + 0x30), i * 0x38 + 0x10, col * 8 + 0x10);
                    col++;
                    _SetTextColor(0xf);
                }
                p++;
            }
        }
    }
    _Func_801e41c(*(int *)(st + 0x30), 0, 0xa, 0x1c, 0xa);
    iwram_3001e8c[0xea3] = 1;
    g[0xea6] = 0;
}
