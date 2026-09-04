/* OvlFunc_924_200cf44  [overlays/rom_7ac2d8]
 *
 * Source asm: goldensun/asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_c_c.s
 * (the path this park recorded originally, ovl_35b8_a_a_c_a_c.s, is stale --
 * the file was split since.  Locate a park's asm by grep, not by its header.)
 *
 * BLOCKER CLASS: a TWO-STATE argument schedule.  2 of 28 in both states.
 *
 * REWRITTEN after the pin lever was applied.  The previous park called this an
 * "arg-interleave at an UNGUARDED site" and stopped there.  That framing was
 * incomplete: the pin DOES reach the instruction the park called unreachable.
 * It just buys it by breaking something else.
 *
 *     ROM        ldr r3 / mov r1,#210 / mov r2,#150 / lsl r2 / mov r0,#11 / lsl r1
 *     baseline   ldr r3 / mov r1,#210 / mov r2,#150 / lsl r2 / lsl r1 / mov r0,#11
 *     any pin    ldr r3 / mov r2,#150 / mov r1,#210 / lsl r2 / mov r0,#11 / lsl r1
 *
 * Read those three lines together, because they are the whole finding.  The
 * ROM's head is `mov r1, mov r2`; its tail is `mov r0, lsl r1`.  The baseline
 * gets the HEAD right and the TAIL wrong.  Pinning r0 gets the TAIL right --
 * exactly the two instructions the old park despaired of -- and flips the HEAD.
 * Both states are 2 of 28.  The ROM is a third state neither reaches.
 *
 * THE TIE IS STRUCTURAL, which is the only kind that counts.  Six forms, all
 * BYTE-IDENTICAL to each other:
 *
 *   1. all three of r0/r1/r2 pinned, r1 initialised at its declaration and
 *      declared first, r2 and r0 uninitialised and assigned in ROM order
 *   2. all three pinned, r1 and r2 both initialised at their declarations
 *   3. r0 pinned ALONE, the two shifts left as literal expressions in the call
 *   4. r0 pinned alone, the two bases as named locals shifted at the call
 *   5. r0 pinned alone, both shifts precomputed into named locals
 *   6. r0+r1 pinned; and 7. r0+r2 pinned, the other base left an expression
 *
 * Those vary the pin COUNT (one, two, three), WHICH registers are pinned,
 * initialised against uninitialised, declaration position, and three separate
 * spellings of the arguments.  Every one of them lands on the same six
 * instructions.  The head flip is caused by the mere PRESENCE of a pin on r0,
 * not by where its assignment or declaration sits.
 *
 * WHY THIS MATTERS BEYOND THIS FUNCTION.  The pin's two knobs -- declaration
 * position and assignment position -- are documented as reaching placements
 * that ordinary locals cannot, and four parks fell to exactly that this round.
 * This is the boundary of that lever, measured: the knobs move the pinned
 * register's own `mov`, and they do NOT control the relative order of two
 * OTHER movs that the post-allocation scheduler is free to swap.  Pinning r1
 * and r2 as well (forms 1, 2, 6, 7) does not pin their ORDER.
 *
 * Previously measured, all still 2 unless noted, none combined with a pin:
 * struct field `|=` 2; `unsigned char *` with `*p = 8 | *p` 4; naming both
 * split builds 2; naming only the first 2; naming only the second 4; naming
 * the slot 2; a shared slot local across both calls 2; __MapActor_SetPos with
 * no prototype 2; --no-rerun-cse 2; --no-sched2 8; --O1 10.
 *
 * NEXT THING TO TRY: a data dependence that forces the head order and then
 * folds away -- `p1 = 0xd2; p2 = p1 - 0x3c;` -- on the theory that the order
 * survives the fold.  Untested; likely CSE folds it and takes the order with
 * it.  Everything reachable by ORDER and by PINS has been tried.
 *
 * STILL WORTH ACTING ON ELSEWHERE, and now with evidence: the near-twin park
 * src/non_matching/ovl_798dc4/2008d68.c -- same `|= 8` at iwram_3001f30 +
 * 0x71c, same call tail -- is parked at 2 of 22 on an `orr` register-role
 * swap.  The struct-typing lever is CONFIRMED correct here (field `|=` gives 2
 * where `unsigned char *` gives 4), so that park should be re-attacked with a
 * struct and a named field.
 */
typedef struct S {
    unsigned char pad[0x71c];
    unsigned char flags;
} S;

extern unsigned int iwram_3001f30;
extern void __MapActor_SetPos(int a, int x, int y);
extern void __Func_8096fb0(int a, int b);
extern void __Func_80970f8(int a, int b);
extern void __Func_809728c(void);
extern void __FieldMove(int a);
extern void __Func_8097174(void);

void OvlFunc_924_200cf44(void)
{
    S *p;

    p = (S *)iwram_3001f30;
    __MapActor_SetPos(0xb, 0xd2 << 18, 0x96 << 18);
    __Func_8096fb0(0x5d, 1);
    __Func_80970f8(3, 0xb);
    p->flags |= 8;
    __Func_809728c();
    __FieldMove(1);
    __Func_8097174();
}
