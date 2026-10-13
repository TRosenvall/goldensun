/* OvlFunc_956_200804c  --  0x0200804c -- NON-MATCHING.
 *
 * *** READ THE FLAG LINE BEFORE ANY FIGURE BELOW. ***
 * THIS BODY IS MEASURED AT ALIAS_CFLAGS (-fno-strict-aliasing).  Both figures:
 *
 *   -fno-strict-aliasing   ref 132 / ours 132 instructions, 316 / 316 BYTES,
 *                          145 / 145 stream entries -- ALL THREE LENGTHS EQUAL.
 *                          positional figure 23 of 145, aligned-equal 128 (88.3%)
 *   plain -O2              ref 132 / ours 128, 316 / 312 bytes -- gap -4,
 *                          figure unreadable
 *
 * THE BODY THIS REPLACES, for comparison, both at the same flags:
 *   -fno-strict-aliasing   145/145 entries, 316/316 bytes, figure 35 of 145,
 *                          aligned-equal 119 (82.1%)
 *   plain -O2 (the park's own measurement)  145 vs 147 entries, 316 vs 324
 *                          bytes, figure 81 of 145, aligned-equal 94 (64.8%)
 *
 * So: the LENGTH GAP IS CLOSED and the ALIGNED residue went 94 -> 128 of 145.
 * The old header's 81 of 145 was a PHASE count, not a distance.
 *
 * parkcheck RE-MEASURES AT PRODUCTION FLAGS AND WILL READ THIS PARK AS WORSE.
 * That is the batch-343 trap and it is expected here: at plain -O2 this body is
 * four instructions SHORT, because -fstrict-aliasing lets gcc reorder the
 * `short *dst` store against the `unsigned char *t` table read and the
 * `act->f8` load.  Do not "fix" the figure by reverting the body.
 *
 * Verify with (NOTE THE FLAG -- objcmp cannot be given one, so use b_dump.py's
 * recipe or add the ALIAS_CFLAGS rule first):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/ovl_7e0928/200804c.c \
 *       asm/overlays/rom_7e0928/ovl_30_a_c_c_a_a.s --func OvlFunc_956_200804c
 *
 * LANDING SHAPE: NO SPLIT.  asm/overlays/rom_7e0928/ovl_30_a_c_c_a_a.s is the
 * WHOLE file -- ONE function, no data.  overlays/rom_7e0928/overlay.ld:20 names
 * the object once.  MIND THE BANK: a file of the same name exists in
 * rom_7b4558 (overlays/rom_7b4558/overlay.ld:35); this park is the rom_7e0928
 * one.  PIN-FREE, DEVICE-FREE.
 * NO LABEL NEEDS `.global`: .L4c20, .L5480 and .L5484 are already exported by
 * asm/overlays/rom_7e0928/ovl_30_c_c_c_c_c_c.s (lines 8, 26/30, 31).
 *
 * ============ THE FOUR LEVERS, IN THE ORDER THEY PAID ============
 *
 * 0. -fno-strict-aliasing (ALIAS_CFLAGS, an established group -- Makefile:235).
 *    On the PARKED body, unchanged: figure 81 -> 35 and the instruction count
 *    goes +2 -> 0 with the byte size EXACT.  Found by tools/autosweep.py's flag
 *    axis in one command.  The park's own "FLAG SCREEN -- no flag row helps"
 *    listed only three rows and never tried this one.
 *
 * 1. *** THE PARK'S RANGE-TEST CONSTANT HAD AN EXTRA DIGIT. ***  Its lever 3
 *    derives the bounds correctly in prose -- "LOW = -0x31ffff and
 *    HIGH = -0x31ffff + 0x13fffe = -0x1e0001" -- and then writes
 *    `d > -0x3200000`, which is TEN TIMES TOO LARGE.  Our pool held
 *    `.word 0xfce00000` / `.word 0xffe20000` against the ROM's
 *    `.word 0x0031ffff` / `.word 0x0013fffe`, and gcc emitted TWO compares and
 *    TWO branches where the ROM has `add r3,r14 / cmp r3,r7 / bhi`.
 *
 *    AND THE CORRECTED CONSTANT IS NOT ENOUGH -- THE TEST MUST BE INCLUSIVE.
 *    Measured on four spellings (scratch_elev/b345/B/work/probe_range.c):
 *        d > -0x320000  && d < -0x1e0000   TWO compares, both bounds pooled
 *        d >= -0x31ffff && d <= -0x1e0001  add 0x31ffff / cmp 0x13fffe / bhi
 *        (unsigned)(d + 0x31ffff) <= 0x13fffe   identical to the line above
 *        d > -100 && d < 100               folds (so it is not strictness alone)
 *    `fold_range_test` (fold-const.c:3757-3820) reaches TRUTH_ANDIF_EXPR by
 *    fallthrough from the TRUTH_AND_EXPR case (fold-const.c:5961-5968 ->
 *    `truth_andor:` -> :6031), and `build_range_check` (:3573-3613) produces
 *    exactly the ROM's form once `low`/`high` are the inclusive bounds.  Write
 *    the bounds the way the ROM states them and it folds.
 *
 * 2. THE TABLE BYTE MUST GO THROUGH A `signed char` LOCAL.  The ROM reads it as
 *        ldrb r3,[r1] / lsl r3,#24 / asr r5,r3,#24      (3 instructions)
 *    and `signed char *t; v = *t++;` gives
 *        mov r5,#0 / ldrsb r5,[r1,r5]                   (2 instructions)
 *    -- which is the other half of the -1.  THE ROM'S FORM IS THE EXPANDER'S
 *    NATURAL OUTPUT: `extendqisi2` (arm.md:3449-3488) copies a MEM operand into
 *    a QImode register and then emits ashift 24 / ashiftrt 24 for TARGET_THUMB.
 *    combine then re-merges those three into `(sign_extend:SI (mem:QI))`,
 *    matching `*thumb_extendqisi2_insn` (arm.md:3543), whose Thumb `ldrsb`
 *    needs an INDEX REGISTER -- hence the `mov r5,#0`.  Seven spellings
 *    measured (probe_qi.c a1-a7); the merge is blocked by anything that makes
 *    the byte land in its own named QImode pseudo first:
 *        signed char *t; v = *t++;                 ldrsb   (the park)
 *        v = (signed char)*t++;  (t unsigned)      ldrsb
 *        v = *t++; v = (v<<24)>>24;                ldrb/lsl/asr, 3 regs
 *        signed char b; b = *t++; v = b;           ldrb/lsl/asr   <-- THIS ONE
 *        t[i] indexing with signed char *t         ldrb/lsl/asr, but -13 aligned
 *    The `signed char b` form is the ROM's register shape too: the byte temp and
 *    `v` are two distinct pseudos (the ROM's r3 and r5), where the in-place
 *    forms coalesce them.  Alone: aligned 120 -> 126 of 145.
 *
 * 3. TWO DEFINITION-ORDER MOVES, EACH WORTH 1 AND THEY COMPOSE.  126 -> 128.
 *      * `i = 0x12;` written BEFORE `j = 0x21;`, with the `for` left bare.  The
 *        ROM's preheader is `mov r2,#0xb / mov r6,#0x12 / mov r9,r3 /
 *        mov r10,r2 / mov r7,#0x21` -- the loop counter is defined BETWEEN the
 *        0xb and the 0x21, which LUID order (and so sched2's final tie-break)
 *        preserves from expand.  This is the batch-344 definition-order rule;
 *        the park had screened DECLARATION position three ways and correctly
 *        found all three inert, which is a different axis.
 *      * `t = ...` written before `dst = ...` in the else arm, so the .L5480
 *        read is the first thing in the block, as in the ROM.
 *
 * ============ WHAT IS LEFT: 23 of 145, IN FOUR PLACES ============
 *
 *   (a) ref[26:32], the then-arm preheader: a 3-way permutation plus an r2/r3
 *       role swap.  ROM `mov r9,r3` BEFORE `mov sl,r2`; ours the other way.
 *   (b) ref[38] `adds r3, r6, r3` against ours `adds r3, r3, r6` -- ONE
 *       encoding, Thumb ADD(3) operand order.  The park's lever 4 closed this
 *       by measuring three source orderings all identical (fold canonicalises
 *       the PLUS); I did not re-open it and I did not refute it.
 *   (c) ref[84:95], the `0xc1 << 1 + base -> ip` block: the ROM emits all four
 *       contiguously using r3 (the register that just died holding &.L5480);
 *       ours splits them around the table-pointer add and uses r2.
 *   (d) ref[111:112] / ref[117:118]: the two `mov rX, ip / strh r5,[rX]` pairs
 *       use r3-then-r2 in the ROM and r2-then-r3 in ours -- a pure swap worth
 *       4 encodings, almost certainly downstream of (c).
 *
 * THE PARK'S PASS ATTRIBUTION, carried forward and still correct as corrected:
 * the body of the header blamed sched1, and the batch-305 correction notes
 * sched1 DOES NOT RUN in this build (no .11/.12 sched dump at -O2; the sequence
 * is 17.lreg 18.greg 19.flow2 20.ce2 23.sched2 25.jump2 26.mach).  Everything
 * in (a), (c) and (d) is sched2 (`rank_for_schedule`) or the allocation that
 * fixed its input, and `-fno-schedule-insns` rules nothing out.
 *
 * AUTOSWEPT (batch 345, --max 60).  ANCHOR AXIS CLOSED: 27 of 27 boundaries
 * inert at 81 (4 produced no row -- untested, not inert).  FLAG AXIS: the only
 * real row is lever 0.  `-fno-strength-reduce` reads 74 and is a FALSE lead --
 * it moves the instruction gap to -3.  `-fno-gcse` is exactly inert at 81.
 *
 * WHAT THE FUNCTION IS.  132 instructions, a per-frame two-phase animation
 * driver over map slots 0x12..0x16, keyed on a frame counter (.L5484, unsigned,
 * wrapping at 0x11) and a 2-bit rotating index (.L5480).  On the counter's zero
 * frame it advances the index and re-animates the whole slot run from the
 * 6-byte-stride table .L4c20; on every other frame it walks the same table row
 * and stamps a value at base+0x182 when the actor is within a narrow depth band
 * and the level nibble and the table entry agree.  The two `if`s in that arm
 * are deliberately NOT an `else if`.
 */
struct Actor {
    unsigned char pad00[8];
    int f8;
    unsigned char padc[0x10 - 0xc];
    int f10;
};

extern unsigned char gState[];
extern unsigned char iwram_3001ebc[];
extern signed char L4c20[] __asm__(".L4c20");
extern int L5480 __asm__(".L5480");
extern unsigned int L5484 __asm__(".L5484");

extern struct Actor *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_956_200804c(void)
{
    struct Actor *act;
    unsigned char *base;
    unsigned char *t;
    signed char b;
    short *dst;
    int lvl;
    int i;
    int j;
    int v;
    int d;
    int k;
    int c;

    k = 0x1f4;
    base = *(unsigned char **)iwram_3001ebc;
    act = __MapActor_GetActor(*(int *)(gState + k));
    lvl = act->f10 >> 20;
    if (L5484 == 0) {
        L5480 = (L5480 + 1) & 3;
        c = 0xb;
        i = 0x12;
        j = 0x21;
        for (; i <= 0x16; i++) {
            v = L4c20[L5480 * 6 + (i - 0x12)];
            __MapActor_SetAnim(i, v);
            __MapActor_SetAnim(i + 5, v + 8);
            __Func_8010704(0x20, 0xb, 1, 2, j, c);
            if (v != 7)
                __Func_8010704(0x4a, 0xc, 1, 1, j, c);
            j += 2;
        }
        __MapActor_SetAnim(0x1c, L4c20[L5480 * 6 + 5]);
    } else {
        t = (unsigned char *)&L4c20[L5480 * 6];
        dst = (short *)(base + 0x182);
        for (i = 0x12; i <= 0x16; i++) {
            b = *t++;
            v = b;
            d = act->f8 - (i << 21);
            if (d >= -0x31ffff && d <= -0x1e0001) {
                if (lvl == 0xb && v == 4)
                    *dst = v;
                if (lvl == 0xc && v == 5)
                    *dst = v;
            }
        }
    }
    L5484++;
    if (L5484 > 0x11)
        L5484 = 0;
}
