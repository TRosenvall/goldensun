// fakematch
/* OvlFunc_883_20092bc  --  0x020092bc
 *
 * Was the whole of goldensun/asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_a_c_c_c.s,
 * so no split was needed.
 *
 * 146 instructions, twenty-six pinned call sites, two crossed __MapActor
 * fills each taking one barrier after their first mov. `tools/crossed.py`
 * reported `crossed-sites=2` during selection and both were anchored from the
 * listing before the first compile.
 *
 * A PIN ASSIGNED BEFORE A CALL AND USED AFTER IT IS SILENTLY DROPPED, and that
 * cost the only real attempt here. The tail sets a behaviour script on an actor
 * whose task pointer is written first:
 *
 *     mov r0, #0x16 / bl __MapActor_GetActor / ldr r3, =OvlFunc_883_200d72c
 *     ldr r1, =gScript_883__0200e248 / str r3, [r0, #0x6c] / mov r0, #0x16
 *     bl __MapActor_SetBehavior
 *
 * Written with r1 pinned and assigned BEFORE the statement containing
 * __MapActor_GetActor, the `ldr r1` DISAPPEARS ENTIRELY -- the function comes
 * out 145 lines against 146 and __MapActor_SetBehavior is called with r1 never
 * set. r1 is call-clobbered, the call sits between the pin's assignment and its
 * use, so the value cannot survive; gcc drops the dead store and does not
 * rematerialise, because a hard-register pin is not a value it can reload.
 *
 * This is the mirror of the rematerialisation lever and it is worth stating as
 * a hazard rather than a lever: THE PIN MAKES A VALUE DIE ACROSS A CALL, WHICH
 * IS THE POINT WHEN THE ROM REBUILDS IT AND A BUG WHEN THE ROM DOES NOT. The
 * tell is a function one instruction SHORT with an argument register never
 * written -- read the missing instruction, not the diff count. Plain literals
 * with no pin at that site are exact.
 *
 * The two byte read-modify-writes on the actor flag both put the LOADED value
 * in the destination, so both take the int-local form, and both are written
 * with their statements interleaved into the following call's argument fill
 * because that is where the ROM puts the `strb`. The shared constant `1` is a
 * named callee-saved local because the ROM keeps it in r6 across the two sites.
 */
extern unsigned char gScript_883__0200f59c[];
extern unsigned char gScript_883__0200f5ec[];
extern unsigned char gScript_883__0200e248[];
extern void OvlFunc_883_200d72c(void);

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_SetBehavior(int slot, int s);
extern void __MapActor_WaitScript(int slot);
extern void __MessageID(int id);
extern void __SetFlag(int id);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093054(int a, int b);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_883_20092bc(void)
{
    unsigned char *a;
    register int k __asm__("r6");
    int t;

    a = __MapActor_GetActor(0x16);
    __CutsceneStart();
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0; q1 <<= 10; q2 <<= 10;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_Jump(0, 5, 0);
    a += 0x5a;
    { PIN3; q0 = 0; q1 = 0xd7; q2 = 0x193; __Func_809218c(q0, q1, q2); }
    {
        PIN3;
        t = *a; k = 1; t |= k;
        q1 = 0xa6;
        *a = t;
        q0 = 0x16; q1 <<= 16; q2 = 0x1770000;
        __MapActor_SetPos(q0, q1, q2);
    }
    { PIN3; q1 = 0x80; q0 = 0x16; q1 <<= 6; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    {
        PIN3;
        t = *a; q1 = 0xa0; t ^= k; q2 = 0xa0;
        *a = t;
        q0 = 0x16; q1 <<= 10; q2 <<= 10;
        __MapActor_SetSpeed(q0, q1, q2);
    }
    __MapActor_Jump(0x16, 4, 0);
    { PIN3; q2 = 0x18b; q0 = 0x16; q1 = 0xca; __Func_80921c4(q0, q1, q2); }
    { PIN2; q1 = 1; q0 = 0; __MapActor_SetAnim(q0, q1); }
    __CutsceneWait(0xa);
    { PIN3; q1 = 0xb0; q0 = 0; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xc0; q0 = 0x16; q1 <<= 6; q2 = 0x18;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0; q0 = 0; q1 <<= 1;
      __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = 2; q0 = 0; __Func_809259c(q0, q1); }
    __CutsceneWait(0x14);
    {
        PIN3;
        q1 = 0xc0; __asm__ volatile ("" : : "r" (q1));
        q2 = 0x80; q2 <<= 9; q0 = 0x16; q1 <<= 9;
        __MapActor_SetSpeed(q0, q1, q2);
    }
    { PIN2; q1 = (int)gScript_883__0200f59c; q0 = 0;
      __MapActor_SetBehavior(q0, q1); }
    __CutsceneWait(0xa);
    { PIN3; q2 = 0; q0 = 0x16; q1 = 0x103; __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = (int)gScript_883__0200f5ec; q0 = 0x16;
      __MapActor_SetBehavior(q0, q1); }
    __MapActor_WaitScript(0);
    { PIN3; q1 = 0x80; q2 = 0xed; q0 = 0; q1 <<= 1; q2 <<= 1;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0xc0; q1 <<= 8; q2 = 0; q0 = 0;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_WaitScript(0x16);
    {
        PIN3;
        q1 = 0x80; __asm__ volatile ("" : : "r" (q1));
        q2 = 0xe4; q2 <<= 1; q0 = 0x16; q1 <<= 1;
        __Func_80921c4(q0, q1, q2);
    }
    __MapActor_SetAnim(0, 1);
    { PIN3; q1 = 0x80; q2 = 0x14; q0 = 0x16; q1 <<= 7;
      __Func_8092adc(q0, q1, q2); }
    { PIN2; q1 = 2; q0 = 0x16; __Func_809259c(q0, q1); }
    __CutsceneWait(0x14);
    __MessageID(0xfce);
    { PIN2; q1 = 0; q0 = 0x16; __Func_8093054(q0, q1); }
    *(void **)(__MapActor_GetActor(0x16) + 0x6c) = OvlFunc_883_200d72c;
    __MapActor_SetBehavior(0x16, (int)gScript_883__0200e248);
    __SetFlag(0x823);
    __CutsceneEnd();
}
