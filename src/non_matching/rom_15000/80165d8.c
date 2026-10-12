/* Func_80165d8 -- 0x080165d8, asm/rom_15000/rom_15e8c_a_c_c_c_a_a.s
 *
 * NON-MATCHING, **14 of 74 stream encodings** -- batch 343 brief C.  WAS 30.
 *   ref 74 encodings / ours 74;  **70 INSTRUCTIONS AGAINST 70** (16-bit);
 *   size equal, relocations equal (objcmp prints no SIZE and no RELOCATIONS
 *   line);  first differing index 34.  So the figure IS a distance.
 *   PINS 0 (tools/shimcount.py prints NO ROWS).  DEVICE-FREE: the two
 *   `do { } while (0)` anchors emit nothing and shimcount does not count them,
 *   so this body carries NO pass-3 debt.  FLAG-FREE: production flags only.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/80165d8.c \
 *     asm/rom_15000/rom_15e8c_a_c_c_c_a_a.s --func Func_80165d8
 *
 * SPLIT SHAPE: none needed.  tools/piece.py -> rom_15e8c_a_c_c_c_a_a.s, **1
 * function**, all 1 parked.  tools/datacheck.py exits 0 silently (no data
 * section).  The previous header said "two functions"; the piece holds one.
 *
 * TWO STALE THINGS IN THE OLD HEADER, both corrected here:
 *   * its subject line named `asm/rom_15000/rom_15e8c_a_c_c_c_a.s`, which DOES
 *     NOT EXIST.  The live reference is the `..._a_a.s` its recipe already used.
 *   * "SIZE EXACT ON THE FIRST CANDIDATE -- 74 instructions against 74" quotes
 *     the STREAM count on the instruction line.  The instruction counts are
 *     70/70.  Both equal either way, so the old verdict stood; this is the
 *     ninth/tenth instance of that recurring slip, not an error of substance.
 *
 * AUTOSWEEP, BOTH CHEAP AXES, RECORDED SO NOBODY RE-SCREENS THEM.  On the OLD
 * (30) body: flag axis found `-fno-strength-reduce` at 28, every other group
 * exactly inert at 30 (-O1 67, -fno-schedule-insns2 68, -ffixed-r7 37); anchor
 * axis found anchor11 at 26 and anchor12 at 28, with 6 of 31 placements
 * producing NO ROW (untested, not inert).
 *
 * ========================= THE THREE EDITS, 30 -> 14 =========================
 *
 * EDIT 1 -- THE LOOP COUNTERS ARE UNSIGNED IN THE ROM.  Worth 4.
 *
 * Nobody had looked at the loop comparisons.  Two of them were wrong:
 *
 *     idx 53   ref d9f8 bls.n    ours ddf8 ble.n     the copy loop
 *     idx 65   ref d9f8 bls.n    ours dafa bge.n     the zeroing loop
 *
 * `i <= 3` on a signed `int` is a SIGNED compare; the ROM's is UNSIGNED.
 * `unsigned int i;` fixes both.  It also fixes a second defect in the same
 * edit, which is why index 65 read `bge` and not merely `ble`: OUR ZEROING
 * LOOP WAS A DOWN-COUNTER AND THE ROM'S IS AN UP-COUNTER.
 *
 *     rom    movs r1,#0  ...  adds r1,#1 / cmp r1,#3 / bls
 *     ours   movs r1,#3  ...  subs r1,#1 / cmp r1,#0 / bge
 *
 * That is `check_dbra_loop`, **loop.c:4408**, called from `strength_reduce`
 * (**loop.c:3848**), itself gated on `flag_strength_reduce` at **loop.c:1132**.
 * Hence autosweep's `-fno-strength-reduce` row.
 *
 *   >> THE UNSIGNED COUNTER SUBSUMES THE FLAG.  The unsigned body measures 26
 *      and the unsigned+anchor body 22 IDENTICALLY with and without
 *      -fno-strength-reduce.  So this park needs NO Makefile flag group, and a
 *      flag row in an autosweep table can be a SYMPTOM OF A SOURCE DEFECT
 *      rather than a lever.  Worth knowing before anyone adds a file to
 *      STRENGTH_CFLAGS on the strength of a sweep row. <<
 *
 * Controls, all measured: `i != 4` (sign-free but not unsigned) is EXACTLY
 * INERT at 30, so it is the unsignedness and not the avoidance of `<=`.  A
 * `u16` counter is 76 with RELOCDIFF and +20 bytes.  And "two is not N" RUNS
 * THE OTHER WAY here -- separate counters per loop are WORSE: two names 56,
 * three names 58.  The single shared name is the ROM's.
 *
 * EDIT 2 -- THE STORE ORDER, DERIVED FROM THE ALLOCATOR THEN MEASURED.  Worth 7.
 *
 * The old header's "NEXT" asked for a testable hypothesis about allocation
 * order among the three single-use parameters.  There is one and it is
 * arithmetic.  From .17.lreg on the unsigned body:
 *
 *     32 (a)  2 refs, span 34      34 (c)  2 refs, span 21
 *     33 (b)  2 refs, span 27      35 (d)  2 refs, span 23      all pref LO_REGS
 *
 * `allocno_compare` (**global.c:600-618**) prices an allocno
 * floor_log2(n_refs) * n_refs / live_length * 10000 * size.  With n_refs = 2
 * and size 1 for all four that collapses to **20000 / span, so their order is
 * PURELY THE SPAN ORDER.**
 *
 * THE MODEL WAS CHECKED, NOT ASSUMED: it reproduces .18.greg's priority line
 * `;; 10 regs to allocate: 40 38 41 39 36 124 34 35 33 32` for ALL TEN
 * allocnos (**global.c:1875-1881** prints `allocno_order` ascending, so that
 * line IS the priority order).
 *
 * Allocation then walks REG_ALLOC_ORDER (**arm.h:989**) inside LO_REGS, i.e.
 * 3,2,1,0,4,5,6,7:
 *
 *     ours  c > d > b > a   ->  c=r2  d=r6  b=r7  a=r12  (no LO reg left; pass 1)
 *     ROM                   ->  c=r2  a=r6  d=r7  b=r12
 *     so the ROM's priority order is  c > a > d > b
 *
 * Both sides spend exactly ONE `mov rX, ip`: the ROM on b's `strh` (which
 * cannot take a high register), we on a's `str`.  That one decision was ~12 of
 * the 22 encodings, which is why so much moved for so little.
 *
 * THE REQUIREMENT IS THEREFORE  span(c) < span(a) < span(d) < span(b)  -- `a`
 * must die EARLY and `b` LAST.  A store's position sets its span, so I swept
 * the position of `s->f0 = a;` against `s->f12 = b;` over the eleven-store
 * block: 138 variants in one container, zero compile fails.  `f0` at position 0
 * with `f12` at 4 measures **19** and `first` moves from index 1 to 32 -- the
 * ENTIRE parameter rotation resolves and `str r6,[r0,#0]` now matches the ROM.
 * The prediction held.  A second round on `s->f24 = f;`'s position: 19 -> 15.
 *
 * Reordering stores to eleven DISTINCT non-volatile fields of one struct is
 * semantics-preserving and leaves every per-opcode memory total identical,
 * which is the screen crossfire exists for.
 *
 * EDIT 3 -- TWO ANCHORS, worth 1 each on top of the above.
 *
 * anchor11 (after `s->f12 = b;`) is autosweep's win on the parked body and
 * survives every later edit.  Re-sweeping the anchor axis ON THE EDITED BODY
 * then found a second placement at `p = s->f8;` in the else arm, 15 -> 14.
 *
 *   >> THE ANCHOR AXIS ON THE PARKED BODY IS NOT THE ANCHOR AXIS ON AN EDITED
 *      BODY.  Re-running it after each source change paid twice here.  batch 342
 *      recorded "autosweep cannot be aimed at a CANDIDATE, only an installed
 *      park" as OPEN; scratch_elev/b343/C/c_anchorsweep.py closes the anchor
 *      half of that gap by importing autosweep's own body_span/boundaries so
 *      the placement rule cannot drift from the tool the parks quote. <<
 *
 * ===================== WHAT THE REMAINING 14 IS, NAMED =====================
 *
 *   34-39   the order inside the store block.  The ROM loads `f`
 *           (`ldr r3,[sp,#20]`) EARLY and puts `str r6,[r0,#0]` between
 *           `movs r2,#0` and the three zero stores; ours emits it two slots
 *           earlier.  Pure sched2 placement at equal count.          5
 *   45-46   counter init against pointer bump in the copy-loop preheader:
 *   59-60   ROM `movs r1,#0` then `adds r2,#8`, ours the reverse.  Both
 *           loops, same shape.                                       4
 *   57-58   the zeroing loop's r2/r3 swap -- the ROM holds the POINTER in r3
 *   62-63   and the POOLED ZERO in r2, ours the reverse.  This is the SAME
 *           first-allocated-takes-r3 question as edit 2, one level down, and
 *           the span model above is the instrument for it.           4
 *   71-72   epilogue `pop {r1} / bx r1` against `pop {r0} / bx r0`.   2
 *
 * NEXT, and it is the same lever that just worked: price the zeroing loop's
 * pointer and pooled-zero allocnos out of .17.lreg and move whichever store
 * sets their spans.  Mind batch 342's correction that REG_N_REFS is loop-depth
 * weighted at **flow.c:5114-5116** -- an IN-LOOP reference counts TWO -- which
 * matters here and did not matter for edit 2, where all four parameters are
 * referenced at depth 0.
 *
 * MEASURED AND INERT OR WORSE, carried forward from the old header: `i < 3`
 * instead of `i != 3` (32), an early `return` instead of the guarded block (30,
 * unchanged), reassigning the shifted parameter in place (62 -- much worse; the
 * ROM computes `c << 8` into a scratch and keeps the parameter).  NOTE those
 * were measured against the OLD body and are therefore UNTRUSTED against this
 * one, by batch 342's rule about inherited controls.
 *
 * WHAT IT DOES, unchanged and still correct: finds the first free slot of three
 * in the 0x28-byte table at iwram_3001e8c + 0x620, fills eleven fields, then
 * copies four halfwords in from the caller's buffer or zeroes them.  The slot
 * scan checks THREE slots, not four -- it advances and tests the counter
 * together, so the fourth is stepped onto but never read.  `ldr r2, =0` in the
 * zeroing arm is a POOLED ZERO and the recorded halfword exception rather than
 * a symbol: the store is `strh`, gcc narrows the constant to HImode, and HImode
 * constants go to the pool.
 */
#include "gba/types.h"

struct Slot {
    int f0;
    u16 f4;
    u16 f6;
    u16 f8[4];
    u16 f10;
    u16 f12;
    u16 f14;
    u16 f16;
    u16 f18;
    u16 f1a;
    u16 pad1c;
    u16 f1e;
    u16 f20;
    u16 pad22;
    u16 f24;
    u16 pad26;
};

extern unsigned char *iwram_3001e8c;

void Func_80165d8(int a, int b, int c, int d, u16 *src, int f)
{
    struct Slot *e;
    struct Slot *s;
    u16 *p;
    unsigned int i;

    e = (struct Slot *)(iwram_3001e8c + (0xc4 << 3));
    s = NULL;
    for (i = 0; i != 3; i++) {
        if (e->f0 == 0) {
            s = e;
            break;
        }
        e = (struct Slot *)((u8 *)e + 0x28);
    }
    if (s != NULL) {
        s->f0 = a;
        s->f1e = c << 8;
        s->f4 = c << 8;
        s->f6 = d << 8;
        s->f12 = b;
        s->f16 = 0xf;
        s->f1a = 0xa;
        do { } while (0);
        s->f14 = 0;
        s->f18 = 0;
        s->f24 = f;
        s->f20 = 0;
        if (src != NULL) {
            p = s->f8;
            for (i = 0; i <= 3; i++) {
                *p = *src;
                src++;
                p++;
            }
        } else {
            p = s->f8;
            do { } while (0);
            for (i = 0; i <= 3; i++) {
                *p = 0;
                p++;
            }
        }
        s->f10 = 0;
    }
}
