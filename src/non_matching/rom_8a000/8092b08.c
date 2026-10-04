/* Func_8092b08 (SetSlotDrawPriority) -- 0x08092b08.  PARKED at 17 of 37,
 * PIN-FREE.  Pin-free figure 17; with three pins it is BYTE EXACT (0 of 37).
 * ref: asm/rom_8a000/rom_92950_a_c_c.s  (multi-function .s; --func)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_8a000/8092b08.c \
 *     asm/rom_8a000/rom_92950_a_c_c.s --func Func_8092b08
 *   -> XX ENCODINGS differ in 17 place(s) (ref 37, ours 37)
 *
 * Pins: 0.  The ONLY change from the batch-319 body is that `prio &= 3;` now
 * precedes `m = -0xd;`.  That edit is EXACTLY INERT pin-free (17 either way) and
 * it is the last two encodings of the pinned landing -- a measured candidate
 * prerequisite, parked in place so pass 3 does not have to re-find it.
 *
 * ================================ THE FIGURE IS ENTIRELY REGISTER ASSIGNMENT ==
 * The park said "r5 and r6 are exchanged and every later reference follows.
 * Nothing else differs."  There are TWO independent swaps, not one:
 *
 *       value                                   rom    ours
 *       prio, held across GetFieldActor          r5     r6
 *       a = GetFieldActor(slot)                  r6     r5
 *       s = a->sprite                            r1     r4
 *       v = (prio & 3) << 2                      r4     r1
 *
 * The s/v pair is worth SIX on its own, so "nothing else differs" cost a reader
 * six encodings.  Measured ladder (pins as INSTRUMENTS, tools/crossfire.py
 * depth 5, every row at ref 37 / ours 37 unless flagged):
 *
 *       BASE                                                 17
 *       v -> r4                                              16
 *       s -> r1                                              15
 *       s -> r1  +  v -> r4                                  11
 *       prio copied to a local pinned r5 (alone, or used)    17  EXACTLY INERT
 *       prio@r5 + used + v -> r4                              8
 *       prio@r5 + used + s -> r1                             12
 *       prio@r5 + used + s -> r1 + v -> r4                    2
 *       ...plus `prio &= 3` above `m = -0xd`                  0  BYTE EXACT
 *       a -> r6, in ANY combination                 39 insns, COUNT
 *
 * Two readings from that table.  (1) The prio pin is EXACTLY INERT alone and
 * worth EIGHT crossed with the v pin -- the crossing law in its "inert for want
 * of a prerequisite" shape, and the reason one-at-a-time testing saw one swap.
 * (2) Pinning `a` costs two instructions, confirming batch 322's bound that a pin
 * on a call-crossing local changes the prologue push set.  `a` must be reached by
 * the allocator, never by a pin.
 *
 * ---------------------------------------- WHY PIN-FREE IS BLOCKED, FROM SOURCE --
 * `.18.greg` prints `;; 2 regs to allocate: 34 33`, so the GLOBAL allocator picks
 * r5/r6 and `allocno_compare` (global.c:607) decides, with `live_length` as the
 * denominator.  `.17.lreg`:
 *       34 (`a`)     n_refs 5, live_length 23  ->  floor_log2(5)*5/23 * 10000 = 4347
 *       33 (`prio`)  n_refs 2, live_length 14  ->  floor_log2(2)*2/14 * 10000 = 1428
 * so `a` is allocated first and REG_ALLOC_ORDER (arm.h:989 -- 3,2,1,0,12,14,4,5,6)
 * hands the first call-crossing allocno r5.  The ROM needs `prio` first, which
 * needs n_refs 4: floor_log2(4)*4/14 = 5714.
 *
 * prio WOULD have 4 refs.  `.13.combine` holds `(set (reg/v 33) (and (reg/v 33)
 * (reg 47)))` -- the mask IN PLACE on prio, two sets and two uses.  `.15.regmove`
 * then prints
 *       Could fix operand 2 of insn 51 matching operand 0.
 *       Fixed operand 2 of insn 51 matching operand 0.
 * and the dest becomes reg 47, the constant-3 pseudo: `and r1, r1, r6`.  prio
 * drops to 2 refs and loses r5.
 *
 * That is `regmove.c:1199-1205`, and the gate is `replacement_quality`
 * (regmove.c:341): a pseudo COPIED FROM A HARD REGISTER scores 1, anything not
 * copied scores 3, and regmove declines to retarget only when
 * quality(commutative partner) >= quality(src).  `prio` is a parameter, so its
 * pseudo is `(set reg33 (reg r1))` with REG_DEAD on r1, `regno_src_regno[33] = 1`
 * (regmove.c:1113-1122), quality 1 -- against the fresh constant's 3.  So 1 >= 3
 * is false and the retarget always fires.
 *
 * `*thumb_andsi3_insn` (arm.md:1710) is what exposes it: operand 1 carries "%0",
 * i.e. tied AND commutative, so `find_matches` makes operand 2 a tie candidate
 * too.  A non-commutative two-address op would never be retargeted -- which is
 * exactly why the landed sibling src/rom_8a000/rom_96cdc_c_a_b.c keeps
 * `lsl r5, r5, #16 / asr r5, r5, #16` IN PLACE on its own parameter pseudo.
 *
 * MEASURED PIN-FREE, ALL EXACTLY INERT (and `.17.lreg` confirms regs 33/34 keep
 * n_refs 2/14 and 5/23 bit-identical in every one, so they never reached the
 * question):  `(prio & 3) << 2` inlined; `(prio << 2)` written twice;
 * `prio * 4`; `m = ~0xc`; a separate local `p = prio & 3; v = p << 2;` (and with
 * the shift inlined twice); `prio &= 3` hoisted above `s = a->sprite`;
 * `m = -0xd` moved after `v`; early-return guards instead of nested ifs;
 * recomputing `v` between the two byte stores.
 * MEASURED WORSE: `a->flags &= 0xfe` before the second byte store, 29;
 * `p = prio << 2; v = p & 0xc;` 18; a block-local `three = 3` used as the mask
 * (an attempt at regmove.c:1646's `reg_is_remote_constant_p` escape) 29-30,
 * because the constant becomes a cross-block allocno.
 *
 * NEXT, for pass 3: this is a 3-pin / 0-encoding body (prio, s, v -- none of them
 * call-crossing-with-a-push-change) sitting behind ONE structural fact about
 * parameter pseudos.  See reports/pass3-depin.md.
 */
#include "gba/types.h"
#include "actor.h"

extern struct Actor *GetFieldActor(int slot);

void Func_8092b08(int slot, int prio)
{
    struct Actor *a;
    unsigned char *s;
    int m, v;

    a = GetFieldActor(slot);
    if (a != 0) {
        if ((a->drawKind & 0xf) == 1) {
            s = (unsigned char *)a->sprite;
            prio &= 3;
            m = -0xd;
            v = prio << 2;
            s[9] = (s[9] & m) | v;
            s[0x15] = (s[0x15] & m) | v;
            a->flags &= 0xfe;
        }
    }
}
