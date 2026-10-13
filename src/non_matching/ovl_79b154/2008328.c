/* OvlFunc_907_2008328 -- NON-MATCHING, 8 of 92 encodings; ALIGNED residue 3.
 *   LENGTH GAP CLOSED: instructions 74/74, SIZE 220/220 bytes, 18/18 32-bit
 *   entries, no pad.  The figure IS a distance now.
 *   WAS: 60 of 92, instructions 74/75, SIZE 220 against 224.  (batch 345 D)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_79b154/2008328.c \
 *     asm/overlays/rom_79b154/ovl_30_c_a_a_c_c_c_c_c_c_a.s --func OvlFunc_907_2008328
 *
 * PER-REGION SPLIT of the old +1 (regions cut at the bl relocations):
 *   prologue+loop (ref 0-15)     ref 16 / ours 16   equal
 *   index+PlaySound (ref 16-29)  ref 14 / ours 14   equal
 *   Func_8010560 args (30-37)    ref  8 / ours  8   equal (ORDER differed)
 *   SetSpeed (38-43)             ref  6 / ours  6   equal
 *   GetActor/0x100 store (44-55) ref 12 / ours 13   *** THE +1 WAS HERE ***
 *   tail (56-end)                ref 33 / ours 33   equal
 *
 * THE +1 WAS A REUSED LOCAL, NOT A MISSING OPTIMISATION OF ANY PASS.
 * The park reused `p` for both __MapActor_GetActor results and `w` for both
 * reads of iwram_3001ebc.  One pseudo used in the loop AND after it is not
 * block-local, so local-alloc cannot coalesce it with the call's return
 * register; global_alloc gave it r2 and gcc emitted `adds r2, r0, #0` before
 * `adds r2, #85`.  The ROM does `adds r0, #85` IN PLACE -- no copy.  Giving
 * the second GetActor result its own local `q` makes that def/use pair
 * block-local, local-alloc takes r0, and the copy disappears.  60 -> 24 and
 * the lengths equalise.
 *
 * AND THE SAME SPLIT ON `w` FIXED THE r6/r7 ROTATION FOR FREE.  The park's
 * standing blocker 1 was "the ROM holds the iwram pointer in r7 and the loop's
 * stored zero in r6; ours has them swapped ... both assignment orders screened,
 * both 79".  That was a measurement of the reused-name body: reusing `w` for
 * the second read made its live range span the whole function (4 refs / ~52
 * insns) and beat the loop-hoisted zero in allocno_compare
 * (global.c:600-618).  With a second local `w2` the range stops at the first
 * use, the zero wins r6 and `w` takes r7 -- the ROM's assignment, with NO
 * priority lever applied.  Routes 1 and 2 of the allocno table, together.
 * Combined (q + w2): 60 -> 14, instructions and bytes equal.
 *
 * Park blocker 2 (TABLE-ACCESS ORDER) IS ALSO CLOSED.  The ROM evaluates the
 * two __Func_8010560 shorts FIRST and the word into r0 LAST, and gcc did the
 * word first; the park recorded "dropping the callee's prototype changes
 * nothing (60 either way)", which reproduces.  Naming the two shorts in int
 * locals before the call gives the ROM's order exactly -- including the
 * 3-operand `adds r3, r0, #2` that keeps the shared k*4 index live for the
 * trailing word load, and the ROM's pool order (.L1d0c at +0xd4 before .L1cf0
 * at +0xd8; it was reversed before, which is where 6 of the 14 lived).
 * 14 -> 8, ALIGNED 8 -> 3.
 *
 * THE RESIDUE IS THREE SCHEDULER TIE-BREAKS, ALL ONE SHAPE, AND IT IS A LUID
 * QUESTION, NOT A COST OR PRIORITY ONE.  In each case the ROM emits the
 * FINAL insn of a multi-insn argument constant AFTER the other argument
 * registers are loaded, and we emit it immediately:
 *   ref 41/42  movs r0,#0 then lsls r1,r1,#8   (SetSpeed, 0x80 << 8)
 *   ref 61-64  movs r2,#4;  movs r0,#0; movs r1,#0; negs r2,r2
 *   ref 67-70  movs r2,#16; movs r0,#0; movs r1,#3; negs r2,r2
 * Read out of .23.sched2 with -fsched-verbose=6, basic block 8:
 *     insn  code  bb dep prio cost   units
 *      296   173   0   1    3    1   core : 185 297     (movs r2,#4)
 *      297   125   0   2    2    1   core : 185         (negs r2,r2)
 *      180   173   0   2    2    1   core : 185         (movs r0,#0)
 *      182   173   0   2    2    1   core : 185         (movs r1,#0)
 *   Ready list (t = 35):  182 180 297  -->  scheduling insn <<<297>>>
 * All three are priority 2; rank_for_schedule (haifa-sched.c:4029-4113) walks
 * every rung to the LAST one: the class rung at :4069-4096 gives all three
 * class 3 because insn_cost(296 -> 297) == 1 (:4077-4078), the dependent-count
 * rung at :4098-4109 is 1 against 1 against 1, so it falls through to
 * `INSN_LUID (tmp) - INSN_LUID (tmp2)` at :4114 -- and because tmp is *y and
 * tmp2 is *x the array sorts DESCENDING by LUID while ready_remove takes the
 * last element, so THE LOWEST LUID WINS.  Program order is
 * movs#4 / negs / movs r0 / movs r1, so negs has the lowest LUID of the three
 * and goes first.  The ROM's order is what you get when the negs is the LAST
 * of the four in program order, i.e. when the `-4` is NOT copied into a pseudo
 * by precompute_register_parameters (calls.c:805-854, the
 * `rtx_cost (value, SET) > 2 && SMALL_REGISTER_CLASSES && reg_parm_seen` gate
 * at :848-854) but materialised into the hard register by
 * load_register_parameters instead.  So the question to answer next is WHY
 * rtx_cost of this constant is > 2 here, not which spelling reorders the
 * statements -- no spelling can, because every spelling of a constant argument
 * goes through the same gate.
 *
 * MEASURED INERT, and AGAINST THIS BODY (not the park's): all of these are
 * 8/aligned 3, byte-for-byte unchanged --
 *   `nn = 4; ... -nn` and `nn = -4; ... nn`, per-arm and shared across arms;
 *   `0 - 4` / `0 - 0x10`; a `short` third parameter; no prototype at all on
 *   __Func_809228c / __Func_8092208 / __MapActor_SetSpeed; named int locals for
 *   the two SetSpeed shift results; a named zero for the first argument of
 *   __Func_809228c.  WORSE: a shared `sv = 0x80` feeding both SetSpeed shifts
 *   (46); a named zero for SetSpeed's first argument (16); the SetSpeed locals
 *   defined in reverse order (10).
 * FLAG AXIS, all against this body: -fno-schedule-insns, -fno-sched-interblock,
 *   -fno-gcse, -fno-rerun-cse-after-loop, -fno-strict-aliasing,
 *   -fno-rerun-loop-opt, -fno-expensive-optimizations all exactly 8.
 *   -fno-schedule-insns2 is WORSE (14, aligned 5) -- which is the control that
 *   says the residue really is sched2 and that sched2 must stay ON.
 *   -ffixed-r7 is 96 at +6 instructions.
 *
 * NEXT: read arm_rtx_costs (config/arm/arm.c) for CONST_INT under TARGET_THUMB
 * and find a constant form whose cost is <= 2, or establish that the gate is
 * unreachable from source and that this is a 3-encoding compiler-state floor.
 */
extern char *iwram_3001ebc;
extern short L1d0c[] __asm__(".L1d0c");
extern void *L1cf0[] __asm__(".L1cf0");

extern void __PlaySound(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __Func_8010560(void *s, int a, int b);
extern void __Func_809228c(int a, int b, int c);
extern void __Func_8092208(int a, int b, int c);
extern void __Func_8092b08(int a, int b);
extern void __Func_8091e9c(int n);

void OvlFunc_907_2008328(void)
{
    char *w;
    char *w2;
    unsigned char *p;
    unsigned char *q;
    unsigned int i;
    int k;
    int t;
    int ya;
    int yb;

    w = iwram_3001ebc;
    __CutsceneStart();
    for (i = 8; i <= 0x41; i++) {
        p = __MapActor_GetActor(i);
        if (p != 0)
            p[0x55] = 0;
    }
    t = *(unsigned short *)(w + (0xb6 << 1));
    t -= 3;
    k = (short)t;
    if (k == 6)
        __PlaySound(0xbc);
    else
        __PlaySound(0x9e);
    ya = L1d0c[k * 2];
    yb = L1d0c[k * 2 + 1];
    __Func_8010560(L1cf0[k], ya, yb);
    __MapActor_SetSpeed(0, 0x80 << 8, 0x80 << 7);
    q = __MapActor_GetActor(0);
    q[0x55] = 0;
    w2 = iwram_3001ebc;
    *(int *)(w2 + (0xe0 << 1)) = 0x100;
    if (k == 6) {
        __MapActor_SetAnim(0, 2);
        __Func_809228c(0, 0, -4);
    } else {
        __Func_8092208(0, 3, -0x10);
    }
    if (k == 4)
        __Func_8092b08(0, 3);
    else
        __Func_8092b08(0, 2);
    __CutsceneWait(0x10);
    __Func_8091e9c(k + 3);
    __CutsceneEnd();
}

