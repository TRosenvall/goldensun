/* Cluster Func_8019da8..Func_8019da8 extracted from goldensun/asm/rom_15000/rom_19d2c.s.
 *
 * Total .text for this TU = 160 bytes (= 0xa0). Never attempted before batch 273.
 * No pins, no flags.
 *
 * TWO INSTRUCTIONS OFF ON THE FIRST CANDIDATE, and the fix is that the `-4` passed as
 * Func_801ec6c's sixth argument is a NAMED LOCAL, not a literal.
 *
 * As a literal gcc rematerialises it at the call site rather than paying a
 * callee-saved register for it, and the function then spends only r8-r10 where the ROM
 * also pushes r11. Naming it buys the extra register and the push list matches.
 *
 * That is the recorded "a register you do not spend is a value the ROM carries that
 * your reading missed" tell, with the PUSH LIST as the cheap signal -- read the
 * prologue before hunting the body.
 */
/* Func_8019da8  --  0x08019da8
 *
 * OpenMenuById.  Resolves the portrait through GetPortrait, opens a UI box
 * whose width depends on the +0xEA4 style byte, and populates it via
 * Func_801ec6c.  Returns the box handle, or 0.
 *
 * Load-bearing: the -4 passed as Func_801ec6c's sixth argument is a NAMED
 * LOCAL, not a literal.  Written as a literal, gcc rematerialises it
 * (`mov r3, #4 / neg r3, r3`) at the call site rather than paying a
 * callee-saved register, and the function uses only r8..r10; the ROM keeps it
 * in r9 and so pushes r11 as well.  Naming it gives the ROM's
 * `mov r9, r3 / mov r8, r9`.  Func_801ec6c's declared return type is inert
 * here (void / int / void * all match).
 */
extern unsigned char *iwram_3001e8c;
extern int GetPortrait(int b);
extern int CreateUIBox(int a, int b, int c, int d, int e);
extern void Func_801ec6c(int b, int c, unsigned int a, int d, int e, int f);

int Func_8019da8(int a0, int a1, int a2, int a3)
{
    unsigned char *base;
    int win;
    int k;
    int j;

    base = iwram_3001e8c;
    if (GetPortrait(a0) == -1)
        return 0;
    j = -4;
    k = j;
    if (base[0xea4] != 0) {
        win = CreateUIBox(a2, a3, 6, 5, 2);
        k = 0;
    } else {
        win = CreateUIBox(a2, a3, 5, 5, 2);
    }
    if (win != 0)
        Func_801ec6c(a0, a1, -1, win, k, j);
    return win;
}
