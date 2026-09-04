/* CORRECTION, next round: the "same value" reasoning below is WRONG.
 * A minimal reproducer shows DIFFERENT constants transpose identically, so
 * equality of the values is a coincidence of this case. The real rule is that
 * gcc emits the two movs in the order their consuming SHIFTS appear, and the
 * ROM here wants the movs and the shifts in OPPOSITE orders -- which is
 * unreachable, because writing the argument inline flips the mov pair and
 * takes the tail with it. Two mutually exclusive states, the ROM a third.
 * See "the same-value movs class is really MOV ORDER SLAVED TO SHIFT ORDER"
 * in docs/elevation.md. The park stands; its diagnosis is superseded.
 */
/* OvlFunc_939_2008ff0  --  0x02008ff0  [asm/overlays/rom_7c460c/ovl_314_c_a_c_a.s]
 *
 * NOT MATCHING. Best screen 2 of 157, LENGTH EXACT. The candidate kept below
 * is that form; it is two instructions from done.
 *
 * A long cutscene behind one save flag. Chosen by tools/templated.py at 0.76
 * over seventeen shared symbols.
 *
 * FIVE LEVERS LANDED, 162 of 165 down to 2 of 157:
 *
 *   1. THE FLAG ID IS REMATERIALISED. `0x91 << 2` is tested and then set, and
 *      gcc holds it in r5 across the two calls where the ROM rebuilds
 *      `mov r0, #0x91 / lsl r0, #2` at each. Pinning r0 forces it.
 *
 *   2. SO IS THE SPEED PAIR. `0x80 << 10` and `0x80 << 9` feed three
 *      __MapActor_SetSpeed calls. gcc hoists both into r5/r6, which costs so
 *      many registers that it SPILLS r8 and r10 -- `mov r6, r10 / mov r5, r8 /
 *      push {r5, r6}` and the matching epilogue, EIGHT instructions the ROM
 *      does not have. Pinning r1/r2 per call removed the spill and fixed the
 *      length in one step.
 *
 *      That is the clearest case yet of the hoisting class being expensive
 *      rather than merely different: the wrong-register-file tell was not a
 *      `mov` but a pair of high registers appearing in a function that has no
 *      business touching them.
 *
 *   3. AND SO IS `0x80 << 1`, at the last two of three __Func_809218c calls.
 *      Same fix, per call.
 *
 *   4. A POOL LOAD HOISTED ABOVE A CALL IT SURVIVES. `ldr r5, =0x2409` belongs
 *      after __MapActor_Emote and gcc issues it before. `do { } while (0)`
 *      between them emits nothing, splits the scheduling region, and puts it
 *      back -- the batch-189 barrier, used deliberately.
 *
 *   5. BLOCK-SCOPED PINS REACHED A SITE FUNCTION-SCOPED ONES DID NOT. The
 *      Emote call wants `mov r2, #0x3c` between `mov r1, #0x81` and its shift.
 *      Function-scope pins reused across a dozen calls left it at the end;
 *      three fresh `register` declarations in a nested block placed it. Worth
 *      recording as a property of the lever: a pin reused across many call
 *      sites is weaker than one declared beside the call it serves.
 *
 * WHAT REMAINS -- TWO INDEPENDENT MOVS HOLDING THE SAME VALUE:
 *
 *     rom    mov r1, #0x80 / mov r2, #0x80 / lsl r2, #9 / mov r0, #9 / lsl r1, #10
 *     ours   mov r2, #0x80 / mov r1, #0x80 / lsl r2, #9 / mov r0, #9 / lsl r1, #10
 *
 * This is the batch-192 mechanism exactly: both registers receive the SAME
 * constant, so there is no dependence to order them and gcc schedules them as
 * it likes. It appears to order the two movs by which one its SHIFT consumes
 * first -- lsl r2 comes first here, so mov r2 goes first. The ROM has the
 * opposite. The other two __MapActor_SetSpeed calls in this same function have
 * `lsl r1` first and match with no trouble, which is what isolates the cause.
 *
 * SEVEN STRUCTURALLY DISTINCT FORMS, ALL 2 OF 157:
 *
 *     block pins, ROM assignment order                     2
 *     block pins, the two assignments swapped              2
 *     block pins, do-while(0) between the assignments      2
 *     block pins, q0 assigned before both shifts           2
 *     block pins, r1 chain complete before r2 starts       2  (residue moves to 57)
 *     block pins, r2 chain complete before r1 starts       2
 *     no pins, the bare `0x80 << 10, 0x80 << 9` literals   2  (residue moves to 57)
 *
 * They vary pin PRESENCE, pin SCOPE, assignment order, and how the two
 * mov/shift chains interleave -- not merely the order of statements over one
 * skeleton, which is the test batch 192 set for whether a tie means anything.
 * Two of them move the residue elsewhere without reducing it.
 *
 * Plain `int` locals instead of pins are far WORSE, 102 differing and the
 * function one instruction short, so the pins are load-bearing even though
 * they do not finish it.
 *
 * NEXT: nothing at this site by ordering or by pinning. The open question is
 * whether anything makes gcc pick the other mov first when both shifts are
 * pending -- a construct that reverses the scheduler's preference without
 * introducing a dependence, since a dependence emits a register copy instead
 * of the immediate (measured in src/non_matching/ovl_793768/2008e0c.c).
 */

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_809259c(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_809218c(int a, int b, int c);
extern void __StartTask(void *fn, int arg);
extern void OvlFunc_939_2009240(void);

void OvlFunc_939_2008ff0(void)
{
    unsigned char *a;
    int msg;
    register int p0 __asm__("r0");
    register int p1 __asm__("r1");
    register int p2 __asm__("r2");

    p0 = 0x91;
    p0 <<= 2;
    if (__GetFlag(p0) != 0)
        return;
    p0 = 0x91;
    p0 <<= 2;
    __SetFlag(p0);
    __CutsceneStart();
    a = __MapActor_GetActor(0);
    __Func_809280c(8, 0, 0);
    __Func_809280c(9, 0, 0);
    __Func_809259c(8, 1);
    __Func_809259c(9, 1);
    __CutsceneWait(0x14);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0x81; q2 = 0x3c; q1 <<= 1; q0 = 8;
        __MapActor_Emote(q0, q1, q2);
    }
    do { } while (0);
    msg = 0x2409;
    __MessageID(msg);
    __ActorMessage(8, 0);
    p1 = 0x80; p2 = 0x80; p0 = 0; p1 <<= 10; p2 <<= 9;
    __MapActor_SetSpeed(p0, p1, p2);
    p1 = 0x80; p2 = 0x80; p0 = 8; p1 <<= 10; p2 <<= 9;
    __MapActor_SetSpeed(p0, p1, p2);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0x80; q2 = 0x80; q2 <<= 9; q0 = 9; q1 <<= 10;
        __MapActor_SetSpeed(q0, q1, q2);
    }
    __MapActor_SetAnim(9, 4);
    __CutsceneWait(0x23);
    __MessageID(msg + 1);
    __ActorMessage(9, 0);
    __MapActor_Emote(8, 0x103, 0x1e);
    __MessageID(msg + 2);
    __ActorMessage(8, 0);
    __MapActor_SetAnim(9, 3);
    msg += 3;
    __CutsceneWait(0x19);
    __MessageID(msg);
    __ActorMessage(9, 0);
    __Func_809218c(8, *(short *)(a + 0xa) - 1, *(short *)(a + 0x12));
    __MapActor_WaitMovement(8);
    __Func_809218c(0, 0xa0, 0xd8);
    __Func_809218c(8, 0x98, 0xc8);
    __Func_809218c(9, 0xa8, 0xc8);
    __MapActor_WaitMovement(8);
    __MapActor_WaitMovement(9);
    __MapActor_WaitMovement(0);
    __Func_809280c(8, 0, 0);
    __Func_809280c(9, 0, 0);
    __CutsceneWait(0xc);
    p2 = 0x88; p0 = 0; p1 = 0xa0; p2 <<= 1;
    __Func_809218c(p0, p1, p2);
    p2 = 0x80; p0 = 8; p1 = 0x98; p2 <<= 1;
    __Func_809218c(p0, p1, p2);
    p2 = 0x80; p1 = 0xa8; p2 <<= 1; p0 = 9;
    __Func_809218c(p0, p1, p2);
    __MapActor_WaitMovement(8);
    __MapActor_WaitMovement(9);
    __MapActor_WaitMovement(0);
    __CutsceneEnd();
    __StartTask(OvlFunc_939_2009240, 0xc8 << 4);
}
