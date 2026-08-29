/* OvlFunc_909_2009984 -- NON-MATCHING.
 * Blocker class: ARGUMENT INTERLEAVE, one-expensive-value variant.
 * 19 lines against the ROM's 18, 14 differing.
 *
 * Three identical calls, each `mov r1, #K / mov r0, #n / lsl r1, #8 /
 * mov r2, #0` -- the cheap slot id BETWEEN the base and its shift. gcc emits
 * the shift first and the cheap movs after, every time.
 *
 * Same class as src/non_matching/ovl_79c738/2008150.c, which carries the
 * analysis, and settled by probe in docs/elevation.md, "The argument
 * interleave: settled by probe". Eleven source forms were tested there and
 * none moves it. Screened here only because the census filter passes calls
 * whose last setup line is cheap -- which is the false negative recorded in
 * ovl_78c76c/20095d4.c.
 *
 * Nothing further was tried, because the class is settled.
 */
extern void __Func_8092adc(int a, int b, int c);

void OvlFunc_909_2009984(void)
{
    __Func_8092adc(1, 0xc0 << 8, 0);
    __Func_8092adc(2, 0xc0 << 8, 0);
    __Func_8092adc(3, 0xa0 << 8, 0);
}
