/* Func_80191cc (0x080191cc) -- NON-MATCHING: 485 encodings of 510 differ (objcmp).
 * THAT NUMBER IS NOT A DISTANCE: reference 510 instructions / 1152 bytes, ours 520 /
 * 1176, so the streams misalign at index 1 and objcmp reports the whole tail.  The
 * honest measure is an instruction-stream diff with registers and immediates
 * normalised: 478 reference instructions, 344 of them identical, ~203 differing (t1n.s in scratch).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_15000/80191cc.c \
 *     asm/rom_15000/rom_1908c_a.s --whole
 *
 * asm/rom_15000/rom_1908c_a.s holds THIS FUNCTION ONLY and no data section, so
 * landing it is a plain whole-file conversion: no split, no linker-script change.
 *
 * SHIM PRESENT (must be booked in fakematch.txt if this ever lands as written):
 *   __asm__ ("")   after the first Func_8003dec call.  Without it jump2's
 *   find_cross_jump merges the two identical `ldrb r1,[e+0xf] / mov r0,o /
 *   bl Func_8003dec` tails into one; the ROM keeps both (two R_ARM_THM_CALL
 *   relocations against Func_8003dec at 0x428 and 0x436).  `do { } while (0)` in
 *   the same place does NOT stop the merge -- measured, it leaves one call.
 *
 * ============================ THE BLOCKER ============================
 * REGISTER ALLOCATION, and it is local-alloc (local-alloc.c find_free_reg) that
 * decides it, not global-alloc.
 *
 * The ROM's long-lived values are
 *     r6 = e (list cursor)   r7 = o (= e + 0x10)   r9 = base   r10 = q   r11 = i
 *     r5 = the byte-5 bitfield accumulator in case 2
 *     r4 = the byte-7 bitfield accumulator in case 2, CALLER-SAVED across __umodsi3
 *     r8 = e->f8 held across __umodsi3
 * Ours gets
 *     r4 = e (caller-saved around EVERY call in the loop: 17 str/ldr pairs)
 *     r7 = o   r8 = q   r9 = i   r10 = base   r5, r6 = the two byte accumulators
 * and never touches r11.  Every `[r6,#N]` vs `[r4,#N]` line and every
 * `str r4,[sp]` / `ldr r4,[sp]` pair in the diff is downstream of that one swap.
 *
 * WHY, with the arithmetic.  `e` is pseudo 34 and `o` pseudo 35 (read off
 * t1v.c.17.lreg / .18.greg: insn 42 sets reg/v 34 from (mem:SI (reg/v 33)), which
 * is `e = *(struct Ent **)q`).  The greg dump's allocation order is
 *     ;; 10 regs to allocate: 740 747 71 36 35 34 37 33 32 38
 * so `o` is allocated before `e`.  `e`'s class is LO_REGS (it is a load base), and
 * global.c find_reg puts call_used_reg_set into `used1` for any allocno with
 * calls_crossed != 0 -- with -fcall-used-r4 that removes r0-r3, r4, r12, r14, so
 * the only candidates are r5, r6, r7.  local-alloc has ALREADY taken r5 and r6 for
 * the two case-2 byte accumulators (they are block-local quantities, and
 * local_alloc runs first and is never overridden), `o` then takes r7, and `e` finds
 * nothing.  find_reg only then falls to its caller-save retry
 * (global.c:1150, CALLER_SAVE_PROFITABLE(n_refs, calls) == 4*calls < n_refs,
 * regs.h:184), which succeeds for `e` because it has many refs -- and hands it r4.
 *
 * So the lever is not `e` at all: it is stopping local-alloc from spending TWO
 * callee-saved low registers in case 2.  local-alloc's own caller-save retry
 * (local-alloc.c:2072, same CALLER_SAVE_PROFITABLE macro) is what puts the ROM's
 * byte-7 accumulator in r4; it needs 4*calls_crossed < n_refs, and with exactly one
 * call crossed (__umodsi3) that means the quantity must have at least FIVE
 * references.  Ours has three or four.  That is the arithmetic to beat, and the
 * next attempt should aim straight at it.
 *
 * WHAT MOVED THE NEEDLE (measured, normalised-diff equal-count out of 478):
 *   - `o->mode = 0;` written BEFORE `o->c6 = 2;` instead of after: 337 -> 342.
 *     This is the reason it matters: both bitfield writes mask with 0x3f, cse2
 *     shares one pseudo for the constant, and whichever write comes first forces
 *     gcc to COPY the constant (`mov r4, r3` in the ROM) instead of consuming it
 *     in place.  With `mode` first, ours emits the copy too -- the ROM's
 *     three-quantity shape (constant, byte-5 value, byte-7 value) appears, and the
 *     whole __umodsi3 caller-save block collapses to the ROM's.  This is batch
 *     292's "naming a common subexpression can remove a copy the ROM keeps" lever
 *     running through a bitfield MASK rather than a source expression.
 *   - `signed char` not `s8` for the two signed table reads (s8 is plain char,
 *     which is unsigned here): turns `ldrb` into the ROM's `ldrsb`.
 *   - `*(volatile u16 *)(base + 0x12b6)`: the ROM loads that halfword TWICE in
 *     case 2 from one cse'd address (`ldrh r3,[r1]` then `ldrh r0,[r1]`).  Without
 *     volatile cse2 replaces the second with `mov r0,r3`.
 *
 * WHAT WAS INERT OR WORSE (all measured):
 *   - declaration-order permutation (e/o before base/q): INERT, exactly 337/210,
 *     confirming docs/elevation.md's rule that it only matters on an exact tie.
 *   - a named `y = e->f8;` before the % expression, to reproduce the ROM's
 *     `mov r8, r3`: 342 -> 316.  WORSE.  The ROM does hold e->f8 in r8 across
 *     __umodsi3 while we reload it after, but forcing it with a local costs more
 *     than it buys; it must fall out of the allocation, not be written.
 *   - `o->mode = 0;` moved after the `o->f4` store: 342 -> 336.
 *   - `o->mode = 0;` first of all five byte writes: 338.
 *   - `o->mode` between c4 and c5: 339.
 *
 * ================= WHAT IS ESTABLISHED, AND IS REUSABLE =================
 * 1. The walk is the landed neighbour's.  src/rom_15000/rom_1908c_c_a_c_a.c
 *    (Func_80197c4) already walks `iwram_3001e8c + (0xa0 << 3)` in steps of 0x24
 *    eight times testing `*(u16 *)(q + 0x16)`; copy that spelling, including the
 *    `(0xa0 << 3)`.
 * 2. THE INNER LOOP'S ENTRY IS A `goto` INTO THE BODY.  The ROM jumps to the
 *    BOTTOM block, and that block computes `sel = (frame >> 2) & 7` BEFORE the
 *    `cmp e,#0`.  A plain `while (e) { sel = ...; ... }` puts sel at the top of the
 *    body instead.  `goto test;` into a do-while, with `test:` immediately before
 *    the `while (e != 0)`, is what places it where the ROM has it.
 * 3. THE STACK STRUCT NEEDS NO SPECIAL TYPE, and this cost half a session to
 *    establish, so it is written down here.  The ROM fills the three 16-bit fields
 *    of the 8-byte stack struct passed to Func_8003d28 with plain `strh` in two
 *    cases and with SImode read-modify-write against 0xffff0000 / 0xffff in the
 *    third -- same address, which looks like one variable with two store modes.
 *    IT IS NOT A TYPE DIFFERENCE.  gcc-2.96 emits the RMW form whenever it has NOT
 *    already materialised the struct's address in a register, and `strh` once it
 *    has (the address is needed for the call anyway).  A four-way probe
 *    (scratch_elev/b292/H/probe.c: `struct {u16 w,h,a,p;}` vs
 *    `struct {u32 w:16; u32 h:16; u32 a:16; u32 p:16;}`) compiles to BYTE-IDENTICAL
 *    output -- plain u16 members are correct, and the bitfield spelling buys
 *    nothing.  The pooled 0xffff of that RMW is then reused by cse as the -1 of
 *    `e->fc - 1` later in the same case, which is why the ROM writes
 *    `add r3, r5` there instead of `sub r3, #1`; that falls out for free.
 * 4. THE OBJ SUB-OBJECT AT +0x10 IS BITFIELDED, and these masks are read straight
 *    off the ROM:
 *      +0x05 byte : c0:2 (0x03) c2:2 (0x0c) c4:1 (0x10) c5:1 (0x20) c6:2 (0xc0)
 *      +0x06/7    : x:9 via ldrh/strh with 0x1ff and 0xfffffe00,
 *                   pri:5 via BYTE ops on +7 (mask 0x3e), mode:2 (mask 0xc0)
 *      +0x08 half : tile:10 with 0x3ff / 0xfffffc00
 *    The byte-unit-vs-halfword-unit choice in the ROM is exactly gcc's smallest
 *    unit containing the field, which is the evidence they are bitfields and not
 *    hand-written masks.  Every byte-5 and byte-7 sequence in the candidate below
 *    matches the ROM instruction for instruction.
 * 5. ONE RESIDUAL TYPING QUESTION.  The ROM loads the 0x1ff and 0x3ff value masks
 *    with `ldr` (SImode) where the bitfield spelling below gives `ldrh` (HImode) --
 *    about eight encodings.  stor-layout.c get_best_mode picks HImode for a 9-bit
 *    field at bit offset 48 and store_bit_field then converts the value to HImode.
 *    Writing those two fields as explicit masks on raw casts
 *    (`*(u16 *)&o->x6 = (v & 0x1ff) | (*(u16 *)&o->x6 & ~0x1ff)`, the bank's own
 *    idiom) does give SImode constants but cost far more elsewhere: measured
 *    292/300 against this file's 344/203.  Do not repeat that whole-file swap;
 *    apply it to ONE field at a time if it is tried again.
 * 6. The switch is `switch (e->state)` over cases 2..18 with 3 and 13 absent
 *    (`sub r3,#2 / cmp r3,#0x10 / bls` plus a 17-word table).  The ROM's case-body
 *    ORDER is 2, 5, 6, 7, 4, 17, 14/15/16, 18, 8, <shared block>, 9/10/11/12, and
 *    that is the order below; the shared block that cases 6 and 8 branch to when
 *    e->fc == 0 is a LABEL between case 8 and case 9, reached by goto -- writing it
 *    twice and letting cross-jumping merge it does not put it there.
 * 7. Callee shapes confirmed by use: UploadSpriteGFX and Func_8003d28 return int,
 *    Random returns unsigned (the ROM's `lsr #16` after *3 is unsigned), sin/cos
 *    take and return int and the ROM takes `>> 14` as an arithmetic shift.
 *    Func_8003dec's r1 is written before r0, so `void` was NOT tried and is the
 *    first thing to try next time (this bank inverts the int-return lever).
 */

#include "gba/types.h"

struct Spr {
    u32 f0;
    u8  f4;
    u8  c0:2, c2:2, c4:1, c5:1, c6:2;
    u16 x:9, pri:5, mode:2;
    u16 tile:10, t10:6;
};

struct Ent {
    struct Ent *next;
    u8  pad4;
    u8  state;
    u16 f6;
    u8  f8;
    u8  f9;
    u16 fa;
    u16 fc;
    u8  fe;
    u8  ff;
    struct Spr spr;
};

struct Req {
    u16 w;
    u16 h;
    u16 ang;
    u16 pad;
};

extern u8 *iwram_3001e8c;
extern u32 iwram_3001800;
extern u8 Data_368d4[];
extern u8 L33e60[] __asm__(".L33e60");
extern u8 L33eb0[] __asm__(".L33eb0");
extern u8 L33ee8[] __asm__(".L33ee8");

extern int UploadSpriteGFX(int id, int n, void *src);
extern int Func_8003d28(struct Req *r);
extern void Func_8003dec(struct Spr *o, int n);
extern void Func_801908c(struct Ent *e);
extern u32 Random(void);
extern int sin(int a);
extern int cos(int a);

void Func_80191cc(void)
{
    u8 *base;
    u8 *q;
    struct Ent *e;
    struct Spr *o;
    u32 fc;
    u32 sel;
    int i;
    u16 t;
    struct Req s;

    base = iwram_3001e8c;
    q = base + (0xa0 << 3);
    i = 0;
L1:
    if (*(u16 *)(q + 0x16) & 1) {
        fc = iwram_3001800;
        e = *(struct Ent **)q;
        goto test;
        do {
            o = &e->spr;
            if (*(u16 *)(q + 0x12) == 4) {
                e->fc = 2;
                e->state = 8;
            }
            switch (e->state) {
            case 2:
                if (*(volatile u16 *)(base + 0x12b6) == 0x60)
                    break;
                o->tile = UploadSpriteGFX(*(volatile u16 *)(base + 0x12b6), 0x80,
                                          &Data_368d4[sel << 7]);
                e->fe = o->tile;
                o->c2 = 0;
                o->c4 = 0;
                o->c5 = 1;
                o->mode = 0;
                o->c6 = 2;
                o->f4 = e->f8 + L33e60[iwram_3001800 % 0x50] + 2;
                o->c0 = 0;
                o->pri = 0;
                break;
            case 5:
                if ((iwram_3001800 & 1) == 0)
                    break;
                o->x = e->f6 + ((((Random() * 3) >> 16) + ((Random() * 3) >> 16)) >> 1) - 1;
                o->f4 = e->f8 + ((((Random() * 3) >> 16) + ((Random() * 3) >> 16)) >> 1) - 1;
                break;
            case 6:
                if (e->fc == 0)
                    goto hide;
                s.w = 0x200;
                s.h = 0x200;
                s.ang = 0;
                o->pri = Func_8003d28(&s);
                o->c0 = 3;
                o->x = e->f6 - 5;
                o->f4 = e->f8 - 5;
                e->fc = e->fc - 1;
                break;
            case 7:
                s.w = 0x100;
                s.h = 0x100;
                e->fc = e->fc + 0x300;
                s.ang = e->fc;
                o->pri = Func_8003d28(&s);
                o->c0 = 1;
                o->x = e->f6 - (sin(s.ang + 0xe800) >> 14) - 2;
                o->f4 = e->f8 - (cos(s.ang + 0x6800) >> 14) - 2;
                break;
            case 4:
                if (iwram_3001800 & 1)
                    e->fc = e->fc + 1;
                t = e->fc % 0x14;
                o->x = e->f6 + *(signed char *)(L33eb0 + t * 2);
                t = e->fc % 0x14;
                o->f4 = e->f8 + L33eb0[t * 2 + 1] - 2;
                break;
            case 17:
                e->fc = e->fc + 1;
                o->f4 = e->f8 - L33ee8[e->fc & 0xf];
                break;
            case 14:
            case 15:
            case 16:
                e->fc = e->fc + 1;
                o->f4 = e->f8 + L33ee8[e->fc & 0xf];
                break;
            case 18:
                e->fc = e->fc + 1;
                o->x = e->f6 - *(signed char *)(L33ee8 + (e->fc & 0xf));
                o->f4 = e->f8 + L33ee8[e->fc & 0xf];
                break;
            case 8:
                if (e->fc == 0)
                    goto hide;
                s.w = 0x140;
                s.h = 0x140;
                s.ang = 0;
                o->pri = Func_8003d28(&s);
                o->c0 = 3;
                o->x = e->f6 - 8;
                o->f4 = e->f8 - 8;
                e->fc = e->fc - 1;
                break;
            hide:
                o->pri = 0;
                o->c0 = 0;
                o->x = e->f6;
                o->f4 = *(u16 *)&e->f8;
                break;
            case 9:
            case 10:
            case 11:
            case 12:
                Func_801908c(e);
                break;
            }
            if (e->state == 2) {
                if (*(volatile u16 *)(base + 0x12b6) != 0x60) {
                    Func_8003dec(o, e->ff);
                    __asm__ ("");
                }
            } else if (e->state != 0xd) {
                Func_8003dec(o, e->ff);
            }
            fc = iwram_3001800;
            e = e->next;
        test:
            sel = (fc >> 2) & 7;
        } while (e != 0);
    }
    i++;
    q += 0x24;
    if (i != 8)
        goto L1;
}
