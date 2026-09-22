/* Cluster OvlFunc_896_2008390..OvlFunc_896_20086f4 extracted from
 * goldensun/asm/overlays/rom_78ef88/ovl_314_c_c_a.s.
 *
 * THIS FILE CONVERTS WHOLE: the `.s` held exactly these two functions and both matched, so no
 * split was needed and no linker line changed. Verified as ONE TU against the whole reference --
 * 678 encodings each side, disassembly identical, both relocation tables 180 lines.
 *
 * Requires _MSG_1075 and _MSG_1076; see message.sym for the mechanism and the internal control.
 *
 * Never attempted before batch 280. FOURTEEN PIN SITES EACH -- two fakematch rows. No flags.
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
extern unsigned char gL5088[] __asm__(".L5088");
extern unsigned char iwram_3001ebc[];

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __MessageID(int id);
extern void __SetFlag(int id);
extern void __WaitFrames(int n);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_800fe9c(void);
extern void __Func_8010560(int p, int a, int b);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8012350(void);
extern void __Func_8019908(int a, int b);
extern void __Func_801776c(int a, int b);
extern void __Func_8091200(int a, int b);
extern void __Func_8091254(int a);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void OvlFunc_896_200c248(int a, int b);
extern int OvlFunc_896_200c260(int a, int b, int c, int d);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

#include "message.h"

void OvlFunc_896_2008390(void)
{
    int t;
    unsigned char *q;
    int i;
    int a;
    int b;
    int s1;
    int s2;
    int u;
    int v;
    int h;
    int m;

    __CutsceneStart();
    __PlaySound(0x8d);
    i = 0x0;
    do {
        __Func_8091200(0x403a52, 0x1);
        __Func_8091254(0x8);
        __CutsceneWait(0x8);
        __Func_8091200(0x80 << 9, 0x1);
        __Func_8091254(0x8);
        __CutsceneWait(0x8);
        if (i == 0x1)
            __Func_8012330(0x80 << 9, 0x80 << 9, 0x80 << 9);
        i++;
    } while (i != 0x6);
    { PIN3; q0 = 0x80 << 9; q1 = 0x80 << 10; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __Func_80933d4(0x26666, 0x4ccc);
    { PIN4; q0 = 0xa6 << 18; q1 = -0x1; q2 = 0x1f10000; q3 = 0x1;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x80 << 9; q1 = 0x80 << 10; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    __PlaySound(0x90);
    t = (int) gL5088;
    __Func_8010560(t, 0x60, 0x1d);
    a = 0x29;
    b = 0x1d;
    __Func_8010704(0x0, 0x0, 0x1, 0x1, a, b);
    s1 = 0x1;
    s2 = 0x2;
    __CopyMapTiles(0x57, 0x2a, 0x29, 0x1f, s1, s2);
    __CutsceneWait(0x28);
    __Func_8012330(0x0, 0x0, 0x0);
    __Func_80933d4(0x66666, 0xcccc);
    { PIN4; q0 = 0x1370000; q1 = -0x1; q2 = 0x1f10000; q3 = 0x1;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    { PIN3; q0 = 0x80 << 9; q1 = 0x80 << 10; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0x14);
    __PlaySound(0x90);
    __Func_8010560(t, 0x4a, 0x1d);
    u = 0x13;
    __Func_8010704(0x0, 0x0, 0x1, 0x1, u, b);
    __CopyMapTiles(0x57, 0x2a, 0x13, 0x1f, s1, s2);
    __CutsceneWait(0x28);
    __Func_8012330(0x0, 0x0, 0x0);
    { PIN4; q0 = 0x2970000; q1 = -0x1; q2 = 0xc0 << 16; q3 = 0x1;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    { PIN3; q0 = 0x80 << 9; q1 = 0x80 << 10; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0x14);
    __PlaySound(0x90);
    __Func_8010560(t, 0x60, 0xa);
    v = 0xa;
    __Func_8010704(0x0, 0x0, 0x1, 0x1, a, v);
    __CopyMapTiles(0x57, 0x2a, 0x29, 0xc, s1, s2);
    __CutsceneWait(0x28);
    q = *(unsigned char **) iwram_3001ebc;
    *(int *) (q + (0xe0 << 1)) = 0x202;
    __MapTransitionOut();
    __WaitMapTransition();
    { PIN4; q0 = 0x2c60000; q1 = -0x1; q2 = 0xed << 17; q3 = 0x0;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_800fe9c();
    __WaitFrames(0x1);
    { PIN3; q0 = 0x80 << 10; q1 = 0x80 << 9; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x28);
    __PlaySound(0x121);
    { PIN3; q0 = -0x1; q1 = -0x1; q2 = 0xe666;
      __Func_8012330(q0, q1, q2); }
    __Func_8012350();
    __CutsceneWait(0x14);
    __CopyMapTiles(0x0, 0x28, 0x2b, 0x42, 0x3, 0x3);
    __CutsceneWait(0x14);
    { PIN4; q0 = 0xdc; q1 = 0xb2 << 18; q2 = 0x80 << 13; q3 = 0xe8 << 17;
      h = OvlFunc_896_200c260(q0, q1, q2, q3); }
    __CutsceneWait(0x28);
    __Func_8019908(h, 0x1);
    m = (int) (&_MSG_1075);
    __Func_801776c(m, 0x1);
    { PIN3; q0 = 0x9; q1 = 0x80 << 6; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x5; q1 = 0x80 << 6; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __MapTransitionOut();
    __WaitMapTransition();
    { PIN4; q0 = 0xe7 << 17; q1 = -0x1; q2 = 0xaf << 17; q3 = 0x0;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_800fe9c();
    __WaitFrames(0x1);
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x28);
    __MapActor_Jump(0x9, 0x4, 0x1e);
    __MessageID(m - 0x1);
    OvlFunc_896_200c248(0x9, 0x14);
    __MapTransitionOut();
    __WaitMapTransition();
    __Func_80933f8(0x2c60000, -0x1, 0xed << 17, 0x0);
    __Func_800fe9c();
    __WaitFrames(0x1);
    __MapTransitionIn();
    __WaitMapTransition();
    __SetFlag(0x83c);
    __CutsceneEnd();
}


void OvlFunc_896_20086f4(void)
{
    int t;
    unsigned char *q;
    int i;
    int a;
    int b;
    int s1;
    int s2;
    int u;
    int c;
    int h;
    int m;

    __CutsceneStart();
    __PlaySound(0x8d);
    i = 0x0;
    do {
        __Func_8091200(0x404a4e, 0x1);
        __Func_8091254(0x8);
        __CutsceneWait(0x8);
        __Func_8091200(0x80 << 9, 0x1);
        __Func_8091254(0x8);
        __CutsceneWait(0x8);
        if (i == 0x1)
            __Func_8012330(0x80 << 9, 0x80 << 9, 0x80 << 9);
        i++;
    } while (i != 0x6);
    { PIN3; q0 = 0x80 << 9; q1 = 0x80 << 10; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __Func_8012330(0x0, 0x0, 0x0);
    __Func_80933d4(0x59999, 0xb333);
    { PIN4; q0 = 0xec << 17; q1 = -0x1; q2 = 0xc4 << 15; q3 = 0x1;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    { PIN3; q0 = 0x80 << 9; q1 = 0x80 << 10; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0x14);
    __PlaySound(0x90);
    t = (int) gL5088;
    __Func_8010560(t, 0x54, 0x4);
    a = 0x1d;
    u = 0x4;
    __Func_8010704(0x0, 0x0, 0x1, 0x1, a, u);
    s1 = 0x1;
    s2 = 0x2;
    __CopyMapTiles(0x57, 0x2a, 0x1d, 0x6, s1, s2);
    __CutsceneWait(0x28);
    __Func_8012330(0x0, 0x0, 0x0);
    { PIN4; q0 = 0x1570000; q1 = -0x1; q2 = 0x1710000; q3 = 0x1;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    { PIN3; q0 = 0x80 << 9; q1 = 0x80 << 10; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0x14);
    __PlaySound(0x90);
    __Func_8010560(t, 0x4c, 0x15);
    b = 0x15;
    __Func_8010704(0x0, 0x0, 0x1, 0x1, b, b);
    __CopyMapTiles(0x57, 0x2a, 0x15, 0x17, s1, s2);
    __CutsceneWait(0x28);
    __Func_8012330(0x0, 0x0, 0x0);
    __Func_80933d4(0x33333, 0x6666);
    { PIN4; q0 = 0x1570000; q1 = -0x1; q2 = 0x1f10000; q3 = 0x1;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    { PIN3; q0 = 0x80 << 9; q1 = 0x80 << 10; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0x14);
    __PlaySound(0x90);
    __Func_8010560(t, 0x4c, 0x1d);
    __Func_8010704(0x0, 0x0, 0x1, 0x1, b, a);
    __CopyMapTiles(0x57, 0x2a, 0x15, 0x1f, s1, s2);
    __CutsceneWait(0x28);
    q = *(unsigned char **) iwram_3001ebc;
    *(int *) (q + (0xe0 << 1)) = 0x202;
    __MapTransitionOut();
    __WaitMapTransition();
    { PIN4; q0 = 0xb2 << 18; q1 = -0x1; q2 = 0x98 << 16; q3 = 0x0;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_800fe9c();
    __WaitFrames(0x1);
    { PIN3; q0 = 0x80 << 10; q1 = 0x80 << 9; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x28);
    __PlaySound(0x121);
    { PIN3; q0 = -0x1; q1 = -0x1; q2 = 0xe666;
      __Func_8012330(q0, q1, q2); }
    __Func_8012350();
    __CutsceneWait(0x14);
    __CopyMapTiles(0x0, 0x28, 0x2b, 0x2e, 0x3, 0x3);
    c = 0xb2 << 18;
    __CutsceneWait(0x14);
    { PIN4; q0 = 0xdd; q1 = c; q2 = 0x80 << 13; q3 = 0x90 << 16;
      h = OvlFunc_896_200c260(q0, q1, q2, q3); }
    __CutsceneWait(0x28);
    __Func_8019908(h, 0x1);
    m = (int) (&_MSG_1076);
    __Func_801776c(m, 0x1);
    { PIN3; q0 = 0x9; q1 = 0xe0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x5; q1 = 0xe0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __MapTransitionOut();
    __WaitMapTransition();
    { PIN4; q0 = 0xe7 << 17; q1 = -0x1; q2 = 0xaf << 17; q3 = 0x0;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_800fe9c();
    __WaitFrames(0x1);
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x28);
    __MapActor_Jump(0x9, 0x4, 0x1e);
    __MessageID(m - 0x2);
    OvlFunc_896_200c248(0x9, 0x14);
    __MapTransitionOut();
    __WaitMapTransition();
    __Func_80933f8(c, -0x1, 0x98 << 16, 0x0);
    __Func_800fe9c();
    __WaitFrames(0x1);
    __MapTransitionIn();
    __WaitMapTransition();
    __SetFlag(0x83d);
    __CutsceneEnd();
}

