// fakematch
/* OvlFunc_882_200950c  --  0x0200950c
 *
 * Cut out of goldensun/asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_c_c_a.s.
 *
 * A cutscene with two dialogue lines drawn from consecutive message ids. Ten
 * call sites want interleaved argument fills and are pinned with each site's
 * own ROM order; the rest are ascending and take plain literals.
 *
 * THE MESSAGE BASE IS A PINNED CALLEE-SAVED LOCAL, the same lever as
 * src/overlays/rom_7b2078/ovl_314_c_c_a_c_c_c_a_a_a_c_b.c. The ROM loads
 * `0xe67` into r5, feeds __MessageID with `mov r0, r5`, advances it in place
 * with `add r5, #4`, and feeds the second call from the same register. Written
 * as a plain `int`, constant propagation folds both uses to their own pool
 * entries and the register is never taken. `register int m __asm__("r5")` with
 * `m += 4` between the two uses reproduces it.
 *
 * ONE BARRIER HERE, NOT TWO, AND THAT IS THE ENTRY WORTH KEEPING. The pinned
 * load was scheduled one statement early, into __MapActor_Emote's argument
 * group. The sibling function needed the intervening call BRACKETED with a
 * `do { } while (0)` on each side; here the trailing barrier alone is exact and
 * ADDING THE LEADING ONE COSTS TWO INSTRUCTIONS -- it over-constrains the Emote
 * fill and transposes `mov r1, #0x80` against `mov r2, #0x14`.
 *
 * The difference is how far the load was hoisted. In the sibling it had crossed
 * TWO statements and needed a wall on each side; here it crossed ONE, so the
 * wall behind it is all that is required and the wall in front only removes
 * freedom the scheduler was using correctly. THE BRACKET IS NOT A RECIPE:
 * add one wall, measure, and add the second only if the load is still moving.
 */
extern unsigned char gScript_882__0200c8c0[];

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_RunScript(int slot, unsigned char *s);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __SetFlag(int id);
extern void __Func_809202c(void);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_8093054(int a, int b);

#define PIN3 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1"); \
             register int q2 __asm__("r2")
#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")

void OvlFunc_882_200950c(void)
{
    register int m __asm__("r5");

    __CutsceneStart();
    { PIN3; q1 = 0x83; q0 = 0; q1 <<= 1; q2 = 0x32a; __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0x83; q0 = 0x14; q1 <<= 17; q2 = 0x3250000; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0x83; q0 = 0x14; q1 <<= 1; q2 = 0x339; __Func_809218c(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0; q1 <<= 10; q2 <<= 9; __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_Jump(0, 2, 0);
    { PIN3; q1 = 0x8d; q2 = 0x357; q0 = 0; q1 <<= 1; __Func_80921c4(q0, q1, q2); }
    __MapActor_SetAnim(0x14, 1);
    __MapActor_Jump(0, 4, 0);
    { PIN3; q2 = 0; q1 = 0x14; q0 = 0; __Func_8092848(q0, q1, q2); }
    __Func_809202c();
    __CutsceneWait(0x1e);
    __Func_80925cc(0, 2);
    { PIN3; q1 = 0x80; q2 = 0x14; q1 <<= 1; q0 = 0x14; __MapActor_Emote(q0, q1, q2); }
    do { } while (0);
    m = 0xe67;
    __MessageID(m);
    { PIN2; q1 = 0; q0 = 0x14; __ActorMessage(q0, q1); }
    __CutsceneWait(0x14);
    { PIN2; q1 = 0; q0 = 0x14; __Func_8093054(q0, q1); }
    m += 4;
    { PIN2; q1 = 2; q0 = 0x14; __Func_80925cc(q0, q1); }
    __MessageID(m);
    { PIN3; q2 = 0x14; q0 = 0x14; q1 = 0; __Func_8093040(q0, q1, q2); }
    {
        register int q0 __asm__("r0");
        register unsigned char *q1 __asm__("r1");
        q1 = gScript_882__0200c8c0; q0 = 0x14;
        __MapActor_RunScript(q0, q1);
    }
    __SetFlag(0x835);
    __CutsceneEnd();
}
