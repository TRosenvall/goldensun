/* AnimTransitionOut -- asm/rom_c9000/rom_cc5d8_c_c.s, 0x080cd104, 166 ROM lines.
 *
 * NON-MATCHING: 141 encodings of 169 differ (objcmp).
 *
 * SIZE EXACT (348 bytes = 348) and INSTRUCTION COUNT EXACT (169 = 169) on the
 * FIRST candidate and on every variant since, so 141 IS a true distance.  The
 * RELOCATION SET AND ORDER ARE IDENTICAL (Random, WaitFrames, WaitFrames,
 * iwram_3001ef0) but the two WaitFrames sites sit FOUR BYTES LATE
 * (0xb2/0x13a against 0xae/0x136) and the pool word re-syncs at 0x154 -- i.e.
 * a two-instruction bulge inside variant 1's body that is given back before
 * the end, which is the same +3/-3 bookkeeping the blocker below describes.
 * The RE is complete -- both variants, both
 * loop nests, the tile-address arithmetic, the signed /8 vs & 7 mix and the two
 * separate signed range tests all reproduce.  What does not reproduce is the
 * WHOLE-FUNCTION REGISTER ASSIGNMENT: one spill decision differs and it renames
 * almost every register in the function.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/cd104_AnimTransitionOut.c \
 *     asm/rom_c9000/rom_cc5d8_c_c.s --func AnimTransitionOut
 *
 * THE SPLIT THIS NEEDS.  rom_cc5d8_c_c.s carries a `.rodata` section, so
 * converting anything in it needs a text/data split and the data must keep its
 * own object.  But AnimTransitionOut ITSELF REFERENCES NO `.rodata` LABEL:
 * every reference (.Lee064, .Lee06a, .Lee070, .Lee07c, .Lee088) is inside
 * BaseAnim_Tentacle, lines 141-199.  So this conversion needs NO new `.global`.
 * `.Lee058 / .Lee05c / .Lee060` are already `.global` and are referenced from
 * OUTSIDE this file (zero references inside it).  Full datacheck output and the
 * export accounting for the other function are in the batch report.
 *
 * ============================================================
 * THE ONE BLOCKER, NAMED: THE ROM SPILLS `st` AND WE DO NOT.
 *
 *   ROM   `sub sp, #0x88`  -- 0x80 array at sp+8, TWO scalar slots: st at
 *         [sp] and the second argument at [sp,#4].  Per frame iteration it
 *         RELOADS st (`ldr r1,[sp]` at .Lcd1a6 / .Lcd22e).
 *   ours  `sub sp, #132`   -- 0x80 array at sp+4, ONE scalar slot (the
 *         argument at [sp]); `st` lives in r11 for the whole function.
 *
 * That frees r11 for st in ours, which pushes the constant 7 into `ip`, which
 * costs `mov r3,ip` twice in the address chain plus a `mov lr,r2` for a spilled
 * intermediate (+3), balanced by not needing the ROM's `mov r12,r11` and its
 * two `mov r3,#1` (-3) -- which is why the instruction count is exactly right
 * while almost every register name is wrong.  The ROM's hi-register roles are
 * r8=lim, r9=fb, r10=inc, r11=rnd, r12=a per-frame copy of rnd, with 7 in the
 * LOW callee-saved r4; ours are r8=inc, r9=rnd, r10=fb, r11=st, ip=7.
 *
 * PASS: global_alloc / reload (the spill choice), not scheduling and not any
 * expression shape.  Everything tried to move it is INERT at 141:
 *   - an explicit per-frame `qb = rnd;` to reproduce the ROM's r12 (cse folds
 *     it away);  - `st` declared last, and two permutations of the int
 *     declarations (declaration order is a SLOT lever and there is only one
 *     slot to order);  - a named `m = 7;` local for the mask;  - `*q++` as one
 *     expression.
 *   - re-reading `st` from the global at each use instead of caching it: 161
 *     of 168 AND ONE INSTRUCTION SHORT -- worse and the wrong length.
 *   - pinning the rnd base to r11 (the ROM's register): 141 of 167, TWO
 *     INSTRUCTIONS SHORT.  The pin removes the copy the ROM has.
 *
 * ============================================================
 * THREE LEVERS THAT LANDED HERE.
 *
 *  (a) THE FRAME LOOP IS A `goto` LOOP; THE TWO INNER LOOPS ARE REAL LOOPS.
 *      Read straight off what each loop hoists.  The ROM's frame loop hoists
 *      NOTHING -- `mov r3,#1` (shared by `inc += 1` and `1 - a1`) and
 *      `mov r4,#7` are both INSIDE it -- while the i-loop and the n-loop DO get
 *      their invariants hoisted into the frame-loop body (`mov r12,r11`,
 *      `mov r4,#7`).  As a `do { } while (lim <= 0x100);` gcc hoists the 1 into
 *      a hi register (`mov fp,r1` + `mov r3,fp`) and the 7 into `ip`, costing
 *      FOUR instructions and the exact length.  Writing it
 *      `frame1: ... if (lim <= 0x100) goto frame1;` restores 169 = 169.
 *      This is docs/elevation.md's "a loop that hoists nothing was a goto loop"
 *      applied to ONE loop of a THREE-DEEP nest, and it is the first time the
 *      rule has been used selectively per level.
 *  (b) `yy >= 0 && yy <= 0x7f` MUST BE TWO STATEMENTS.  As one `&&` gcc folds
 *      it to a single UNSIGNED `cmp r1,#0x7f / bhi`; the ROM has
 *      `cmp r1,#0 / blt` AND `cmp r1,#0x7f / bgt`, two SIGNED tests.  The two
 *      `if (yy < 0) goto skip;` / `if (yy > 0x7f) goto skip;` statements give
 *      exactly that.  (`if (yy >= 0) if (yy <= 0x7f)` also folds.)
 *  (c) THE TILE ADDRESS MIXES SIGNED DIVISION AND MASKING, AND BOTH ARE
 *      VISIBLE.  `asr rN,#3` preceded by `cmp rN,#0 / bge / add rN,#7` is
 *      `v / 8` on a SIGNED int; a bare `and rN,r4` is `v & 7`.  So the
 *      expression is `(((a / 8) * 16 + b / 8) * 8 + (a & 7)) * 8 + (b & 7)`,
 *      NOT `>> 3` for the first pair -- four `/ 8` and four `& 7` in this one
 *      function and the two forms are not interchangeable.  And the two
 *      variants SWAP a and b: variant 1 is
 *      `(n/8, yy/8, n&7, yy&7)`, the other is `(yy/8, n/8, yy&7, n&7)`.
 *  (d) `iwram_3001ef0` AND THE VALUE 4 BYTES BELOW IT ARE ONE SYMBOL.  The ROM
 *      does `ldr r3,=iwram_3001ef0 / ldr r1,[r3] / sub r3,#4 / ldr r3,[r3]`,
 *      which is docs/elevation.md's "a runtime derive of one address from
 *      another proves ONE symbol plus an offset" -- so
 *      `*(unsigned char **)((char *)&iwram_3001ef0 - 4)`, exactly the idiom
 *      already landed in src/rom_c9000/rom_cd508_b.c for iwram_3001f00 - 0x8c.
 *      This comes out right first try; no `.sym` entry is needed.
 *
 * MEASURED ALTERNATIVE: indexing the random table as `rnd[n]` instead of
 * walking a `q` pointer measures 134 of 169 (still exact size and count) but
 * puts the walking pointer in `ip` and costs `mov r2,ip / mov r2,#1 /
 * add ip,ip,r2` where the ROM has `ldrb r3,[r0] / add r0,#1`.  141 with the
 * right inner-loop shape is the better place to restart from than 134 with the
 * wrong one; the whole gap is the one spill either way.
 */
extern unsigned char *iwram_3001ef0;
extern unsigned int Random(void);
extern void WaitFrames(int n);

void AnimTransitionOut(int variant, int a1)
{
    unsigned char *st;
    unsigned char *fb;
    unsigned char *q;
    int i, k, lim, inc, yy, val, o, n;
    unsigned char rnd[0x80];

    fb = iwram_3001ef0;
    st = *(unsigned char **)((char *)&iwram_3001ef0 - 4);
    for (k = 0; k != 0x80; k++)
        rnd[k] = Random() & 0x3f;
    if (variant == 1) {
        lim = 0;
        inc = 1;
        i = 0;
    frame1:
            lim += inc;
            inc += 1;
            if (i != lim) {
                val = 1 - a1;
                do {
                    n = 0;
                    q = rnd;
                    do {
                        yy = i - *q;
                        q++;
                        if (yy < 0)
                            goto skip1;
                        if (yy > 0x7f)
                            goto skip1;
                        o = (((n / 8) * 16 + yy / 8) * 8 + (n & 7)) * 8 + (yy & 7);
                        fb[o] = val;
                    skip1:
                        n++;
                    } while (n != 0x80);
                    i++;
                } while (i != lim);
            }
            *(int *)(st + 0x7824) = 1;
            WaitFrames(1);
            if (lim <= 0x100)
                goto frame1;
    } else {
        lim = 0;
        inc = 1;
        i = 0;
    frame2:
            lim += inc / 2;
            inc += 4;
            if (i != lim) {
                val = 1 - a1;
                do {
                    n = 0;
                    q = rnd;
                    do {
                        yy = i - *q;
                        q++;
                        if (yy < 0)
                            goto skip2;
                        if (yy > 0x7f)
                            goto skip2;
                        o = (((yy / 8) * 16 + n / 8) * 8 + (yy & 7)) * 8 + (n & 7);
                        fb[o] = val;
                    skip2:
                        n++;
                    } while (n != 0x80);
                    i++;
                } while (i != lim);
            }
            *(int *)(st + 0x7824) = 1;
            WaitFrames(1);
            if (lim <= 0xbf)
                goto frame2;
    }
}
