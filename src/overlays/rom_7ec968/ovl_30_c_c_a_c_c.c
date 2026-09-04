// fakematch
/* OvlFunc_963_2008334  --  0x02008334
 *
 * Cut out of goldensun/asm/overlays/rom_7ec968/ovl_30_c_c_a_c.s, by hand:
 * the stem's _b suffix was already taken by a GENERATED .s belonging to an
 * elevated neighbour, so this went to _c with the remainder in _d.
 *
 * A cutscene setup block. Two actors get a speed, a facing, a position and an
 * animation, then two dialogue calls close it out.
 *
 * THE PLAIN SPELLING IS 35 OF 60 WITH THE LENGTH ALREADY EXACT, and the reason
 * is worth stating because it is the general case, not a quirk of this
 * function. The ROM materialises its constants AGAIN at every call:
 *
 *      mov r1, #0x80 / mov r2, #0x80 / mov r0, #8 / lsl r1, #9 / lsl r2, #8
 *      bl  __MapActor_SetSpeed
 *      mov r1, #0x80 / mov r2, #0x80 / mov r0, #9 / lsl r1, #9 / lsl r2, #8
 *      bl  __MapActor_SetSpeed
 *
 * Write that as named locals and gcc does the sensible thing instead: it
 * computes 0x80 << 9 and 0x80 << 8 once, parks them in r5 and r6, and feeds
 * every later call with `mov r1, r5`. The prologue widens to
 * `push {r5, r6, lr}` to pay for it. Writing the literals inline at each call
 * changes NOTHING -- byte-identical, still 35 -- because cse1 commons them
 * unconditionally and no flag reaches that pass.
 *
 * THE PIN BREAKS IT, AND THIS IS THE FINDING. Constant rematerialisation is
 * recorded as needing a DOMINATING BRANCH for cse to have a boundary it will
 * not cross. This function is straight-line -- not one conditional in sixty
 * instructions -- so by that rule it should be unreachable. It is not:
 *
 *     register int p0 __asm__("r0");
 *     register int p1 __asm__("r1");
 *     register int p2 __asm__("r2");
 *     p1 = 0x80; p2 = 0x80; p0 = 8; p1 <<= 9; p2 <<= 8;
 *     __MapActor_SetSpeed(p0, p1, p2);
 *
 * repeated per call, matches on the first try. r0 through r2 are CALL-CLOBBERED,
 * so pinning a constant there tells gcc the value cannot survive the `bl`, and
 * it has no choice but to build it again for the next one. The dominating
 * branch was never the mechanism -- it was only the usual way of arranging for
 * the value to be dead. A pin arranges the same thing directly, and it does so
 * in straight-line code.
 *
 * That also removes the r5/r6 spill, which is why the prologue narrows back to
 * `push {r5, lr}` on its own. r5 still appears, and correctly: the 7 shared by
 * the two __Func_8010704 calls IS live across a call in the ROM, so it stays an
 * ordinary local and gcc gives it the callee-saved register it needs.
 *
 * The per-call assignment ORDER is the ROM's, read off the listing, and it
 * varies between otherwise identical calls -- the two __Func_8092adc calls put
 * `mov r2, #0` in different places, which is exactly the kind of placement only
 * the pin's second knob reaches.
 */

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_963_2008334(void)
{
    register int p0 __asm__("r0");
    register int p1 __asm__("r1");
    register int p2 __asm__("r2");
    int e;

    __CutsceneStart();
    p1 = 0x80; p2 = 0x80; p0 = 8; p1 <<= 9; p2 <<= 8;
    __MapActor_SetSpeed(p0, p1, p2);
    p1 = 0x80; p2 = 0x80; p0 = 9; p1 <<= 9; p2 <<= 8;
    __MapActor_SetSpeed(p0, p1, p2);
    p2 = 0xc0; p0 = 8; p1 = 0x88; p2 <<= 1;
    __Func_809218c(p0, p1, p2);
    p2 = 0xc0; p0 = 9; p1 = 0x98; p2 <<= 1;
    __Func_80921c4(p0, p1, p2);
    p1 = 0x80; p0 = 8; p1 <<= 7; p2 = 0;
    __Func_8092adc(p0, p1, p2);
    p1 = 0x80; p2 = 0; p0 = 9; p1 <<= 7;
    __Func_8092adc(p0, p1, p2);
    p0 = 8; p1 = 1;
    __MapActor_SetAnim(p0, p1);
    e = 7;
    __Func_8010704(6, 0x1b, 1, 1, e, 0x1b);
    __Func_8010704(9, 0x1a, 2, 1, e, 0x1a);
    __CutsceneEnd();
}
