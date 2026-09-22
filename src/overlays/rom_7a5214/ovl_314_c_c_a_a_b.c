/* Cluster OvlFunc_918_200869c..OvlFunc_918_200869c extracted from
 * goldensun/asm/overlays/rom_7a5214/ovl_314_c_c_a_a.s (2 functions; the sibling
 * OvlFunc_918_2008918 stays in assembly).
 *
 * Never attempted before batch 280. SIXTEEN PIN SITES -- one fakematch row. No flags.
 *
 * ===== THE RESIDUE AT THIS SIZE IS ONE DEFECT REPEATED, NOT A NEW KIND OF DEFECT =====
 *
 * Unpinned, the four functions landed together in batch 280 measured 170 / 213 / 253 / 262
 * differing lines, and essentially ALL of it was one thing: cse1 commons the shifted constants
 * and the repeated pool loads into callee-saved registers, where the ROM rebuilds them per site.
 * Pinning every multi-constant argument site took one of them from 170 to ZERO on its own. So
 * three structural rules applied uniformly, plus one per-function tell -- not "many levers each
 * moving one or two instructions".
 *
 * ===== PIN4 IS HARMFUL AT A CALL THAT HAS STACK ARGUMENTS (new) =====
 *
 * Pinning r3 at a six-argument call claims r2 and r3, so reload pushes the two stack-slot
 * temporaries into r4 -- three spurious `mov r4, rN` and a four-instruction overrun. On one of
 * these functions, dropping just those two pins was 96 differing to 22.
 *
 * THE TELL IS THAT THE ROM USES NO r4 AT ALL in any of the four. The landed precedents only ever
 * used PIN2/PIN3, and this is why: PIN4 is for calls whose arguments all fit in registers.
 *
 * And the stack-argument pair itself wants ITS OWN NAMED LOCALS, one per slot:
 * `movs r3,#24 / movs r2,#9 / str r3,[sp] / str r2,[sp,#4]` is two fresh registers both stored,
 * where literals make gcc reuse one register and interleave. That was 22 to 0.
 *
 * ===== REFERENCING A .global DOT-PREFIXED DATA LABEL FROM C (new to landed code) =====
 *
 *     extern unsigned char gL5088[] __asm__(".L5088");
 *     t = (int) gL5088;        -> ldr rN, .Lx / .word .L5088 / R_ARM_ABS32 .L5088
 *
 * No generated `.s` in the tree referenced a `.L`-style DATA symbol before this batch. The label
 * is `.global` in asm/overlays/rom_78ef88/ovl_314_c_c_c_c_c_c.s:488, verified at landing, so no
 * label.sym entry is needed -- try the __asm__ rename before requesting one.
 *
 * THE VALUE MUST BE HELD IN AN `int`. The ROM only ever `mov`s it, which is what lets it live in
 * r8/r10; an `unsigned char *` local gets a low register and `adds r0, rN, #0` instead.
 */
extern int _AREA_2d;
extern unsigned char gState[];
extern unsigned char iwram_3001ebc[];
extern unsigned char gScript_918__02009db4[];
extern unsigned char gScript_918__02009ddc[];
extern unsigned char gScript_918__02009e04[];
extern unsigned char gScript_918__02009e2c[];

extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int a);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_Surprise(int slot, int a);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __MapActor_RunScript(int slot, unsigned char *s);
extern void __Func_8091eb0(int a, int b);
extern void __Func_8091f90(int a, int b);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092b08(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void OvlFunc_918_2009424(int a);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_918_200869c(void)
{
    unsigned char *p;
    unsigned char *q;
    int f;

    f = __GetFlag(0x3);
    __CutsceneStart();
    __PlaySound(0x11);
    __MessageID(0x14ce);
    __Func_8093040(0x8009, 0x0, 0x14);
    __PlaySound(0x1d);
    { PIN3; q0 = 0x0; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetSpeed(0x3, 0x80 << 9, 0x80 << 8);
    p = __MapActor_GetActor(0x3) + 0x23;
    *p &= 0xfe;
    __Func_8092b08(0x3, 0x2);
    p = __MapActor_GetActor(0x0) + 0x23;
    *p &= 0xfe;
    __Func_8092b08(0x0, 0x2);
    p = __MapActor_GetActor(0x0);
    if (p != 0)
        __MapActor_SetPos(0x1, *(int *) (p + 0x8), *(int *) (p + 0x10));
    p = __MapActor_GetActor(0x0);
    if (p != 0)
        __MapActor_SetPos(0x2, *(int *) (p + 0x8), *(int *) (p + 0x10));
    if (f != 0) {
        p = __MapActor_GetActor(0x0);
        if (p != 0)
            __MapActor_SetPos(0x3, *(int *) (p + 0x8), *(int *) (p + 0x10));
        __MapActor_SetBehavior(0x3, gScript_918__02009e2c);
    }
    __MapActor_SetBehavior(0x0, gScript_918__02009db4);
    __MapActor_SetBehavior(0x1, gScript_918__02009ddc);
    __MapActor_RunScript(0x2, gScript_918__02009e04);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x0; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0xc0 << 8; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(0x8, 0xb);
    __CutsceneWait(0xa);
    __MapActor_SetAnim(0x8, 0x8);
    __CutsceneWait(0x14);
    OvlFunc_918_2009424(0x8);
    { PIN2; q0 = 0x8008; q1 = 0x0;
      __ActorMessage(q0, q1); }
    __Func_809259c(0x0, 0x2);
    __Func_809259c(0x1, 0x2);
    __Func_809259c(0x3, 0x2);
    __Func_809259c(0x2, 0x2);
    { PIN3; q0 = 0x0; q1 = 0x80 << 1; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0x80 << 1; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0x80 << 1; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    __MapActor_Emote(0x2, 0x80 << 1, 0x3c);
    OvlFunc_918_2009424(0xb);
    { PIN3; q0 = 0x8008; q1 = 0x0; q2 = 0xa;
      __Func_8093040(q0, q1, q2); }
    __Func_809259c(0x0, 0x1);
    __Func_809259c(0x1, 0x1);
    __Func_809259c(0x3, 0x1);
    __Func_80925cc(0x2, 0x1);
    { PIN2; q0 = 0x8008; q1 = 0x0;
      __ActorMessage(q0, q1); }
    { PIN2; q0 = 0x0; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    { PIN2; q0 = 0x1; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    { PIN2; q0 = 0x3; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    __MapActor_Surprise(0x2, 0x81 << 1);
    __CutsceneWait(0x28);
    OvlFunc_918_2009424(0xb);
    __ActorMessage(0x8008, 0x0);
    q = *(unsigned char **) iwram_3001ebc;
    *(int *) (q + (0xe0 << 1)) = 0x200;
    *(int *) (q + 0x1c8) = 0x40;
    gState[0x22b] = 0x3;
    __Func_8091f90((int) (&_AREA_2d), 0x13);
    __Func_8091eb0(0x24, 0x0);
    __CutsceneEnd();
}
