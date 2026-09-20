/* Cluster Func_80b1470..Func_80b153c extracted from goldensun/asm/rom_b0000/rom_b0070_a_a_c_c_c_a_a.s.
 *
 * Total .text for this TU = 420 bytes (= 0x1a4), being 204 + 216.
 * Preserves the original ROM layout when slotted between
 * asm/rom_b0000/rom_b0070_a_a_c_c_c_a_a_a_b.o and asm/rom_b0000/rom_b0070_a_a_c_c_c_a_a_a_d.o in
 * goldensun/stage1.ld.
 *
 * Never attempted before batch 274. No pins, no flags. Each function was verified on
 * its own against the reference (204 bytes / 93 encodings and 216 / 95) before being
 * combined here; the combined unit is gated on make compare. _MSG_c90 and _MSG_ca0 are
 * new in message.sym under the same argument as _MSG_ad0 and _MSG_c20.
 *
 * ================ A `goto` LOOP SUPPRESSES STRENGTH REDUCTION ================
 *
 * Func_80b1470 is the reason this round took as long as it did, and the lever is worth
 * more than the function.
 *
 * The ROM recomputes the item address at BOTH access sites -- `lsl r3, r5, #1 /
 * add r3, #0xd8` -- and carries only `i*2` across the back edge (`mov r2, #0` in the
 * preheader, `mov r2, r3` at the latch). That is NO GIV AT ALL, just gcse PRE of the
 * multiply. EVERY structured spelling of the loop makes gcc strength-reduce the address
 * instead, which costs a pseudo and reshuffles all six callee-saved registers:
 *
 *   for + break                                   95 lines, 82 differing
 *   for with a compound condition                 95, 82
 *   while with an inner break                     90, 84
 *   `it = u->items;` then `it[i]`                 95, 70
 *   byte-cast address, either parenthesisation    94, 84
 *   a named `off = (i << 1) + 0xd8`, all three
 *     parenthesisations                           96, 71
 *   `off = i * 2;` then `+ off + 0xd8`           103, 101
 *   A `goto` LOOP                                 96, 1  (the pool word alone)
 *
 * WHY NO SOURCE TWEAK REACHES IT, read out of loop.c:4544 rather than guessed:
 * strength reduction fires when `v->lifetime * threshold * benefit < insn_count`, with
 * `threshold = (has_call ? 1 : 2) * (3 + n_non_fixed_regs)` -- about 17 here -- a
 * `lifetime` of 1 for a DEST_ADDR giv, and the two address givs COMBINING to benefit 15.
 * That is 255 against 28, so it reduces unconditionally. `.08.loop` names it:
 * `giv at 125 reduced to (reg:SI 74)`.
 *
 * A `goto`-built loop has no NOTE_INSN_LOOP_BEG, so `loop_optimize` never sees it and
 * `strength_reduce` never runs. Everything else -- allocation, scheduling, the
 * `mov r2, r3` carry -- then fell out on the first try.
 *
 * SO: when the ROM RECOMPUTES a loop address that gcc reduces to a pointer, and no
 * structured spelling reaches it, the loop was written with `goto`. `-fno-strength-reduce`
 * is not available per-file for this, and it would be the wrong tool anyway -- it would
 * disable the pass for the whole translation unit.
 *
 * ================ Func_80b153c: A SINGLE EXIT AND A `ret` LOCAL ================
 *
 * It sat at 97 of 98 through ELEVEN spellings -- a named flags local, a named mask,
 * `((u8 *)info)[3]`, two pointer names, `goto out`, moving `st` and `n`, dropping the
 * final return, and three declaration orders -- ALL IDENTICAL at 1 differing. The missing
 * instruction was the reload copy `mov r1, r8` before `ldrb r2, [r1, #3]`.
 *
 * `ret = 1; if (...) { ... } return ret;` fixed it. The residue was reload's
 * find_equiv_reg reusing the still-live r0 instead of copying out of r8, and the extra
 * variable changes the pressure enough that it no longer can.
 *
 * A ONE-INSTRUCTION RESIDUE AT A CALL-RESULT COPY IS A PRESSURE SYMPTOM, NOT A SPELLING
 * ONE -- try the single-exit restructure before sweeping spellings. The same lever landed
 * Func_80b1e80 in this batch, where an early `return ret` kept the value in r5 and never
 * built a frame, and wrapping the body in a single `if` spilled it to [sp] exactly as the
 * ROM does.
 *
 * TWO SMALLER READINGS KEPT:
 *   - `ldrsh` AT OFFSET 0 WANTS A TYPED STRUCT FIELD. `(short)*(unsigned short *)info` --
 *     the idiom that is RIGHT in the sibling Func_80b17e4 -- gives `ldrh / lsl #16 /
 *     asr #16` here; a `short price` member at offset 0 gives the ROM's
 *     `mov r3, #0 / ldrsh r1, [r2, r3]`. A case where transferring a sibling's idiom
 *     verbatim is wrong, because that sibling's `info` is an `unsigned char *`.
 *   - `gState.f10` must be `unsigned int` for `__udivsi3`; the file-mate
 *     src/rom_b0000/rom_b0070_a_a_c_c_a_c_b.c declares it `int`, which gives `__divsi3`.
 *   - MIN-OF-TWO IS A TERNARY, NOT AN `if`. `n = n > f(x) ? f(x) : n;` gives the ROM's
 *     `ble / b / mov / mov` join; the `if` form folds the store into the taken arm.
 */
#include "message.h"

typedef struct { unsigned char pad00[0xd8]; unsigned short items[1]; } Unit;
typedef struct { unsigned char pad000[0x3a8]; unsigned char f3a8; unsigned char pad3a9; unsigned char f3aa; } State;
typedef struct { unsigned char pad00[0x10]; unsigned int f10; unsigned char pad14[0x2ac]; } GlobalState;
typedef struct { short price; unsigned char pad02; unsigned char f03; } ItemInfo;

extern unsigned char iwram_3001f2c[];
extern GlobalState gState;

extern Unit *_GetUnit(int unit);
extern ItemInfo *_GetItemInfo(int item);
extern void _Func_8016478(int box);
extern int _CheckItem(int unit, int item);
extern void _Func_8019908(int a, int b);
extern void _Func_801e7c0(int a, int b, int c, int d);
extern unsigned char *_Func_801eb90(int item, int a, int box, int x, int y);
extern void Func_80b04dc(int msg);
extern int _Func_8078ad0(int item, int b);
extern void Func_80b0a6c(int a, int b, int c);
extern int Func_80b1614(int a, int b, int c);
void Func_80b1470(int box, int unit, int item)
{
    Unit *u;
    int slot;
    int i;
    int x;
    int y;

    u = _GetUnit(unit);
    x = 8;
    y = 8;
    if (box == 0)
        return;
    _Func_8016478(box);
    slot = _CheckItem(unit, item);
    if (slot != -1) {
        _Func_8019908((u->items[slot] >> 11) + 1, 5);
        _Func_801e7c0(MSG_c90, box, 0, 0);
    } else {
        _Func_801e7c0(0xc8f, box, 0, 0);
    }
    i = 0;
    if (u->items[i] == 0)
        return;
loop:
    _Func_801eb90(u->items[i], 0x1b, box, x, y)[0xf] = 0xfc;
    x += 0x10;
    if (i == 4) {
        x = 8;
        y += 0x10;
    }
    if (i == 9) {
        x = 8;
        y += 0x10;
    }
    i++;
    if (i > 0xe)
        return;
    if (u->items[i] != 0)
        goto loop;
}

int Func_80b153c(int unit, int item)
{
    State *st;
    Unit *u;
    ItemInfo *info;
    int slot;
    int have;
    int n;
    int ret;

    st = *(State **)iwram_3001f2c;
    u = _GetUnit(unit);
    info = _GetItemInfo(item);
    ret = 1;
    if ((info->f03 & 0x10) != 0) {
        Func_80b04dc(MSG_ca0);
        slot = _CheckItem(unit, item);
        if (slot != -1)
            have = (u->items[slot] >> 11) + 1;
        else
            have = 0;
        n = 0x1e;
        if (info->price != 0)
            n = gState.f10 / info->price;
        if ((signed char)st->f3aa == 2)
            n = n > _Func_8078ad0(item, 0) ? _Func_8078ad0(item, 0) : n;
        n += have;
        if (n > 0x1e)
            n = 0x1e;
        st->f3a8 = 0xc;
        Func_80b0a6c(0, 0x80, 0x30);
        ret = Func_80b1614(have, n, info->price);
    }
    return ret;
}
