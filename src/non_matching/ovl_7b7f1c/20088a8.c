/* OvlFunc_930_20088a8  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_c_a.s
 * Best screen: 5 instructions in disagreeing regions, of 24 (rom 24, ours 23).
 *
 * BLOCKER CLASS: constant-CSE across argument setup.
 *
 * Sibling of src/non_matching/ovl_7b7f1c/2008870.c -- same three calls, same
 * stack-argument pair, different literals.  The stack-arg-pair lever fixes the
 * same part it fixes there and is kept.
 *
 * What is left is the middle call, which passes -1 TWICE.  The ROM builds each
 * one separately:
 *
 *      mov r1, #0x1 / mov r2, #0x1 / mov r0, #0x64 / neg r1, r1 / neg r2, r2
 *
 * gcc builds -1 once and copies:
 *
 *      mov r2, #0x1 / neg r2, r2 / mov r1, r2
 *
 * WHAT WAS TRIED
 *
 *  1. Two separate named locals, `x = 1; y = 1;` negated independently.  gcc
 *     CSEs them -- they hold the same value and there is nothing to tell them
 *     apart.  5 of 24.
 *  2. Passing the literals directly, `__Func_808edac(0x64, -1, -1)`.
 *     BYTE-IDENTICAL to (1).
 *
 * The basic-block lever is the standard answer to constant-CSE in argument
 * setup, and it does not apply: this is straight-line code with no branch to
 * put the second constant behind.  That is the lever's clause (b) and it is a
 * hard requirement, not a preference.
 *
 * Two functions in this file now sit at 5 and 2 instructions for the same
 * reason -- argument setup that gcc arranges after the source has any say.
 */
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_808edac(int a, int b, int c);
extern void __MapActor_SetPos(int a, int x, int y);

void OvlFunc_930_20088a8(void)
{
    int e;
    int f;
    int x;
    int y;

    e = 0x15;
    f = 9;
    __Func_8010704(0x15, 0x49, 1, 1, e, f);
    x = 1;
    y = 1;
    __Func_808edac(0x64, -x, -y);
    __MapActor_SetPos(0xe, 0, 0);
}
