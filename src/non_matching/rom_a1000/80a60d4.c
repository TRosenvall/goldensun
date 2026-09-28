/* Func_80a60d4 -- 0x080a60d4, asm/rom_a1000/rom_a5534_c_a_a.s
 * NON-MATCHING, 191 encodings of 307.  A TRUE DISTANCE -- 307 = 307 encodings and no SIZE line, so size and instruction count both
 * match.  READ `--align` alongside it: 88 of 316.  Zero shims in both classes.
 * Tail split from a six-function file; no data crosses either way and the one inbound code
 * reference becomes a link-time relocation, so no export is needed.
 *
 * (Claim line first on purpose: parkcheck reads the FIRST `N encodings of M`.)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_a1000/80a60d4.c \
 *     asm/rom_a1000/rom_a5534_c_a_a.s --func Func_80a60d4
 *
 * NOT MATCHING. objcmp --func Func_80a60d4: 191 of 307 encodings differ, ref 307 / ours
 * 307 -- SIZE AND INSTRUCTION COUNT BOTH EXACT, and all 32 relocations present with the
 * same symbols in the same order.  tryc --align: 88 of 316.  191 is therefore a TRUE
 * DISTANCE; it is high only because objcmp is positional.  Read the count honestly, though:
 * thirteen relocations from `Func_80a1a40` onward sit at +2 and the last two pool words are
 * back at the ROM's offsets, so the totals agree by CANCELLATION -- one extra 2-byte
 * instruction early (the else arm's `mov r8,r9`) against one missing late (`mov r9,r8`).
 * Both belong to the single blocker.
 *
 * SIX FUNCTIONS IN THE FILE, confirmed by `grep -ci func_start` (Func_80a5788,
 * Func_80a5b94, Func_80a5cc0, Func_80a5fe0, Func_80a602c, Func_80a60d4).  Func_80a60d4 is
 * the SIXTH AND LAST, so the split is a TAIL split.  No `.data` anywhere in the file and
 * no data label crosses the boundary; the one inbound code reference (Func_80a602c calls
 * Func_80a60d4) becomes a link-time relocation.  NO EXPORT IS NEEDED.
 *
 * SIGNATURE.  r1 is dead from the first instruction, so the second parameter is unused.
 * It is kept because the caller, src/non_matching/rom_a1000/80a602c.c, passes two:
 * `Func_80a60d4((unsigned short *)(r7 + 0x82 * 4), r6)`.  Declaring one parameter
 * instead is byte-identical.
 *
 * WHAT THE READINGS FROM BATCH 292 WERE WORTH (all four TESTED, three held):
 *  - exit `ldrh r2,[r3,r0]` naming the OFFSET as operand 0: HELD, but see the roster
 *    lever below -- the winning spelling at that site is `roster[cursor]`, not the cast.
 *  - counter argument `*(signed char *)(state + 0x1e)` read with register-offset `ldrsb`:
 *    HELD exactly.  `s8` would be unsigned in this tree.  Thumb-1 has no immediate-offset
 *    LDRSB at all, so the `mov rN,#0x1e / ldrsb` pair is forced and needs no help.
 *  - `.La61dc` as `do { } while (--n >= 0)` with a descending halfword pointer: HELD,
 *    `q = (unsigned short *)(state + 0xa5 * 2); i = 3; do { *q-- = 0x1e; i--; } while
 *    (i >= 0);` reproduces all five instructions including the signed `bge`.
 *  - TAIL split with no export: HELD.
 *
 * FOUR LEVERS LANDED HERE, 125 differing -> 88, each measured as a single drop:
 *
 * 1. `roster[cursor]` AT THE EXIT SITE IS WHAT SPILLS THE PARAMETER, 125 -> 102 and it is
 *    THE WHOLE FRAME.  The ROM keeps `roster` in memory at sp+0x1c with a 0x20 frame and
 *    reloads it five times; the natural `*(unsigned short *)(ofs + (int)roster)` keeps it
 *    in r11 with a 0x1c frame.  The mechanism is register pressure: `roster[cursor]`
 *    written at the FINAL read creates a second `cursor * 2` pseudo, which makes seven
 *    quantities (ofs, that second pseudo, cursor, the 0x268 pointer, win, p, roster)
 *    compete for the six callee-saved registers left after r7 takes `state`, and `roster`
 *    is the one that loses.  A THREE-SITE SWEEP over the cast form vs the array form at
 *    the A-button read, the direction read and the exit read (eight combinations, all
 *    measured) shows ONLY THE EXIT SITE MATTERS: every combination with the array form
 *    there reads 93, every combination with the cast form reads 98 and the frame reverts
 *    to 0x1c.  The two in-loop sites are free.
 *
 * 2. `signed char` LOCALS ARE HELD ZERO-EXTENDED, SO A CAST TO int IS TWO MORE SHIFTS --
 *    and the cast must sit AFTER the intervening call, 102 -> 93.  `arm.h:597` gives
 *    QImode `UNSIGNEDP = 1` UNCONDITIONALLY (HImode is the `TARGET_MMU_TRAPS` case
 *    batch 293 documented), so `unsigned char u = f();` is `lsl #24 / lsr #24` and
 *    `r = (signed char)u;` is `lsl #24 / asr #24` -- the ROM's four-shift chain.  SIX
 *    spellings were measured and five collapse to two shifts, because combine folds
 *    `sign_extend(subreg:QI(zero_extend(x)))`:
 *      `(signed char)(unsigned char)f()`                  2 shifts
 *      `unsigned char u = f(); r = (signed char)u;`       2 shifts (u dies in the cast)
 *      callee declared to return `unsigned char`           2 shifts
 *      `unsigned char u; signed char sc = u; r = sc;`     2 shifts
 *      `signed char r = f();` + `if (r == 0)`             2 shifts, zero-extend only
 *      `u = f(); free(buf); r = (signed char)u;`          FOUR -- exact
 *    The discriminator is the CALL between the two casts: with `free(buf)` in between the
 *    fold does not happen.  `if (r == 0)` alone never forces the sign extension -- gcc
 *    compares the zero-extended value -- so the cast has to be a separate statement.
 *
 * 3. MATERIALISE THE SHARED ZERO BEFORE THE ONE, 93 -> 88.  The ROM's entry emits
 *    `mov r3,#0 / mov r2,#1` then four stores.  Source order `refresh = 1; ret = 0;
 *    done = 0;` emits the 1 first and loses four positions; `ret = 0; refresh = 1;
 *    done = 0;` and `ret = 0; done = 0; refresh = 1;` both emit the 0 first and both
 *    read 88.  This is the batch-293 "move each shared zero" lever with no HImode store
 *    anywhere near it.
 *
 * 4. DECLARATION ORDER SETS THE STACK SLOTS, 110 -> 107, and it is exact.  Spilled
 *    locals take slots from the HIGH end in DECLARATION order, so the ROM's layout
 *    (count 0x18, refresh 0x14, ret 0x10, unit 0xc, done 0x8) requires `unit` declared
 *    between `ret` and `done`, which is not where a reader would put it.  With `unit`
 *    declared up with the other pointers it lands at 0x18 and every slot below it moves.
 *
 * Also measured and exact: `Func_80a1a40((ofs + cursor) * 8 - 0xa, 0x10)` must keep `ofs`
 * as a VARIABLE -- `(cursor * 2 + cursor) * 8` folds to `cursor * 24` and buys a `mul`;
 * the two pooled constants `=0x1e` and `=0x1a` are plain HImode literal stores and want
 * no int carrier (the batch-293 rule, in its pooling direction); `n = 0xa2 * 2 + ofs;
 * *(unsigned short *)((int)state + n)` is required for the ROM's `strh r2,[r7,r3]` --
 * writing the three-term sum inline reassociates to `(state + ofs) + 0x144` and builds
 * the address instead.
 *
 * BLOCKER: THE TWO `cursor * 2` PSEUDOS HAVE SWAPPED ROLES, and it is the last cluster
 * plus the one misplaced instruction.  The ROM has r8 = `cursor * 2` serving the pointer,
 * the Func_80a1a40 argument, both key reads and the exit, and r9 = a ONE-USE COPY
 * (`mov r9, r8`) serving only the `0x1a` store.  This candidate has them the other way
 * round: the declared `ofs` takes r9 and the CSE temp takes r8.  The cost is visible in
 * the else arm, where the ROM writes `mov r3,r10 / lsl r3,#1 / mov r8,r3` and this writes
 * a fourth instruction `mov r8,r9` because BOTH pseudos are live at the join -- that is
 * the +2 bytes that shifts thirteen relocations.
 *
 * The copy cannot be conjured from source.  NINE spellings of the `0x1a` index were
 * measured and every one either folds to a single pseudo or leaves the roles swapped:
 * `m = cursor * 2` in the same block (local CSE substitutes and combine then propagates
 * the copy away), `m = ofs`, `n = 0xa2 * 2; n += ofs`, `m = 0xa2 * 2; n = m + ofs`, and
 * the inline three-term forms.  A copy insn only survives when its destination pseudo is
 * live where the source is not, and both of these are live across the same inner loop.
 * The pass is `combine`'s copy propagation over an `(set (reg) (reg))` produced by
 * `cse_insn`'s `use_related_value` substitution; /opt/camelot-gcc/gcc-2.96/gcc/combine.c.
 * Retry alongside a change that shortens one pseudo's live range, not by itself -- this
 * is exactly the coupled pair that single drops cannot find.
 *
 * RE-MEASURED AT THE END, in case these were coupled to something already fixed (they are
 * not): a NAMED `unsigned char *sel = state + 0x9a * 4;` pointer assigned in the loop
 * preheader -- the shape that turned out to be worth 84 -> 49 on this batch's Func_80c2724
 * -- is 88 -> 98 HERE, so the plain `*(state + 0x9a * 4)` expression plus loop-invariant
 * motion of the address is right for this function and the two neighbours want opposite
 * answers.  And `m = ofs;` placed in the INNER loop's preheader, where the ROM's copy
 * physically sits, is inert at 88: the copy is propagated away wherever it is written.
 *
 * Remaining residue after that: the `count` spill store is scheduled before the second
 * `ldrsb` rather than after it (reload + sched2, four positions), `mov r0,#0x2` and
 * `ldr r0,[r7,#0x28]` are each one position early in their argument setups, and the first
 * `gKeyPress` load is split by one instruction.  All scheduling, all downstream of the
 * register swap.
 *
 * SHIMS: ZERO.  No `register ... __asm__` declaration and no `__asm__(".equ ...")` line.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_a1000/80a60d4.c \
 *     asm/rom_a1000/rom_a5534_c_a_a.s --func Func_80a60d4
 */
extern unsigned char *iwram_3001f2c;
extern volatile unsigned int gKeyPress;
extern volatile unsigned int gKeyRepeat;
extern unsigned char *_GetUnit(int id);
extern int Func_80a10d0(void *slot, int a, int b, int c, int d, int e);
extern void Func_80a33d4(unsigned char *state, unsigned int win);
extern unsigned char *_Func_801eb64(int a, int b, int c, int d, int e);
extern void Func_80a6384(int id);
extern void Func_80a112c(int win, int id, int a, int b);
extern void Func_80a6614(int win, int id);
extern void Func_80a1804(unsigned char *state, int id);
extern int _GetFlag(int id);
extern void _ClearFlag(int id);
extern void _Func_80164ac(int win);
extern void _Func_8016498(int win);
extern void Func_80a23c0(int win);
extern void Func_80a1a40(int x, int y);
extern void WaitFrames(int n);
extern void _PlaySound(int id);
extern void *Func_8004938(unsigned int size);
extern int Func_80a68ec(unsigned char *unit, void *buf, int flag);
extern void free(void *p);

int Func_80a60d4(unsigned short *roster, unsigned short *list)
{
    unsigned char *state;
    unsigned char *box;
    unsigned short *p;
    unsigned short *q;
    void *buf;
    int count;
    int cursor;
    int refresh;
    int ret;
    unsigned char *unit;
    int done;
    int ofs;
    int i;
    int r;
    int win;
    int v;
    int n;
    int m;
    unsigned char u;

    state = iwram_3001f2c;
    count = *(signed char *)(state + 0x1e);
    cursor = *(signed char *)(state + 0x1c);
    ret = 0;
    refresh = 1;
    done = 0;
    *(state + 0x9a * 4) = ret;
    unit = _GetUnit(*(unsigned short *)(cursor * 2 + (int)roster));
    if (Func_80a10d0(state + 0x20, 0xd, 3, 0x11, 0xa, 2))
        Func_80a33d4(state, *(unsigned int *)(state + 0x20));
    if (Func_80a10d0(state + 0x28, 0xd, 0xd, 0x11, 4, 2)) {
        box = _Func_801eb64(2, 0, *(int *)(state + 0x28), 0, ret);
        *(unsigned char **)(state + 0x87 * 4) = box;
        box[5] = 0xd;
    }
    while (_GetFlag(0xa8 * 2) == 0) {
        if (refresh) {
            refresh = 0;
            cursor = (cursor + count) % count;
            ofs = cursor * 2;
            p = (unsigned short *)(ofs + (int)roster);
            win = *(int *)(state + 0x24);
            unit = _GetUnit(*p);
            Func_80a6384(*p);
            Func_80a112c(win, *p, 0, 0);
            Func_80a6614(*(int *)(state + 0x28), *p);
            Func_80a1804(state, *p);
            q = (unsigned short *)(state + 0xa5 * 2);
            i = 3;
            do {
                *q-- = 0x1e;
                i--;
            } while (i >= 0);
            n = 0xa2 * 2 + ofs;
            *(unsigned short *)((int)state + n) = 0x1a;
            if (_GetFlag(0x151) == 0 && done == 0) {
                _Func_80164ac(*(int *)(state + 0x2c));
                _Func_8016498(*(int *)(state + 0x2c));
                Func_80a23c0(*(int *)(state + 0x2c));
                done = 1;
            } else {
                _ClearFlag(0x151);
            }
        } else {
            ofs = cursor * 2;
        }
        Func_80a1a40((ofs + cursor) * 8 - 0xa, 0x10);
        WaitFrames(1);
        if (gKeyPress & 1) {
            if (*(state + 0x86 * 4)) {
                _PlaySound(0x70);
                ret = roster[cursor];
                goto out;
            }
            _PlaySound(0x72);
        }
        if ((gKeyPress & 0x200) || (gKeyPress & 0x100)) {
            ret = roster[cursor];
            if (gKeyPress & 0x200)
                *(state + 0x9a * 4) = 1;
            else
                *(state + 0x9a * 4) = 2;
            buf = Func_8004938(0x40);
            u = Func_80a68ec(unit, buf, 1);
            free(buf);
            r = (signed char)u;
            if (r == 0) {
                *(state + 0x9a * 4) = r;
                _PlaySound(0x72);
            } else {
                _PlaySound(0x70);
                goto out;
            }
        }
        if (gKeyPress & 2) {
            _PlaySound(0x71);
            ret = -1;
            goto out;
        }
        if (gKeyRepeat & 0x20) {
            _PlaySound(0x6f);
            refresh = 1;
            cursor -= 1;
        }
        if (gKeyRepeat & 0x10) {
            _PlaySound(0x6f);
            refresh = 1;
            cursor += 1;
        }
    }
    ofs = cursor * 2;
out:
    state[0x1c] = cursor;
    v = roster[cursor];
    *(int *)(state + 8) = v;
    *(state + 0x21a) = v;
    return ret;
}
