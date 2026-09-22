/* Cluster OvlFunc_896_2008a98..OvlFunc_896_2008a98 extracted from
 * goldensun/asm/overlays/rom_78ef88/ovl_314_c_c_a_c_a_a.s -- ONE function in its .s, no split.
 *
 * Never attempted before batch 280. TWELVE PIN SITES -- one fakematch row. No flags.
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

extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __WaitFrames(int n);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
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
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern int OvlFunc_896_200c260(int a, int b, int c, int d);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_896_2008a98(void)
{
    int t;
    unsigned char *q;
    int i;
    int n;
    int s1;
    int s2;
    int u;
    int v;
    int w;
    int x;
    int h;

    __PlaySound(0x8d);
    i = 0x0;
    do {
        __Func_8091200(0x4049d2, 0x1);
        __Func_8091254(0x8);
        __CutsceneWait(0x8);
        __Func_8091200(0x80 << 9, 0x1);
        __Func_8091254(0x8);
        __CutsceneWait(0x8);
        if (i == 0x1)
            __Func_8012330(0x80 << 9, 0x80 << 9, 0x80 << 9);
        i++;
    } while (i != 0x6);
    { PIN3; q0 = 0x80 << 10; q1 = 0x80 << 9; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0x1e);
    { PIN2; q0 = 0x26666; q1 = 0x4ccc;
      __Func_80933d4(q0, q1); }
    { PIN4; q0 = 0xa7 << 16; q1 = -0x1; q2 = 0x2110000; q3 = 0x1;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    { PIN3; q0 = 0x80 << 9; q1 = 0x80 << 10; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0x14);
    __PlaySound(0x90);
    t = (int) gL5088;
    __Func_8010560(t, 0x41, 0x1f);
    n = 0xa;
    u = 0x1f;
    __Func_8010704(0x0, 0x0, 0x1, 0x1, n, u);
    s1 = 0x1;
    s2 = 0x2;
    __CopyMapTiles(0x57, 0x2a, 0xa, 0x21, s1, s2);
    __CutsceneWait(0x28);
    __Func_8012330(0x0, 0x0, 0x0);
    __Func_80933d4(0x66666, 0xcccc);
    { PIN4; q0 = 0x1870000; q1 = -0x1; q2 = 0xb1 << 16; q3 = 0x1;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    { PIN3; q0 = 0x80 << 9; q1 = 0x80 << 10; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0x14);
    __PlaySound(0x90);
    __Func_8010560(t, 0x4f, 0x9);
    v = 0x18;
    w = 0x9;
    __Func_8010704(0x0, 0x0, 0x1, 0x1, v, w);
    __CopyMapTiles(0x57, 0x2a, 0x18, 0xb, s1, s2);
    __CutsceneWait(0x28);
    __Func_8012330(0x0, 0x0, 0x0);
    __Func_80933d4(0x26666, 0x4ccc);
    { PIN4; q0 = 0x2470000; q1 = -0x1; q2 = 0xc1 << 16; q3 = 0x1;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    { PIN3; q0 = 0x80 << 9; q1 = 0x80 << 10; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0x14);
    __PlaySound(0x90);
    __Func_8010560(t, 0x5b, 0xa);
    x = 0x24;
    __Func_8010704(0x0, 0x0, 0x1, 0x1, x, n);
    __CopyMapTiles(0x57, 0x2a, 0x24, 0xc, s1, s2);
    __CutsceneWait(0x28);
    q = *(unsigned char **) iwram_3001ebc;
    *(int *) (q + (0xe0 << 1)) = 0x202;
    __MapTransitionOut();
    __WaitMapTransition();
    { PIN4; q0 = 0xe8 << 16; q1 = -0x1; q2 = 0x1dd0000; q3 = 0x0;
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
    __CopyMapTiles(0x0, 0x28, 0xd, 0x42, 0x3, 0x3);
    __CutsceneWait(0x14);
    { PIN4; q0 = 0xdf; q1 = 0xe8 << 16; q2 = 0x80 << 13; q3 = 0xe8 << 17;
      h = OvlFunc_896_200c260(q0, q1, q2, q3); }
    __CutsceneWait(0x28);
    __Func_8019908(h, 0x1);
    __Func_801776c(0x1077, 0x1);
}
