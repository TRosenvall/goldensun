/* GetLocationName -- 0x0808b158, asm/rom_8a000/rom_8ace0_a_a_c_a_c_a.s
 *
 * NON-MATCHING, 18 of 61 encodings  (batch 347 brief C; was 62 of 61).
 *   58 instructions against the ROM's 58, 128 bytes against 128, stream 61
 *   against 61 -- so for THIS body the figure IS a true distance.  It was NOT
 *   one for the body this park carried until batch 347: that body was 60
 *   instructions and 132 bytes, and the header asserted "the figure IS a
 *   distance" two lines below its own "COUNT DIFFERS (ref 61, ours 63)" and
 *   "SIZE ref 128 bytes, ours 132".  Read objcmp's INSTRUCTION COUNT and SIZE
 *   lines before the figure; they are printed above it.
 *   RELOCATIONS differ: same two symbols, the `bl GetMapArea` at 0x0c against
 *   the ROM's 0x10.  That is a consequence of the statement order below, i.e.
 *   part of the 18, not a separate blocker.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_8a000/808b158.c \
 *     asm/rom_8a000/rom_8ace0_a_a_c_a_c_a.s --func GetLocationName
 *
 * ALIGNED RESIDUE (tools/aligncmp.py): 19 in 12 hunks -> 16 in 13 hunks.
 * Two functions in the piece (Func_808b090 is parked at 84 of 96), so landing
 * this one needs tools/split_s.py; tools/datacheck.py reports no data section
 * and piece.py reports no exported non-function symbols.  0 devices.
 *
 * WHAT CLOSED THE +2, and it is one cause at two sites:
 *
 *   `r = NULL;` MOVED TO AFTER THE CALL, plus the `goto hit` join.
 *
 * The +2 was entirely the prologue and the epilogue: we saved TWO high
 * callee-saved registers (r8 and r10) where the ROM saves one (r8), which is
 * `mov r7,sl / mov r6,r8 / push {r6,r7}` against `mov r7,r8 / push {r7}` and
 * the mirror in the epilogue -- +1 and +1.  Nothing in the loop body differed
 * in count.
 *
 * WHY, from the dumps (.17.lreg / .18.greg of the old body):
 *   `-fcall-used-r4` leaves exactly THREE low callee-saved registers, r5/r6/r7,
 *   and the old body had FOUR values live across `bl GetMapArea` (a, b, e and
 *   the result) plus TWO born after it (the map area, the hoisted -1).  The map
 *   area took r6 -- a low callee-saved register it does not need -- which
 *   pushed a call-crossing value up to r10.
 *
 *   find_reg (global.c:967) blocks r12/r14 for any pseudo with
 *   calls_crossed != 0: `used1 = call_used_reg_set` at global.c:988, and
 *   CALL_USED_REGISTERS has 1 at indices 12 and 14 (arm.h:789-795).  A pseudo
 *   with calls_crossed == 0 gets `fixed_reg_set` instead, so r12 and r14 are
 *   reachable for it -- and they come 5th and 6th in REG_ALLOC_ORDER
 *   (arm.h:989), before r4.
 *
 *   Moving `r = NULL;` after the call takes the RESULT out of the
 *   call-crossing set, so it becomes eligible for r14 and lands there; the
 *   push list is then {r5,r6,r7,lr} + r8 exactly as the ROM's is.
 *
 * MECHANISM BOUND, with the evidence, because it steers the remaining 16:
 *   The ROM's exact assignment is e=r5, b=r6, a=r7, result=r8, area=r12,
 *   -1=r14.  Ours is e=r5, area=r6, b=r7, a=r8, result=r14, -1=r12.  To get
 *   the ROM's, the map-area pseudo must leave LO_REGS, and there are only two
 *   routes in find_reg:
 *     (a) PREFERRED CLASS.  regclass gives it `pref LO_REGS` because its only
 *         definition is a copy of the hard register r0 (the call's return) and
 *         r0 is in LO_REGS: .17.lreg reads `Register 35 costs: LO_REGS:0 ...
 *         HI_REGS:2`.  The hoisted -1, whose definition is a copy of a PSEUDO,
 *         reads `LO_REGS:0 ... HI_REGS:0` and gets `pref HI_REGS` on
 *         regclass.c:1206's `reg_class_subunion` tie-break -- which is why IT
 *         reaches r12 and the map area does not.  Both are operand 2 of
 *         `cbranchsi4` (arm.md:5156-5157, "l,r"/"rI,r"), so the compare
 *         contributes nothing either way.
 *     (b) PRIORITY.  allocno_compare (global.c:600-618) is
 *         floor_log2(n_refs)*n_refs/live_length.  .17.lreg: a = 3 refs / 41
 *         insns (pri 731), b = 3/40 (750), the map area = 3/36 (833).  Its
 *         definition is the first insn after the call, so its live_length can
 *         only ever be SHORTER than a's and b's, and 3 refs is the minimum a
 *         set-plus-one-in-loop-use can have (the in-loop use is weighted twice,
 *         flow.c:5114-5116).  So (b) cannot be reached from the source, and
 *         route (a) is the only one left.
 *
 * MEASURED, all at 58 instructions and 128 bytes unless noted (figures are
 * objcmp stream ndiff of 61):
 *   `r = NULL;` after the call                        20
 *   + `goto hit` join                                 18   <- this body
 *   + `goto out` instead of `break`                   18 (identical)
 *   + area compared as `area == e->id`                18 (identical)
 *   + declaration order permuted                      18 (identical)
 *   + `e->sub == b` before `e->sub == -1`             23
 *   `goto hit` join WITHOUT moving `r = NULL;`        62 at 60 instructions
 *   `r = NULL;` and `e = L9ddd8;` both after the call 31
 *   `area = GetMapArea();` first                      31
 *   `e = L9ddd8;` after the call                      61 at 60 instructions
 *   `for (;;)` with an explicit break                 20
 *   `do { } while` with the test peeled               59 at 52 instructions
 *   no result variable, two `return`s                 58 at 56 instructions
 *                   (CONFIRMS the old header's bracket: the variable is real)
 *   `int id = e->id;` hoisted                         60 at 50 instructions
 *   swapped compare operand order throughout          62 at 60 instructions
 *
 * BOTH CHEAP AXES ARE CLOSED (tools/autosweep.py, batch 347, on the OLD body):
 *   flag axis -- all eight groups inert or worse; -O1 56/61 and -fno-gcse
 *   55/61 both RAISE the aligned residue (19 -> 41, 19 -> 29).
 *   anchor axis -- 11 of 11 boundaries, none better than base.
 *   Re-run both against THIS body before trusting them again.
 *
 * NEXT: route (a) above.  The question is one sentence: what source makes the
 * map-area pseudo's single definition a copy of a PSEUDO rather than of hard
 * r0, without adding an instruction?  The hoisted -1 is the worked example of
 * the same pseudo shape winning r12 in this very function.
 */
#include "gba/types.h"

struct Loc {
    s16 id;
    s16 sub : 15;
    u16 flag : 1;
    void *name;
};

extern struct Loc L9ddd8[] __asm__(".L9ddd8");
extern int GetMapArea(void);

void *GetLocationName(int a, int b)
{
    struct Loc *e;
    int area;
    void *r;

    e = L9ddd8;
    area = GetMapArea();
    r = NULL;
    while (e->id != -1) {
        if (e->flag) {
            if (e->id == a) goto hit;
        } else if (e->id == area) goto hit;
        e++;
        continue;
hit:
        if (e->sub == -1 || e->sub == b) { r = e->name; break; }
        e++;
    }
    return r;
}
