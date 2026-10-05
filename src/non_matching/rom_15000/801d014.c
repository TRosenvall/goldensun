/* Func_801d014 (0x0801d014) -- NON-MATCHING, 3 of 91 encodings.
 *
 *   SIZE IS EXACT (220 bytes both), ENCODING COUNT IS EXACT (91 = 91), and
 *   ALL FOUR RELOCATIONS ARE EXACT -- objcmp prints no SIZE and no
 *   RELOCATIONS line.  NO PINS, NO FLAGS, NO SYMBOLS.  The literal pool is
 *   the ROM's pool in the ROM's order.
 *
 *   WAS 37 of 91 (batch 319 backfill figure, re-derived in batch 324 before
 *   anything was changed).  37 -> 3 by ONE edit.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/801d014.c \
 *     asm/rom_15000/rom_1ca1c_c_a_c.s --func Func_801d014
 *
 * SPLIT SHAPE: none needed for a park.  tools/datacheck.py on
 * asm/rom_15000/rom_1ca1c_c_a_c.s reports no data section and no required
 * data export; the file holds this one function.  shimcount.py: PIN-FREE.
 *
 * ========================================================================
 * WHAT THE 37 WAS
 * ========================================================================
 * Four runs, ONE cause:
 *   14  copies 1, 2 and 4 (pooled source offset + pooled destination offset)
 *    9  copy 3, whose source offset 0x20c is built `mov #0x83 / lsl #2`
 *    7  copy 5, whose destination offset 0x598 is built `mov #0xb3 / lsl #3`
 *    7  the LITERAL POOL -- three transposed pairs at 0xa0/0xa4/0xa8,
 *       0xb0/0xb4 and 0xc4/0xc8.  Not a separate blocker: push_minipool_fix
 *       orders pool words by `addr + range`, i.e. by first reference, so the
 *       pool transposition is a CONSEQUENCE of the three instruction runs.
 *   14 + 9 + 7 + 7 = 37 exactly.
 *
 * Every run is the same transposition.  The ROM forms the SOURCE address,
 * loads the byte, and only THEN forms the destination address:
 *   rom   ldr r1,=gState / ldr r0,=0x205 / adds r3,r1,r0 / ldr r0,=0x594
 *         ldrb r2,[r3] / adds r3,r4,r0 / strb r2,[r3]
 * The old body formed the destination first.
 *
 * ========================================================================
 * THE MECHANISM, READ IN THE COMPILER -- AND WHY NO SPELLING OF ONE
 * STATEMENT CAN FIX IT
 * ========================================================================
 * `expand_assignment` (`expr.c:3402`) has exactly ONE path that expands the
 * RHS before the LHS, and it is gated at `expr.c:3604` on
 *     TREE_CODE (from) == CALL_EXPR
 * Every other assignment reaches `expr.c:3643`
 *     to_rtx = expand_expr (to, NULL_RTX, VOIDmode, EXPAND_MEMORY_USE_WO);
 * and only then `store_expr (from, to_rtx, want_value)`.  So within ONE
 * statement the destination address is ALWAYS formed first, full stop.
 *
 * Confirmed in the RTL: `.19.flow2` had the destination add as insn 48 and
 * the source add as insn 54 (lower number = created earlier = expand order),
 * and BOTH pooled-offset reload loads targeted the same hard register r0
 * (insns 192 and 195), so `.23.sched2`'s dependence table carried the
 * anti-dependence
 *     ;;  48  5  0  3  108  1  ... core : 258 168 64 57 195
 * i.e. 48 -> 195.  sched2 therefore cannot undo it either; the order is
 * nailed by expand plus the r0 reuse.
 *
 * THE CURE IS TO MAKE IT TWO STATEMENTS, so the source address is formed in
 * the earlier one:  `t = g[0x205];  p[0x594] = t;`
 *
 * ========================================================================
 * THE OLD HEADER'S MEASURED TABLE WAS WRONG ON ITS DECIDING ROW
 * ========================================================================
 * It recorded exactly this lever -- "`t = g[0x205]; p[0x594] = t;` (name the
 * VALUE)  74, 43" -- and concluded "BOTH MATERIALISATION LEVERS MAKE IT
 * WORSE", "naming is a floor, not a ceiling", "NEXT: nothing source-level."
 * The whole verdict rests on that one number, and THE TEMPORARY'S TYPE IS THE
 * WHOLE LEVER:
 *     `int t;`             49   (-4 bytes, relocations differ)
 *     `unsigned char t;`    3   (size exact, relocations exact)
 * The 43 is within noise of the `int` row.  `u8`, `unsigned short`, five
 * separate `unsigned char` temporaries and the comma form `t = g[s], p[d] = t`
 * all measure 3 as well.
 *
 * Measured and worse: all loads hoisted to the top (77); the constant store
 * before each copy (70); all copies then all constant stores (71); the
 * constant store between a copy's load and its store (70); hoisting copy 3's
 * load above pair 2's constant store (11); moving pair 2's constant store
 * after pair 3's (11); five temporaries with copy 3 hoisted (11); swapping
 * pair 3's two stores (14); the constant store through a named local (26).
 * Inert at 3: `gState[0x20c]` for `g[0x20c]`, `0x20c` for `0x83 << 2`.
 *
 * ========================================================================
 * THE REMAINING 3 -- A STRUCTURAL ONE-POINT sched2 GAP
 * ========================================================================
 * Ref indices 36-38 (0x48-0x4c), inside copy 3:
 *   ROM   lsls r0,r0,#2 | movs r3,#15 | strb r3,[r2]
 *   ours  movs r3,#15   | strb r3,[r2] | lsls r0,r0,#2
 * `.23.sched2` at t = 78:
 *     ;;  Ready list (t = 78):    270  102
 *     ;;      --> scheduling insn <<<102>>> on unit core
 *     insn 102  (set (reg:QI 3 r3) (const_int 15))   prio 90
 *     insn 270  (set (reg:SI 0 r0) (ashift r0, 2))   prio 89
 * `rank_for_schedule`'s first rung is priority, so a 1-point gap decides it
 * outright and the CLASS / dependent-count / LUID rungs are never reached.
 *
 * The gap is STRUCTURAL and arrives by TWO independent paths, each worth
 * exactly +1, which is why reordering statements cannot close it:
 *     270 -> 110 (`adds r3,r1,r0`, prio 88) + 1                = 89
 *     102 -> 103 (`strb r3,[r2]`,  prio 89) + 1                = 90
 *       103 -> 110 (WAR on r3) + 1                             = 89
 *       103 -> 112 (`zero_extend (mem:QI)`, prio 87) + 2       = 89
 * Cut either of 103's two edges and the other still holds it at 89, so
 * `movs #15` stays exactly one point above `lsls`.  The memory edge
 * 103 -> 112 is out of reach of the alias lever in any case: both sides are
 * char-precision references and `lang_get_alias_set`
 * (`c-common.c:3348-3351`) returns 0 for any char-precision reference.
 *
 * NEXT: the three insns need insn 270's priority raised or 102's lowered by
 * one, which no source-level reordering reaches.  Nothing else is open.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern unsigned char *galloc_ewram(int tag, int size);
extern unsigned char gState[];
extern void Func_801cf48(void);
extern int StartTask(void *fn, int pri);

void Func_801d014(void)
{
    unsigned char *p;
    unsigned char *g;
    unsigned char t;

    p = galloc_ewram(0x14, 0xc5 << 3);
    DMA3_CLEAR(p, 0xc5 << 3);
    g = gState;
    t = g[0x205];
    p[0x594] = t;
    p[0x599] = 0x18;
    t = g[0x206];
    p[0x595] = t;
    p[0x59a] = 0xf;
    t = g[0x83 << 2];
    p[0x596] = t;
    p[0x59b] = 3;
    t = g[0x20a];
    p[0x597] = t;
    p[0x59c] = 2;
    t = g[0x22a];
    p[0xb3 << 3] = t;
    p[0x59d] = 2;
    StartTask(Func_801cf48, 0xc8 << 4);
}
