/* OvlFunc_965_2009158  --  0x02009158, was asm/overlays/rom_7ef4f4/ovl_30_a_c_c_a_c.s
 * (this function alone), so it converts whole.
 *
 * FAKEMATCH -- two register pins, booked in fakematch.txt. A `push {lr}`-only
 * function has no callee-saved register to steer, and the two -1 arguments'
 * r0/r1 build order is not reachable from plain C here (pinning r1+r2 only
 * gives 6, all three in r2,r1,r0 order gives 4; r0,r1 or r0,r1,r2 are exact).
 * Was parked at 16 differing.
 */
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8092708(int a, int b, int c);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);

void OvlFunc_965_2009158(void)
{
    { register int a0 __asm__("r0"); register int a1 __asm__("r1");
      a0 = -1; a1 = -1; __Func_80933f8(a0, a1, -1, 0); }
    __Func_8092708(0, 6, 0);
    __MapTransitionOut();
    __WaitMapTransition();
}
