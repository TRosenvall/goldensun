/* Cluster OvlFunc_899_200abf0..OvlFunc_899_200adb4 extracted from
 * goldensun/asm/overlays/rom_794ac0/ovl_30_a_c_c_c_c_c_c.s.
 *
 * THIS FILE CONVERTS WHOLE -- both functions of the `.s` matched, so no split was needed.
 * Never attempted before batch 279. Thirteen PIN3 sites in the first function and fifteen in the
 * second, all required -- two fakematch rows. `OvlFunc_899_200adb4` additionally requires
 * `_AREA_15`, added to area.sym this batch (see that file for the criterion and the seven literal
 * spellings that fail).
 *
 * `q0` ASSIGNED FIRST INSIDE PIN3 IS THE WHOLE ARGUMENT-ORDER FIX, and on OvlFunc_899_200abf0 it
 * was worth 80 differing to ZERO on that one change with no other edit. The ROM writes
 * `mov r1 / mov r2 / mov r0 / lsl r1 / lsl r2`; plain C and PIN3 with `q0` last both give the
 * `mov r0` at the end. All five insns are one sched2 region, the two `mov`s feeding an `lsl` carry
 * priority 2 and the rest priority 1, so the priority-1 group falls to INSN_LUID -- and `q0 = N;`
 * written textually first gives `mov r0` the lowest LUID.
 *
 * A `do { } while (0);` BROKE A sched2 TIE THAT NO BARRIER OR FLAG COULD, in OvlFunc_899_200adb4.
 * `gState[0x22b] = 3;` emitted `ldr r2(0x22b) / ldr r3(gState)` where the ROM has them the other
 * way round -- and because gcc's Thumb pool is emitted in SCHEDULED-INSN ORDER, the two pool words
 * swapped too, which is a useful thing to know when a pool looks permuted.
 *
 * `.23.sched2` gives both insns priority 68 and cost 2, so `rank_for_schedule` falls through to
 * FORWARD DEPENDENT COUNT: the `const_int 555` insn has THREE forward dependents (an add, the
 * call, and a REG_DEP_OUTPUT from the later `mov r2, #3`) against the gState pool load's TWO, and
 * MORE DEPENDENTS WINS.
 *
 * **THAT IS THE TIE-BREAK TO READ AFTER `prio`, BEFORE LUID** -- the notebook previously stopped
 * at LUID. Measured and rejected there: register-pinned r3/r2, a byte-cast store, a named base
 * local, a named offset local, three `__asm__` barrier forms, and adding a trailing call (which
 * raises BOTH counts by one and is therefore inert) -- all 6 differing. The `do { } while (0);`
 * before the store is 2. Its BASIC-BLOCK BOUNDARY re-regions the scheduler; variants with a base
 * local work identically, so the boundary is the lever and the local is not.
 *
 * Neither residue was an allocation problem: `.17.lreg` priorities were never consulted on these
 * two, and `.23.sched2`'s dependence table was the dump that mattered. Worth noting against the
 * five-entry allocation triage -- a residue where only scratch registers rotate is NOT always
 * `order_regs_for_reload` and unreachable; here the tie-break inputs were source-reachable.
 *
 * A flag sweep on the second function (-fno-strict-aliasing, -fno-rerun-cse-after-loop, -fno-gcse,
 * -fno-schedule-insns2, -fno-expensive-optimizations, -fno-cse-follow-jumps, -fno-force-mem,
 * -fno-thread-jumps, -fno-peephole) moved nothing. And -fno-schedule-insns2 would be unusable in
 * any case: both functions share this one `.c`, and the FIRST one depends on sched2.
 *
 * The donor was found by grepping src/ for the literal store `gState[0x22b] = 3;` --
 * src/overlays/rom_7e7574/ovl_9dc_c_a_c_c_a_a_c_c_c_c_a_b.c, a 173-instruction cutscene in an
 * UNRELATED overlay ending in the same __Func_8091f90 / gState[0x22b] / __Func_8091eb0 sequence.
 * Compiling that donor with -da and diffing its dependence table against ours gave the exact
 * reason the schedules differed: it has TWO trailing calls, so both candidates reached three
 * dependents and LUID decided there. Diffing a solved function's DUMP against yours is a stronger
 * move than diffing its source.
 */

extern unsigned char *iwram_3001ebc;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_WaitMovement(int slot);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __SetCameraTarget(int slot, int a);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_8093530(void);
extern void __Func_800fe9c(void);
extern void OvlFunc_899_200c5cc(void);
extern void OvlFunc_899_200c5f4(int a, int b);
extern void OvlFunc_899_200c60c(int a, int b, int c);
extern void OvlFunc_899_200c63c(int a, int b, int c);

#define PIN3 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1"); \
             register int q2 __asm__("r2")

void OvlFunc_899_200abf0(void)
{
    __CutsceneStart();
    { PIN3; q0 = 0xa; q1 = 0xc6 << 18; q2 = 0xd0 << 17; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xc8 << 18; q2 = 0xc8 << 17; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0xc2 << 18; q2 = 0xd0 << 17; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xc0 << 6; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xc0 << 6; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0xc0 << 6; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xc4 << 18; q2 = 0xe0 << 17; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xca << 18; q2 = 0xd8 << 17; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xc2 << 18; q2 = 0xd8 << 17; __MapActor_SetPos(q0, q1, q2); }
    __MapActor_SetAnim(0, 0x13);
    __MapActor_SetAnim(1, 0x13);
    __MapActor_SetAnim(2, 0x13);
    __MapActor_GetActor(0)[0x23] = 2;
    __MapActor_GetActor(1)[0x23] = 2;
    __MapActor_GetActor(2)[0x23] = 2;
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(2), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(1), 0);
    { PIN3; q0 = 8; q1 = 0xb0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x209;
    __SetCameraTarget(0, 0);
    __Func_8093530();
    __Func_800fe9c();
    OvlFunc_899_200c5cc();
    __CutsceneWait(0x3c);
    OvlFunc_899_200c63c(0xa, 3, 0x14);
    __MessageID(0x12dd);
    OvlFunc_899_200c5f4(0xa, 0x1e);
    OvlFunc_899_200c5f4(8, 0x1e);
    { PIN3; q0 = 0xb; q1 = 0xca << 2; q2 = 0xe4 << 1; __Func_809218c(q0, q1, q2); }
    __Func_809218c(0xc, 0xc6 << 2, 0xe4 << 1);
    __MapActor_WaitMovement(0xc);
    __Func_8092adc(0xc, 0, 0);
    __MapActor_WaitMovement(0xb);
    __Func_8092adc(0xb, 0, 0);
    __CutsceneWait(0x1e);
    OvlFunc_899_200c63c(0xb, 3, 0x14);
    OvlFunc_899_200c5f4(0xb, 0x14);
    OvlFunc_899_200c60c(0xc, 0, 0x1e);
    OvlFunc_899_200c5f4(0xc, 0x3c);
    __CutsceneEnd();
}

/* Declarations used only by OvlFunc_899_200adb4. */
extern unsigned char gState[];
extern int _AREA_15;
extern void __WaitFrames(int n);
extern void __ActorMessage(int slot, int a);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8091eb0(int a, int b);
extern void __Func_8091f90(int a, int b);
extern void __Func_8091fa8(int a, int b);
extern void OvlFunc_899_200c624(int a, int b, int c);

void OvlFunc_899_200adb4(void)
{
    int m;

    { PIN3; q0 = 0xa; q1 = 0xc6 << 18; q2 = 0xd0 << 17; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xc8 << 18; q2 = 0xc8 << 17; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0xc2 << 18; q2 = 0xd0 << 17; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xc0 << 6; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xc0 << 6; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0xc0 << 6; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xc6 << 18; q2 = 0xdc << 17; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xca << 18; q2 = 0xd8 << 17; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xc6 << 18; q2 = 0xe4 << 17; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xb0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xb0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    __Func_809280c(8, 0xa, 0);
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x209;
    __SetCameraTarget(0, 0);
    __Func_8093530();
    __Func_800fe9c();
    __WaitFrames(1);
    *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x20;
    OvlFunc_899_200c5cc();
    __CutsceneWait(0x3c);
    __MessageID(0x12e1);
    __Func_80925cc(0xb, 1);
    __CutsceneWait(0x14);
    OvlFunc_899_200c5f4(0xb, 0x1e);
    __Func_80925cc(0xc, 1);
    __CutsceneWait(0x14);
    __ActorMessage(0xc, 0);
    OvlFunc_899_200c624(0xa, 0xb, 0x1e);
    __MapActor_SetAnim(0xa, 3);
    OvlFunc_899_200c63c(0xb, 3, 0x1e);
    OvlFunc_899_200c624(0xa, 0xc, 0x1e);
    __MapActor_SetAnim(0xa, 3);
    OvlFunc_899_200c63c(0xc, 3, 0x28);
    { PIN3; q0 = 0xa; q1 = 0xc0 << 6; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xc0 << 6; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0xc0 << 6; q2 = 0; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    OvlFunc_899_200c63c(0xa, 4, 0x14);
    __ActorMessage(0xa, 0);
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x80 << 2;
    m = (int)(&_AREA_15);
    __Func_8091f90(m, 0x11);
    __Func_8091fa8(m, 0x10);
    do { } while (0);
    gState[0x22b] = 3;
    __Func_8091eb0(0xc, 5);
}
