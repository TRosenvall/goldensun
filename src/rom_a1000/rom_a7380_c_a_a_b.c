/* Func_80a8114 -- RunStatusPage1, 0x080a8114, 369 ROM instructions.
 *
 * EXACT: objcmp "OK Func_80a8114 -- 872 bytes, 385 encodings and 44
 * relocations identical", measured three times.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/rom_a1000/rom_a7380_c_a_a_b.c \
 *     asm/rom_a1000/rom_a7380_c_a_a_a.s --func Func_80a8114
 * (before the split, the reference is asm/rom_a1000/rom_a7380_c_a_a.s.)
 * tools/aligncmp.py shows one extra hunk -- ref `.short 0x0000` against our
 * `nop` as the very last encoding.  That is aligncmp's own gap, not a residue:
 * it does NOT append the build's trailing `.text / .align 2, 0`, which objcmp
 * does, and objcmp reads the tail as identical.  It is the documented
 * zero-vs-nop tail; settle it with `make compare` after the split.
 *
 * THE SPLIT.  asm/rom_a1000/rom_a7380_c_a_a.s holds TWO functions,
 * Func_80a8088 (0x080a8088, OpenStatusBody) and Func_80a8114, and NO
 * .section .data -- tools/datacheck.py is silent, so the split is pure text and
 * needs no `.global`.  Cut it as:
 *     asm/rom_a1000/rom_a7380_c_a_a_a.s   Func_80a8088, stays hand-written
 *     src/rom_a1000/rom_a7380_c_a_a_b.c   this file
 * and replace the single `rom_a7380_c_a_a.o` line in BOTH the .text list
 * (stage1.ld:1329) and the .rodata list (stage1.ld:1434) with the two new
 * objects, in that order.  Verify the split is byte-neutral before committing
 * the conversion.
 *
 * SHIMS: ZERO.  tools/shimcount.py reports no register pins, no .equ shims and
 * no "+r" barriers, so no fakematch.txt row.
 *
 * Runs page 1 of the status screen: opens the body window, blanks the four
 * tints at state+0x23c, then loops -- the outer loop rebuilds the per-unit row
 * table through Func_80a8604 / Func_80a8b10, the inner one walks the d-pad,
 * A returns 1, B returns -1, and L/R change character and leave through the
 * outer test.  Watches save flags 0x150 (abort) and nothing else.
 *
 * ============================================================
 * FOUR LEVERS CARRIED IT FROM 335 DIFFERING TO ZERO.  Two are new, and the
 * first two are worth lifting into docs/elevation.md.
 *
 * 1. THE NAMED-CONSTANT BASE WORKS WHEN BOTH USES ARE IN ONE BASIC BLOCK --
 *    and that is the BOUND on the batch-298 finding.  The ROM prints two
 *    adjacent strings as
 *        ldr r5, =0xb06 / mov r0, r5 / ... / add r0, r5, #1
 *    which by "gcc-2.96 NEVER chains plain CONST_INTs" cannot come from two
 *    literals.  Writing `int msg = 0xb06; f(msg, ...); f(msg + 1, ...);`
 *    reproduces it EXACTLY here, where the batch-298 entry says the same
 *    spelling needs -fno-gcse.  The difference is the CFG: gcse's cprop_insn
 *    gates on `oprs_not_set_p (reg, insn)` FIRST and skips any use whose
 *    register is set earlier in the SAME block, before it ever looks at
 *    availability -- and calls do not end a basic block in C, so two calls plus
 *    the definition are one block.  When the uses straddle a join (as in the
 *    sibling Func_80a7a34, parked) cprop folds them and nothing source-level
 *    stops it.  So the rule is: NAMED BASE, SAME BLOCK, TWO OR MORE USES.
 *    (Two uses matters: with only one use left, cse/combine propagate the
 *    constant into it and the base disappears -- measured.)
 *
 * 2. A NAMED LOCAL FOR THE LAST ARGUMENT REVERSES THE ARGUMENT-SETUP ORDER,
 *    AND THAT IS WHAT SCHED2 TIE-BREAKS ON.  The final two differing encodings
 *    were `mov r1,#0xc8 / ldr r0,=Func_80a19a0 / lsl r1,#4` against the ROM's
 *    `mov r1,#0xc8 / lsl r1,#4 / ldr r0,=Func_80a19a0`.  calls.c's
 *    load_register_parameters walks arguments FORWARD, so a literal r1 argument
 *    is emitted after r0's pool load; `lsl r1` and `ldr r0` then tie on
 *    priority and rank_for_schedule falls back to INSN_LUID, which favours the
 *    pool load.  `{ int pri = 0xc80; StartTask(Func_80a19a0, pri); }` computes
 *    the second argument into a pseudo BEFORE the argument loop, flipping the
 *    luids.  Two encodings, and it was the last thing between 2 and 0.
 *    Three near-misses that did NOT work, so do not retry them: `0xc80u`,
 *    `(void *)Func_80a19a0`, and a `void (*)(void)` prototype.
 *
 * 3. THE BYTE TRUNCATION HAS TO BE SPELLED OUT, AND IT WAS WORTH 42 -> 13.
 *    The ROM keeps the SHIFTED value live for the test:
 *        lsl r0,#24 / mov r1,#0 / lsr r2,r0,#24 / str r1,[..] / str r2,[..]
 *        / cmp r0,#0
 *    -- three registers.  `n = (unsigned char)f(); if (n == 0)` gives the
 *    destructive two-register form `lsl r0,#24 / lsr r0,#24 / cmp r0,#0`, which
 *    costs one quantity and ROTATES EVERY LOW REGISTER IN THE FUNCTION BY ONE
 *    from that point on -- 15 separate hunks, all of them `ROM rN` against
 *    `ours r(N+1 mod 4)`, and every one of them collapsed when this single site
 *    was fixed.  What reproduces it is naming the shifted value:
 *        int raw = f() << 24;
 *        n = (unsigned int)raw >> 24;
 *        if (raw == 0) ...
 *    `& 0xff`, `!n`, and a separate `(unsigned char)raw` temp all measure at
 *    42 or worse.  GENERAL: a whole-function low-register rotation is ONE
 *    missing or extra short-lived quantity; find the first differing hunk and
 *    count the registers it uses, do not chase the rotation.
 *
 * 4. `x != 0` AS A VALUE IS THE neg/orr/lsr IDIOM AND HAS TO BE WRITTEN OUT.
 *    `hasDjinn = _GetNumDjinn(-1) != 0;` compiles to a branch
 *    (`cmp / beq / mov #1`); the ROM has `neg r3,r0 / orr r3,r0 / lsr r3,#31`.
 *    `!!x`, `x ? 1 : 0` and an unsigned local all give the branch.  Writing
 *    `(unsigned int)(-d | d) >> 31` gives the ROM, worth 335 -> 294 differing.
 *    This is the same class as the `mvn/neg/orr/lsr` note in
 *    src/rom_a1000/rom_a7380_a_c_a_b.c and the identical spelling already
 *    landed in src/overlays/rom_7c5efc/ovl_30_c_c_a_b.c
 *    (`1 - ((unsigned int)(-r3 | r3) >> 31)`), so the spelling has precedent.
 *
 * SMALLER THINGS THAT MATTERED:
 *
 *   - FRAME AND DECLARATION ORDER.  0x28 exact on the first try.  `buf` is the
 *     only addressable local so expand gives it the top slot (0x20); the six
 *     spilled scalars follow in DECLARATION order downwards -- result 0x1c,
 *     n 0x18, flagA 0x14, done 0x10, hasDjinn 0xc, p 8 -- with [sp,#0] and
 *     [sp,#4] left for the outgoing 5th and 6th arguments of Func_80a10d0 and
 *     _Func_80164d4.  Note the order is the DECLARATION order, not the
 *     initialisation order: the ROM writes result, then hasDjinn, then done,
 *     then p.
 *   - `redraw = 0;` BEFORE `m = (signed char)n;`.  The ROM materialises the 0
 *     and stores r9 first; hoisting the cast into `int m = (signed char)n;`'s
 *     initialiser puts it ahead and costs the whole block's registers.  Same
 *     for `flagA = 0;` before the `n` store.  Both were 13 -> 2.
 *   - `(signed char)n` IS THE MODULO'S DIVISOR.  n is stored ZERO-extended
 *     (`lsr`) and read back SIGN-extended (`lsl/asr`), which is only an int
 *     local written from a `(unsigned char)` truncation and read through a
 *     `(signed char)` cast.  It is also hoisted ABOVE `if (page == 0)`: sched2
 *     cannot cross a block boundary, so the cast is in the source before the
 *     if, not inside the arm that uses it.
 *   - `redraw = 1;` BEFORE the decrement in the Left and Down arms.  The ROM
 *     negates the FIRST materialised 1 and copies the second into r9
 *     (`mov r0,#1 / mov r3,#1 / neg r0,r0 / mov r9,r3 / add r8,r0`); the
 *     increment arms reuse one register and are order-blind.  This is the
 *     OPPOSITE of the sibling Func_80a7a34, where the decrement had to come
 *     first -- so measure it, do not assume it.
 *   - `else if (idx <= 3)` with the arms in that order, for the ROM's `bgt`.
 *     Straight from src/rom_a1000/rom_a7380_c_a_b.c's lever 4, which is the
 *     landed Func_80a847c this function calls.
 *   - BOTH COUNTED LOOPS are plain `for (i = 3; i >= 0; i--)` over the u16[4]
 *     at 0x23c.  The `sub r2,#1` at the TOP of the body is sched2 hoisting the
 *     bottom decrement, not a do/while.  The pooled `0x68` and `0x80` are
 *     HImode store constants, which gcc-2.96 Thumb ALWAYS pools -- no lever
 *     needed, and they are exact as written.
 *   - THE ZERO IS ONE CONSTANT.  `page = 0`, `idx = 0` and `result = 0` all come
 *     from the single `mov r0,#0`, and reload_cse_move2add then derives the
 *     `-1` argument of _GetNumDjinn from it as `sub r0,#1` rather than
 *     `mov/neg`.  That falls out; nothing had to be written for it.
 *
 * STRUCT.  0x08 u32 (the selected roster entry, stored as a word from a u16
 * load), 0x14 struct Cursor * (visible at +5, the same layout as
 * src/rom_a1000/rom_a7380_a_c_a_c_b.c's), 0x1c signed char (the party slot,
 * read with the Thumb register-offset `ldrsb` a plain `signed char` field gives
 * for free), 0x24 and 0x2c two UI-box handles, 0x208 u16[8] roster,
 * 0x219 u8 (the party size), 0x21a u8 (the current unit), 0x23c u16[4] tints.
 * 0x24/0x2c and the 0x208 roster agree with Func_80a8088 in the same reference
 * file and with the landed Func_80a76d0 / Func_80a77a4.
 */
extern volatile unsigned int gKeyPress;
extern volatile unsigned int gKeyRepeat;

extern int _GetFlag(int id);
extern int _GetUnit(int id);
extern int _GetNumDjinn(int who);
extern void _PlaySound(int id);
extern void WaitFrames(int n);
extern void _Func_8016498(unsigned int win);
extern void _Func_80164ac(unsigned int win);
extern void _Func_80164d4(unsigned int win, int x, int y, int w, int h);
extern void _Func_801e7c0(int id, unsigned int win, int x, int y);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Func_80a19a0(void);
extern void Func_80a1a40(int x, int y);
extern void Func_80a1ac0(int x, int y);
extern int Func_80a10d0(void *slot, int a, int b, int c, int d, int e);
extern void Func_80a8578(unsigned int win, int which, int mode);
extern void Func_80a8604(unsigned int win, int id, int mode);
extern int Func_80a8b10(void *buf, int a, int id);
extern void Func_80a9d84(void);

struct Cursor {
    unsigned char pad0[5];
    unsigned char visible;
};

struct State {
    unsigned char pad0[8];
    unsigned int f8;
    unsigned char pad0c[8];
    struct Cursor *cursor;
    unsigned char pad18[4];
    signed char sel;
    unsigned char pad1d[7];
    unsigned int f24;
    unsigned char pad28[4];
    unsigned int f2c;
    unsigned char pad30[0x1d8];
    unsigned short roster[8];
    unsigned char pad218;
    unsigned char b219;
    unsigned char b21a;
    unsigned char pad21b[0x21];
    unsigned short tint[4];
};
extern struct State *iwram_3001f2c;
extern void Func_80a847c(int a, int b, unsigned char *p, int d);
extern void Func_80a8508(unsigned int win, int which, unsigned char *flags);
extern void Func_80a1804(struct State *st, int id);

int Func_80a8114(void)
{
    unsigned char buf[8];
    int result;
    int n;
    int flagA;
    int done;
    int hasDjinn;
    unsigned char *p;
    struct State *st = iwram_3001f2c;
    int page = 0;
    int idx = 0;
    int redraw;
    int i;

    result = 0;
    {
        unsigned int d = _GetNumDjinn(-1);
        hasDjinn = (unsigned int)(-d | d) >> 31;
    }
    Func_80a10d0(&st->f2c, 0, 0, 0x1e, 5, 2);
    StopTask(Func_80a19a0);
    for (i = 3; i >= 0; i--)
        st->tint[i] = 0x68;
    done = 0;
    Func_80a1ac0(-10, 0x58);
    p = &st->b21a;

    while (!done && !_GetFlag(0x150)) {
        _GetUnit(*p);
        Func_80a8604(st->f24, *p, 1);
        {
            int raw = Func_80a8b10(buf, 1, *p) << 24;

            flagA = 0;
            n = (unsigned int)raw >> 24;
            if (raw == 0)
                n = 1;
            else
                flagA = 1;
        }
        redraw = 1;
        while (!_GetFlag(0x150)) {
            if (redraw) {
                int m;

                redraw = 0;
                m = (signed char)n;
                page = (page + 2) % 2;
                if (page == 0) {
                    idx = (idx + m) % m;
                    _Func_8016498(st->f2c);
                    if (flagA == 0) {
                        int msg = 0xb06;

                        _Func_801e7c0(msg, st->f24, 0x50, -0x18);
                        _Func_801e7c0(msg + 1, st->f24, 0, -0x18);
                    }
                } else {
                    _Func_8016498(st->f2c);
                    if (hasDjinn)
                        idx = (idx + 8) % 8;
                    else
                        idx = (idx + 7) % 7;
                }
                Func_80a847c(page, idx, buf, 0);
                _Func_80164ac(st->f2c);
                WaitFrames(1);
                if (page == 0)
                    Func_80a8508(st->f2c, idx, buf);
                else
                    Func_80a8578(st->f2c, idx, hasDjinn);
            }
            st->cursor->visible = 1;
            if (page == 0)
                Func_80a1a40(-10, (idx << 4) + 0x58);
            else if (idx <= 3)
                Func_80a1a40(0x18, (idx << 3) + 0x30);
            else
                Func_80a1a40(0x30, (idx << 3) + 0x50);
            WaitFrames(1);
            if (gKeyRepeat & 0xf0)
                Func_80a847c(page, idx, buf, 1);
            if (gKeyPress & 1) {
                _PlaySound(0x70);
                done = 1;
                result = 1;
                break;
            }
            if (gKeyPress & 2) {
                _PlaySound(0x71);
                done = 1;
                result = -1;
                break;
            }
            if (gKeyRepeat & 0x40) {
                _PlaySound(0x6f);
                redraw = 1;
                idx--;
            }
            if (gKeyRepeat & 0x80) {
                _PlaySound(0x6f);
                redraw = 1;
                idx++;
            }
            if (gKeyRepeat & 0x10) {
                _PlaySound(0x6f);
                redraw = 1;
                page++;
            }
            if (gKeyRepeat & 0x20) {
                _PlaySound(0x6f);
                redraw = 1;
                page--;
            }
            if ((gKeyRepeat & 0x100) || (gKeyRepeat & 0x200)) {
                int k;

                _PlaySound(0x6f);
                k = st->sel;
                if (gKeyRepeat & 0x100)
                    k += 1;
                else
                    k -= 1;
                k = (k + st->b219) % st->b219;
                st->f8 = st->roster[k];
                *p = st->roster[k];
                st->sel = k;
                Func_80a1804(st, st->roster[k]);
                break;
            }
        }
    }
    _Func_80164ac(st->f2c);
    _Func_8016498(st->f2c);
    _Func_80164d4(st->f24, 0x40, 0x38, 0xe0, 0x60);
    {
        int pri = 0xc80;

        StartTask(Func_80a19a0, pri);
    }
    for (i = 3; i >= 0; i--)
        st->tint[i] = 0x80;
    Func_80a9d84();
    return result;
}
