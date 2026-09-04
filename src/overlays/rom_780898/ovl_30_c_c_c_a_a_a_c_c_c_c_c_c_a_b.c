// fakematch
/* OvlFunc_883_200b1b4  --  0x0200b1b4
 *
 * Cut out of goldensun/asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_c_c_c_a.s.
 *
 * TWO CROSSED SITES, both closed with the batch-206 volatile-asm barrier, and
 * one of them is literally the call that lever was first measured on:
 * `__Func_8012330(-1, -1, 0xe666)`, which
 * src/non_matching/ovl_7d30e0/2008b68.c parked and
 * src/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_a_c_a_c_a.c elevated. Seeing it
 * again in a different overlay is the argument for the lever being general
 * rather than a property of that one function.
 *
 * THE BARRIER IS PER-MOV, AND THAT IS THE NEW PART. The first site is a
 * three-register fill:
 *
 *     mov r0, #0x80 / mov r1, #0x80 / mov r2, #0x80 / lsl r2, #9 / lsl r0, #10 / lsl r1, #10
 *
 * -- movs r0, r1, r2 against shifts r2, r0, r1. One barrier after `q0 = 0x80`
 * fixes r0's position and leaves r1 and r2 transposed at 2 of 89, because the
 * remaining pair still orders itself by which shift consumes first. A second
 * barrier after `q1 = 0x80` is exact. So the rule is not "barrier the first
 * mov" but BARRIER EACH MOV WHOSE POSITION IS WRONG, IN ROM ORDER: n movs
 * needing a specific order need n-1 barriers, since the last one has nothing
 * left to be ordered against.
 *
 * The three __Actor_SetSpriteFlags calls subscript __MapActor_GetActor's result
 * directly rather than through a named local, so the address dies inside the
 * statement and stays in r0 -- the lever from
 * src/overlays/rom_799abc/ovl_30_c_c_c_c_b.c. The script pointer IS a named
 * local, because it is used at three call sites across intervening calls and
 * the ROM holds it in r5 exactly as that requires.
 */
extern unsigned char gScript_883__0200e65c[];

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __WaitFrames(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int n);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __MapActor_RunScript(int slot, unsigned char *s);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __SetFlag(int id);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);

#define PIN3 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1"); \
             register int q2 __asm__("r2")

void OvlFunc_883_200b1b4(void)
{
    unsigned char *s;

    __CutsceneStart();
    __Actor_SetSpriteFlags(__MapActor_GetActor(0xc), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0xd), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0xe), 0);
    __MapActor_SetAnim(0xc, 0);
    __MapActor_SetAnim(0xd, 0);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        q1 = 0; q0 = 0xe;
        __MapActor_SetAnim(q0, q1);
    }
    __WaitFrames(0x14);
    {
        PIN3;
        q0 = 0x80; __asm__ volatile ("" : : "r" (q0));
        q1 = 0x80; __asm__ volatile ("" : : "r" (q1));
        q2 = 0x80; q2 <<= 9; q0 <<= 10; q1 <<= 10;
        __Func_8012330(q0, q1, q2);
    }
    s = gScript_883__0200e65c;
    __MapActor_SetBehavior(0xc, s);
    __WaitFrames(0xa);
    {
        register int q0 __asm__("r0");
        register unsigned char *q1 __asm__("r1");
        q1 = s; q0 = 0xd;
        __MapActor_SetBehavior(q0, q1);
    }
    {
        PIN3;
        q0 = 1; __asm__ volatile ("" : : "r" (q0));
        q1 = 1; q2 = 0xe666; q1 = -q1; q0 = -q0;
        __Func_8012330(q0, q1, q2);
    }
    __WaitFrames(0x14);
    {
        register int q0 __asm__("r0");
        register unsigned char *q1 __asm__("r1");
        q1 = s; q0 = 0xe;
        __MapActor_RunScript(q0, q1);
    }
    { PIN3; q1 = 0x80; q2 = 0x28; q0 = 0xb; q1 <<= 1; __MapActor_Emote(q0, q1, q2); }
    __Func_80925cc(0xb, 2);
    { PIN3; q1 = 0xd0; q1 <<= 8; q2 = 0xa; q0 = 0xb; __Func_8092adc(q0, q1, q2); }
    __MessageID(0x1c90);
    __Func_8093040(0xb, 0, 0x28);
    { PIN3; q2 = 0x14; q0 = 0xb; q1 = 0; __Func_809280c(q0, q1, q2); }
    __ActorMessage(0xb, 0);
    { PIN3; q1 = 0x80; q1 <<= 8; q2 = 0xa; q0 = 0xb; __Func_8092adc(q0, q1, q2); }
    __SetFlag(0x305);
    __CutsceneEnd();
}
