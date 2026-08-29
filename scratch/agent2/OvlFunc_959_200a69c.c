/* OvlFunc_959_200a69c  --  NOT MATCHING, 10 differing of 46
 * ref: asm/overlays/rom_7e7574/ovl_9dc_c_c_a_a_a.s
 *
 * Straight-line.  Two problems, one solved:
 *
 * 1. CONSTANT CSE across four call sites (0x80<<9, 0x80<<8 twice each, and
 *    0xec<<1 twice).  Bare literals give 29 differing of 46 with r5/r6 pushed
 *    and `mov r1, r5` at every site.  There is no branch, so the basic-block
 *    lever is unavailable.  Splitting each into TWO STATEMENTS at each site --
 *    `p = 0x80; p <<= 9;` -- defeats the CSE with no branch: 29 -> 10, and the
 *    push list becomes the ROM's bare {lr}.  (Same lever as
 *    scratch/agent2/OvlFunc_884_2008780.c; see its header for the control.)
 *
 * 2. RESIDUE, all ten instructions: r0 against a shift.
 *        rom   mov r1,#0x80 / mov r2,#0x80 / mov r0,#0xb / lsl r1,#9 / lsl r2,#8
 *        ours  mov r1,#0x80 / mov r2,#0x80 / lsl r1,#9 / lsl r2,#8 / mov r0,#0xb
 *    at all four sites.  This is the documented "r0-against-a-shift rotation"
 *    class (OvlFunc_911_2008304, OvlFunc_888_20085cc, OvlFunc_948_2009fd8).
 *    Measured, all 10 or worse: int/void return type on both callees, an int
 *    return on the preceding __ActorMessage, the slot named as its own local
 *    (assigned before and after the shifts), the two shifts assigned in either
 *    order, the shift moved back into the argument expression (35 -- the CSE
 *    returns), and -fno-rerun-cse-after-loop / -fno-schedule-insns / -fno-gcse
 *    / -fno-expensive-optimizations / -fno-caller-saves / -fno-peephole.
 *    -fno-schedule-insns2 and -O1 are 16.
 *
 * NOTE: the reference keeps its literal pool inside the function, so even an
 * OK screen here would need make compare.
 */
extern void __Func_8093500(int a, int b);
extern void __Func_8093530(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int n);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __Func_809218c(int slot, int a, int b);
extern void __SetCameraTarget(int slot, int b);
extern void __MapActor_WaitMovement(int slot);
extern void __MapActor_SetAnim(int slot, int n);

void OvlFunc_959_200a69c(void)
{
    int p;
    int q;

    __Func_8093500(0xb, 1);
    __Func_8093530();
    __CutsceneWait(0x3c);
    __MessageID(0x247c);
    __ActorMessage(0xd, 0);
    p = 0x80;
    q = 0x80;
    p <<= 9;
    q <<= 8;
    __MapActor_SetSpeed(0xb, p, q);
    p = 0x80;
    q = 0x80;
    p <<= 9;
    q <<= 8;
    __MapActor_SetSpeed(0xf, p, q);
    p = 0xec;
    p <<= 1;
    __Func_809218c(0xb, p, 0xb4);
    p = 0xec;
    p <<= 1;
    __Func_809218c(0xf, p, 0xb4);
    __SetCameraTarget(0xb, 1);
    __MapActor_WaitMovement(0xb);
    __MapActor_SetAnim(0xb, 4);
    __CutsceneWait(0x1e);
}
