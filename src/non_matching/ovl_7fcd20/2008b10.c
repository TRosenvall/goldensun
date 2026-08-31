/* OvlFunc_974_2008b10 -- 0x02008b10  (asm/overlays/rom_7fcd20/ovl_30_c_c_a_c_a_c_a.s)
 *
 * BLOCKER: PRE hoisting of a repeated constant. 55 of 64, one line short.
 * See "When gcc HOISTS a repeated constant, exactly: dominance" in
 * docs/elevation.md -- this is that rule, not a new shape.
 *
 * -0x64 is passed to __ModifyHP three times. Each occurrence costs `mov`+`neg`,
 * the first dominates the other two, so gcc computes it once at the top of the
 * function -- ahead of the first call -- and holds it in r5:
 *
 *     rom   mov r1, #0x64 / neg r1, r1        at every one of the three sites
 *     ours  mov r5, #0x64 / neg r5, r5        once, then `mov r1, r5`
 *
 * The hoist is what makes the body diverge everywhere after instruction 1, and
 * the one-line shortfall is the three rebuild pairs collapsing to three copies.
 *
 * THE PER-USE-SITE NAMING LEVER DOES NOT REACH IT, as predicted. Three separate
 * `int` locals, each holding -0x64:
 *
 *     assigned at the top of the function        55 differ
 *     assigned immediately before each use       55 differ
 *
 * Both byte-identical to the bare literal form. That lever is documented
 * against CSE of identical constants WITHIN one basic block, where it closed
 * OvlFunc_948_20095f0. This is the other pass: partial redundancy elimination
 * folds the initialisers to one rtx before it runs, so three named locals and
 * one literal are the same input to it. The two passes want different remedies
 * and only one of them has a remedy at all.
 *
 * The rest of the function reads cleanly and is worth keeping for whenever the
 * dominance class is reached: eight HP/PP adjustments, two unit records stamped
 * through __GetUnit, and four __CalcStats calls whose argument order really is
 * 0, 1, 3, 2 in the ROM. The 1 stored to both records and the 0x131 field
 * offset ARE hoisted by the ROM as well -- `push {r5, r6}` covers exactly those
 * two -- so those hoists are correct and only the -0x64 one is wrong.
 *
 * THIS FUNCTION SHOULD NEVER HAVE BEEN OFFERED. tools/filtered.py rejects a
 * candidate that repeats an expensive constant, but its detector only paired
 * `mov` with a later `lsl`, and `mov`+`neg` is a split two-instruction build in
 * exactly the same way -- docs/elevation.md says so explicitly under the
 * argument-order lever, and that sentence was read while writing the filter and
 * not acted on. The filter now pairs `neg` too; it rejects this function, and
 * the candidate pool drops from 9 to 5, so four of the nine on offer carried
 * the same defect. A detector written from a doc that already generalised the
 * shape should implement the generalisation, not the example.
 */
extern void __Func_801776c(int a, int b);
extern void __ModifyHP(int a, int d);
extern void __ModifyPP(int a, int d);
extern void *__GetUnit(int a);
extern void __CalcStats(int a);

void OvlFunc_974_2008b10(void)
{
    unsigned char *u;

    __Func_801776c(0xc1b, 1);
    __ModifyHP(0, -0x64);
    __ModifyHP(1, -0x64);
    __ModifyHP(2, -0x21);
    __ModifyHP(3, -0x64);
    __ModifyPP(0, -0x32);
    __ModifyPP(1, -0x28);
    __ModifyPP(2, -0x23);
    __ModifyPP(3, -0x14);
    u = (unsigned char *)__GetUnit(0);
    u[0x131] = 1;
    u[0xa0 << 1] = 1;
    u = (unsigned char *)__GetUnit(1);
    u[0x98 << 1] = 1;
    u[0x131] = 2;
    __CalcStats(0);
    __CalcStats(1);
    __CalcStats(3);
    __CalcStats(2);
}
