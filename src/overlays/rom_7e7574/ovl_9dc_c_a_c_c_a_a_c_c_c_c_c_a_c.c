// fakematch
/* OvlFunc_959_200a1c4  --  0x0200a1c4
 *
 * From goldensun/asm/overlays/rom_7e7574/ovl_9dc_c_a_c_c_a_a_c_c_c_c_c_a_c.s,
 * which held this function alone, so no split was needed.
 *
 * PARKED AT 22 OF 60 on DUPLICATE-CONSTANT CSE. Closed in four steps, each
 * aimed at a difference already read off the diff:
 *
 *     as parked                                   22 differing
 *     + r0-r2 pinned at the first __MapActor_Emote 11
 *     + the same at the second                      9
 *     + a barrier before the message base            2
 *     + the last argument fill pinned              MATCH
 *
 * 1. THE SHARED CONSTANT. `0x80 << 1` is the second argument of both
 *    __MapActor_Emote calls. The ROM rebuilds it at each; gcc builds it once
 *    into r5 and copies. The instruction COUNT is identical either way -- four
 *    -- which is why this never showed as a length difference, only as a
 *    register-role divergence that then displaced r5's real occupant. Pinning
 *    r1 at each site forces the rebuild.
 *
 * 2. A POOL LOAD HOISTED OVER TWO CALLS IT SURVIVES. `ldr r5, =0x240d` belongs
 *    after both __Func_809280c calls and gcc issues it before them.
 *    `do { } while (0)` immediately before the assignment puts it back;
 *    `__asm__ volatile("")` in the same place is byte-identical.
 *
 * 3. THE LAST ARGUMENT FILL RUNS BACKWARDS. The second __Func_809280c wants
 *    `mov r2 / mov r1 / mov r0`, the reverse of the order gcc picks -- and the
 *    reverse of what the FIRST __Func_809280c call in the same function gets,
 *    which needs no help at all. Pinning the three registers and assigning them
 *    in the ROM's order matches.
 *
 * THAT THIRD STEP QUALIFIES SOMETHING BATCH 196 STATED TOO BROADLY. The
 * discriminator written in src/non_matching/ovl_7ebdfc/2008120.c says a mov
 * with no consuming operation has nothing to order it, and that park's two
 * zeros are indeed unreachable. But here all three movs are constants, two of
 * them zeros, none has a consumer -- and the pin sets their order without
 * difficulty.
 *
 * The difference is what is being asked. In 2008120 the movs must be placed
 * INSIDE another register's two-instruction build, between its `mov` and its
 * `neg`; here they only have to be ordered AMONG THEMSELVES. A pin fixes where
 * its own register is written relative to other pinned writes, and that is
 * enough for the second case and not for the first. The 2008120 rule should be
 * read as being about interleaving into a build, not about consumers in
 * general.
 */

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_809228c(int a, int b, int c);
extern void __MapActor_SetBehavior(int slot, int b);
extern void __MapActor_SetAnim(int slot, int a);
extern void __Func_809280c(int a, int b, int c);
extern void __PlaySound(int id);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MessageID(int id);
extern void __ActorMessage(int actor, int b);
extern void __MapTransitionOut(void);
extern void __CutsceneWait(int n);
extern void __Func_8091e9c(int n);
extern void __SetFlag(int id);

void OvlFunc_959_200a1c4(void)
{
    int base;

    __CutsceneStart();
    __Func_809228c(0, 0, 0);
    __MapActor_SetBehavior(0, 1);
    __MapActor_SetAnim(0, 1);
    __PlaySound(0x71);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0x80;
        q0 = 0x15;
        q1 <<= 1;
        q2 = 0;
        __MapActor_Emote(q0, q1, q2);
    }
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0x80;
        q0 = 0xd;
        q1 <<= 1;
        q2 = 0x3c;
        __MapActor_Emote(q0, q1, q2);
    }
    __Func_809280c(0x15, 0, 0);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q2 = 0;
        q1 = 0;
        q0 = 0xd;
        __Func_809280c(q0, q1, q2);
    }
    do { } while (0);
    base = 0x240d;
    __MessageID(base);
    __ActorMessage(0xd, 0);
    __MapActor_Emote(0, 0x81 << 1, 0x1e);
    base += 1;
    __MessageID(base);
    __ActorMessage(0xd, 0);
    __MapTransitionOut();
    __CutsceneWait(0x3c);
    __Func_8091e9c(0x3c);
    __CutsceneEnd();
    __SetFlag(0x225);
}
