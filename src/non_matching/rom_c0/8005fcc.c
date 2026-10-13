/* Func_8005fcc (0x08005fcc) -- NON-MATCHING, still PARKED.
 *
 *   25 of 87 encodings, AND THE LENGTH GAP IS CLOSED: instructions 79 == 79,
 *   bytes 188 == 188, pool words 7 == 7, RELOCATIONS CLEAN.  So this figure is
 *   a TRUE DISTANCE for the first time.  Was 74 of 87 at ref 79 / ours 80
 *   (gap +1), i.e. a phase figure.  ALIGNED residue 63 -> 65 of 87 (74.7%).
 *   Batch 345 brief E.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_c0/8005fcc.c \
 *     asm/rom_c0/rom_5cf8_a_a_a_c_c_a_c_a.s --func Func_8005fcc
 *
 * PINS: 0.  DEVICES: 0.  FAKEMATCH: no.
 *
 * THE PIECE.  asm/rom_c0/rom_5cf8_a_a_a_c_c_a_c_a.s holds TWO functions and both
 * are parked -- this one and Func_8006088.  THE OLD HEADER'S "NEITHER CONVERTS
 * ALONE" IS FALSE and the shared brief says so outright: tools/split_s.py cuts
 * before/target/after, so either member can be cut out alone.  Landing this one
 * does not wait on its mate.
 *
 * ===== PER-REGION SPLIT: THE OLD +1 WAS FOUR SEPARATE SITES, NOT ONE =====
 *
 * Counted block by block against the reference's own labels.  The old body's net
 * +1 was +1 / -1 / +2 / -1 across four regions -- the batch-344 rule exactly:
 *
 *   region                                  ref   OLD body   THIS body
 *   prologue + `p[1] == 0` gate               7      8 (+1)      7
 *   mask block (`t == 8`)                     5      4 (-1)      5
 *   `v`/`p[0x14]` guard                      12     12          12
 *   the IME/IE/SIOCNT/IF/TM3CNT block        23     25 (+2)     23
 *   `.L602a` p[1] = 1                         2      2           2
 *   `.L602e` p[0xb]++                         3      3           3
 *   `.L6034` first OR + pool dump            10      9 (-1)     10
 *   `.L6064` second OR                        7      7           7
 *   `.L6072` third OR                         7      7           7
 *   epilogue                                  3      3           3
 *   TOTAL                                    79     80          79
 *
 * The old +2/+1 were ONE cause: the serial pointer went to `ip`, so the
 * prologue paid `mov ip, r1` and the SIOCNT byte accesses each paid a
 * `mov rN, ip` because Thumb cannot use a high register as a load base.
 *
 * ===== THE THREE LEVERS, AND TWO OF THEM WERE IN THE PARK'S OWN NEGATIVES ====
 *
 * 1. `one` MUST BE A FUNCTION-SCOPE LOCAL ASSIGNED AT THE TOP, NOT BLOCK-SCOPED.
 *    The old header recorded "`one = 1` named and block-scoped was the best of
 *    its round".  It is the block scope that costs `ip`: a block-local quantity
 *    is taken by LOCAL-ALLOC before global alloc runs, so `one` got r4 and the
 *    serial pointer, a global allocno, found only high registers left.  Hoisting
 *    it to function scope makes it a global allocno that loses the race instead,
 *    and the pointer gets a low callee-saved register.  Worth 3 instructions
 *    (80 -> 77) on its own.  THE `int` IS LOAD-BEARING: `REG_IME = 1` with the
 *    literal pools the 1, because REG_IME is `vu16` and in HImode a const_int
 *    always pools (*thumb_movhi_insn, arm.md:4318-4320, `mn` at alternative ONE
 *    against the immediate `I` at FIVE) -- measured, pool 7 -> 8.
 * 2. THE MASK MUST BE A NAMED FUNCTION-SCOPE LOCAL TOO.  The old header recorded
 *    "naming all three of `one`, the mask and 0x40 was worse than naming one".
 *    Against the hoisted-`one` body naming the mask is worth +1 in the right
 *    direction (77 -> 78): it restores the ROM's `mov r3,#0x88 / mov r4,r5 /
 *    and r4,r3`, where the unnamed literal gives `mov r6,#136 / and r6,r6,r5`.
 *    Thumb's `andsi3` ties the destination to one operand (`%0`), and with the
 *    mask dying at the AND gcc ties the destination to the MASK's pseudo and
 *    needs no copy; a named mask with a longer range makes it tie to the copy of
 *    `s` instead, which is the ROM's shape.  BOTH NEGATIVES EXPIRED SILENTLY --
 *    they were measured against a body that no longer exists.
 * 3. THE RETURN ACCUMULATOR IS THREE VARIABLES, NOT ONE.  `r` -> `rb` -> `rc`,
 *    one per OR step, is what produces the ROM's `mov r0, r2` at `.L6064`: the
 *    ROM computes the first two steps in r2 and moves the value to r0 for the
 *    rest.  A single `r` gives r0 throughout and is one instruction SHORT.  A
 *    TWO-way split is exactly inert; "two is not N" again.
 *
 * Levers 1+2+3 are jointly the landing of the length gap: 1 alone -2, 1+2 -1,
 * 1+3 -1, 1+2+3 ZERO.  None of the three is sufficient and none is inert.
 *
 * Two further -1s, compositional with each other:
 *   * `r = p[3] | (p[2] << 8);` rather than `(p[2] << 8) | p[3]` -- the ROM
 *     emits `ldrb r3,[r7,#2] / ldrb r2,[r7,#3] / lsl r3,#8 / orr r2,r3`, i.e.
 *     both loads before the shift.
 *   * The high-half clear through an `int` temp (`h = ...[1]; h &= ~0x40;
 *     ...[1] = h;`) rather than `...[1] &= ~0x40`.  This is what gets the ROM's
 *     `sub r3, #0x42`: the wide `~0x40` needs two instructions on its own, so
 *     reload_cse_move2add rewrites it as a sub off the 1 that `one` left in r3,
 *     and the compound-assignment form narrows the mask to QImode 0xbf, where
 *     `mov r3,#191` is already one instruction and move2add has nothing to win.
 *   27 -> 26 each, 25 together.
 *
 * ===== WHAT IS LEFT: ONE REGISTER ROTATION AND ONE SCHEDULING SLOT =====
 *
 * 25 of 87, and roughly 22 of them are a single four-way rotation of the
 * long-lived values:
 *
 *     value          ROM   ours
 *     t (s & 0x88)    r4    r5
 *     s               r5    r6
 *     sio             r6    r7
 *     p               r7    r4
 *
 * REG_ALLOC_ORDER (arm.h:989) hands out 4, 5, 6, 7 in that order among the
 * callee-saved low registers, so this says the ROM's allocno priority order is
 * t > s > sio > p and ours is p > t > s > sio -- OURS IS THE REVERSE AND `p`
 * HAS TO LOSE THE RACE, not win it.  `allocno_compare` (global.c:600-618) is
 * floor_log2(n_refs)*n_refs/live_length, and `p` has ~11 references over the
 * whole function against `t`'s three, so every arithmetic route points the
 * wrong way; the lever is not priority (routes 4 and 5 of the shared brief).
 * The remaining 3 are one scheduling slot: `ldr r1, =REG_IE` sorts above
 * `strh r2,[r0]` for us and below it in the ROM.
 *
 * NEXT: the rotation.  Do NOT re-screen the flag axis or the anchor axis.
 *
 * ===== MEASURED NEGATIVES, batch 345 =====
 *
 * AUTOSWEEP on the old body: both cheap axes CLOSED.  Flags 8/8, best 74
 * (inert); -O1 68 but at gap -1, -fno-schedule-insns2 79, -ffixed-r7 91.
 * Anchor axis all 24 boundaries, all 74.
 *
 * Flag axis RE-RUN on THIS body (a flag inert on the parked body need not stay
 * inert): 11 groups, (none) 27, -fno-gcse / -fno-strict-aliasing /
 * -fno-strength-reduce / -fno-rerun-loop-opt / -fno-schedule-insns /
 * -fno-cse-follow-jumps all exactly 27, -fno-schedule-insns2 31,
 * -fno-rerun-cse-after-loop 40 at gap +1, -O1 58 at gap -1, -ffixed-r7 90 at
 * gap +3, -fno-expensive-optimizations 60 at gap -1.  NOTHING on that axis.
 *
 * Source probes against THIS body, all at gap 0 unless stated:
 *   - `t` unsigned                                        25 (inert)
 *   - `s` as `int`                                        27 (worse)
 *   - `sio` assigned after the 32-bit read                25 (inert)
 *   - `~0x40` as `-0x41` / as `(u32)` cast                27 / 27
 *   - `~0x40` as `one - 0x42`                             26 -- the same -1 as
 *     the `h` temp and NOT compositional with it (same statement)
 *   - REG_IME before/after the IE read-modify-write       57 at gap -1 / 88 at +2
 *   - `p` assigned after `s`, or after `sio`              27 / 27 (inert)
 *   - two-way accumulator split instead of three          -1 short
 *   - `t = s; t &= 0x88;` as two statements               exactly inert, gcc
 *     folds it (the old header got this right)
 *   - dropping `one` for the literal 1                    gap -2, pool 7/8
 *
 * WHAT IS RIGHT AND SHOULD BE KEPT (carried forward, all still measured true):
 * the 32-bit read of REG_SIOCNT through a `vu32 *`; the byte access on the high
 * half; `REG_IF = 0xc0` pooled BY THE REFERENCE TOO and kept a halfword store;
 * `REG_TM3CNT = 0xc963` with gcc deriving 0x400010c from REG_IF by itself
 * (`sub r2, #0xf6`); the `(s << 26) >> 30` bit extraction; the three `|=` flag
 * bits in the tail; and the batch-334 MODE finding -- routing the IE
 * read-modify-write through an `unsigned int` temp so it is built in SImode,
 * since *thumb_movsi_insn (arm.md:3838-3840) has the alternative order REVERSED
 * against *thumb_movhi_insn and SImode therefore never pools.
 */

#include "gba/types.h"
#include "gba/io.h"

extern unsigned char ewram_2002240[];

int Func_8005fcc(void)
{
    unsigned char *p;
    vu32 *sio;
    unsigned int s;
    unsigned int v;
    unsigned int ie;
    int r;
    int rb;
    int rc;
    int t;
    int h;
    int mask;
    int one;

    one = 1;
    mask = 0x88;
    p = ewram_2002240;
    sio = (vu32 *)&REG_SIOCNT;
    s = *sio;
    if (p[1] == 0) {
        t = s & mask;
        if (t == 8) {
            v = (unsigned char)(s & 4);
            if (v == 0 && *(int *)(p + 0x14) == -1) {
                REG_IME = v;
                ie = ~0x80;
                ie &= REG_IE;
                ie |= 0x40;
                REG_IE = ie;
                REG_IME = one;
                h = ((vu8 *)sio)[1];
                h &= ~0x40;
                ((vu8 *)sio)[1] = h;
                REG_IF = 0xc0;
                REG_TM3CNT = 0xc963;
                p[0] = t;
            }
            p[1] = 1;
        }
        p[0xb]++;
    }
    r = p[3] | (p[2] << 8);
    if (p[0] == 8)
        r |= 0x80;
    rb = r;
    if (p[9] != 0)
        rb |= 0x80 << 5;
    rc = rb;
    if (((s << 26) >> 30) > 1)
        rc |= 0x80 << 6;
    return rc;
}
