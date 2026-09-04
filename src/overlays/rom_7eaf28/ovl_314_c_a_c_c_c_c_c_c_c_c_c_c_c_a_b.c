// fakematch
/* OvlFunc_960_2008838  --  0x02008838
 *
 * Cut out of goldensun/asm/overlays/rom_7eaf28/ovl_314_c_a_c_c_c_c_c_c_c_c_c_c_c_a.s.
 *
 * A guarded cutscene: three save-flag tests, and on all three passing a long
 * straight-line body of emotes, dialogue lines and waypoints.
 *
 * THIS FUNCTION CONTRADICTS A RULE IN docs/elevation.md AND THE TOOL BUILT FROM
 * IT. tools/crossed.py reports `crossed-sites=1 AVOID`, and the entry it
 * implements says that when the ROM's mov order runs against its shift order
 * "no arrangement of pins, barriers or statement order reaches it, and the
 * function should be parked on that basis rather than swept". THE SITE HERE IS
 * CROSSED AND IT IS REACHABLE. The call is
 *
 *     mov r1, #0xdc / mov r2, #0x9d / lsl r2, #3 / mov r0, #0xd / lsl r1, #1
 *
 * -- movs r1 then r2, shifts r2 then r1 -- and with the three argument registers
 * pinned and the assignments written in the ROM's order it sits at 2 of 156,
 * the two movs transposed, exactly as the entry predicts. Adding ONE barrier on
 * the first pinned register after its assignment,
 *
 *     q1 = 0xdc; __asm__ volatile ("" : : "r" (q1)); q2 = 0x9d; ...
 *
 * closes it. Barriering both q1 and q2 also matches, so the minimal form is
 * kept. Writing the shift inline as `q1 << 1` at the call instead scores 2 and
 * is the state the entry already describes.
 *
 * The mechanism is not mysterious once seen: the crossed shape needs the first
 * mov materialised BEFORE the second argument's build begins, and that is a
 * scheduling constraint rather than an ordering of operands, so an operand
 * rewrite genuinely cannot reach it and a scheduling barrier trivially can. The
 * documented two-state trap is a property of the SPELLINGS that were swept, not
 * of the shape. The entry and crossed.py both need amending, and the two
 * functions parked on this basis -- src/non_matching/ovl_7c460c/2008ff0.c at 2
 * of 157 and src/non_matching/ovl_7d30e0/2008b68.c -- should be re-screened
 * with this lever before anything else is picked up.
 *
 * PROCESS NOTE: crossed.py exists precisely to reject candidates like this one
 * before any work goes into them, and it was not run during selection. Running
 * it would have skipped a function that matches. It is a filter against a rule
 * that has now been measured wrong, which is the strongest argument for running
 * the screen and reading the residue rather than trusting the pre-filter.
 *
 * Everything else here is ordinary. The three flag tests are one `&&` chain;
 * gcc inverts each condition and branches over a long jump on its own because
 * the epilogue is out of Thumb conditional-branch range, so that shape is not a
 * source tell. Thirteen call sites want interleaved argument fills and are
 * pinned with their assignments in each site's own ROM order; the rest are
 * ascending and take plain literals. Both __MapActor_GetActor results are named
 * locals because each is tested against zero before its fields are read, and
 * the `ldrsh` register-offset form falls out on its own -- Thumb-1 has no
 * immediate-offset encoding for a signed halfword load.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093054(int a, int b);

#define PIN3 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1"); \
             register int q2 __asm__("r2")

void OvlFunc_960_2008838(void)
{
    unsigned char *p;

    if (__GetFlag(0x9a << 4) != 0 && __GetFlag(0x1b7) == 0
        && __GetFlag(0x9b << 4) != 0) {
        __SetFlag(0x9b5);
        __CutsceneStart();
        __MessageID(0x2633);
        p = __MapActor_GetActor(0);
        if (p != 0)
            __MapActor_SetPos(0xd, *(int *)(p + 8), *(int *)(p + 0x10));
        { PIN3; q1 = 0xc0; q0 = 0xd; q1 <<= 8; q2 = 0; __Func_809280c(q0, q1, q2); }
        { PIN3; q1 = 0xdc; q2 = 0x9d; q0 = 0; q1 <<= 1; q2 <<= 3; __Func_80921c4(q0, q1, q2); }
        { PIN3; q1 = 0x80; q0 = 0xd; q1 <<= 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
        { PIN3; q1 = 0xde; q2 = 0x9b; q0 = 0; q1 <<= 1; q2 <<= 3; __Func_80921c4(q0, q1, q2); }
        { PIN3; q1 = 0x80; q0 = 0; q1 <<= 1; q2 = 0x28; __MapActor_Emote(q0, q1, q2); }
        { PIN3; q1 = 0x80; q2 = 0x1e; q0 = 0; q1 <<= 7; __Func_8092adc(q0, q1, q2); }
        __MapActor_DoAnim(0xd, 4);
        __ActorMessage(0xd, 0);
        { PIN3; q0 = 0; q1 = 0x105; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
        { PIN3; q2 = 0x3c; q0 = 0xd; q1 = 0x105; __MapActor_Emote(q0, q1, q2); }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            q1 = 0; q0 = 0xd;
            __ActorMessage(q0, q1);
        }
        __CutsceneWait(0x1e);
        __Func_80925cc(0xd, 2);
        __ActorMessage(0xd, 0);
        { PIN3; q1 = 0xc0; q2 = 0x1e; q0 = 0xd; q1 <<= 8; __Func_8092adc(q0, q1, q2); }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            q1 = 0; q0 = 0xd;
            __Func_8093054(q0, q1);
        }
        __CutsceneWait(0x1e);
        { PIN3; q1 = 0x83; q2 = 0x3c; q0 = 0xd; q1 <<= 1; __MapActor_Emote(q0, q1, q2); }
        __ActorMessage(0xd, 0);
        __MapActor_DoAnim(0xd, 3);
        __ActorMessage(0xd, 0);
        { PIN3; q0 = 0xd; q1 = 0xb333; q2 = 0x5999; __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q1 = 0xdc; __asm__ volatile ("" : : "r" (q1)); q2 = 0x9d; q2 <<= 3; q0 = 0xd; q1 <<= 1; __Func_80921c4(q0, q1, q2); }
        __ActorMessage(0xd, 0);
        __MapActor_DoAnim(0, 3);
        __MapActor_SetAnim(0xd, 2);
        p = __MapActor_GetActor(0);
        if (p != 0)
            __MapActor_TravelTo(0xd, *(short *)(p + 0xa), *(short *)(p + 0x12));
        __MapActor_WaitMovement(0xd);
        __MapActor_SetPos(0xd, 0, 0);
        __CutsceneEnd();
    }
}
