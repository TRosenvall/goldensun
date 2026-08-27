/* OvlFunc_943_200b950  --  0x0200b950
 *
 * Cut out of goldensun/asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_c_a.s.
 *
 * Repaints a doorway in four strips and then one tile of the wall above it.
 *
 * THE TWO SHARED STACK VALUES ARE NAMED, THE THIRD IS NOT, and the ROM says
 * which is which. `mov r5, #1 / mov r6, #5` are set once before the first call
 * and re-stored at each of the four; the `4` in the third call is built fresh
 * into r3. So two locals and one literal, not three of either.
 *
 * That is batch 95's rule -- two DIFFERENT values in the two stack slots need
 * two registers, so both get named -- and the fourth call passes the same local
 * to both slots, which the ROM shows as `str r5` twice. The fifth call takes a
 * different pair (0x11 and 0x28) and gets its own two locals.
 *
 * Matched on the first screen.
 */
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_943_200b950(void)
{
    int one;
    int five;
    int e;
    int f;

    one = 1;
    five = 5;
    __CopyMapTiles(0x4e, 0x27, 0x4e, 0x28, five, one);
    __CopyMapTiles(0x4e, 0x27, 0x4e, 0x29, five, one);
    __CopyMapTiles(0x4e, 0x27, 0x4f, 0x2a, 4, one);
    __CopyMapTiles(0x4e, 0x27, 0x52, 0x2b, one, one);
    e = 0x11;
    f = 0x28;
    __Func_8010704(0x11, 0x26, 5, 2, e, f);
}
