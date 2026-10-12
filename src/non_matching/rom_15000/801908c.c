/* Func_801908c -- 0x0801908c, asm/rom_15000/rom_18cac_c.s.   (PARK -- HOLD)
 *
 * NOT MATCHING: 22 encodings of 151 differ, and this IS a true distance --
 *   ref 320 bytes / 151 encodings, ours 320 / 151, FIRST DIFFERING INDEX 21,
 *   and BOTH relocations identical in type, symbol and offset
 *   (R_ARM_THM_CALL Func_8003d28 at 0xb4, R_ARM_ABS32 Data_366f8 at 0xf4).
 *   Re-measured batch 342 brief D; the park's figure survived unchanged.
 *
 * ONE function, NO data section (datacheck.py exit 0, no EXPORTS line), so
 * landing this is a plain WHOLE-FILE conversion: no split, no export, no
 * linker-script change.  ZERO pins (tools/shimcount.py).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_15000/801908c.c \
 *     asm/rom_15000/rom_18cac_c.s --whole
 *
 * The prototype and every struct here are FIXED BY A NEIGHBOUR, not guessed:
 * src/non_matching/rom_15000/80191cc.c calls Func_801908c(e) with struct Ent *,
 * and its struct Ent / struct Spr / struct Req are reused verbatim.  Every
 * bitfield mask in the ROM (0xc1 on byte 7 = pri:5 at bit 9, 0xfc on byte 5 =
 * c0:2, 0x1ff / 0xfffffe00 on the halfword at +6 = x:9) matches that park's
 * table 4 exactly.
 *
 * THREE LEVERS LANDED EARLIER, 30 -> 26 -> 22 (all still in the body):
 *  1. `int g = 0x100;` BEFORE `struct Spr *o = &e->spr;`  (30 -> 26)
 *  2. A BLOCK-LOCAL `{ u32 a = e->fc; ... }` PER SWITCH ARM, not one
 *     function-scope local shared by arms 9 and 10.  (26 with, 30 without)
 *  3. `u32 c = a; ... a = a + 1; e->fc = a;` -- the increment on a local that
 *     is MODIFIED, with a separate copy carrying the index.  (26 -> 22)
 *
 * ================= THE SPLIT OF THE 22, CORRECTED (batch 342) =================
 *
 * The old header read "The 22 are: 10 in arms 11/12 + 8 a pure r2<->r3 ROLE SWAP
 * in arms 9/10 + 2 a sched2 rotation of `ldr r2,=Data_366f8` past
 * `strh r3,[r6,#0xc]`."  THAT IS 20, NOT 22, and the "2 sched2" are not a
 * separate site.  Read off the index-aligned side-by-side the partition is 8+14:
 *
 * ARMS 9 AND 10 (.L190b6 / .L190c8) -- 4 EACH, 8 TOTAL, and it reduces to ONE
 * decision: WHICH HARD REGISTER THE `ldrh` TARGETS.
 *     rom  ldrh r2,[r6,#0xc] / add r3,r2,#1 / strh r3,[r6,#0xc] / mov r3,#0x1f
 *     ours ldrh r3,[r6,#0xc] / add r2,r3,#1 / strh r2,[r6,#0xc] / mov r2,#0x1f
 *     both ldr r1,=Data_366f8 / and r3,r2 / lsl r3,#0x1 / ldrh ...
 * Everything after the load follows from it: the increment takes the other
 * register, the store kills it, and `mov #0x1f` reuses the dead one.
 *
 * ARMS 11 AND 12 (.L190da / .L190f0) -- 7 EACH, 14 TOTAL.  "Ours compares the
 * LOAD where the ROM compares the COPY" is the SYMPTOM.  **THE CAUSE IS THAT THE
 * ROM ALLOCATES THREE REGISTERS IN THE ARM WHERE WE ALLOCATE TWO.**
 *     rom  ldrh r3 / mov r0,r3 / cmp r0,#7 / bhi / add r3,#1 /
 *          ldr r2,=Data_366f8 / strh r3 / lsl r3,r0,#2 / add r3,#0x20 /
 *          ldrh r7,[r2,r3]
 *     ours ldrh r2 / mov r3,r2 / cmp r2,#7 / bhi / add r2,#1 / strh r2 /
 *          lsl r3,#2 / ldr r2,=Data_366f8 / add r3,#0x20 / ldrh r7,[r2,r3]
 * The ROM's table pointer gets its OWN register (r2) and is loaded BEFORE the
 * `strh`, so it is live while the load is still live; we load it AFTER the
 * `strh` and REUSE the load's dead register.  Three live quantities against two,
 * and the copy is consequently pushed to r0 in arm 11 and r1 in arm 12 -- the
 * third and fourth entries of `REG_ALLOC_ORDER`, which is
 * `{3, 2, 1, 0, 12, 14, 4, 5, ...}` (config/arm/arm.h:989).
 * **SO THE LEVER IS THE TABLE POINTER'S LIVE RANGE, NOT THE COMPARE'S OPERAND**,
 * and every spelling tried so far has attacked the compare.
 *
 * THE PASS, NAMED, WITH ITS GATE.  The old header said "the copy's PLACEMENT is
 * regmove's, and which operand the compare takes was fixed earlier".  "Earlier"
 * is cse, and the decision is `canon_reg` (cse.c, case REG) returning
 * `qty_table[q].first_reg`, set by `make_regs_eqv` (cse.c).  For the COPY to
 * become canonical -- which is what leaves `cmp c` standing -- make_regs_eqv
 * needs BOTH:
 *   (1) the copy's range to escape the current cse extended basic block
 *       (uid_cuid[REGNO_LAST_UID (new)] > cse_basic_block_end, OR
 *        uid_cuid[REGNO_FIRST_UID (new)] < cse_basic_block_start), and
 *   (2) uid_cuid[REGNO_LAST_UID (new)] > uid_cuid[REGNO_LAST_UID (firstr)].
 * CONDITION 2 ALREADY HOLDS in this body (the copy's last use, the table index,
 * is after the load's last use, the `strh`).  CONDITION 1 IS THE GATE, and the
 * one semantics-preserving way to open it was measured: ONE function-scope copy
 * local shared between arms 11 and 12 opens it via the FIRST_UID clause and DOES
 * flip the canonicalisation -- confirmed in the disassembly -- but the flip goes
 * the WRONG WAY.  With the copy canonical cse DELETES the copy outright and the
 * load targets the copy's pseudo, giving the 149-instruction three-operand-add
 * shape: `ldrh r0 / cmp r0,#7 / bhi / add r3,r0,#1 / strh r3 / ldr r2,= /
 * lsl r3,r0,#2`.  127 of 151, dsize -4, RELOCDIFF, first diff at 13 because the
 * shared pseudo goes to global.c instead of local-alloc and the switch dispatch
 * changes with it.  (The old header's argument that the ROM's distinct r0/r1
 * proves distinct PSEUDOS does not follow -- local-alloc splits one pseudo's
 * disjoint ranges anyway -- but the measured 127 settles the question.)
 *
 * ========================= MEASURED, BATCH 342 ==============================
 * 68 variants in three containers, every arms-9/10 spelling crossed against
 * every arms-11/12 spelling.  NOTHING BEATS 22.
 *
 * ARMS 9/10:   base 22 | `0x1f & X` 22 | named increment `u32 X1 = X+1;` 22
 *   | the table read BEFORE the store 134 and 95 (dsize +-4, RELOCDIFF)
 *   | `u32 X1 = X+1; g = tbl[...]; e->fc = X1;` 30
 *   | a named mask local `u32 m = 0x1f;` before or after the load 28
 *   | `do { } while (0)` before / between / after the arm's statements 22/24/22
 * ARMS 11/12:  base 22 | guard on the LOAD instead of the copy 22 (BIT-IDENTICAL
 *     -- cse had already rewritten it, so the two spellings are one point)
 *   | `u32 c = (u16)a;` 22 | `c = a & 0xffff;` 22 | `c = a & 0xffffffff;` 22
 *   | `u16 c = a;` 22 -- EVERY IDENTITY OBFUSCATION IS FOLDED BEFORE CSE
 *   | copy declared INSIDE the if 24 | with a 3-operand `e->fc = c+1` 127
 *   | ONE SHARED function-scope copy local, three decl positions, 127 each
 *   | the table VALUE read into a local before the store 92 (RELOCDIFF)
 *   | `do { } while (0)` before the increment / the store / the table read
 *     **127 EACH** | `do { } while (0)` after the whole `if` 22
 *
 * *** A NEGATIVE WORTH RECORDING ON ITS OWN: AN ANCHOR INSIDE A SWITCH ARM'S
 * `if` BODY COSTS 105 ENCODINGS HERE (22 -> 127) AND CHANGES THE LENGTH. ***
 * Batch 339 brief C's "an anchor downstream of the residue can fix the residue"
 * was tested in all four in-arm positions and every one is catastrophic, because
 * the anchor splits the arm's basic block and the dispatch is re-emitted.  The
 * advice needs the qualifier "downstream WITHIN THE SAME BASIC BLOCK".
 *
 * MEASURED INERT (earlier batches, all exactly 26 on the lever-2 base):
 * `0x1f & v`; naming the masked index in a local (30); `*(Data_366f8 + i)` (26);
 * `int` instead of `u32` for the arm locals (26); a `u16 *t = Data_366f8;` local
 * PER ARM (28, WORSE).
 * MEASURED WORSE: -fno-regmove (52); -fno-rerun-cse-after-loop (101 at this
 * body, was recorded 34 at an older one).
 * FLAG AXIS, THIS BODY (tools/autosweep.py, batch 342): -fno-gcse 22,
 * -fno-strict-aliasing 22, -fno-strength-reduce 22, -fno-rerun-loop-opt 22 (all
 * inert); -fno-schedule-insns2 49, -fno-rerun-cse-after-loop 101, -O1 146,
 * -ffixed-r7 154.  BOTH CHEAP AXES CLOSED.
 * MEASURED SHORT (149 instructions, 2 under, NOT comparable): every spelling
 * that lets cse delete the copy outright, e.g. a plain
 * `u32 a = e->fc; if (a <= 7) { e->fc = a + 1; g = tbl[...]; }`.
 * THE 316b CROSS, still valid: the DisplayMenuArrowCursor "two locals of
 * different width" idiom is inert here, because it needs the SURVIVING use to be
 * WIDER than the compare and both ends are the same width.
 *
 * A measurement DEVICE, kept as an instrument and NOT in the code below:
 * `__asm__ ("" : "+r" (c))` after `u32 c = a;` in each arm measures 12 of 151 at
 * exact size, count and relocations -- so the arms-11/12 group IS one
 * substitution and nothing else.
 *
 * WHAT TO TRY NEXT, and it is not what has been tried: make the TABLE POINTER's
 * live range in arms 11/12 start before the `strh` WITHOUT changing the
 * instruction count, so three quantities compete and the copy is pushed down
 * REG_ALLOC_ORDER to r0/r1.  Reading the table VALUE into a local is the naive
 * form and costs length (92); look for a spelling that moves only the ADDRESS
 * computation forward.
 *
 * NO SHIMS in the code below: no `register ... __asm__`, no `__asm__ ("")`,
 * no .equ, no per-file flags, no fakematch row.
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

extern u16 Data_366f8[];

extern int Func_8003d28(struct Req *r);

void Func_801908c(struct Ent *e)
{
    int g = 0x100;
    struct Spr *o = &e->spr;
    u32 v;
    struct Req s;

    switch (e->state) {
    case 9:
        {
            u32 a = e->fc;
            e->fc = a + 1;
            g = Data_366f8[a & 0x1f];
        }
        break;
    case 10:
        {
            u32 b = e->fc;
            e->fc = b + 1;
            g = Data_366f8[b & 0x1f] >> 1;
        }
        break;
    case 11:
        {
            u32 a = e->fc;
            u32 c = a;
            if (c <= 7) {
                a = a + 1;
                e->fc = a;
                g = Data_366f8[0x10 + c * 2];
            }
        }
        break;
    case 12:
        {
            u32 b = e->fc;
            u32 d = b;
            if (d <= 7) {
                b = b + 1;
                e->fc = b;
                g = Data_366f8[0x10 + d * 2] >> 1;
            }
        }
        break;
    }

    if (g == 0x100) {
        o->pri = 0;
        o->c0 = 0;
        o->x = e->f6;
        o->f4 = *(u16 *)&e->f8;
    } else {
        s.w = g;
        s.h = g;
        s.ang = 0;
        o->pri = Func_8003d28(&s);
        if (g > 0x100) {
            o->c0 = 3;
            o->x = e->f6 - 8;
            o->f4 = e->f8 - 8;
        } else {
            o->c0 = 1;
            o->x = e->f6;
            o->f4 = *(u16 *)&e->f8;
        }
    }
}
