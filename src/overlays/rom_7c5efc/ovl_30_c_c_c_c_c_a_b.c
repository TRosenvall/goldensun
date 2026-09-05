// fakematch
/* OvlFunc_941_2009448  --  0x02009448
 *
 * Cut out of goldensun/asm/overlays/rom_7c5efc/ovl_30_c_c_c_c_c_a.s, which
 * also holds OvlFunc_941_2009760 (still asm) -- landing this needs the .s
 * split first.  Built at the TREE DEFAULT -O2: nothing in the Makefile
 * captures asm/overlays/rom_7c5efc/ovl_30_c_c_c_c_c_a%, so the scratch screen
 * and the landed build see the same flags (checked with tryc.makefile_flags on
 * the _a and _b split names as well as the parent).
 *
 * 302 instructions of straight-line cutscene.  The ROM pushes ONLY {r5, lr},
 * and r5 holds the message base 0x254e and nothing else: every other repeated
 * constant is rebuilt at every use.  That is the whole shape of the problem --
 * plain C is 310 lines, 298 differing, with `push {r5, r6, lr}` plus a
 * `mov r6, r8 / push {r6}` high-register spill.
 *
 * 1. THIRTEEN PINS, ONE PER DUPLICATED EXPENSIVE CONSTANT USE.  cse1 commons
 *    each repeated pool-or-mov+lsl value into one pseudo whose range straddles
 *    calls, so global-alloc must give it a callee-saved register; r5, r6 and
 *    then r8 fill up.  Assigning the value to a hard CALL-CLOBBERED argument
 *    register makes it dead across the next bl, so gcc has nothing to carry it
 *    in and must rematerialise.  The values: 0x105 (x2 __MapActor_Emote),
 *    0xa0 << 7 (x5 __Func_8092adc), 0x80 << 7 (x4), 0x84 << 2 and 0xa0 << 2
 *    (x2 each, __Func_809218c).
 *
 *    ONE PIN AT THE FIRST USE CAN COVER A LATER ONE.  Pinning the FIRST
 *    __MapActor_Emote is worth 303 of 303 if dropped; pinning the second is
 *    INERT, because with the first use written straight into r1 there is no
 *    pseudo left for CSE to hand to the second site.  Same for one of the five
 *    0xa0 << 7 sites and one of the four 0x80 << 7 sites -- but only ONE of
 *    each may go: dropping two of the 0xa0 << 7 pins puts the value back in r6
 *    (69 differing).  Sixteen pinned sites were stripped one at a time (four
 *    inert), then every 2- and 3-subset of those four was measured; the
 *    surviving thirteen are each load-bearing under a third round.
 *
 * 2. THE ARGUMENT-FILL ORDER INSIDE A PIN IS SOURCE ORDER, and the shift goes
 *    where the ROM puts it, not at the end of the block.  Writing
 *    `q1 = 0xa0; q0 = 0; q2 = 0; q1 <<= 7;` instead of
 *    `q1 = 0xa0; q0 = 0; q1 <<= 7; q2 = 0;` is 16 lines of `lsl`/`mov r2`
 *    transposition across the eight __Func_8092adc sites: 25 differing -> 9.
 *
 * 3. THE MESSAGE BASE'S POOL LOAD NEEDS A LOOP BARRIER, AND IT MUST BE A LOOP.
 *    sched2 hoists `ldr r5, =0x254e` to the top of its scheduling region --
 *    nine slots above the ROM's placement, and it also reverses the literal
 *    pool (0x254e ahead of 0x105).  It is the ONLY insn sched2 moves across a
 *    call, because r5 is the only call-saved value in the function and so the
 *    only one with no anti-dependence holding it down.  No flag reaches it
 *    without breaking something else: -fno-schedule-insns2 puts the load right
 *    but reverts every argument group to source order (61 differing), and
 *    -fno-sched-interblock, -fno-sched-spec, -fno-sched-spec-load,
 *    -fno-schedule-insns, -fno-gcse, -fno-cse-follow-jumps, -fno-cse-skip-
 *    blocks, -fno-expensive-optimizations, -fno-force-mem, -fno-rerun-cse-
 *    after-loop, -fno-caller-saves, -fno-function-cse, -fno-peephole,
 *    -fno-thread-jumps, -fno-strict-aliasing and -fno-inline are all inert.
 *    A `do { } while (0)` before the assignment ends the region and fixes it
 *    (9 -> 2), exactly the lever documented in
 *    src/overlays/rom_7b2078/ovl_314_c_c_a_c_c_c_a_a_a_c_b.c.
 *
 *    NEW: IT IS THE LOOP, NOT THE LABEL.  `while (0) ;` is byte-identical, but
 *    `goto B; B: ;` -- a bare label in the same place -- is INERT, as are
 *    `if (0) ;`, `;` and `{ }`.  So this is not label-shifting; what bounds the
 *    scheduling region is the loop note jump.c leaves behind.  Nothing is
 *    emitted for it: the compiled output carries no extra label.
 *    `register int m __asm__("r5")`, which is the other half of the lever in
 *    the rom_7b2078 file, is INERT here (still 9 differing) -- m already lands
 *    in r5 on its own, so only the barrier is scaffolding.
 *
 * 4. THE BARRIER COSTS ONE MORE PIN.  Ending the region after
 *    __Func_809280c(1, 0, 0) leaves that call's argument group with no
 *    scheduler to reverse it, so it comes out r0/r1/r2 against the ROM's
 *    r2/r1/r0 (2 differing).  Pinning it in the ROM's order closes it.
 *
 * The three __MapActor_GetActor(0) / TravelTo blocks are plain `if (p != 0)`
 * guards; `mov r3, #0xa / ldrsh r1, [r0, r3]` is just Thumb's only ldrsh form.
 *
 * The assumption behind all of the above, stated so it can be re-derived: r5
 * belongs to the message base and to NOTHING else, which the two-register
 * `push {r5, lr}` proves.  A sibling that pushes r6 as well breaks the reading
 * and the thirteen pins with it.
 */
extern void __MapActor_Emote(int slot, int id, int n);
extern void __Func_809280c(int slot, int a, int b);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int n);
extern void __Func_80925cc(int slot, int a);
extern void __CutsceneWait(int n);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __Func_8092adc(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_809228c(int slot, int a, int b);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_809218c(int slot, int a, int b);
extern void __MapActor_SetPos(int slot, int x, int y);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __Func_8093500(int a, int b);
extern void __Func_8093530(void);
extern void __SetCameraTarget(int a, int b);
extern void __SetFlag(int id);

#define PIN3 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1"); \
             register int q2 __asm__("r2")

void OvlFunc_941_2009448(void)
{
    unsigned char *p;
    int m;

    { PIN3; q0 = 1; q1 = 0x105; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q2 = 0; q1 = 0; q0 = 1; __Func_809280c(q0, q1, q2); }
    do { } while (0);
    m = 0x254e;
    __MessageID(m);
    __ActorMessage(1, 0);
    __Func_80925cc(3, 1);
    __MessageID(m + 1);
    __ActorMessage(3, 0);
    __Func_809280c(2, 0xd, 0);
    __CutsceneWait(0x3c);
    __MessageID(m + 2);
    __ActorMessage(2, 0);
    __Func_809280c(0xd, 2, 0);
    __MapActor_Emote(0xd, 0x105, 0x46);
    __MessageID(m + 3);
    __ActorMessage(0xd, 0);
    __MapActor_DoAnim(0xc, 4);
    __MessageID(m + 4);
    __ActorMessage(0xc, 0);
    __Func_8092adc(0xc, 0xc0 << 6, 0);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0xc, 3);
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(2, 3);
    __MapActor_SetAnim(3, 3);
    __CutsceneWait(0x3c);
    __Func_809228c(0xd, -0x10, 0);
    __MapActor_WaitMovement(0xd);
    __MapActor_SetAnim(0xd, 1);
    __CutsceneWait(0x28);
    __MapActor_DoAnim(0xd, 3);
    __Func_8092adc(0xd, 0xa0 << 7, 0);
    __CutsceneWait(0x1e);
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(2, 3);
    __MapActor_SetAnim(3, 3);
    { PIN3; q2 = 0x84; q1 = 0x9c; q0 = 0xc; q2 <<= 2; __Func_809218c(q0, q1, q2); }
    __CutsceneWait(0x14);
    { PIN3; q2 = 0x84; q1 = 0xa4; q0 = 0xd; q2 <<= 2; __Func_809218c(q0, q1, q2); }
    __MapActor_WaitMovement(0xc);
    { PIN3; q2 = 0xa0; q1 = 0xa8; q0 = 0xc; q2 <<= 2; __Func_809218c(q0, q1, q2); }
    __MapActor_WaitMovement(0xd);
    { PIN3; q2 = 0xa0; q0 = 0xd; q1 = 0xa8; q2 <<= 2; __Func_809218c(q0, q1, q2); }
    { PIN3; q1 = 0xa0; q0 = 0; q1 <<= 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xa0; q0 = 2; q1 <<= 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xa0; q0 = 3; q1 <<= 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xa0; q1 <<= 7; q2 = 0; q0 = 1; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    { PIN3; q1 = 0x80; q0 = 0; q1 <<= 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 2; q1 <<= 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 3; q1 <<= 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(1, 0x80 << 7, 0);
    __CutsceneWait(0x3c);
    __MapActor_SetPos(0xd, 0, 0);
    __MapActor_SetPos(0xc, 0, 0);
    __CutsceneWait(0x6e);
    __MessageID(m + 5);
    __ActorMessage(1, 0);
    __MapActor_DoAnim(3, 3);
    __CutsceneWait(0x1e);
    __MessageID(m + 6);
    __ActorMessage(3, 0);
    __MapActor_DoAnim(2, 3);
    __MessageID(m + 7);
    __ActorMessage(2, 0);
    __CutsceneWait(0x8c);
    m += 8;
    __Func_809280c(1, 0, 0);
    __MessageID(m);
    __ActorMessage(1, 0);
    __MapActor_SetAnim(1, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(1, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(1);
    __MapActor_SetPos(1, 0, 0);
    __MapActor_SetAnim(2, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(2, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(2);
    __MapActor_SetPos(2, 0, 0);
    __MapActor_SetAnim(3, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(3, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(3);
    __MapActor_SetPos(3, 0, 0);
    __CutsceneWait(0x1e);
    __Func_8093500(0, 1);
    __Func_8093530();
    __SetCameraTarget(0, 0);
    __SetFlag(0x94f);
}
