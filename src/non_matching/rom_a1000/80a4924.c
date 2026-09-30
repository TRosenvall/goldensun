/* Func_80a4924 -- DrawAbilityDescription, 0x080a4924, 457 ROM instructions.
 * PARKED.
 * NON-MATCHING, 132 of 505 encodings differ.
 *
 * SIZE IS EXACT (1168 = 1168) AND THE INSTRUCTION COUNT IS EXACT (505 = 505),
 * so 132 is a true distance.  tools/aligncmp.py: 447 of 505 aligned-equal
 * (88.5%), 68 differing in 49 hunks, and 30 of those hunks are pool/branch
 * offsets behind the defects listed below.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_a1000/80a4924.c \
 *     asm/rom_a1000/rom_a47b4_a_c_a_c.s --func Func_80a4924
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py src/non_matching/rom_a1000/80a4924.c \
 *     asm/rom_a1000/rom_a47b4_a_c_a_c.s Func_80a4924 -v
 *
 * THE SPLIT.  NONE NEEDED.  asm/rom_a1000/rom_a47b4_a_c_a_c.s holds this one
 * function and no .section .data; tools/datacheck.py is silent.  Its two
 * neighbours in the family are already elevated (rom_a47b4_a_c_a_b.c =
 * Func_80a4800, rom_a47b4_a_c_b.c = Func_80a4db4), and .Laf21c/.Laf220 are
 * already `.global` in asm/rom_a1000/rom_a47b4_c_c_c_c_c_c.s, so the two
 * __asm__ label externs need no new export.
 *
 * SHIMS: ZERO (tools/shimcount.py reports none).
 *
 * SIGNATURE READ OFF THE CALLER.  src/rom_a1000/rom_a4800... (rom_a47b4_a_c_a_b.c)
 * declares `extern void Func_80a4924(int box, int id);` and calls
 * Func_80a4924(box, id), so it is (int, int) returning void.  Func_80a4db4's
 * landed signature in rom_a47b4_a_c_b.c fixes the 5-argument call shape.
 *
 * WHAT CLOSED 447 OF 505, in the order it paid:
 *
 * 1. `line` IS AN s8 (signed char) HELD ZERO-EXTENDED.  Every read is
 *    `lsl #24 / asr #24` and every write back is `lsl #24 / lsr #24`; arm.h's
 *    PROMOTE_MODE forces UNSIGNEDP, so a signed char pseudo lives zero-extended
 *    and each use pays the sign-extension.  `line * 8` then falls out as
 *    `asr rN, r6, #21` off the same `lsl r6, line, #24`, and the post-switch
 *    `line++` reuses that r6 as `add r3, r6, #1<<24 / lsr #24`.  An int `line`
 *    reproduces none of this.
 *
 * 2. THE SLOT SWITCH HAS A `case 0:`.  The ROM's jump table is 28 words and the
 *    dispatch does NOT subtract a minimum, so minval is 0 -- yet the loop
 *    already skips kind 0 with `cmp r3,#0 / bne / b <increment>` before the
 *    magnitude is even loaded.  Writing `case 0: case 0x18: break;` explicitly
 *    alongside the `if (kind == 0) continue;` is what makes the table 28 entries
 *    instead of 27.  Worth 466 -> 390 on objcmp's first screen.
 *
 * 3. THE ARM BODIES ARE IN SOURCE ORDER 1-6/0x1a, 0xf-0x16, 7-0xe,
 *    0x17/0x19/0x1b -- NOT numeric.  Read off the BODY order in the reference:
 *    .La4a94, .La4aaa, (.La4af0 = the cross-jumped shared tail of those two),
 *    .La4b06, .La4b7a.  Cross-jumping keeps the LATER copy, so arm A preceding
 *    arm C is forced by A's `b .La4af0`.
 *
 * 4. ONE LOCAL (`mag`) SPANS ALL THREE MAGNITUDE SITES -- info->f8, info->fa and
 *    the slot's signed byte.  That single allocno takes r7 and forces the ROM's
 *    otherwise-pointless `mov r0, r7` before each Func_80a4db4 call, and it
 *    pushes `win` out to r8, which is what makes the prologue exact.  Three
 *    separate temps let each value go straight to r0 and put win in r4 (r4 is
 *    call-clobbered here, -fcall-used-r4), changing the register save mask.
 *
 * 5. `i` IS ONE VARIABLE SHARED BY BOTH LOOPS.  A second index for the slot
 *    loop measures 401/505 against 440; the shared one is required.
 *
 * 6. `i = 0;` MUST BE WRITTEN ADJACENT TO `any = 0;`, ABOVE THE `if`, AND THE
 *    LOOP SPELLED `for (; i < 4; i++)`.  The ROM materialises the 0 once
 *    (`mov r1,#0` for the flag) and reaches `i` with `mov r9, r1`; with
 *    `for (i = 0; ...)` gcc emits a SECOND `mov r0,#0`, which flips the pool
 *    alignment parity and costs a `.short 0` pad as well.  Worth 408 -> 445.
 *    (`for (i = any; ...)` -- semantically identical there -- does NOT do it.)
 *
 * 7. THE TWO STRING-PAIR SITES INCREMENT THE BASE IN PLACE.  `base = 0xb73;
 *    call(base); line++; base++; call(base);` gives the ROM's `add r6,#1`;
 *    `call(base + 1)` gives one instruction fewer (`adds r0, r6, #1`).
 *
 * 8. THE 24-BIT EMPTINESS TEST IS A WORD LOAD AT +8.  `ldr r3,[r0,#8] /
 *    and r3, 0xffffff` overlaps the s16 at +8 and the s8 at +0xa that the very
 *    next blocks read with ldrsh/ldrsb, so it is spelled `((int *)info)[2]`.
 *    There are no stores to the record, so no aliasing flag is needed.
 *    (The ldrsh/ldrsb register-offset forms are NOT evidence of anything --
 *    Thumb has no immediate-offset LDRSH/LDRSB at all.)
 *
 * 9. `(id & 0xf800) / 0x800` IS A SIGNED DIVISION, spelled as a division.  The
 *    ROM emits the full `cmp/bge/add 0x7ff/asr #11` correction even though the
 *    masked value is provably non-negative; gcc-2.96's expand_divmod does not
 *    consult nonzero_bits.  Reusing `mag` for the quotient (rather than a fresh
 *    local) is worth 2 encodings -- it is the same r7 allocno as lever 4.
 *
 * ============================================================
 * THE BLOCKERS.  Three, all downstream of one mechanism.
 *
 * BLOCKER 1, THE ROOT CAUSE, 2 encodings: THE CONSTANT 1 THAT RELOAD
 * MATERIALISES FOR `i++` IS INHERITED BY THE POST-LOOP `info->f3 & 1` MASK IN
 * THE ROM AND NOT IN OURS.  Pass: reload's find_equiv_reg / inheritance.
 *
 *     rom   .La4b9e: mov r3,#1 / add r9,r3 / mov r0,r9 / cmp r0,#3 / bgt / b
 *           .La4baa: mov r1,r10 / ldrb r2,[r1,#3] / and r3,r2
 *     ours           movs r1,#1 / add r9,r1 / mov r2,r9 / cmp r2,#3 / bgt / b
 *                    mov r3,sl  / ldrb r2,[r3,#3] / movs r3,#1 / ands r3,r2
 *
 * `i` is allocated to r9, so Thumb cannot `add r9,#1` and RELOAD, not expand,
 * materialises the 1.  The mask, by contrast, is a real pseudo: Thumb's andsi3
 * takes register operands only.  In the ROM the mask's reload inherits the
 * still-live r3; in ours it does not, and the ROM's r3 being busy is also why
 * the ROM copies iwram info into r1 where we use r3.  Everything downstream --
 * the `mov r0,r9` vs `mov r2,r9` role swap and a `.short 0` pool pad -- follows
 * from this one instruction.  Measured inert: `int one = 1;` used for both the
 * increment and the mask (439/507, WORSE); `1 & info->f3`; `(info->f3 & 1) == 1`;
 * loading f3 into a temp first; `i <= 3` instead of `i < 4`; the slot loop as a
 * `while` with two `i++`s (408/509, much worse).
 *
 * BLOCKER 2, 2 encodings, AND THE PRICE OF LEVER 2: THE `info->fc` SWITCH'S
 * LOW-BOUND TEST.  Pass: stmt.c's emit_case_nodes / node_has_low_bound.
 *
 *     rom   cmp r3,#1 / beq case1 / cmp r3,#1 / ble default / cmp r3,#2 / beq
 *     ours  cmp r3,#1 / beq case1 /                           cmp r3,#2 / beq
 *
 * With only `case 1:` and `case 2:` present, node 1's right child is a leaf and
 * emit_case_nodes takes the "cannot process node->right normally" shortcut,
 * dropping the `< low` branch.  ADDING AN EMPTY `case 0:` REPRODUCES THE ROM
 * EXACTLY -- balance_case_nodes then makes case 1 the root with case 0 as its
 * left child, and the both-children path emits `bgt <right>` which jump.c
 * inverts into the ROM's `ble default`.  That is measured and confirmed in
 * scratch_elev/b301i/a4924_x1.c, where the whole hunk aligns.
 *
 * BUT it costs +2 instructions, and until blocker 1 is solved those +2 are the
 * only thing cancelling blocker 1's +1 and its pad's +1.  So there are two
 * spellings of the same program:
 *     a4924_w2.c  (THIS FILE)  505/505, size 1168 exact, 132 of 505, 447 aligned
 *     a4924_x1.c  (+ case 0:)  507/505, size 1172,       261 of 507, 440 aligned
 * x1 is the shape the ROM has; w2 is two-short-and-two-long and therefore scores
 * better on every figure.  SOLVE BLOCKER 1 AND THEN ADD `case 0:` TO THE fc
 * SWITCH -- that is the whole remaining gap.
 *
 * BLOCKER 3, 8 encodings, 4 sites: THE 0xb3b POOL LOAD IS SCHEDULED ONE SLOT
 * EARLY AND SO CANNOT REUSE r3.  Pass: sched1.
 *
 *     rom   ldrb r0,[r1,r3] / lsl r6,r5,#24 / ldr r3,=0xb3b / add r0,r3
 *     ours  ldr r2,=0xb3b   / ldrb r0,[r1,r3] / ... / adds r0,r0,r2
 *
 * r3 holds i*4+0x18 and dies at the ldrb; the ROM reuses it for the constant,
 * ours still has it live.  Measured inert: `info->slots[i].kind + 0xb3b`
 * (operand order); naming the reloaded kind in a per-arm local.
 *
 * ALSO INERT, RECORD SO NOBODY REPEATS THE SWEEP:
 *   - `i = 0;` inside the else branch rather than above the `if` (408/507).
 *   - naming the division quotient as its own local rather than reusing `mag`.
 *   - a third string-id base variable for the 0xb6f pair.  The ROM keeps 0xb6f
 *     in r9 and derives 0xb70 as `mov r0,r9 / add r0,#1`; every spelling tried
 *     lets cse fold it to a fresh shiftable constant `mov r0,#0xb7 / lsl #4`,
 *     because the def `base = 0xb6f` dominates the use inside one basic block.
 *     Same encoding count, so it is cosmetic, but it is unexplained.
 *
 * STRUCT.  The _GetItemInfo record as this function sees it: +0x02 u8 (non-zero
 * = has an effect record at all), +0x03 u8 flag byte (bit 0 -> string 0xb76,
 * bit 2 -> 0xb69, bit 3 -> 0xb6a, bit 4 -> the 0xb6f/0xb70 pair), +0x08 s16 and
 * +0x0a s8 (the two labelled quantities under 0xaf7/0xaf8), the word at +8 read
 * whole and masked 0xffffff as the "any quantity at all" test, +0x0c u8 target
 * kind (0/4 nothing, 1 -> 0xb63, 2 -> 0xb71..0xb74 by bit 10 of the caller's
 * flags, 3 -> 0xb65), and +0x18 four 4-byte slots whose byte 0 is the effect
 * kind and byte 1 the signed magnitude.
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
extern unsigned char Laf21c[] __asm__(".Laf21c");
extern unsigned char Laf220[] __asm__(".Laf220");

struct Slot {
    unsigned char kind;
    signed char mag;
    unsigned char pad2[2];
};

struct Info {
    unsigned char pad0[2];
    unsigned char f2;
    unsigned char f3;
    unsigned char pad4[4];
    short f8;
    signed char fa;
    unsigned char fb;
    unsigned char fc;
    unsigned char padd[0xb];
    struct Slot slots[4];
};

extern struct Info *_GetItemInfo(int id);
extern void _Func_801e7c0(int id, int win, int x, int y);
extern void _Func_801ea08(int v, int n, int win, int x, int y);
extern void _UIDrawText(unsigned char *s, int win, int x, int y);
extern void _Func_8019000(int win, int a, int b, int c, int d);
extern void _Func_8019908(int a, int b);
extern void Func_80a4db4(int val, int a, int win, int x, int y);

void Func_80a4924(int win, int id)
{
    struct Info *info;
    int done;
    signed char line;
    int i;
    int any;
    int mag;
    int base;
    int seen;
    int n;

    line = 0;
    done = 0;
    info = _GetItemInfo(id & 0x1ff);
    if (info->f2 != 0) {
        any = 0;
        i = 0;
        if ((((int *)info)[2] & 0xffffff) != 0)
            any = 1;
        else {
            for (; i < 4; i++) {
                if (info->slots[i].kind != 0 || info->fc == 3) {
                    any = 1;
                    break;
                }
            }
        }
        if (any == 1) {
            _Func_801e7c0(0xb6d, win, 0x10, 0);
            line = 1;
        }
        if (info->f8 != 0) {
            _Func_801e7c0(0xaf7, win, 0, line * 8);
            mag = info->f8;
            Func_80a4db4(mag, 3, win, 0x40, line * 8);
            line++;
        }
        if (info->fa != 0) {
            _Func_801e7c0(0xaf8, win, 0, line * 8);
            mag = info->fa;
            Func_80a4db4(mag, 3, win, 0x40, line * 8);
            line++;
        }
    }
    for (i = 0; i < 4; i++) {
        if (info->slots[i].kind == 0)
            continue;
        mag = info->slots[i].mag;
        switch (info->slots[i].kind) {
        case 0:
        case 0x18:
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 0x1a:
            _Func_801e7c0(0xb3b + info->slots[i].kind, win, 0, line * 8);
            Func_80a4db4(mag, 3, win, 0x40, line * 8);
            break;
        case 0xf:
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
        case 0x14:
        case 0x15:
        case 0x16:
            _Func_8019000(win, (unsigned char)((info->slots[i].kind - 0xf) % 4) + 1, 0, line, 2);
            _Func_801e7c0(0xb3b + info->slots[i].kind, win, 8, line * 8);
            Func_80a4db4(mag, 3, win, 0x40, line * 8);
            break;
        case 7:
        case 8:
        case 9:
        case 0xa:
        case 0xb:
        case 0xc:
        case 0xd:
        case 0xe:
            _Func_801e7c0(0xb3b + info->slots[i].kind, win, 0, line * 8);
            _UIDrawText(Laf21c, win, 0x40, line * 8);
            if (mag > 9) {
                _Func_801ea08(1, 1, win, 0x48, line * 8);
                _UIDrawText(Laf220, win, 0x50, line * 8);
                _Func_801ea08(mag - 0xa, 1, win, 0x58, line * 8);
            } else {
                _Func_801ea08(0, 1, win, 0x48, line * 8);
                _UIDrawText(Laf220, win, 0x50, line * 8);
                _Func_801ea08(mag, 1, win, 0x58, line * 8);
            }
            break;
        case 0x17:
        case 0x19:
        case 0x1b:
            _Func_801e7c0(0xb3b + info->slots[i].kind, win, 0, line * 8);
            break;
        }
        line++;
    }
    if (info->f3 & 1) {
        _Func_801e7c0(0xb76, win, 0, line * 8);
        line++;
    }
    if (info->fc == 3) {
        _Func_801e7c0(0xb65, win, 0, line * 8);
        done = 1;
        line++;
    }
    if (info->fc != 4 && info->fc != 0) {
        if (done == 0) {
            _Func_801e7c0(0xb6e, win, 0x10, line * 8);
            line++;
        }
        switch (info->fc) {
        case 1:
            _Func_801e7c0(0xb63, win, 0, line * 8);
            line++;
            break;
        case 2:
            if (id & 0x400) {
                base = 0xb73;
                _Func_801e7c0(base, win, 0, line * 8);
                line++;
                base++;
                _Func_801e7c0(base, win, 0, line * 8);
                line++;
            } else {
                base = 0xb71;
                _Func_801e7c0(base, win, 0, line * 8);
                line++;
                base++;
                _Func_801e7c0(base, win, 0, line * 8);
                line++;
            }
            break;
        }
    }
    if (info->f3 & 0x10) {
        if (line != 0)
            line++;
        base = 0xb6f;
        _Func_801e7c0(base, win, 0x10, line * 8);
        line++;
        mag = (id & 0xf800) / 0x800;
        _Func_8019908(mag + 1, 5);
        _Func_801e7c0(base + 1, win, 0, line * 8);
        line++;
    }
    seen = 0;
    if (line == 0) {
        if (info->f3 & 4) {
            _Func_801e7c0(0xb69, win, 0, 0);
            seen = 1;
        }
        if (seen == 0 && (info->f3 & 8)) {
            _Func_801e7c0(0xb6a, win, 0, 0);
            seen = 1;
        }
        if (seen == 0)
            _Func_801e7c0(0xb6c, win, 0, 0);
    }
}
