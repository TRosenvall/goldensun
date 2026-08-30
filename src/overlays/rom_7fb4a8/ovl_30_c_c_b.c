/* Cluster OvlFunc_971_20091bc..OvlFunc_971_20091bc extracted from
 * goldensun/asm/overlays/rom_7fb4a8/ovl_30_c_c.s.
 *
 * TWO DECLARATIONS OF ONE CALLEE, AND THE CALL SITES PICK BETWEEN THEM.
 * This is the lever, and it is new.
 *
 * The function calls __CloseUIBox twice with the same arguments.  The ROM emits
 * them in DIFFERENT orders:
 *
 *      first call (result discarded)   mov r0, r5 / mov r1, #1
 *      second call (returned)          mov r0, r5 / mov r1, #1
 *
 * -- both r0 first -- but gcc, given one `extern int __CloseUIBox(int, int);`,
 * emits `mov r1, #1 / mov r0, r5` at the FIRST site and the ROM's order at the
 * second.  The park recorded that and called it the blocker: neither argument
 * is a split build, so the interleave lever has nothing to work with, and the
 * no-prototype lever moves both sites together and so can only ever fix one.
 *
 * THE RETURN TYPE IS WHAT DIFFERS BETWEEN THE TWO SITES, and a call whose
 * result is discarded can be given its own VOID-RETURNING alias of the same
 * symbol:
 *
 *      extern void CloseBoxV(int h, int n) __asm__("__CloseUIBox");
 *
 * The first site calls CloseBoxV, the second keeps the `int` prototype and is
 * returned.  Both orders come out right and the function matches exactly.
 *
 * This generalises the batch-147 prototype lever in the direction it most
 * needed: the declaration is a PER-CALL-SITE choice, not a per-callee one.
 * When one site of a repeated call is right and another is wrong, the answer is
 * two declarations, not one different one.
 *
 * ALSO CONFIRMED HERE, from the park: the epilogue register is the return type.
 * The ROM ends `pop {r5} / pop {r1} / bx r1`; gcc pops the return address into
 * r0 when r0 is dead, i.e. when the function returns void.  Declaring this
 * function `int` and writing `return __CloseUIBox(h, 1);` is what produces the
 * ROM's epilogue.
 *
 * Screened and rejected, all still 2 differing: storing the first call's result;
 * an alias with an EMPTY parameter list rather than a void return; the alias
 * applied to the returned site instead.  Deleting __CloseUIBox's declaration
 * outright also matches, but only because it makes both sites unprototyped --
 * it is the same fix by accident and does not survive a third call site.
 */
extern void __PlaySound(int id);
extern int __Func_8017658(int id, int a, int b, int c);
extern int __Func_8017364(void);
extern void __WaitFrames(int n);
extern void __Func_801faa8(void);
extern int __CloseUIBox(int h, int n);
extern void CloseBoxV(int h, int n) __asm__("__CloseUIBox");

int OvlFunc_971_20091bc(void)
{
    int h;

    __PlaySound(0x55);
    h = __Func_8017658(0x292a, 5, 4, 1);
    while (__Func_8017364() == 0)
        __WaitFrames(1);
    __Func_801faa8();
    CloseBoxV(h, 1);
    __WaitFrames(1);
    h = __Func_8017658(0x292b, 5, 4, 1);
    while (__Func_8017364() == 0)
        __WaitFrames(1);
    return __CloseUIBox(h, 1);
}
