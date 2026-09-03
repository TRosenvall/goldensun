/* OvlFunc_959_200cf60  --  0x0200cf60
 * asm/overlays/rom_7e7574/ovl_9dc_c_c_c_a_a_c.s, line 202 (second of four).
 *
 * PARKED at 14 aligned of 147, length exact. Every other instruction -- the
 * jump table, all 18 slots, the case groupings, the cross-jumped tails, the
 * stack-argument pair, the epilogue -- is exact.
 *
 * BLOCKER CLASS: constant CSE, straight-line sub-case. THIS ONE IS CLOSED, and
 * the park exists to record a specimen rather than to invite another attempt.
 * See "Constant CSE inside ONE basic block: closed, with a number" in
 * docs/elevation.md. The short form: cse.c scores a pseudo at 1 and any other
 * expression at twice its rtx_cost, and arm_rtx_costs gives a Thumb CONST_INT
 * as a SET the value 0 below 256 and COSTS_N_INSNS(2) or (3) at or above it.
 * The register therefore always wins above 256. COST is a property of the
 * const_int, and every C spelling folds to the same const_int before cse runs,
 * so THERE IS NO SPELLING. Do not sweep this again.
 *
 * The residue is exactly that: gcc keeps 0xc80 in a callee-saved register across
 * the task-start calls where the ROM rebuilds `mov r1, #0xc8 / lsl r1, #4` at
 * each site, which costs a push and a pop the ROM does not have. Confirmed at
 * -da: by .03.cse -- the FIRST cse pass -- the three sets in one block are
 * already collapsed, and .09.cse2, the rerun that -fno-rerun-cse-after-loop
 * disables, is byte-identical to it. The flag cannot help because the damage
 * precedes it, and it measures 20 against 20.
 *
 * THE [cse] MARKER WAS RIGHT ABOUT THE FUNCTION AND WRONG ABOUT THE CONSTANT.
 * It fired on the iwram repeats, which are in mutually exclusive switch arms and
 * reload correctly with no flag at all. The constant that actually blocks is
 * 0xc80, whose repeats are in ONE straight-line block -- which the detector's
 * "label between the repeats" test would have cleared. A useful limitation to
 * know: the detector finds a repeat, not the repeat that matters.
 *
 * TWO LEVERS DID FIRE.
 *
 * THE RETURN-TYPE ORACLE PAID ON A VALUE NOBODY USES. Declaring the task-start
 * callee `int` rather than `void` is worth 17 to 14 with an identical body. The
 * ROM ignores the result at all five sites; the declaration still reorders the
 * pool load of the function address against the argument build. The converse
 * holds on the same function: one of the other callees must stay `void`, and
 * declaring IT `int` costs 22. So the oracle is per-callee and both directions
 * are real -- which is why reading r0's fill position at each call is worth more
 * than a guess about what a helper "probably" returns.
 *
 * THE STACK-ARGUMENT PAIR LEVER applied cleanly, 20 to 17: both values are built
 * fresh into separate registers before either is stored, so both need names
 * assigned next to the call. Note the second value also appears as a REGISTER
 * argument at the same site and the ROM builds it twice -- so the stack copy
 * gets a local and the register argument stays a literal. Naming both is wrong.
 *
 * ~18 source spellings and ~25 flags measured; nothing except those two levers
 * moved it, and -fno-schedule-insns2 and -O1 are much worse (40).
 *
 * Screened with tools/tryc.py --align. Not built.
 */
extern unsigned char gState[];
extern unsigned char *iwram_3001ebc;

extern void OvlFunc_959_200d470(void);
extern void OvlFunc_959_2009718(void);
extern void OvlFunc_959_200975c(void);
extern void OvlFunc_959_20097bc(void);
extern void OvlFunc_959_2009528(void);
extern void OvlFunc_959_20099e8(void);
extern void OvlFunc_959_200969c(void);

extern int __StartTask(void (*fn)(void), int prio);
extern void __WaitFrames(int n);
extern void __Func_800fe9c(void);
extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __Func_80108c4(int a);
extern void __Func_8092950(int a, int b);

void OvlFunc_959_200cf60(void)
{
    unsigned char *g;
    int e;
    int f;

    OvlFunc_959_200d470();
    g = gState;
    switch (*(short *)(g + (0xe1 << 1))) {
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
        *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x200;
        __StartTask(OvlFunc_959_2009718, 0xc8 << 4);
        __StartTask(OvlFunc_959_200975c, 0xc8 << 4);
        __StartTask(OvlFunc_959_20097bc, 0xc8 << 4);
        __Func_80108c4(0xe0 << 4);
        break;
    case 12:
    case 19:
        *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x209;
        __Func_80108c4(0xc0 << 4);
        break;
    case 16:
    case 17:
    case 18:
        *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x200;
        __StartTask(OvlFunc_959_2009528, 0xc8 << 4);
        __StartTask(OvlFunc_959_20099e8, 0xc8 << 4);
        __WaitFrames(1);
        __Func_800fe9c();
        __WaitFrames(1);
        e = 0x6e;
        f = 9;
        __Func_80105d4(0x65, 9, 0xa, 8, e, f);
        __Func_80108c4(0xe0 << 4);
        break;
    case 13:
    case 14:
    case 15:
        *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x200;
        __StartTask(OvlFunc_959_200969c, 0xc8 << 4);
        break;
    default:
        *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x200;
        __Func_80108c4(0xe0 << 4);
        break;
    }
    __Func_8092950(0x12, 1);
    __Func_8092950(0x11, 1);
    __Func_8092950(0x15, 1);
    __Func_8092950(0xc, 1);
    __Func_8092950(0xd, 1);
    __WaitFrames(1);
}
