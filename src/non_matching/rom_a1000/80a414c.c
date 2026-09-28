/* Func_80a414c -- RunTargetPicker -- 348 instructions.
 * NON-MATCHING, 347 encodings of 363.  ours 361 encodings / 828 bytes against ref 363 / 832, so 347 is NOT a true distance; read this one by ALIGNED ROWS -- 367 of 382 are identical once registers and label numbers are normalised, and only ~6 real instruction items differ.
 *
 * (This claim line is first on purpose: tools/parkcheck.py reads the FIRST
 *  `N encodings of M` in the header, and the drop ladder below is full of
 *  `N of M` strings whose earliest is the ladder's WORST rung, not the claim.)
 * Reference: asm/rom_a1000/rom_a1814_c_c_c_a_a_a.s  (NOTE: the brief's first
 * path, asm/rom_a1814_c_c_c_a_a_a.s, does not exist -- the file is under
 * asm/rom_a1000/.)  ONE function, datacheck clean -> WHOLE-FILE conversion.
 *
 * Verify:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_a1000/80a414c.c asm/rom_a1000/rom_a1814_c_c_c_a_a_a.s --whole
 *
 * STRUCTURE READ (from the .s, before any compile):
 *  - takes NO arguments; returns `sel` (r10) via `mov r0,r10` in the epilogue,
 *    so the return type is int and it is REAL, not the int-return lever.
 *  - frame 0x10: sp+0 outgoing 5th arg, sp+4 the u16* (state+0x220),
 *    sp+8..0xf the `legal` array (r11 = sp+8), filled by Func_80a448c.
 *    Read only with `ldrsb`, so signed char (plain s8 is unsigned in this tree).
 *  - r6 state, r7 x, r8 y, r9 moved, r10 sel.
 *  - the loop is `while (_GetFlag(0x150) == 0)`: entry is `b .La4436`, the test
 *    sits at the BOTTOM and every body path branches back to it -- gcc-2.96's
 *    standard while-layout, so a plain `while` is right (no do/while, no guard).
 *  - A-press on an ILLEGAL target FALLS THROUGH to the B test (`b .La43c8`
 *    after the 0x72 sound), so the A and B tests are two sequential `if`s, not
 *    an else-if chain.  Only the legal-A arm leaves the loop.
 *  - the 6-entry jump table at .La4380 is gcc's own tabular switch
 *    (`cmp #5 / bhi default`); cases 2,3,5 share the 0x70 block and `default`
 *    is a SEPARATE 0x70 block, so default must be spelled out separately.
 *
 * BANK LEVER CONFIRMED, AND IT SPLITS THE TWO DIVISORS.  `% 3` and `/ 3` are
 * `mov r1,#3 / bl __modsi3` / `__divsi3` -- LIBCALLS, so the divisor is a
 * VARIABLE that cprop folded to 3 after expand_divmod had already committed to
 * the libcall.  But `(y + 2) % 2` in the same block is the INLINE signed
 * expansion (lsr#31 / add / asr#1 / lsl#1 / sub), so that 2 is a literal.  And
 * `sel = y * n + x` comes out as `lsl #1 / add` -- a multiply by the SAME
 * variable, folded to 3 and then strength-reduced by combine, which a libcall
 * cannot be.  So one variable `n = 3` feeds %, / and *, while the row count 2
 * is a bare literal.  That asymmetry is the readout, not a guess.
 *
 * `ldr r3,=0x75` IS A SYMBOL, NOT A LITERAL.  0x75 < 256, so a bare literal is
 * the single instruction `mov r3,#0x75` and gcc would never pool it.  Pooling
 * means a relocation: `_MSG_75`, ALREADY ADMITTED in message.sym:220 (it was
 * booked for the neighbouring Func_80a5614).  Same tell as _MSG_182 in
 * src/rom_a1000/rom_a4f08_b.c.  Not a shim -- a real extern.
 *
 * Offsets confirmed by arithmetic on the ROM's own re-use of a pooled base:
 * `ldr r1,=0x21a ... sub r1,#0xa6` -> 0x174, and `0x97<<2 = 0x25c` then
 * `sub r1,#0xe8` -> 0x174 again.  Both halfword reads are state+0x174.
 *
 * DROP LADDER (objcmp --whole, ref 363 encodings / 832 bytes):
 *   first candidate, `sel = y * n + x` .... 234 of 363 (ours 363 / 832) TRUE dist.
 *   `sel = y * 3 + x` (literal) ........... 347 of 363 (ours 361 / 828)
 *
 * READ THE TWO NUMBERS THE OTHER WAY ROUND -- the raw count is misleading here.
 * Aligning the two instruction streams with register names and label numbers
 * normalised away:
 *   with `y * n`:  364 of 382 rows identical, and the multiply is WRONG
 *                  (ours `mov r1,#3 / mov r3,rY / mul r3,r1`, ROM `lsl r3,rY,#1
 *                  / add r3,r8`).  Size and count match only by coincidence --
 *                  the mul region is +1 and something else is -1.
 *   with `y * 3`:  367 of 382 rows identical and the multiply MATCHES.
 * So the literal is correct and 347 is a register-rename cascade, not distance.
 * The kept candidate is the `y * 3` one.  234 IS a true distance (size and count
 * both match) but it is a true distance to the WRONG code.
 *
 * BANK LEVER CONFIRMED AND REFINED -- THE TWO 3s ARE DIFFERENT OBJECTS.
 * `%` and `/` are `mov r1,#3 / bl __modsi3` / `__divsi3`, i.e. LIBCALLS, so
 * their divisor is a VARIABLE that cprop folded to 3 only after expand_divmod
 * had already committed to the libcall.  The MULTIPLY is `lsl #1 / add`, which a
 * variable cannot produce (it comes out as `mul`), so that 3 is a LITERAL.  The
 * ROM materialises 3 exactly three times, all three feeding libcalls, which is
 * the readout.  Net: `x = (x + n) % n` with `n` a variable, `y = (y + 2) % 2`
 * with 2 a literal (it is the INLINE signed expansion: lsr#31/add/asr#1/lsl#1/
 * sub), and `sel = y * 3 + x` with 3 a literal.  Getting this backwards costs
 * ~3 rows either way.
 *
 * WHAT IS RIGHT: the whole body.  Every call, every branch, the 6-entry tabular
 * switch, the A-falls-through-to-B control flow, the while-layout, both offset
 * derivations, `_MSG_75`, the signed-char `legal[]` and the two explicit
 * `(signed char)` casts on the divide results (x and y are `int` -- the
 * increments `add r7,#1` / `sub r7,#1` carry NO lsl#24/asr#24, so the narrowing
 * is a cast on the divide, not the variable's type).
 *
 * BLOCKER -- A GLOBAL r1<->r2 SCRATCH RENAME, seeded in the prologue, plus a
 * 2-instruction shortfall.  Only these rows differ structurally:
 *
 *   1. PROLOGUE ORDER (the seed).  The ROM materialises the constant 0 TWICE --
 *      `mov r1,#0 / mov r10,r1` for sel, then `mov r2,#0 / mov r8,r2` for y --
 *      because sched2 placed the `legal` address computation BETWEEN them.  gcc
 *      schedules the two stores adjacently, so cse2 shares ONE zero register.
 *      That changes which caller-saved register is free next, and from there
 *      every scratch temp in the function is r1 where the ROM has r2 and vice
 *      versa.  That single swap is what turns ~13 differing rows into 347
 *      differing ENCODINGS.
 *      The ROM also spends 3 insns on the `legal` address (`mov r1,#8 / add
 *      r1,sp / mov r11,r1`) where gcc spends 2 (`add r3,sp,#8 / mov r11,r3`);
 *      Thumb has no `add rH,sp,#imm`, so this is reload's choice of how to
 *      reach a HI_REGS destination.
 *   2. `mov r0,r3` x2, at both Func_80a3ef0 calls: the ROM loads state[0x21a]
 *      into r3 and copies it to r0; gcc loads straight into r0.  This is the
 *      entire 2-instruction shortfall (361 vs 363).
 *   3. `sub r1,#0xa6` vs our `mov r2,#0xba / lsl r2,#1`: the ROM derives
 *      state+0x174 from the ALREADY-POOLED 0x21a.  gcc does exactly this at the
 *      SECOND site (`sub r2,#0xe8` off the 0x25c register, matching the ROM) and
 *      declines at the first, so it is cse's choice of which live register to
 *      derive from, not a source-shape question.  Both costs are 1 insn.
 *   4. `lsl r3,r2,#2` vs `lsl r3,#2` on the switch index -- 2-operand vs
 *      3-operand form, i.e. whether `sel` stays live.  Register allocation.
 *
 * THE PASS: items 1 and 4 are local-alloc/reload register assignment seeded by
 * sched2's placement of two independent constant stores; items 2 and 3 are cse2.
 * NOT proved unreachable -- unlike Func_80a8604 this has no arithmetic argument
 * behind it, and the shortfall is only 2 instructions. The next thing to try is
 * anything that separates `sel = 0` from `y = 0` in the schedule; if the two
 * zeros split, the cascade should collapse and this becomes a small number.
 *
 * TRIED AND INERT (all still 347): six statement orders for the four
 * initialisations (sel/state/y/moved/n in every useful permutation); three
 * declaration orders including `sel` first and `legal` first; precomputing
 * state[0x21a] into a local before each Func_80a3ef0 call; `break` instead of
 * `goto out` for the two loop exits (identical code).
 *
 * NO SHIMS IN THIS FILE.  `_MSG_75` is a real extern, ALREADY ADMITTED at
 * message.sym:220 -- no new .sym row needed.
 */

extern unsigned char *iwram_3001f2c;
extern volatile unsigned int gKeyPress;
extern volatile unsigned int gKeyRepeat;
extern int _MSG_75;

extern int __modsi3(int a, int b);
extern int __divsi3(int a, int b);
extern int _GetFlag(int flag);
extern void _PlaySound(int id);
extern void WaitFrames(int n);
extern void StartTask(void *fn, int prio);
extern void _Func_8016498(int win);
extern void _Func_801e41c(int win, int a, int b, int c, int d);
extern void _Func_801e7c0(int msg, int win, int x, int y);
extern void Func_80a112c(int a, int b, int c, int d);
extern void Func_80a1a40(int x, int y);
extern void Func_80a1ac0(int x, int y);
extern void Func_80a345c(void);
extern void Func_80a3c08(void);
extern void Func_80a3c98(void);
extern void Func_80a3ef0(int a, int b, int c);
extern int Func_80a4110(int x, int y);
extern int Func_80a413c(int x, int y);
extern void Func_80a448c(void *legal);
extern void Func_80a45cc(void *legal, int win);
extern void Func_80a4eb8(void);
extern void Func_80a51d0(void);

int Func_80a414c(void)
{
    unsigned char *state;
    unsigned short *mode;
    signed char legal[8];
    int win;
    int sel;
    int x;
    int y;
    int moved;
    int n;
    int prev;
    int f;
    int t;

    n = 3;
    sel = 0;
    y = 0;
    moved = 1;
    state = iwram_3001f2c;
    Func_80a448c(legal);
    mode = (unsigned short *)(state + 0x220);
    x = 0;
    if (*mode != 1) {
        Func_80a345c();
        _Func_8016498(*(int *)(state + 0x34));
        win = *(int *)(state + 0x10c);
        Func_80a4eb8();
        _Func_8016498(win);
        _Func_801e41c(win, 0, 3, 0x10, 3);
        Func_80a51d0();
        Func_80a45cc(legal, win);
        _Func_8016498(*(int *)(state + 0x2c));
        _Func_801e7c0((*(unsigned short *)(state + 0x178) & 0x1ff) + (int)&_MSG_75,
                      *(int *)(state + 0x2c), 0, 0);
    }
    *mode = sel;
    prev = *(signed char *)(state + 0x25d);
    if (prev == -1) {
        if (legal[2] == 1) {
            x = 2;
            y = 0;
        }
        if (legal[3] == 1) {
            x = 0;
            y = 1;
        }
        if (legal[1] == 1) {
            x = 1;
            y = 0;
        }
        if (legal[4] == 1) {
            x = 1;
            y = 1;
        }
        if (legal[0] == 1) {
            x = 0;
            y = 0;
        }
    } else {
        x = (signed char)(prev % n);
        y = (signed char)(prev / n);
        sel = y * 3 + x;
    }
    t = Func_80a4110(x, y);
    Func_80a1ac0(t, Func_80a413c(x, y));
    while ((f = _GetFlag(0x150)) == 0) {
        if (moved != 0) {
            moved = 0;
            x = (x + n) % n;
            y = (y + 2) % 2;
            sel = y * 3 + x;
            Func_80a3c98();
            if (sel > 2) {
                *(char *)(state + 0x25c) = 1;
                Func_80a3ef0(*(unsigned char *)(state + 0x21a),
                             *(unsigned short *)(state + 0x174), 0);
                if (sel == 3)
                    StartTask(Func_80a3c08, 0xc80);
            } else if (sel != 0) {
                *(char *)(state + 0x25c) = 0;
                Func_80a3ef0(*(unsigned char *)(state + 0x21a),
                             *(unsigned short *)(state + 0x174), 0);
            } else {
                Func_80a112c(*(int *)(state + 0x24),
                             *(unsigned char *)(state + 0x21a), 0, 0);
            }
        }
        t = Func_80a4110(x, y);
        Func_80a1a40(t, Func_80a413c(x, y));
        WaitFrames(1);
        if (gKeyPress & 1) {
            if (legal[sel] == -1) {
                _PlaySound(0x72);
            } else {
                switch (sel) {
                case 0:
                    _PlaySound(0xae);
                    break;
                case 1:
                    _PlaySound(0xaf);
                    break;
                case 2:
                case 3:
                case 5:
                    _PlaySound(0x70);
                    break;
                case 4:
                    _PlaySound(0x75);
                    break;
                default:
                    _PlaySound(0x70);
                    break;
                }
                *(char *)(state + 0x25d) = sel;
                break;
            }
        }
        if (gKeyPress & 2) {
            _PlaySound(0x71);
            sel = -1;
            *(char *)(state + 0x25d) = sel;
            break;
        }
        if (gKeyRepeat & 0x40) {
            y--;
            moved = 1;
            _PlaySound(0x6f);
        } else if (gKeyRepeat & 0x80) {
            y++;
            moved = 1;
            _PlaySound(0x6f);
        } else if (gKeyRepeat & 0x10) {
            x++;
            moved = 1;
            _PlaySound(0x6f);
        } else if (gKeyRepeat & 0x20) {
            x--;
            moved = 1;
            _PlaySound(0x6f);
        }
    }
    *(char *)(state + 0x25c) = 0;
    Func_80a3c98();
    return sel;
}
