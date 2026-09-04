// fakematch
/* OvlFunc_974_2008b10  --  0x02008b10
 *
 * From goldensun/asm/overlays/rom_7fcd20/ovl_30_c_c_a_c_a_c_a_c.s, which held
 * this function alone, so no split was needed.
 *
 * A scripted party-damage step: eight HP/PP adjustments, two unit records
 * stamped, four stat recalculations.
 *
 * PARKED AT 55 OF 64, one line short, on PRE HOISTING OF A REPEATED CONSTANT.
 * `-0x64` goes to __ModifyHP three times; each occurrence costs `mov`+`neg`,
 * the first dominates the other two, so gcc computes it once and holds it in
 * r5 while the ROM rebuilds it at every site:
 *
 *     rom   mov r1, #0x64 / neg r1, r1        at all three
 *     ours  mov r5, #0x64 / neg r5, r5        once, then `mov r1, r5`
 *
 * THE PARK'S NEGATIVE RESULT WAS RIGHT AND ITS REASON IS WORTH KEEPING. Three
 * separate `int` locals holding -0x64 -- assigned at the top, and assigned
 * immediately before each use -- are both byte-identical to the bare literal at
 * 55 differing. The per-use-site naming lever is documented against CSE of
 * identical constants inside one block; this is partial redundancy elimination,
 * which folds the three initialisers to one rtx BEFORE that lever can matter.
 * Three named locals and one literal are the same input to it.
 *
 * A PIN DOES NOT ARGUE WITH PRE, it removes the question. Assigning the value
 * through `register int q1 __asm__("r1")` at each of the three sites forces the
 * rebuild, because r1 is call-clobbered and nothing can survive the `bl`. That
 * took the function from 55 differing and one line short to SEVEN, with the
 * length exact.
 *
 * THE REMAINING SEVEN WERE A POINTER THAT ADVANCES. The ROM does
 *
 *     add r0, r2 / strb r5, [r0, #0]
 *
 * modifying the unit pointer in place, where indexing gives
 * `add r3, r0, r2 / strb r5, [r3, #0]` into a fresh register. Writing
 * `u += 0xa0 << 1; *u = 1;` reproduces it -- the pointer-advance tell from
 * batch 190.
 *
 * ONLY THE FIRST RECORD ADVANCES. Doing the same to the second __GetUnit block
 * is much worse -- 67 lines against 64, 30 differing -- so that one genuinely
 * indexes, and the two blocks are written differently on purpose. Read each
 * store off the ROM rather than making the pair consistent.
 *
 * The park's other observations hold: the four __CalcStats calls really are in
 * the order 0, 1, 3, 2, and the ROM hoists the stored 1 and the 0x131 offset
 * itself -- `push {r5, r6}` covers exactly those two.
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
    {
        register int q1 __asm__("r1");
        q1 = 0x64;
        q1 = -q1;
        __ModifyHP(0, q1);
    }
    {
        register int q1 __asm__("r1");
        q1 = 0x64;
        q1 = -q1;
        __ModifyHP(1, q1);
    }
    __ModifyHP(2, -0x21);
    {
        register int q1 __asm__("r1");
        q1 = 0x64;
        q1 = -q1;
        __ModifyHP(3, q1);
    }
    __ModifyPP(0, -0x32);
    __ModifyPP(1, -0x28);
    __ModifyPP(2, -0x23);
    __ModifyPP(3, -0x14);
    u = (unsigned char *)__GetUnit(0);
    u[0x131] = 1;
    u += 0xa0 << 1;
    *u = 1;
    u = (unsigned char *)__GetUnit(1);
    u[0x98 << 1] = 1;
    u[0x131] = 2;
    __CalcStats(0);
    __CalcStats(1);
    __CalcStats(3);
    __CalcStats(2);
}
