/* Func_8091f14 -- 0x08091f14, split out of asm/rom_8a000/rom_91584_c_c_a_c_a_a.s.
 *
 * Sets a mode word in gState from the caller's flags, records the result of
 * Func_808b074, and on one state value nudges the field actor before handing
 * control on.
 *
 * FOUR LEVERS, all previously recorded, applied in this order. The path was
 * 49 differing -> 48 -> 30 -> 10 -> 2 -> exact, and every step is a lever that
 * is already written down -- worth noting because nothing here was new until
 * the last one.
 *
 * 1. NAME THE gState BASE (49 -> 48, and size exact). Indexing the symbol
 *    directly at two different offsets makes gcc pool `gState + 0x234` as a
 *    single word and derive the other with `sub r3, #64`. The ROM keeps the
 *    plain symbol in a register and computes both offsets. `gs = gState;` is
 *    the batch-266 base/offset split, applied to a base used twice rather than
 *    to one constant offset.
 *
 * 2. ASSIGN THE BASE *AFTER* THE CONDITIONAL CALL (30 -> 10). The ROM's
 *    `ldr r6, =gState` sits at the join label, not in the prologue, so the
 *    value is born after the branch. Written at the top it is live across the
 *    call, its range is the longest in the function, and local-alloc's
 *    priority formula puts it LAST -- r8 instead of the ROM's r6, rotating
 *    three registers. Moving one assignment down fixes the whole rotation.
 *
 * 3. REASSIGN THE PARAMETER (46 -> 30). The ROM computes `a & 0xff` into r0
 *    and calls straight out of it. A separate local lands in r3 and costs a
 *    `mov r0, r3`; `a &= 0xff;` keeps the value in the parameter's own
 *    register.
 *
 * 4. NAME THE STORED VALUE (10 -> 2). `*(u16 *)(gs + 0x234) = (b + 0x12c) | m;`
 *    builds the destination address first; the ROM builds the value first.
 *    Hoisting the whole right-hand side into `v` swaps the two three-instruction
 *    constant builds into the ROM's order. `v = b + 0x12c;` with the `| m` left
 *    at the store is only half the effect (4 differing), and writing the OR the
 *    other way round is worse (11).
 *
 * THE LAST TWO WERE ARGUMENT FILL ORDER, AND THE RECORDED RULE NEEDS WIDENING.
 * The ROM ends `mov r1, #0 / mov r0, #0 / bl Func_808b320`; a `void` prototype
 * fills r0 first. The notebook says the deferral is caused by the PRECEDING
 * call's return type -- and here that is measurably not it: making
 * Func_808adf0, GetFieldActor or Func_809537c return `int` is INERT, all three
 * still 2 differing.
 *
 * What moves it is THE CALLED FUNCTION'S OWN RETURN TYPE. `int Func_808b320(...)`
 * is exact, and so is dropping its prototype entirely (implicit int). This is
 * the last call in the function and its result is unused, so the rule as
 * written did not cover the case.
 *
 * The explicit `int` is used here rather than the implicit declaration: the
 * tree declares elsewhere that this callee returns void, and the notebook
 * already records that a callee's return type is a PER-CALL-SITE fact about
 * what the original TU declared, not a property of the function.
 *
 * EXACT: 124 bytes, 55 encodings, 7 relocations, measured three times, and
 * clean on tools/tryc.py.
 */
#include "gba/types.h"

extern unsigned char *iwram_3001ebc;
extern unsigned char gState[];
extern void Func_809537c(int a);
extern int Func_808b074(int a);
extern void *GetFieldActor(int id);
extern void Func_808adf0(void *p);
extern int Func_808b320(int a, int b);

void Func_8091f14(int a, int b)
{
    u8 *base;
    u8 *gs;
    int m;
    int r;
    int v;

    base = iwram_3001ebc;
    m = a & 0x800;
    a &= 0xff;
    if (m == 0)
        Func_809537c(a);
    gs = gState;
    v = (b + 0x12c) | m;
    *(u16 *)(gs + 0x234) = v;
    r = Func_808b074(b);
    *(u16 *)(base + 0x17c) = r;
    if (*(s16 *)(base + 0x19e) == 3)
        Func_808adf0((u8 *)GetFieldActor(*(int *)(gs + 0x1f4)) + 8);
    Func_808b320(0, 0);
}
