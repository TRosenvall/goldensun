/* OvlFunc_954_2009214  --  0x02009214
 *
 * 184 instructions of cutscene behind a single `if`. Its residue is the same
 * blocker as OvlFunc_943_200a9d4 in this batch -- CONSTANT CSE ACROSS CALLS --
 * but the cure here is the opposite of a pin, and the pair is worth reading
 * together.
 *
 * THE BLOCKER. Four values are each built twice or more inside one
 * straight-line block: `0xa8 << 16`, `0x9c << 1`, `0xc0 << 9` and `0xc0 << 8`.
 * Each is a two-instruction mov+lsl, so local CSE builds it once and parks it
 * in a call-saved register. With r5, r6 and r7 already spoken for, gcc reached
 * into r8-r11 and grew an eleven-instruction high-register prologue and
 * epilogue the ROM does not have: 198 lines against 184, 191 differing.
 *
 * THE CURE IS TO LOWER THE REFERENCE COUNT, NOT TO PIN. Giving each duplicated
 * value its OWN named local, assigned in the dominating block, takes every
 * pseudo down to REG_N_REFS == 2, and local-alloc then rematerialises it at
 * the use instead of keeping it live across the calls. 191 differing -> 14.
 * One named local per duplicated value is enough; naming both copies buys
 * nothing.
 *
 * AND THE INVERSE, IN THE SAME FUNCTION. `0x94 << 1` is used at FOUR sites and
 * the ROM does keep it in r5 across all of them. Naming it costs 77 differing:
 * a named pseudo with four refs is placed differently and takes the wrong
 * register. Left as a plain literal at all four sites, CSE hoists it itself,
 * with the ROM's interleaved `mov r5,#0x94 / ... / lsl r5,#1` placement. So the
 * rule is not "name duplicated constants" but "name the ones gcc should NOT
 * hoist, and leave the ones it should" -- the same polarity as the
 * loop-invariant rule, appearing twice in opposite directions here.
 *
 * FLAGS DO NOT REACH IT. -fno-rerun-cse-after-loop is byte-identical to the
 * default, 191 differing either way. This is local CSE.
 *
 * NAMING ALSO FIXES ARGUMENT ORDER (14 -> 3). The residual diffs were all one
 * shape: the ROM issues `mov r0, #imm` BEFORE the trailing lsl, we issued it
 * last. A named pseudo is precomputed by precompute_register_parameters before
 * any hard register is loaded, which frees the scheduler to hoist the cheap
 * `mov r0` in front of the shifts.
 *
 * ONE PROTOTYPE IS LOAD-BEARING, AND THE TEMPLATE'S CHOICE DOES NOT TRANSFER
 * (3 -> 0). __Func_8092adc must be DECLARED here. The neighbouring file
 * deliberately leaves that callee undeclared to push r0 to the end of the
 * fill; at this site the ROM wants `mov r1 / mov r0 / lsl r1 / mov r2`, which
 * needs the prototype. The three callees that DO want r0 last here are
 * OvlFunc_common1_1078, _15b8 and _5e4 -- giving those prototypes costs 14
 * differing. Prototype presence is a per-site lever, not a per-file one.
 */
extern unsigned char gState[];

extern void __MessageID(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __ActorMessage(int slot, int n);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __SetCameraTarget(int a, int b);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern int OvlFunc_common1_4cc(int a, int b);
extern void OvlFunc_common1_2c4(void);
extern void OvlFunc_common1_1254(int a);
extern void OvlFunc_common1_1314(int a);
extern void OvlFunc_common1_2060(void);
extern void OvlFunc_common1_588(int a, int b);

void OvlFunc_954_2009214(int a)
{
    unsigned char *g;
    int r;
    int s1, s2, e1, e2, f1, f2, p1, p2;
    int m, q1, q2, x, z;

    g = gState;
    if (*(short *)(g + (0xe1 << 1)) == 2) {
        OvlFunc_common1_2c4();
        return;
    }
    __CutsceneStart();
    r = OvlFunc_common1_4cc(a, 1);
    z = 0xa8 << 16;
    m = 0x9c << 1;
    q1 = 0xc0 << 9;
    q2 = 0xc0 << 8;
    s1 = 0x80 << 9;
    s2 = 0x80 << 8;
    e1 = 0x81 << 1;
    e2 = 0x3c;
    f1 = 0x83 << 1;
    f2 = 0x3c;
    p1 = 0x98 << 1;
    p2 = 0xb8;
    x = 0x9c << 17;
    if (r == 0) {
        __MessageID(0x208c);
        __Func_80933d4(0xc0 << 10, 0xc0 << 7);
        __Func_80933f8(0xa4 << 17, -1, 0xa8 << 16, 1);
        __Func_8093530();
        __ActorMessage(a, 0);
        OvlFunc_common1_1078(0, 0x8c << 1, 0xc8);
        __MapActor_SetSpeed(0, s1, s2);
        __Func_80921c4(0, 0xb4 << 1, 0xc8);
        __CutsceneWait(0x1e);
        __MapActor_Emote(0, e1, e2);
        __ActorMessage(a, 0);
        __Func_80921c4(0, 0x9c << 1, 0xc8);
        __CutsceneWait(0x1e);
        __Func_8092adc(0, 0xc0 << 8, 0xa);
        __MapActor_Emote(0, f1, f2);
        __MapActor_SetSpeed(0, 0xc0 << 9, 0xc0 << 8);
        OvlFunc_common1_15b8(0, 0x94 << 1, 0xb8);
        OvlFunc_common1_15b8(0, 0x94 << 1, 0x98);
        OvlFunc_common1_15b8(0, m, 0x98);
        __Func_8092adc(0, 0x80 << 7, 0xf);
        OvlFunc_common1_2060();
        OvlFunc_common1_1314(0);
        OvlFunc_common1_2060();
        OvlFunc_common1_1314(0);
        __MapActor_SetSpeed(0, q1, q2);
        __Func_80921c4(0, p1, p2);
        __Func_80921c4(0, 0x94 << 1, 0xc0);
        __Func_80921c4(0, 0x94 << 1, 0xc8);
        __Func_8092adc(0, 0, 0xf);
        OvlFunc_common1_2060();
        OvlFunc_common1_1314(0);
        OvlFunc_common1_2060();
        OvlFunc_common1_1314(0);
        __MapActor_SetAnim(0, 1);
        __ActorMessage(a, 0);
        OvlFunc_common1_1254(0);
        __SetCameraTarget(0, 0);
        __MapActor_SetPos(9, x, z);
        OvlFunc_common1_588(a, 1);
    } else if (r == 1) {
        __MessageID(0x208b);
        __ActorMessage(a, 0);
    }
    OvlFunc_common1_5e4(r, a, 1);
    __CutsceneEnd();
}
