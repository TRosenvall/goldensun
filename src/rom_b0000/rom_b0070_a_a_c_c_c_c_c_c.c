/* Func_80b1f4c -- batch 293 brief D target 1.  EXACT.
 * ref: asm/rom_b0000/rom_b0070_a_a_c_c_c_c_c_c.s   (161 insns / 175 encodings / 412 bytes)
 *
 *   objcmp --whole: OK whole file -- 412 bytes, 175 encodings and 19 relocations identical
 *
 * Whole-file conversion: datacheck reports no data, grep -ci func_start = 1.
 * No flags.  NO SHIMS (zero `register ... __asm__` declarations in the code).
 *
 * WHAT IT DOES.  Commits a buy/sell: reads u->items[slot] at five separate sites
 * (the ROM re-loads it after every call, so it is not one CSE'd value), multiplies
 * the unit price by the quantity, picks one of five message ids from an else-if
 * chain, confirms through Func_80b0634, then repeats _Func_8078948 qty times.
 * qty == -1 is the "all" sentinel: it sets a flag and rewrites qty to 1.
 *
 * ================= THE LOAD-BEARING CONSTRUCTS, WITH SINGLE DROPS =================
 *
 * Baseline, everything spelled naturally with plain integer ids:
 *      66 of 179 aligned (tryc --align), 396 bytes vs 412, 169 encodings vs 175.
 *
 * 1. THE MUL OPERAND ORDER.  `Func_80b19cc(...) * qty`, NOT `qty * Func_80b19cc(...)`.
 *    73 -> 71 aligned.  The ROM's `mov r2, r7 / mul r2, r0` copies the QUANTITY (r7),
 *    so per the procedure the OTHER operand -- the price in r0 -- goes on the right...
 *    and that is the spelling that LOST here.  Read this as a confirmation that the
 *    mul lever is a per-site measurement and not a transferable direction: both
 *    orders had to be measured, and the direction that "should" have won did not.
 *
 * 2. SEVEN MESSAGE IDS MUST BE SYMBOLS, NOT CONST_INTs.  66 -> 0.  This is the
 *    whole function.  See the block below -- it is a .sym proposal.
 *
 * 3. `item` DECLARED BEFORE `info`.  14 -> 7 (objcmp, both at exact size and count).
 *    Pure greg allocno ordering: the ROM has item in r10 and info in r9, and with
 *    `info` first the pair is reversed.  Five `mov rD, rN` rows, nothing else.
 *
 * 4. `all` DECLARED IN A NESTED BLOCK OPENED AFTER THE FIRST u->items[slot] READ.
 *    7 -> 0.  This is elevation.md's "an inner scope is the only handle on
 *    spill-slot order", and it is forced here rather than chosen:
 *      - the frame has three spilled values -- `st`, the `slot * 2` scaling temp,
 *        and the `all` flag -- and the ROM puts them at sp+8, sp+4, sp+0;
 *      - `expand_decl` numbers every function-scope local before any statement, so
 *        a function-scope `all` is ALWAYS numbered below a temp that expand creates
 *        while emitting the third statement, and slots run ascending pseudo ->
 *        descending sp;
 *      - so at function scope `all` can only land at sp+4 and the temp at sp+0,
 *        which is exactly the residue.  Moving `all` into a block that opens after
 *        `item = u->items[slot] & 0x1ff;` pushes its pseudo past the temp's and the
 *        two slots swap.  Declaration order among function-scope locals is INERT
 *        here (measured: `all` declared last is 14, unchanged) -- only the nesting
 *        moves it.
 *
 * ===================== .sym PROPOSAL: SEVEN _MSG_ IDS =====================
 *
 * PROPOSED: _MSG_cae, _MSG_caf, _MSG_cb0, _MSG_cb1, _MSG_cb2, _MSG_cb3, _MSG_cb4
 *           (all = their own value, e.g. `_MSG_cb0 = 0x0cb0;`)
 *
 * It clears BOTH halves of the bar: the seven COMPLETE the function (66 -> 0,
 * byte-exact), and the in-function control is four-fold -- 0xcab, 0xcac, 0xcb5 and
 * 0xcb6 are ids in the SAME function, in the same run, feeding the SAME sink
 * (Func_80b0574), and every one of them reproduces byte-exact as a PLAIN LITERAL.
 * Measured individually: dropping each of those four back to an integer leaves the
 * file exact; dropping any of the seven breaks it.  Same function, same consumer,
 * same namespace, opposite behaviour, for two nameable mechanical reasons:
 *
 *   (a) 0xcb0 IS SHIFTABLE -- 0xcb << 4 -- so `thumb_shiftable_const` makes gcc
 *       BUILD it, and a plain literal emits `mov r5, #0xcb / lsl r5, #4` where the
 *       ROM spends a pool word.  gcc pools only what it cannot build, so the pool
 *       word cannot have come from a const_int.  This is message.sym's existing
 *       impossibility argument (_MSG_810, _MSG_ad0, _MSG_c20, _MSG_c90/_MSG_ca0),
 *       and the control is right here: the other ten ids are all unshiftable
 *       (0xcab/0xcaf/0xcb1/0xcb3/0xcb5 odd; 0xcac >> 2, 0xcae >> 1, 0xcb2 >> 1,
 *       0xcb4 >> 2, 0xcb6 >> 1 all still over eight bits) and ten of them pool.
 *
 *   (b) THE OTHER SIX ARE AN IF-CONVERSION GATE, WHICH IS A SECOND IMPOSSIBILITY
 *       ARGUMENT AND -- as far as I can find -- NOT YET IN elevation.md.
 *       ifcvt.c's `noce_try_store_flag_constants` opens with
 *           GET_CODE (if_info->a) == CONST_INT && GET_CODE (if_info->b) == CONST_INT
 *       and a SYMBOL_REF in either arm bails the whole function.  Two things ride
 *       on that gate, both visible in the residue:
 *         - 0xcae/0xcaf have `diff == 1 == STORE_FLAG_VALUE`, so the vanilla path
 *           rewrites `if (flag4) msg = 0xcaf; else msg = 0xcae;` into
 *           `msg = 0xcae + (flag4 != 0)` and emits the ROM-absent
 *           `neg r3, r2 / orr r3, r2 / lsr r3, #31 / add r5, r3` idiom.  The
 *           control is INTERNAL and exact: the other three constant pairs in this
 *           same function -- (0xcb6, 0xcb4) and (0xcb5, 0xcb3) -- have diff 2,
 *           which with thumb's STORE_FLAG_VALUE == 1 and BRANCH_COST < 3 falls off
 *           the end of that function's chain and returns FALSE, so THEY keep the
 *           ROM's branches.  One pair folded, three not, one function, and the
 *           discriminator is arithmetic on the values.
 *         - the same gate blocks a SPECULATION: for every chain arm gcc otherwise
 *           hoists the arm's constant load ABOVE its own test and inverts the
 *           branch (`ldr r5, =0xcb2 / cmp / bne end` for the ROM's
 *           `cmp / beq next / ldr r5, =0xcb2 / b end`), which is one instruction
 *           shorter per arm and is where 10 of the 16 missing bytes came from.
 *       NO FLAG REACHES EITHER.  gcc-2.96 runs if-conversion unconditionally --
 *       `-fno-if-conversion` is not a recognised option (toplev.c has no entry;
 *       the calls at toplev.c:2872 and :3230 are unguarded) -- and measured on the
 *       literal-only control: -fno-thread-jumps 66 (inert), -fno-cse-follow-jumps
 *       66 (inert), -fno-expensive-optimizations 82 (worse), -fno-gcse 117 (much
 *       worse), -fno-schedule-insns2 65-ish (worse, and it does NOT remove the
 *       speculation -- verified in the asm, it only moves it within the block).
 *       So the ROM's shape is unreachable from a const_int by construction, which
 *       is the same class of argument as shiftability rather than a preference.
 *
 * Consumer is Func_80b0574, which message.sym already documents as a message sink
 * (it is _MSG_d1c's consumer).  Named by value, asserting the namespace only --
 * this file's convention for the shiftable ids.
 *
 * LANDING NOTE.  The `__asm__(".equ ...")` block below is what makes this scratch
 * file self-verifying (absolute symbols, so the pool words resolve and objcmp sees
 * no extra relocation).  If the seven rows go into message.sym -- which stage1.ld
 * already INCLUDEs -- DELETE that block and keep the `extern int` declarations, or
 * use include/message.h.  Verified equivalent: with the `.equ` lines stripped the
 * instruction stream is unchanged and the only residue is the seven
 * `ldr rX, =_MSG_xxx` vs `ldr rX, =0xcXX` TEXT rows (7 of 179 aligned, count still
 * exact at 179), which are the same encoding once the linker resolves them.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py <this> \
 *     asm/rom_b0000/rom_b0070_a_a_c_c_c_c_c_c.s --whole
 */
typedef struct { unsigned char pad00[0xd8]; unsigned short items[1]; } Unit;
typedef struct { short price; unsigned char pad02; unsigned char f03; } ItemInfo;
typedef struct { unsigned char pad00[0x20]; void *f20; } State;

extern int _MSG_cae;
extern int _MSG_caf;
extern int _MSG_cb0;
extern int _MSG_cb1;
extern int _MSG_cb2;
extern int _MSG_cb3;
extern int _MSG_cb4;

extern unsigned char iwram_3001f2c[];

extern Unit *_GetUnit(int unit);
extern ItemInfo *_GetItemInfo(int item);
extern int Func_80b19cc(int item);
extern void _Func_8019908(int a, int b);
extern void Func_80b0574(int msg);
extern int Func_80b0634(int a);
extern void _PlaySound(int id);
extern void _Func_8078948(int unit, int slot);
extern void _AddCoins(int n);
extern void Func_80b10cc(void);
extern void Func_80b1dec(void *box, int unit);

void Func_80b1f4c(int unit, int slot, int qty)
{
    State *st;
    Unit *u;
    int item;
    ItemInfo *info;
    unsigned char flag4;
    int total;
    int msg;
    int i;

    st = *(State **)iwram_3001f2c;
    u = _GetUnit(unit);
    item = u->items[slot] & 0x1ff;
    info = _GetItemInfo(item);
    flag4 = info->f03 & 4;
    {
    int all;
    all = 0;
    if (qty == -1) {
        all = 1;
        qty = 1;
    }
    total = Func_80b19cc(u->items[slot]) * qty;
    if (total == 0) {
        _Func_8019908(item, 2);
        Func_80b0574(0xcac);
        return;
    }
    if ((u->items[slot] & 0x200) != 0 && (info->f03 & 2) != 0) {
        _Func_8019908(item, 2);
        Func_80b0574(0xcab);
        return;
    }
    if (all != 0)
        msg = (int)&_MSG_cb2;
    else if ((u->items[slot] & 0x400) != 0)
        msg = (int)&_MSG_cb1;
    else if (qty > 1)
        msg = (int)&_MSG_cb0;
    else if (flag4 != 0)
        msg = (int)&_MSG_caf;
    else
        msg = (int)&_MSG_cae;
    _Func_8019908(item, 2);
    _Func_8019908(total, 5);
    Func_80b0574(msg);
    if (Func_80b0634(0) != 0) {
        if (flag4 != 0 || all != 0)
            msg = 0xcb6;
        else
            msg = (int)&_MSG_cb4;
        Func_80b0574(msg);
        return;
    }
    _PlaySound(0x66);
    if (qty > 0) {
        i = qty;
        do {
            _Func_8078948(unit, slot);
            i--;
        } while (i != 0);
    }
    _AddCoins(total);
    Func_80b10cc();
    Func_80b1dec(st->f20, unit);
    if (flag4 != 0 || all != 0)
        msg = 0xcb5;
    else
        msg = (int)&_MSG_cb3;
    Func_80b0574(msg);
    }
}
