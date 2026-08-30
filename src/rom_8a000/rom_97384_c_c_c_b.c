/* Cluster Func_8097adc..Func_8097adc extracted from goldensun/asm/rom_8a000/rom_97384_c_c_c.s.
 *
 * Total .text for this TU = 120 bytes.
 *
 * FROM THE BRANCH-OVER-POOL CLASS; the pool needed no help.
 *
 * READ THE POOL ORDER OFF THE HAND-WRITTEN .s BEFORE COMPILING ANYTHING. The
 * ROM pulls 0x7fff, 0, 0x294a and 0x5294 out as explicit `.word`s ahead of
 * `.pool`, which is gcc's HImode-constant group. That group appears ONLY when
 * the literals are written straight into the halfword stores -- an `int`
 * intermediate moves them out. The previous park had `v = 0x7fff; *q = v;`,
 * which is the wrong half of that lever, and inherited a pointer/offset
 * machinery from its sibling that does not belong here at all. 26 differing
 * to zero on the first attempt once the pool order was read.
 *
 * The four absolute addresses go in as plain literals; gcc CSEs two of them
 * off the first and pools the remaining one by itself.
 */
extern char *iwram_3001e8c;
extern void Func_8097868(void);
extern void StopTask(void (*f)(void));
extern void _SetUIColor(int a, int b);
extern unsigned char gState[];

void Func_8097adc(void)
{
    char *p;

    p = iwram_3001e8c;
    StopTask(Func_8097868);
    *(unsigned short *)0x50001e2 = 0x7fff;
    *(unsigned short *)0x50001e6 = 0;
    *(unsigned short *)0x50001f6 = 0x294a;
    *(unsigned short *)0x50001f8 = 0x5294;
    _SetUIColor(gState[0x205], gState[0x206]);
    p[0xea4] = 0;
}
