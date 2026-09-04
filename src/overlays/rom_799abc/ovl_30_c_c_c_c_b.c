// fakematch
/* OvlFunc_905_200915c  --  0x0200915c
 *
 * Cut out of goldensun/asm/overlays/rom_799abc/ovl_30_c_c_c_c.s.
 *
 * A short cutscene: an emote, a dialogue line, a turn, three waypoints, and two
 * read-modify-writes of the same actor flag byte on the way in and out.
 *
 * THE FINDING, and it is worth more than the function: NAMING THE ACTOR
 * POINTER COSTS r0. Written the obvious way --
 *
 *     p = __MapActor_GetActor(0xa);
 *     p[0x23] &= 0xfd;
 *
 * -- gcc emits `mov r1, r0 / add r1, #0x23` and does the whole read-modify-write
 * through r1, where the ROM keeps the returned pointer in r0 and issues
 * `add r0, #0x23`. That single copy drags the rest of the block out of step: 45
 * of 72 differing. Dropping the name and subscripting the call result directly
 *
 *     __MapActor_GetActor(0xa)[0x23] &= 0xfd;
 *
 * takes it to 2. The address is then a temp that dies inside the statement, so
 * the allocator has no reason to move it off the return register.
 *
 * FALSE LEAD, recorded because it looked certain: the first guess was that the
 * function-scope `register int p0 __asm__("r0")` argument pin was making r0
 * unavailable for the pointer. Re-screening with every pin group moved into its
 * own block scope gives the IDENTICAL diff, byte for byte. A pin reserves the
 * register at the sites that use it, not across the body, and that hypothesis
 * should not be reached for again.
 *
 * THE LAST TWO INSTRUCTIONS were the `orr` site, and they are a genuinely new
 * boundary on an existing entry. The ROM has the loaded byte in r2 and the
 * constant in r3, accumulating into r3; ours had the roles swapped. FOUR
 * spellings from the existing note -- `|= K`, `K | x`, `x | K`, and an `int`
 * temporary -- ALL SCORE 2, which is the tie the note already predicts. What
 * closes it is naming the DESTINATION of the accumulation as its own statement:
 *
 *     t = q[0x23]; k = 2; k |= t; q[0x23] = k;
 *
 * `k |= t` spells which register the result lands in; `q[0x23] = 2 | t` leaves
 * that to the allocator and it picks the other one. PLAIN `int` LOCALS ARE
 * ENOUGH -- this was first reached with both operands pinned to r2 and r3, and
 * the pins then came out with no change, so they are not part of the match.
 * Pinning ONE operand does not do it either: value-only and constant-only both
 * score 2, and both leave the accumulation running the wrong way. THE
 * DESTINATION FOLLOWS THE TIE, SO BOTH OPERANDS HAVE TO BE NAMED, and naming is
 * all that is required.
 *
 * The `and` site four statements earlier needs none of this -- `&= 0xfd` written
 * directly on the byte gives the ROM's form first time, which is the subreg tie
 * behaving exactly as the existing entry describes. The two sites are the same
 * shape and only one of them needed help; that asymmetry is unexplained.
 *
 * TORN DOWN. Removing the six argument pins and writing the calls as plain
 * literals gives 22 differing and widens the prologue to `push {r5, lr}`, so
 * they stay. The 0x23 offset is past the 5-bit `ldrb` immediate range, which is
 * why the ROM advances the pointer first; that needs no source handle.
 */
extern void OvlFunc_905_20090c8(void);

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __StopTask(void (*f)(void));
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __SetFlag(int id);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);

void OvlFunc_905_200915c(void)
{

    register int p0 __asm__("r0");
    register int p1 __asm__("r1");
    register int p2 __asm__("r2");

    __StopTask(OvlFunc_905_20090c8);
    __CutsceneStart();
    p1 = 0x80; p2 = 0x1e; p0 = 0xd; p1 <<= 1;
    __MapActor_Emote(p0, p1, p2);
    __Func_80925cc(0xd, 2);
    p1 = 0xa0; p2 = 0; p1 <<= 8; p0 = 0;
    __Func_8092adc(p0, p1, p2);
    __MessageID(0x132f);
    __ActorMessage(0xd, 0);
    __MapActor_DoAnim(0xd, 3);
    __CutsceneWait(0x1e);

    __MapActor_GetActor(0xa)[0x23] &= 0xfd;
    p1 = 0x80; p2 = 0x80; p1 <<= 10; p0 = 0xd; p2 <<= 9;
    __MapActor_SetSpeed(p0, p1, p2);
    p1 = 0x96; p0 = 0xd; p1 <<= 2; p2 = 0xd8;
    __Func_80921c4(p0, p1, p2);
    p1 = 0x96; p0 = 0xd; p1 <<= 2; p2 = 0xf8;
    __Func_80921c4(p0, p1, p2);
    p1 = 0x8e; p2 = 0x94; p0 = 0xd; p1 <<= 2; p2 <<= 1;
    __Func_80921c4(p0, p1, p2);
    p1 = 0; p2 = 0; p0 = 0xd;
    __MapActor_SetPos(p0, p1, p2);

    {
        unsigned char *q = __MapActor_GetActor(0xa);
        int t, k;
        t = q[0x23]; k = 2; k |= t;
        q[0x23] = k;
    }
    __SetFlag(0x869);
    __CutsceneEnd();
}
