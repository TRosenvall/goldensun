/* Func_80a9f10 -- ApplyItemEffect, 0x080a9f10, 484 ROM instructions.
 * PARKED.
 * NON-MATCHING, 48 of 569 encodings differ.
 *
 * SIZE IS EXACT (1336 = 1336) AND THE INSTRUCTION COUNT IS EXACT (569 = 569),
 * so 48 is a true distance.  tools/aligncmp.py: 538 of 569 aligned-equal
 * (94.6%), 42 differing in 15 hunks.  The relocation SYMBOL SEQUENCE is
 * IDENTICAL (93 entries) and exactly ONE relocation offset differs, by 2 bytes
 * -- every part of "RELOCATIONS differ" is downstream of blocker 1.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_a1000/80a9f10.c \
 *     asm/rom_a1000/rom_a8604_c_c_c_c_a_a.s --func Func_80a9f10
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py src/non_matching/rom_a1000/80a9f10.c \
 *     asm/rom_a1000/rom_a8604_c_c_c_c_a_a.s Func_80a9f10 -v
 *
 * THE SPLIT.  NONE NEEDED.  asm/rom_a1000/rom_a8604_c_c_c_c_a_a.s holds this one
 * function and no .section .data; tools/datacheck.py is silent.  No `.global`
 * and no shim (tools/shimcount.py reports none).
 *
 * SIGNATURE READ OFF THE CALLER.  src/non_matching/rom_a1000/80a5cc0.c carries
 * `extern int Func_80a9f10(int a, int b, int c, int d);`.  The frame confirms the
 * order: 0x14 of pure spill slots with no outgoing-arg word (no call here takes
 * more than three arguments), and the parameters land highest-first by
 * declaration -- arg0 at [sp,#0x10], arg1 at [sp,#0xc], arg3 at [sp,#8] (arg2
 * stays in r10) -- then the two locals `st` at [sp,#4] and `i` at [sp,#0].
 *
 * WHAT CLOSED 538 OF 569, in the order it paid:
 *
 * 1. THE `target == 9` GUARD IS TWO SEPARATE CALLS, NOT A TERNARY.  This was the
 *    single biggest step, 468 -> 538 and the last two instructions of the count:
 *
 *        if (target != 9) unit = _GetUnit(target);
 *        else             unit = _GetUnit(0);
 *
 *    Cross-jumping merges the two `bl _GetUnit / mov r5,r0` tails, leaving the
 *    ROM's `mov r0,r10 / b / mov r0,#0` pair of arms -- and because r0 is then
 *    written inside the arms, the compare needs its own copy of r10, which is the
 *    ROM's otherwise unexplained `mov r2, sl`.  A ternary
 *    (`_GetUnit(target != 9 ? target : 0)`) lets gcc use the compare's own copy
 *    in r0 as the argument and comes out TWO instructions shorter.
 *
 * 2. THE STAT FIELDS AT 0x34/0x36/0x38/0x3a ARE u16 READ WITH EXPLICIT `(short)`
 *    CASTS.  The reference reads each offset BOTH ways -- `ldrsh` for every
 *    comparison and `ldrh` for the arithmetic that is stored back -- so they are
 *    two distinct expressions, not one cse'd load.  Declaring them `short` gives
 *    `ldrsh` for the arithmetic too and loses the `ldrh` of each pair.  Worth
 *    342 -> 405 together with lever 3.  (The register-offset `ldrsh rN,[r5,r4]`
 *    form is NOT evidence of anything: Thumb has no immediate-offset LDRSH.)
 *
 * 3. THE FIRST SWITCH NEEDS ITS EMPTY CASES SPELLED OUT.  The reference dispatches
 *    `info->f1 & 0xf` through an 11-word jump table, but only 1, 9 and 11 have
 *    bodies; three case labels are below CASE_VALUES_THRESHOLD and gcc emits a
 *    compare chain instead.  Writing `case 2: ... case 8: case 10: break;` is
 *    what produces the table.
 *
 * 4. THE SECOND SWITCH'S ARM BODIES ARE IN SOURCE ORDER 1, 2, 5, 56, 57, 3 --
 *    read off the BODY addresses, not the table: .Laa270(1) < .Laa2ce(2) <
 *    .Laa314(5) < .Laa33e(56) < .Laa358(57) < .Laa38a(3).  case 3 is LAST.
 *
 * 5. CASES 5, 56 AND 57 TEST `== 0`, NOT `!= 0`.  `bne` to the `reason = 0xd`
 *    block means gcc's jumpifnot took the FALSE edge there, so the condition is
 *    `(short)unit->f38 == 0` with the work in the then-arm.  Writing `!= 0` with
 *    the work in the else-arm cost 10 encodings and 18 hunks (447 -> 457 when
 *    fixed), and it is also what stops case 5's tail being cross-jumped with
 *    56/57's.
 *
 * 6. `case 1:`'s HUNDRED IS IN THE ELSE BRANCH: `if (info->f2 != 4) v = ...;
 *    else v = 0x64;`.  With the constant in the THEN branch gcc materialises it
 *    above the test and retargets the branch past the load, one instruction
 *    shorter than the ROM.
 *
 * 7. THE RANDOM ROLL AND ITS RESULT ARE ONE VARIABLE.  `d = (Random() << 2) >> 16;
 *    if (d == 0) d = -1; else d = (d == 1);` -- the ROM's r0 holds the roll, is
 *    consumed by the `eor`, and receives the result.  Two variables let gcc
 *    materialise the -1 above the compare (it cannot when the same register is
 *    still needed), 457 -> 468, and it also fixes five `adds r2, r6, r0` sites.
 *    Random() must be declared UNSIGNED: the ROM's `lsr r0,#16` is a logical
 *    shift, and `int Random(void)` gives `asr`.
 *
 * 8. THE `(x == 1)` IDIOM IS gcc's OWN.  `mov r1,#1 / mov r2,r1 / eor r2,r0 /
 *    neg r3,r2 / orr r3,r2 / lsr r0,r3,#31 / sub r0,r1,r0` is just how gcc-2.96
 *    expands `x == 1` to a 0/1 value in Thumb; nothing had to be written for it.
 *
 * 9. `i` IS A u8 AND THE BOUND IS A u8 FIELD.  `bcc`/`bcs` (unsigned) rather than
 *    `blt`/`bge` comes from fold narrowing `(int)u8 < (int)u8` back to the
 *    unsigned type; `i` spills as a WORD at [sp,#0] with `lsl #24 / lsr #24` on
 *    every increment, which is PROMOTE_MODE holding the QImode pseudo in SImode.
 *
 * ============================================================
 * THE BLOCKERS.  Three, and the first is the root of the relocation difference.
 *
 * BLOCKER 1, ~10 encodings and the one relocation offset: gcse PRE INSERTS THE
 * HOISTED `info->f8` INTO A SPLIT CRITICAL-EDGE BLOCK, THE ROM'S INTO THE END OF
 * THE PREHEADER.  Pass: gcse.c's pre_edge_insert / commit_edge_insertions.
 *
 *     rom   mov r3,fp / movs r4,#0 / ldr r0,[sp,#4] / ldr r1,=0x219 /
 *           ldrb r2,[r3,#8] / str r4,[sp] / adds r3,r0,r1 / ldrb r3,[r3] /
 *           cmp r4,r3 / bcc <loop top> / b <exit>
 *     ours  mov r3,#0 / ldr r4,[sp,#4] / ldr r0,=0x219 / str r3,[sp] /
 *           adds r3,r4,r0 / ldrb r3,[r3] / mov r1,#0 / cmp r1,r3 /
 *           bcc .LCB44 / b <exit> / .LCB44: mov r3,fp / ldrb r2,[r3,#8]
 *
 * BOTH versions PRE the loop-top read of `info->f8` -- the loop bottom's
 * `ldrb r3,[r2,#8] / mov r2,r3` pair matches exactly in both -- but ours lands
 * the preheader copy in gcc's own edge-split block `.LCB44`, which is what
 * `commit_edge_insertions` does when the pred has two successors AND the succ has
 * two predecessors.  The ROM's copy sits at the END of the preheader, which that
 * routine only does for a pred with ONE successor.  The knock-on is the whole
 * hunk: with r3/r2 busy on the f8 load the ROM keeps the loop-counter zero in r4
 * across the `ldrb`, while ours has to materialise a SECOND zero (`mov r1,#0`)
 * for the compare -- +1 instruction here, -1 somewhere else, and the 2-byte shift
 * that moves one `_GetUnit` relocation.  Measured inert: naming the value in an
 * `unsigned char all;` refreshed at the loop bottom (563 instructions, 6 SHORT --
 * it defeats the PRE entirely); spelling the loop as `while` with an explicit
 * `i++` (byte-identical to the `for`).
 *
 * BLOCKER 2, 3 encodings: `amount = info->fa` IS SCHEDULED BEFORE THE SWITCH
 * INDEX INSTEAD OF AFTER IT.  Pass: sched1.
 *
 *     rom   mov r1,fp / ldrb r2,[r1,#1] / mov r3,#0xf / and r3,r2 / sub r3,#1 /
 *           ldrh r6,[r1,#0xa] / cmp r3,#0xa
 *     ours  mov r2,fp / ldrh r6,[r2,#0xa] / ldrb r2,[r2,#1] / ... / cmp r?,#0xa
 *
 * Measured inert: naming the switch index in a local so the `and`/`sub` are
 * written first (571 instructions, WORSE); moving the assignment into case 1.
 *
 * BLOCKER 3, ~8 encodings, LOW-REGISTER TIES.  Pass: local-alloc/reload.
 *   - case 11's four eager loads: the ROM takes `ldrsh r3`(0x3a) / `ldrh r1`(0x3a)
 *     and then `adds r3, r1, r6`; ours takes `ldrsh r1` / `ldrh r3` and the
 *     2-operand `adds r3, r3, r6`.  Its own case 1 uses OUR roles, so this is a
 *     tie broken by the missing `cmp` between the two ldrsh's, not a shape.
 *     Swapping the source order of the two reads is WORSE (529 aligned).
 *   - `lsr r0,r3,#31 / sub r0,r1,r0` against our `lsr r3,r3,#31 / sub r0,r1,r3`.
 *   - `ldr r2,[sp,#8]` against `ldr r3,[sp,#8]` for the `apply` test.
 *   - the `st->party[i]` index block reaches for r2/r4/r0 where we use r4/r0/r1.
 *
 * STRUCT NOTES.  info (_GetMoveInfo): +1 low nibble = the effect kind, +2 = the
 * stat index for the scaling call (4 means use 100), +3 = the status/secondary
 * switch, +8 = 0xff means "the whole party", +0xa u16 = the magnitude.  unit
 * (_GetUnit): 0x10/0x12/0x18/0x1a/0x1c u16 stats and 0x1e a u8, 0x34/0x38 the
 * HP max/current pair and 0x36/0x3a the PP pair (all u16), 0x48 + 4*i a u16 stat
 * table read signed, 0x131 an s8 status byte.  st (iwram_3001f2c): 0x208 a u16[8]
 * party table, 0x219 u8 the party size, 0x25a a halfword reason code -- 0x218 and
 * 0x25a agree with src/non_matching/rom_a1000/80a5cc0.c's view of the same block.
  *
 * *** BATCH-305 CORRECTION: THIS FILE ATTRIBUTES A RESIDUE TO sched1, AND sched1 DOES NOT
 * *** RUN IN THIS BUILD.  Verified with -da at production flags: the dump sequence is
 * *** 17.lreg 18.greg 19.flow2 20.ce2 23.sched2 25.jump2 26.mach -- there is NO sched1 dump,
 * *** because flag_schedule_insns is off at -O2 here, so only the post-reload scheduler runs.
 * *** Re-attribute to sched2 (rank_for_schedule), to combine, or to the ALLOCATION that fixed
 * *** the order.  Relatedly, any "-fno-schedule-insns is inert" note below rules nothing out:
 * *** that flag controls a pass that never runs.  The sched2 tie-break is priority ->
 * *** dependent count (more wins) -> INSN_LUID (lower wins), and LUID preserves EXPAND order.
 * *** See "sched1 DOES NOT RUN IN THIS BUILD" in docs/elevation.md.
*/
struct Pair {
    short a;
    short b;
};

struct Unit {
    unsigned char pad00[0x10];
    unsigned short f10;
    unsigned short f12;
    unsigned char pad14[4];
    unsigned short f18;
    unsigned short f1a;
    unsigned short f1c;
    unsigned char f1e;
    unsigned char pad1f[0x34 - 0x1f];
    unsigned short f34;
    unsigned short f36;
    unsigned short f38;
    unsigned short f3a;
    unsigned char pad3c[0x48 - 0x3c];
    struct Pair f48[0x3a];
    unsigned char f130;
    signed char f131;
};

struct Move {
    unsigned char f0;
    unsigned char f1;
    unsigned char f2;
    unsigned char f3;
    unsigned char pad4[4];
    unsigned char f8;
    unsigned char f9;
    unsigned short fa;
};

struct State {
    unsigned char pad0[0x208];
    unsigned short party[8];
    unsigned char f218;
    unsigned char count;
    unsigned char pad21a[0x25a - 0x21a];
    short reason;
};

extern struct State *iwram_3001f2c;
extern struct Move *_GetMoveInfo(int id);
extern struct Unit *_GetUnit(int id);
extern unsigned int Random(void);
extern int _Func_8079c5c(int a, int b, int c);
extern void _UpdateStatBarPercent(int id);
extern void _Func_8019908(int a, int b);
extern void _CalcStats(int id);

int Func_80a9f10(int id, int chr, int target, int apply)
{
    struct Move *info;
    struct State *st;
    unsigned char i;
    struct Unit *unit;
    int amount;
    int changed;
    int quiet;
    int reason;
    int cur;
    int max;
    int n;
    int d;
    int v;
    int roll;

    info = _GetMoveInfo(id);
    st = iwram_3001f2c;
    changed = 0;
    reason = 0;
    quiet = 0;
    if (target != 9)
        unit = _GetUnit(target);
    else
        unit = _GetUnit(0);
    for (i = 0; i < st->count; i++) {
        if (info->f8 == 0xff) {
            target = st->party[i];
            unit = _GetUnit(target);
        }
        amount = info->fa;
        switch (info->f1 & 0xf) {
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 10:
            break;
        case 1:
            if (apply == 0) {
                if (info->f2 != 4)
                    v = _GetUnit(chr)->f48[info->f2].a;
                else
                    v = 0x64;
                amount = _Func_8079c5c(amount, v, 0x100);
            }
            cur = (short)unit->f38;
            if (cur <= 0) {
                if (quiet == 0)
                    reason = 2;
                break;
            }
            max = (short)unit->f34;
            if (cur == max) {
                if (quiet == 0)
                    reason = 4;
                break;
            }
            unit->f38 = unit->f38 + amount;
            if ((short)unit->f38 > max) {
                amount -= (short)unit->f38 - max;
                unit->f38 = unit->f34;
                if (quiet == 0)
                    reason = 0;
            } else {
                if (quiet == 0)
                    reason = 1;
            }
            _UpdateStatBarPercent(target);
            changed = 1;
            if (info->f8 == 0xff) {
                quiet = 1;
                reason = 3;
            }
            break;
        case 9:
            d = (Random() << 2) >> 16;
            if (d == 0)
                d = -1;
            else
                d = (d == 1);
            switch (id & 0x3fff) {
            case 0x104:
                unit->f10 = unit->f10 + (amount + d);
                reason = 0x10;
                changed = 1;
                break;
            case 0x105:
                unit->f12 = unit->f12 + (amount + d);
                reason = 0x11;
                changed = 1;
                break;
            case 0x108:
                unit->f1c = unit->f1c + (amount + d);
                reason = 0x12;
                changed = 1;
                break;
            case 0x109:
                unit->f1e = unit->f1e + amount;
                reason = 0x13;
                changed = 1;
                break;
            case 0x106:
                unit->f18 = unit->f18 + (amount + d);
                _Func_8019908(3, 5);
                reason = 0x14;
                changed = 1;
                break;
            case 0x107:
                unit->f1a = unit->f1a + (amount + d);
                _Func_8019908(4, 5);
                reason = 0x15;
                changed = 1;
                break;
            }
            break;
        case 11:
            cur = (short)unit->f3a;
            max = (short)unit->f36;
            if (cur == max) {
                if (quiet == 0)
                    reason = 7;
                break;
            }
            unit->f3a = unit->f3a + amount;
            if ((short)unit->f3a > max) {
                amount -= (short)unit->f3a - max;
                unit->f3a = unit->f36;
                if (quiet == 0)
                    reason = 5;
            } else {
                if (quiet == 0)
                    reason = 6;
            }
            _UpdateStatBarPercent(target);
            changed = 1;
            if (info->f8 == 0xff) {
                quiet = 1;
                reason = 8;
            }
            break;
        }
        switch (info->f3) {
        case 1:
            if ((short)unit->f38 <= 0 || (short)unit->f38 == (short)unit->f34) {
                if (quiet == 0)
                    reason = 2;
                break;
            }
            unit->f38 = unit->f38 + amount;
            if ((short)unit->f38 > (short)unit->f34) {
                unit->f38 = unit->f34;
                if (quiet == 0)
                    reason = 0;
            } else {
                if (quiet == 0)
                    reason = 1;
            }
            _UpdateStatBarPercent(target);
            changed = 1;
            break;
        case 2:
            if ((short)unit->f3a == (short)unit->f36) {
                if (quiet == 0)
                    reason = 7;
                break;
            }
            unit->f3a = unit->f3a + amount;
            if ((short)unit->f3a > (short)unit->f36) {
                unit->f3a = unit->f36;
                if (quiet == 0)
                    reason = 5;
            } else {
                if (quiet == 0)
                    reason = 6;
            }
            _UpdateStatBarPercent(target);
            changed = 1;
            break;
        case 5:
            if ((short)unit->f38 == 0) {
                unit->f38 = unit->f34;
                _UpdateStatBarPercent(target);
                changed = 1;
                if (quiet == 0)
                    reason = 0xc;
            } else {
                if (quiet == 0)
                    reason = 0xd;
            }
            break;
        case 56:
            if ((short)unit->f38 == 0) {
                unit->f38 = (short)unit->f34 / 2;
                _UpdateStatBarPercent(target);
                if (quiet == 0)
                    reason = 0xc;
            } else {
                if (quiet == 0)
                    reason = 0xd;
            }
            break;
        case 57:
            if ((short)unit->f38 == 0) {
                unit->f38 = (short)unit->f34 * 7 / 10;
                _UpdateStatBarPercent(target);
                if (quiet == 0)
                    reason = 0xc;
            } else {
                if (quiet == 0)
                    reason = 0xd;
            }
            break;

        case 3:
            if (unit->f131 != 0) {
                unit->f131 = 0;
                changed = 1;
                if (quiet == 0)
                    reason = 0xa;
            } else {
                if (quiet == 0)
                    reason = 0xb;
            }
            break;
        }
        if (info->f8 != 0xff)
            break;
    }
    if (changed == 0) {
        st->reason = reason;
        return -1;
    }
    for (i = 0; i < st->count; i++)
        _CalcStats(st->party[i]);
    st->reason = reason;
    return 0;
}
