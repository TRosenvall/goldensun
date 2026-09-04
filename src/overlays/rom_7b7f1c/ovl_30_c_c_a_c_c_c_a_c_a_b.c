// fakematch
/* OvlFunc_930_2008870  --  0x02008870
 *
 * Cut out of goldensun/asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_c_a.s.
 *
 * Three straight-line calls: place a tile group, clear an actor, move actor
 * 0xe to a fixed spot.
 *
 * PARKED AT 2 OF 24 AND THE PARK WAS WRONG. Its words were that the placement
 * "is not reachable from the source". The residue was one instruction:
 *
 *      rom   mov r1, #0xac / mov r2, #0x98 / mov r0, #0xe / lsl r1, #17 / lsl r2, #16
 *      ours  mov r1, #0xac / mov r2, #0x98 / lsl r1, #17 / lsl r2, #16 / mov r0, #0xe
 *
 * The ROM slots `mov r0, #0xe` between the two bases and the two shifts. The
 * park tried five spellings -- named locals for the arguments, that local
 * assigned before or after the others, the declaration lever, compound shifts
 * -- plus the return-type lever and two flags. All of them varied ORDER among
 * ordinary locals. NONE tried a register pin.
 *
 * Pinning all three argument registers and placing the assignments in the ROM's
 * order matches:
 *
 *     register int p0 __asm__("r0");   p1 = 0xac;
 *     register int p1 __asm__("r1");   p2 = 0x98;
 *     register int p2 __asm__("r2");   p0 = 0xe;
 *                                      p1 <<= 17;  p2 <<= 16;
 *
 * The pins must be declared UNINITIALISED, because an initialiser would place
 * each `mov` at its declaration and the whole point is to interleave `p0`'s
 * between the other two assignments and the two shifts.
 *
 * WHY THE PARK'S REASONING FAILED, and it is the same failure as three others
 * withdrawn this round: "argument-setup order is fixed after these choices are
 * made" is true of ORDINARY locals, and the park generalised it to the source
 * as a whole. A pinned register is not an ordinary local -- it has two
 * independent knobs, declaration position and assignment position, and the
 * second reaches exactly the placements ordering cannot.
 *
 * The park's other findings stand and are still in the file: the two stack
 * arguments of __Func_8010704 are named locals, which fixed five of the
 * original seven. And its note that `--no-sched2` makes this WORSE (6 of 24) is
 * still the right reading -- the scheduler is doing the right thing here and
 * the residue was never its fault.
 */

extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_808edac(int a, int b, int c);
extern void __MapActor_SetPos(int a, int x, int y);

void OvlFunc_930_2008870(void)
{
    int e;
    int f;
    int x;
    int y;

    e = 0x15;
    f = 9;
    __Func_8010704(0x55, 9, 1, 1, e, f);
    __Func_808edac(0x64, 0, 0);
    {
        register int p0 __asm__("r0");
        register int p1 __asm__("r1");
        register int p2 __asm__("r2");
        p1 = 0xac;
        p2 = 0x98;
        p0 = 0xe;
        p1 <<= 17;
        p2 <<= 16;
        __MapActor_SetPos(p0, p1, p2);
    }
}
