/* YesNoMenu2  [rom_15000]  --  0x08028e54
 *
 * Source asm: goldensun/asm/rom_15000/rom_23178_a_a_c_c_a.s
 *
 * Blocker: THE POOL TELL, with the namespace unidentified.
 *
 * The ROM loads 0x24 from the literal pool at the top of the function and keeps
 * it in r8 across three calls:
 *
 *     ldr r3, =0x24 / ... / mov r8, r3 / ... / mov r3, r8 / bl Func_80288a8
 *
 * 0x24 fits in an eight-bit `mov`, and gcc never pools what it can `mov`. So
 * the operand was a SYMBOL REFERENCE in the original source -- the same tell
 * that identified the area ids, the message ids and the file ids. Written as a
 * literal, gcc materialises it at the call site and the register allocation of
 * the whole prologue shifts with it: 16 of 34.
 *
 * WHAT IS MISSING IS THE NAMESPACE, not the mechanism. This tree defines such
 * operands by value in a .sym -- `_AREA_35 = 0x35;` and so on -- which emits no
 * bytes and asserts nothing beyond the value. Doing that here would need a name,
 * and there is not enough evidence for one:
 *
 *   * Func_80288a8 has exactly ONE caller in the whole ROM, this function, so
 *     the parameter cannot be triangulated from other call sites the way
 *     __Func_8091f90's area id was (two elevated files pass _AREA_51 and
 *     _AREA_4d to it).
 *   * only one other place in the ROM pools 0x24 --
 *     asm/overlays/rom_79e5c0/ovl_30_c_a_a_a_a.s:17 -- and whether it is the
 *     same kind of value is unknown.
 *
 * SO THE NEXT STEP IS TO READ Func_80288a8 (asm/rom_15000/rom_23178_a_a_a_a.s,
 * line 10840) and see what it does with its fourth argument. That is a
 * sixty-instruction read and it would settle both sites at once. Inventing a
 * namespace for a single function would be worse than leaving this parked.
 *
 * Everything else about the translation is believed right; with the literal in
 * place the instruction stream is the same length and the only differences are
 * where 0x24 is built and the register shuffling that follows from it.
 */
extern void Func_80284dc(void);
extern void AddMenuBarOption(int a);
extern void Func_80288a8(int a, int b, int c, int d);
extern int Func_8028574(int a);
extern void Func_802851c(void);

int YesNoMenu2(int a, int b, int c)
{
    int k;
    int r;

    k = 0x24;
    Func_80284dc();
    AddMenuBarOption(5);
    AddMenuBarOption(6);
    Func_80288a8(a, b, 3, k);
    r = Func_8028574(c);
    Func_802851c();
    if (r == -1)
        r = 1;
    return r;
}
