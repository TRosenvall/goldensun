/* Task_BlitAnim_BG1Wide (0x080cd358) -- NON-MATCHING, still PARKED.
 *
 *   67 of 83 encodings  (STREAM; pool+pad included).
 *   INSTRUCTIONS ref 70, ours 71 -- GAP +1 (ours LONGER), so the 67 is
 *   PHASE, not distance.  SIZE ref 192, ours 196.
 *   ALIGNED residue (tools/aligncmp.py): 54 of 83 equal, 65.1% --
 *   IMPROVED from the previous body's 50 of 83 / 60.2% at the SAME raw 67.
 *   Batch 345 brief E.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/80cd358.c \
 *     asm/rom_c9000/rom_cd260_a_a_c.s --func Task_BlitAnim_BG1Wide
 *
 * PINS: 0.  DEVICES: 0.  FAKEMATCH: no.
 * SPLIT: none needed -- asm/rom_c9000/rom_cd260_a_a_c.s holds this function
 * alone (it is the _c half of the split that landed Task_BlitAnim).
 *
 * ===== THE GAP IS ONE INSTRUCTION AND IT IS IN case 1, ALONE =====
 *
 * Per-region instruction counts, ref vs ours, counted block by block against
 * the reference's own labels:
 *
 *     prologue+gate+switch tree  18 / 18      .Lcd37c arm   5 / 5
 *     case 0  (.Lcd386)           7 / 7       case 1 (.Lcd396)  15 / *16*
 *     case 2a (.Lcd3b8)          10 / 10      case 2b (.Lcd3ce)  6 / 6
 *     case 3  (.Lcd3dc)           8 / 8       tail (.Lcd3ee)     4 / 4
 *     epilogue (.Lcd3f6)          3 / 3
 *
 * Every other region is instruction-for-instruction the reference.  The single
 * extra instruction is the hoisted pool load in case 1:
 *
 *     rom   mov r2,#0xf0 / ldr r3,=Func_8001af8 / ldr r1,=gBuffer / lsl r2,#7 /
 *           ldr r0,=0x6008000 / bl / ldr r2,=0x7784 / mov r1,#0xf0 /
 *           add r3,r5,r2 / ldr r2,[r3] / ldr r0,=gBuffer / ldr r3,=Func_80008d8 /
 *           lsl r1,#7 / bl / b                                      (15)
 *     ours  *ldr r5,=gBuffer* / mov r2,#240 / ldr r3,=Func_8001af8 / mov r1,r5 /
 *           lsl r2,#7 / ldr r0,=0x6008000 / bl / ldr r2,=0x7784 / mov r1,#240 /
 *           add r3,r6,r2 / ldr r2,[r3] / mov r0,r5 / ldr r3,=Func_80008d8 /
 *           lsl r1,#7 / bl / b                                      (16)
 *
 * The two streams pair up one-to-one: the ROM's two `ldr =gBuffer` are our two
 * `mov rN,r5`, and our extra insn is the `ldr r5,=gBuffer` that materialises
 * the shared value.  p is r5 in the ROM and r6 here for exactly that reason.
 *
 * ===== WHAT BATCH 345 FIXED: THE TWIN'S LENGTH LEVER, PORTED =====
 *
 * THE PREVIOUS HEADER'S "NEXT: nothing.  One repeated constant in one arm." IS
 * REFUTED.  The constant half was reachable all along, and the recipe is in the
 * LANDED TWIN in this same piece, src/rom_c9000/rom_cd260_a_a_b.c
 * (Task_BlitAnim, 0 of 104), which is the same gate, the same switch and the
 * same two calls in case 1:
 *
 *   * THE BATCH-105 BASIC-BLOCK LEVER.  One length local PER USE SITE, all
 *     DEFINED AT THE TOP of the function -- len1, len2, len0 in that order,
 *     copied from the twin.  That is what makes the ROM's `mov r1,#0xf0 /
 *     lsl r1,#7` reappear for the SECOND call instead of a `mov r1,r6` off a
 *     callee-saved copy.  Only cases 0 and 1 need names; cases 2 and 3 keep
 *     the literal, exactly as the twin does (case 2a still gets the ROM's
 *     `add r2,#0x7c` derivation from the 0x7784 already in a register).
 *   * THE int-RETURN LEVER through CopyFn, also from the twin.  Measured INERT
 *     here on its own and in combination (67 either way), but kept because the
 *     twin needs it for the same argument-fill ordering and it costs nothing.
 *
 * Together these took the ALIGNED residue 50 -> 54 of 83 with the raw figure
 * unmoved at 67, which is the shape the batch-344 rule predicts: the figure is
 * phase-bound while the gap is open.
 *
 * ===== WHY THE SAME LEVER DOES NOT WORK ON THE POINTER, WITH CITATIONS =====
 *
 * `b0/b1/b2 = gBuffer` at the top -- the exact analogue of len0/len1/len2 --
 * is NOT inert, it is WORSE (80 of 83), and the asymmetry is in cse's cost
 * model rather than in anything source-level:
 *
 *   * In Thumb a symbol address is not a legitimate immediate, so expand emits
 *     `(set (reg) (mem/u/f:SI (symbol_ref "*.LC2")))` -- a LOAD from the
 *     constant pool (seen in 80cd358.c.00.rtl, insns 61 and 76, two separate
 *     pseudos 44 and 46).  The integer length stays `(const_int 30720)`.
 *   * `canon_hash` sets `hash_arg_in_memory` only when `! RTX_UNCHANGING_P`
 *     (cse.c:2260), and `invalidate_memory` removes only elements whose
 *     `in_memory` is set (cse.c:6068-6080).  A constant-pool MEM is
 *     RTX_UNCHANGING_P, so IT SURVIVES THE INTERVENING `bl` in cse's table --
 *     the call does not re-expose the load.
 *   * `COST` (cse.c:509-514) gives a pseudo 1 and a MEM `notreg_cost`, and
 *     cse.c:1586 states the rule outright: "a constant is the only thing that
 *     can be cheaper than a register".  So cse KEEPS the named integer
 *     constant at both sites and REPLACES the second pool load with the first
 *     load's register.  That is the whole asymmetry, and it is why the
 *     length lever transfers and the pointer lever cannot.
 *
 * The ROM's two `ldr =gBuffer` therefore cannot come from cse at all; they are
 * reload substituting a REG_EQUIV pool MEM at each use, which needs the pseudo
 * to get NO hard register.  Only `p` and `gBuffer` are live across case 1's
 * first call -- two allocnos against three free callee-saved registers
 * (r5/r6/r7; r4 is call-used under -fcall-used-r4) -- so global alloc has no
 * reason to decline either.  NEXT is therefore a construct that puts TWO more
 * values live across that call for free, or one that keeps the second use out
 * of cse's reach; three more free callee-saved registers is the bound to beat,
 * and -ffixed-r7 alone only takes it to two (measured inert, below).
 *
 * ===== MEASURED NEGATIVES, batch 345, against THIS body unless stated =====
 *
 * AUTOSWEEP, on the previous body: both cheap axes CLOSED.  Flag axis 8/8 at
 * 67-89 (best 67, i.e. inert; -ffixed-r7 89, -O1 68, -fno-schedule-insns2 68).
 * Anchor axis 16 of 19 boundaries measured, all 67 or worse; 3 did not compile.
 *
 * Crossed against THIS body (len locals + int CopyFn), all GAP +1 unless noted:
 *   - anchor at all six boundaries inside case 1            67, 67, 67, 66, 66, 67
 *   - `&gBuffer[0]` for the second call / for the first     67 / 67
 *   - `val` read before the first call                      66
 *   - `fill = Func_80008d8` assigned before the first call   67
 *   - `val` read inline in the call (previous spelling)      67
 *   - 11 flag groups incl. -fno-cse-follow-jumps            67 (inert)
 *   - -fno-expensive-optimizations  ndiff 57, GAP *-1*, dsize 0 -- and case 1
 *     IS BYTE-FOR-BYTE UNCHANGED under it, so the -1 is another region losing
 *     an instruction.  A FALSE LEAD, recorded so nobody re-chases it, and
 *     there is no per-file flag group for it in the Makefile anyway.
 *
 * On the PREVIOUS body (figures not comparable with the 67 above):
 *   - `copy = Func_8001af8` hoisted ABOVE the switch: ndiff 44, GAP *0*,
 *     dsize 0, aligned 56 of 83.  IT CLOSES THE LENGTH GAP, and it is still
 *     the WRONG SHAPE: the fn pointer is loaded once into r4 in the gate block
 *     and every call becomes `bl _call_via_r4`, where the ROM loads
 *     `=Func_8001af8` inside each case and uses `_call_via_r3`.  Recorded as
 *     the cheapest known length-equaliser and as a dead end on shape.
 *   - `fill` defined before the first call +6 / `val` before it +7 /
 *     both +10 / `0x6008000` named +6 / `p+0x7784` named +8 /
 *     `fill` hoisted above the switch +6: every attempt to RAISE pressure
 *     across the call costs more than it buys, because the extra value takes a
 *     callee-saved register of its own instead of evicting gBuffer.
 *   - one function-pointer local reused for both calls: exactly inert.
 *
 * WHAT IS RIGHT, and is the reusable part (carried forward, all still true):
 *
 *   THE SWITCH LOWERS EXACTLY.  A plain `switch (x)` over cases 0..3 produces
 *   the ROM's `cmp #1 / beq / cmp #1 / bgt / cmp #0 / beq / b default` tree
 *   with no help.  Do not hand-write the comparison chain.
 *
 *   THE INDIRECT CALLS ARE THE rom_e0524.c IDIOM.  `ldr r3,=Func_8001af8 /
 *   bl _call_via_r3` comes from assigning the callee into a local of
 *   function-pointer type; gcc-2.96 does not propagate it back to a direct
 *   call.  Two DIFFERENT pointer types are needed (different argument lists)
 *   and the two locals stay separate -- AND THE ASSIGNMENT MUST STAY INSIDE
 *   THE CASE, per the hoist negative above.
 *
 *   `0xf0 << 7` IS 0x7800 AND THE ROM DERIVES IT FROM A NEARBY CONSTANT in
 *   case 2a: `ldr r2,=0x7784 / add r2,#0x7c` where the source says `0xf0 << 7`.
 *   gcc finds it on its own -- which is why case 2 must keep the LITERAL and
 *   must NOT get a named length.
 */
#include "gba/types.h"

extern unsigned char *iwram_3001eec;
extern unsigned char gBuffer[];
extern int Func_8001af8(volatile u16 *dst, void *src, u32 len);
extern void Func_80008d8(void *dst, u32 len, u32 value);
extern void BlitFade_Div2(void *src, void *dst, u32 len);
extern void BlitFade_Div4(void *src, void *dst, u32 len);
extern void BlitFade_Sub(void *src, u32 amt, void *dst, u32 len);

typedef int (*CopyFn)(volatile u16 *dst, void *src, u32 len);
typedef void (*FillFn)(void *dst, u32 len, u32 value);

void Task_BlitAnim_BG1Wide(void)
{
    unsigned char *p;
    CopyFn copy;
    FillFn fill;
    u32 val;
    u32 len0;
    u32 len1;
    u32 len2;

    len1 = 0xf0 << 7;
    len2 = 0xf0 << 7;
    len0 = 0xf0 << 7;
    p = iwram_3001eec;
    if (*(int *)(p + 0x7824) != 1)
        return;
    switch (*(int *)(p + 0x7780)) {
    case 0:
        copy = Func_8001af8;
        copy((volatile u16 *)0x6008000, gBuffer, len0);
        break;
    case 1:
        copy = Func_8001af8;
        copy((volatile u16 *)0x6008000, gBuffer, len1);
        val = *(u32 *)(p + 0x7784);
        fill = Func_80008d8;
        fill(gBuffer, len2, val);
        break;
    case 2:
        if (*(int *)(p + 0x7784) == 0x32)
            BlitFade_Div2(gBuffer, (void *)0x6008000, 0xf0 << 7);
        else
            BlitFade_Div4(gBuffer, (void *)0x6008000, 0xf0 << 7);
        break;
    case 3:
        BlitFade_Sub(gBuffer, *(u32 *)(p + 0x7784), (void *)0x6008000, 0xf0 << 7);
        break;
    }
    *(int *)(p + 0x7824) = 0;
}
