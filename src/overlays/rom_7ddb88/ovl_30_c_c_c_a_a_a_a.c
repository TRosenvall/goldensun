/* OvlFunc_955_2008160 -- 0x02008160
 *
 * // fakematch: THREE `register ... __asm__("rN")` pins, all on low or
 * callee-saved registers the ROM itself uses, and no other scaffolding.
 *
 *  - env -> r6.  gcc's allocno priority (floor_log2(n_refs)*n_refs/live_length)
 *    ranks the two loop accumulators above the actor+0x50 pointer, so gcc puts
 *    the pointer in r7 and an accumulator in r6; the ROM has it the other way.
 *    The pin overrides the choice outright.  17 differing of 107 without it.
 *
 *  - p1 -> r0 and c1 -> r3 at the FIRST __Actor_TravelTo.  All three of that
 *    call's constants are `thumb_shiftable_const`, so calls.c's
 *    precompute_register_parameters copies each into a pseudo BEFORE
 *    load_register_parameters emits the r0 copy -- which gives `mov r0, r8` a
 *    higher INSN_LUID than the three `lsl`s, and the post-reload scheduler's
 *    last tiebreak is INSN_LUID.  Assigning the pinned r0 copy FIRST gives it
 *    the lowest LUID and the ROM's order falls out.  The pins work only as a
 *    SET: p1 alone is 6 differing, c1 alone is 4, both together 0.  The second
 *    site needs no pin -- it is dominated by loop 1's back edge, so its named
 *    constants rematerialise and drop out of precompute on their own.
 */
extern void *__MapActor_GetActor(int slot);
extern void __SetFlag(int flag);
extern void __PlaySound(int id);
extern void __Actor_TravelTo(void *actor, int x, int y, int z);
extern void __WaitFrames(int n);
extern void __Actor_WaitMovement(void *actor);
extern void __CutsceneWait(int n);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_955_2008160(void)
{
    unsigned char *actor;
    register unsigned char *env __asm__("r6");
    int i;
    int x2, y2, z2;

    actor = (unsigned char *)__MapActor_GetActor(0x1e);
    env = *(unsigned char **)(actor + 0x50);
    __SetFlag(0xcc << 2);
    *(int *)(actor + 0x34) = 0x1999;
    *(int *)(actor + 0x30) = 0x13333;
    __PlaySound(0xe3);
    {
        register unsigned char *p1 __asm__("r0");
        int a1;
        int b1;
        register int c1 __asm__("r3");
        p1 = actor;
        a1 = 0xa8 << 17;
        b1 = 0xa0 << 12;
        c1 = 0x84 << 17;
        __Actor_TravelTo(p1, a1, b1, c1);
    }
    x2 = 0xa5 << 17;
    y2 = 0xfff00000;
    z2 = 0x84 << 17;
    for (i = 0; i < 10; i++) {
        *(unsigned short *)(env + 0x1e) -= i * 0x24;
        __WaitFrames(1);
    }
    __Actor_TravelTo(actor, x2, y2, z2);
    for (i = 10; i < 32; i++) {
        *(unsigned short *)(env + 0x1e) -= i * 0x24;
        __WaitFrames(1);
    }
    __Actor_WaitMovement(actor);
    __CutsceneWait(2);
    __PlaySound(0xf0);
    *(unsigned short *)(env + 0x1e) = 0;
    __MapActor_SetAnim(0x1e, 4);
    *(int *)(actor + 8) = 0xa8 << 17;
    *(int *)(actor + 0xc) = 0xfff80000;
    *(int *)(actor + 0x10) = 0x84 << 17;
    *(int *)(actor + 0x28) = 0;
    *(int *)(actor + 0x24) = 0;
    {
        int m = 0x14, n = 0x10;
        __Func_8010704(0x13, n, 1, 1, m, n);
    }
    {
        int m = 0x15, n = 0x50;
        __Func_8010704(0x14, n, 1, 1, m, n);
    }
}
