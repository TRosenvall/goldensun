/* Func_80292c4 (DrawDjinnRow) -- NON-MATCHING, 36 of 91 encodings.
 *
 *   SIZE EXACT (200 bytes both), ENCODING COUNT EXACT (91 = 91), ALL NINE
 *   RELOCATIONS IDENTICAL (objcmp prints no RELOCATIONS line).  NO PINS.
 *   Indices 0-17 and 38-39, 44-54, 57-69, 73, 75, 87-99 are byte-exact.
 *
 *   Figure re-derived in batch 324: 36 of 91, size exact, count exact.
 *   THE OLD FIGURE AND ALL THREE OF ITS LEVERS SURVIVED.  The BODY is
 *   unchanged; what this header adds is the blocker's exact arithmetic, two
 *   refuted claims, and a 40-variant crossing that establishes 36 as a floor.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/80292c4.c \
 *     asm/rom_15000/rom_23178_a_c_c_c_a_c.s --func Func_80292c4
 * SPLIT: rom_23178_a_c_c_c.s holds Func_8029274, Func_80292c4 and Func_802938c.
 *
 * ========================================================================
 * THE THREE LEVERS, ALL RE-CONFIRMED (figures from the old header)
 * ========================================================================
 * 1. THE FLAG LOOP IS AN INDEX `for`, NOT A POINTER WALK.  The ROM's exit test
 *    is `cmp r5,r6 / ble` -- SIGNED -- on what are plainly two pointers.
 *    Pointer comparisons are unsigned in gcc, so `p <= buf+0xf` gives `bls`.
 *    `for (j = 0; j < 0x10; j++)` gives `ble`: loop strength reduction
 *    replaces the biv test with the giv but KEEPS THE ORIGINAL COMPARISON'S
 *    SIGNEDNESS.  **A signed cmp/ble between two pointers is a tell for an
 *    index loop.**
 * 2. THE NUL TERMINATOR IS WRITTEN THROUGH THE LOOP INDEX.  `buf[j] = 0`
 *    after the loop (j == 0x10 there) gives the ROM's register-offset store
 *    `mov r3,#0x10 / mov r2,#0 / mov r1,r8 / strb r2,[r1,r3]`;
 *    `buf[0x10] = 0` gives `add r2,sp,#24 / strb` and is worse.
 * 3. THE num CLEAR LOOP RUNS BACKWARD here.  Re-measured in batch 324 across
 *    a 10-form x 4-naming grid (40 variants, below); backward is the floor.
 *
 * ========================================================================
 * THE BLOCKER, NOW WITH EXACT ARITHMETIC
 * ========================================================================
 * `buf` and `num` hold each other's hard register: the ROM has buf in r8 and
 * num in fp (r11), we have the reverse.  Everything else is already the
 * ROM's -- box in sl, y in r9, f in r7, and both `i` and `num+5` spilled to
 * the ROM's slots sp[4] and sp[0].  The 36 breaks down as
 *   14  the prologue's frame-address materialisation order
 *    6  the inner clear loop
 *    2  `mov r2,fp` / `mov r0,fp` for the two num call arguments
 *    2  the two givs of the flag loop
 *    6  the `buf[j] = 0` store and the UIDrawText argument setup
 *    6  the `i` spill update
 *
 * `.18.greg` prints `;; 13 regs to allocate: 47 38 57 58 37 42 32 36 53 34 41
 * 46 52` -- N > 0, so GLOBAL allocation and the priorities are exact.
 * Dispositions: `32 in 10  36 in 9  37 in 7  42 in 8  53 in 11` = box=sl,
 * y=r9, f=r7, **num=r8**, **buf=fp**, with `i` (34) and `num+5` (46) spilled.
 *
 * `allocno_compare` (`global.c:597-620`) in THIS compiler is
 *     pri = (floor_log2 (n_refs) * n_refs / live_length) * 10000 * size
 * ** with NO frequency and NO loop-depth term at all **, ties broken by
 * allocno number ascending.  Reproduced IN SEQUENCE against `.17.lreg`:
 *     47  12/4  -> 90000     42 (num)   8/45 ->  5333
 *     38  14/5  -> 84000     32 (box)   9/61 ->  4426
 *     57  16/16 -> 40000     36 (y)    11/102 -> 3235
 *     58   5/14 ->  7142     53 (buf)   4/47 ->  1702
 *     37  12/52 ->  6923     34 (i)     7/100 -> 1400  ...
 * which is exactly the printed order.  **num beats buf 5333 to 1702**, is
 * allocated sixth, and `find_reg` hands it r8; buf comes ninth and gets r11.
 *
 * THE OLD HEADER'S MECHANISM FOR THIS IS WRONG, though its direction is
 * right.  It said "the backward clear loop puts `num` in the INNER loop's
 * compare, which lifts its priority above `buf`'s ... because its ref sits at
 * loop depth 2."  There is no loop-depth weighting in `allocno_compare`.
 * What the depth-2 compare actually does is add a REF: the backward loop
 * references num twice (`num + 5` and the compare), giving n_refs 8 and
 * `floor_log2` 3, where the ROM's num has only the three depth-1 references.
 *
 * MEASURED, AND THIS IS THE NEW PART: the FORWARD clear loop brings num to
 * **exactly the ROM's 4 refs** (`.17.lreg`: `42 used 4 times across 46
 * insns`) and closes the num-vs-buf gap from 213% to 4% --
 *     num 2*4/46 = 1739   vs   buf 2*4/48 = 1667
 * -- and still loses, because buf's live range is two insns LONGER than
 * num's.  To overtake, buf needs either 9 refs or a live_length under 46.
 * In the ROM buf has FIVE refs (its flag-loop end pointer is computed from
 * buf, `mov r6,r8 / mov r5,r8`, where ours copies the other giv,
 * `mov r5,fp / adds r6,r5,#0`) -- and that fifth ref exists only because buf
 * is in a LOW register, so it is a CONSEQUENCE of the assignment, not a lever
 * on it.  **That circularity is the blocker, stated precisely.**
 *
 * Costed: the forward loop is 79 at +4 bytes with relocations differing,
 * because with num at 1739 `box` (4426) overtakes it and takes r8 instead.
 *
 * ========================================================================
 * REFUTED: THE bp/np CLAIM
 * ========================================================================
 * The old header said naming both arrays through pointer locals
 * (`bp = buf; np = num;`) "gives ALL FIVE of the ROM's registers ... but
 * costs ONE instruction, 92".  Measured: it is **91 instructions, size
 * EXACT, and 40 differing** -- and it does NOT give five of the ROM's
 * registers.  It fixes buf (r8, correct) and scrambles the rest: box -> r9,
 * num -> sl, y -> fp.  `np` alone is EXACTLY INERT at 36, which is what the
 * standing bound predicts -- a second name cannot change `REG_N_REFS`,
 * because copy propagation rewrites the uses back.
 *
 * ========================================================================
 * THE 40-VARIANT CROSSING (tools/sweep_variants.py; 36 IS A FLOOR)
 * ========================================================================
 * 10 clear-loop forms x 4 namings {none, np, bp, both}:
 *   backward pointer, no naming / np only            36  <- this body
 *   backward pointer + bp, or + both                 40
 *   index `for (k=0;k!=5;k++)`, no naming            41
 *   np + {forward, ++p, k!=5, p[k], p<end}           42
 *   index `for (k=0;k<5;k++)` or `for (k=4;k>=0;k--)` 70-76
 *   forward / `*p++` / counted-down forms           71-79
 * Also measured and inert or worse on the backward base: `unsigned j` (79 on
 * the forward base), `buf[0x10] = 0` terminator, a pointer flag loop (65),
 * `&num[5]` for `num + 5`, `unsigned i`, three orders of the f/y/i
 * initialisations -- none reaches below 36.
 *
 * NEXT: break the circularity above, i.e. find a spelling that gives `buf`
 * five references or a live range shorter than num's WITHOUT first putting
 * buf in a low register.  Nothing else in this function is open.
 */
extern void Func_8016478(void *box);
extern void UIDrawText(unsigned char *text, void *box, int x, int y);
extern void Func_8029274(unsigned int v, int n, unsigned char *out);
extern int _GetFlag(unsigned int id);
extern unsigned char L37428[] __asm__(".L37428");
extern unsigned char L3742c[] __asm__(".L3742c");

void Func_80292c4(void *box, int row)
{
    unsigned char num[5];
    unsigned char buf[0x11];
    int i;
    int j;
    int y;
    unsigned int f;
    unsigned char *p;

    Func_8016478(box);
    UIDrawText(L3742c, box, 0x30, 0);
    f = row << 8;
    y = 0x10;
    i = 0;
    do {
        p = num + 5;
        do { p--; *p = 0; } while (p != num);
        Func_8029274(f, 3, num);
        UIDrawText(num, box, 0, y);
        UIDrawText(L37428, box, 0x20, y);
        for (j = 0; j < 0x10; j++) {
            buf[j] = (_GetFlag(f) != 0) + 0x30;
            f++;
        }
        buf[j] = 0;
        UIDrawText(buf, box, 0x30, y);
        i++;
        y += 8;
    } while (i != 0x10);
}
