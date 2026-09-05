// fakematch
/* OvlFunc_926_2009dbc  --  0x02009dbc
 * [asm/overlays/rom_7b2078/ovl_314_c_c_a_c_c_c_a_c.s, the file's ONLY function]
 *
 * 644 instructions of straight-line cutscene script, no branches at all beyond
 * the one gcc emits to hop its own mid-body pool. Byte-exact: 1704 bytes, 655
 * encodings and 188 relocations identical under tools/objcmp.py.
 *
 * ONE LEVER, SIXTY-EIGHT TIMES. `push {r5, lr}` with r5 holding the constant 0
 * for five `str r5, [r0, #0x6c]` stores is the whole of the ROM's callee-saved
 * state. Plain literals give `push {r5, r6, r7, lr}` plus a second push of
 * r8-r11 through r5/r6/r7: gcc hoists every repeated script constant --
 * 0x8000, 0xe80000, 0xa80000, 0xc000, 0x3000, 0xcccc, 0x6666, 0x101 -- into
 * callee-saved registers and rebuilds nothing. 659 lines against 645, 622
 * differing. Pinning every non-ascending or pooled argument site takes it to
 * 645/645 with 3 differing, and one more spelling change finishes it.
 *
 * THE PIN SET IS 68 SITES AND EVERY ONE IS INDIVIDUALLY LOAD-BEARING. A greedy
 * one-at-a-time pass over all 69 candidate sites removed exactly one (see
 * below) and nothing else; there is no second round to run because the first
 * found no inert pin. That is unusual -- the sibling functions in this overlay
 * strip a third to a half of their candidates -- and the reason is that the
 * constants here are shared FUNCTION-WIDE rather than locally: 0x14, 0x12, 3
 * and 2 each appear twenty times or more, so dropping any single pin re-opens
 * the same global CSE and changes the prologue. tryc reports such a drop as
 * "556 differ" only because its diff is positional and a three-line prologue
 * change shifts everything after it; the real residue is the push list.
 *
 * THE ONE PIN THAT CAME OFF IS __Func_80933f8, and it came off because a PIN
 * WAS THE WRONG SHAPE THERE, not because the site was inert. The ROM fills
 * `mov r0,#0xe8 / mov r1,#1 / mov r2,#0xc8 / mov r3,#1 / neg r1 / lsl r2 /
 * lsl r0` -- ascending, with r0's use LAST. Pinned in that exact order the
 * scheduler sinks `mov r0` past r1 and r2, because it prioritises by distance
 * to use and r0's use is the final `lsl`: 3 differing. Four spellings fix it
 * and the plain expression call is one of them, so the pin is simply deleted:
 *
 *     __Func_80933f8(0xe8 << 16, -1, 0xc8 << 16, 1);
 *
 * 0xe80000 is used nowhere else in the function, so there is no CSE to destroy
 * and the shifted build falls out unaided. The lesson is that a residue of 3
 * at a pinned site is a scheduling tie, and the cure can be LESS source, not
 * more.
 *
 * NO SYMBOLS NEEDED. 0x1883, 0xcccc, 0x6666, 0x105, 0x101, 0x898 and 0x89b are
 * all pooled in the ROM, and none of them is `mov`-buildable (none is n << k
 * for n < 256), so const.sym's tell does not fire and plain literals reproduce
 * every pool entry and the pool ORDER -- confirmed by objcmp's relocation and
 * encoding compare, which tryc's `=value` normalisation would have hidden.
 *
 * THE TAIL IS THE ovl_314_c_c_a_c_c_c_a_a_a_c_b.c TAIL and its two documented
 * levers were copied verbatim and re-measured here rather than assumed:
 *   - the split `v = 0x80; v <<= 8;` build (inline `0x80 << 8` pools it: +1
 *     line), and
 *   - the named destination pointer `t` assigned ahead of the value (nesting
 *     the dereference swaps r2/r3: 4 differing).
 * Both named locals are load-bearing and both survive minimisation. The three
 * re-calls of __MapActor_GetActor(0x13) are three separate statements, exactly
 * as the ROM re-calls it.
 *
 * MEASURED WORSE, at the __Func_80933f8 site (all against 645 ROM lines):
 *   q0=0xe8; q1=1; q1=-q1; q2=0xc8; q2<<=16; q3=1; q0<<=16;   3 differing
 *   q3=1; q2=0xc8; q1=1; q0=0xe8; q1=-q1; q2<<=16; q0<<=16;   3 differing
 *   q0=0xe8; do{}while(0); q1=1; q2=0xc8; q3=1; ...           2 differing
 * and exact, besides the plain call kept here:
 *   q0=0xe8; q0<<=16; q1=1; q2=0xc8; q3=1; q1=-q1; q2<<=16;
 *   q0=0xe8; q1=1; q2=0xc8; q3=1; q0<<=16; q1=-q1; q2<<=16;
 *   q0=0xe8<<16; q1=1; q2=0xc8; q3=1; q1=-q1; q2<<=16;
 */
extern unsigned char gScript_926__0200c638[];

extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __MessageID(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_8093054(int a, int b);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);

#define P1 register int q0 __asm__("r0")
#define P2 P1; register int q1 __asm__("r1")
#define P3 P2; register int q2 __asm__("r2")
#define P4 P3; register int q3 __asm__("r3")

void OvlFunc_926_2009dbc(void)
{
    *(int *)(__MapActor_GetActor(0x12) + 0x6c) = 0;
    *(int *)(__MapActor_GetActor(0xd) + 0x6c) = 0;
    *(int *)(__MapActor_GetActor(0xe) + 0x6c) = 0;
    *(int *)(__MapActor_GetActor(0xf) + 0x6c) = 0;
    *(int *)(__MapActor_GetActor(0x10) + 0x6c) = 0;
    __MapActor_SetAnim(0xb, 1);
    { P2; q0 = 0x80; q1 = 0x80; q0 <<= 8; q1 <<= 5; __Func_80933d4(q0, q1); }
    __Func_80933f8(0xe8 << 16, -1, 0xc8 << 16, 1);
    __Func_8093530();
    __MessageID(0x1883);
    { P3; q0 = 0xa; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { P3; q0 = 0xc; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    __Func_809218c(0xa, 0x98, 0xc8);
    { P3; q1 = 0x90; q2 = 0xf8; q0 = 0xc; __Func_80921c4(q0, q1, q2); }
    __MapActor_WaitMovement(0xa);
    __Func_809280c(9, 0x13, 0);
    __Func_809280c(0xb, 0x13, 0);
    __Func_809280c(0xd, 0x13, 0);
    __Func_809280c(0xe, 0x13, 0);
    __Func_809280c(0xf, 0x13, 0);
    __Func_809280c(0x10, 0x13, 0);
    __Func_809280c(0x12, 0x13, 0);
    { P3; q1 = 0xc0; q2 = 0xc0; q0 = 0xa; q1 <<= 9; q2 <<= 8; __MapActor_SetSpeed(q0, q1, q2); }
    { P3; q1 = 0x80; q2 = 0x80; q0 = 0xc; q1 <<= 10; q2 <<= 9; __MapActor_SetSpeed(q0, q1, q2); }
    __Func_809218c(0xa, 0x98, 0xc8);
    __Func_80921c4(0xc, 0x90, 0xf8);
    { P3; q1 = 0x13; q2 = 0; q0 = 0xc; __Func_809280c(q0, q1, q2); }
    __MapActor_WaitMovement(0xa);
    { P3; q2 = 0; q0 = 0xa; q1 = 0x13; __Func_809280c(q0, q1, q2); }
    { P2; q1 = 2; q0 = 0x12; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x14);
    __Func_8093040(0x12, 0, 0x28);
    __Func_809280c(9, 0x12, 0);
    __Func_809280c(0xa, 0x12, 0);
    { P3; q1 = 0xc0; q0 = 0xb; q1 <<= 6; q2 = 0; __Func_8092adc(q0, q1, q2); }
    __Func_809280c(0xc, 0x12, 0);
    { P3; q1 = 0xc0; q0 = 0xd; q1 <<= 6; q2 = 0; __Func_8092adc(q0, q1, q2); }
    __Func_809280c(0xe, 0x12, 0);
    __Func_809280c(0xf, 0x12, 0);
    { P3; q1 = 0x12; q2 = 0; q0 = 0x10; __Func_809280c(q0, q1, q2); }
    __CutsceneWait(0x14);
    { P3; q2 = 0x14; q0 = 0x10; q1 = 0; __Func_8093040(q0, q1, q2); }
    { P2; q1 = 3; q0 = 0x12; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x14);
    { P2; q1 = 3; q0 = 0x10; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x14);
    __Func_8093040(0x10, 0, 0x14);
    { P3; q0 = 0x12; q1 = 0x105; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    { P3; q0 = 0x10; q1 = 0x101; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(0x10, 0, 0x14);
    __Func_8093040(0x12, 0, 0x14);
    { P3; q1 = 0x81; q0 = 0x10; q1 <<= 1; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    { P3; q0 = 0xf; q1 = 0x101; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    { P3; q0 = 0xf; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    __Func_80921c4(0xf, 0xd8, 0xb0);
    { P3; q1 = 0xc0; q0 = 0xf; q1 <<= 6; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0xf, 0, 0x14);
    { P3; q1 = 0xb0; q2 = 0x14; q0 = 0x12; q1 <<= 8; __Func_8092adc(q0, q1, q2); }
    { P2; q1 = 4; q0 = 0x12; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x14);
    { P3; q2 = 0x14; q0 = 0x12; q1 = 0; __Func_8093040(q0, q1, q2); }
    __Func_809259c(9, 2);
    __Func_809259c(0xa, 2);
    __Func_809259c(0xb, 2);
    __Func_809259c(0xc, 2);
    __Func_809259c(0xd, 2);
    __Func_809259c(0xe, 2);
    __Func_809259c(0xf, 2);
    { P2; q1 = 2; q0 = 0x10; __Func_809259c(q0, q1); }
    __CutsceneWait(0x28);
    __Func_80925cc(0xd, 2);
    { P3; q2 = 0x14; q0 = 0xd; q1 = 0; __Func_8093040(q0, q1, q2); }
    { P2; q1 = 3; q0 = 0x12; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x14);
    __Func_8093040(0x12, 0, 0x14);
    { P3; q1 = 0xe0; q0 = 0; q1 <<= 8; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    { P3; q1 = 0xa0; q2 = 0x14; q0 = 0x12; q1 <<= 7; __Func_8092adc(q0, q1, q2); }
    { P2; q1 = 0; q0 = 0x12; __Func_8093054(q0, q1); }
    { P3; q1 = 0x101; q2 = 0; q0 = 9; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(5);
    { P3; q1 = 0x101; q2 = 0; q0 = 0xa; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(5);
    { P3; q1 = 0x101; q2 = 0; q0 = 0xb; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(5);
    { P3; q1 = 0x101; q2 = 0; q0 = 0xc; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(5);
    { P3; q1 = 0x101; q2 = 0; q0 = 0xd; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(5);
    { P3; q1 = 0x101; q2 = 0; q0 = 0xe; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(5);
    { P3; q1 = 0x101; q2 = 0; q0 = 0xf; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(5);
    { P3; q2 = 0; q1 = 0x101; q0 = 0x10; __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x3c);
    __Func_80925cc(0x10, 2);
    { P3; q2 = 0x14; q0 = 0x10; q1 = 0; __Func_8093040(q0, q1, q2); }
    { P2; q1 = 3; q0 = 0x12; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x14);
    __Func_8093040(0x12, 0, 0x14);
    { P3; q0 = 0xf; q1 = 0x101; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(0xf, 0, 0x14);
    { P3; q2 = 0; q1 = 0xf; q0 = 0x12; __Func_809280c(q0, q1, q2); }
    __CutsceneWait(0x14);
    { P2; q1 = 4; q0 = 0x12; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x14);
    __Func_8093040(0x12, 0, 0x28);
    __Func_8092848(0xb, 0xa, 0);
    __Func_8092848(0xc, 0xe, 0);
    { P3; q1 = 0xf; q2 = 0; q0 = 0xd; __Func_8092848(q0, q1, q2); }
    __CutsceneWait(0x3c);
    __Func_809280c(0xa, 0x12, 0);
    __Func_809280c(0xb, 0x12, 0);
    __Func_809280c(0xc, 0x12, 0);
    __Func_809280c(0xd, 0x12, 0);
    __Func_809280c(0xe, 0x12, 0);
    { P3; q1 = 0x12; q2 = 0; q0 = 0xf; __Func_809280c(q0, q1, q2); }
    __CutsceneWait(0x14);
    { P3; q2 = 0x14; q0 = 0x12; q1 = 0; __Func_8093040(q0, q1, q2); }
    { P2; q1 = 2; q0 = 0x12; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x14);
    __MapActor_SetAnim(9, 3);
    __MapActor_SetAnim(0xa, 3);
    __MapActor_SetAnim(0xb, 3);
    __MapActor_SetAnim(0xc, 3);
    __MapActor_SetAnim(0xd, 3);
    __MapActor_SetAnim(0xe, 3);
    __MapActor_SetAnim(0xf, 3);
    { P2; q1 = 3; q0 = 0x10; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x14);
    { P3; q1 = 0xa0; q2 = 0x14; q0 = 0x12; q1 <<= 7; __Func_8092adc(q0, q1, q2); }
    { P2; q1 = 0; q0 = 0x12; __Func_8093054(q0, q1); }
    { P2; q1 = 3; q0 = 0x12; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x14);
    { P3; q2 = 0x14; q0 = 0x12; q1 = 0; __Func_8093040(q0, q1, q2); }
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(9, 3);
    __MapActor_SetAnim(0xa, 3);
    __MapActor_SetAnim(0xb, 3);
    __MapActor_SetAnim(0xc, 3);
    __MapActor_SetAnim(0xd, 3);
    __MapActor_SetAnim(0xe, 3);
    __MapActor_SetAnim(0xf, 3);
    { P2; q1 = 3; q0 = 0x10; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x14);
    { P2; q1 = 2; q0 = 0x12; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x14);
    __Func_8093040(0x12, 0, 0x14);
    { P3; q2 = 0x14; q0 = 0x12; q1 = 0; __Func_8093040(q0, q1, q2); }
    __MapActor_SetAnim(0, 3);
    { P2; q1 = 3; q0 = 0x12; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x14);
    { P3; q1 = 0x80; q2 = 0x14; q0 = 0x12; q1 <<= 8; __Func_8092adc(q0, q1, q2); }
    { P2; q1 = 2; q0 = 0x12; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x14);
    { P3; q2 = 0x14; q0 = 0x12; q1 = 0; __Func_8093040(q0, q1, q2); }
    __MapActor_SetAnim(9, 3);
    __MapActor_SetAnim(0xa, 3);
    __MapActor_SetAnim(0xb, 3);
    __MapActor_SetAnim(0xc, 3);
    __MapActor_SetAnim(0xd, 3);
    __MapActor_SetAnim(0xe, 3);
    __MapActor_SetAnim(0xf, 3);
    { P2; q1 = 3; q0 = 0x10; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x14);
    __Func_809218c(0xa, 0x78, 0xc8);
    { P3; q1 = 0x78; q2 = 0xf8; q0 = 0xc; __Func_809218c(q0, q1, q2); }
    __MapActor_WaitMovement(0xa);
    { P3; q1 = 0x80; q2 = 0x14; q0 = 0xb; q1 <<= 8; __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(0xa, 5);
    { P2; q1 = 5; q0 = 0xb; __MapActor_SetAnim(q0, q1); }
    __MapActor_WaitMovement(0xc);
    {
        register int q0 __asm__("r0");
        register unsigned char *q1 __asm__("r1");
        q1 = gScript_926__0200c638; q0 = 0xc;
        __MapActor_SetBehavior(q0, q1);
    }
    { P3; q0 = 0xf; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    __Func_80921c4(0xf, 0xd8, 0xa8);
    __Func_80921c4(0xf, 0xe8, 0xa8);
    { P3; q1 = 0xc0; q2 = 0x14; q0 = 0xf; q1 <<= 8; __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0xf, 3);
    { P3; q1 = 0xe8; q2 = 0xa8; q1 <<= 16; q2 <<= 16; q0 = 0x13; __MapActor_SetPos(q0, q1, q2); }
    *(int *)(__MapActor_GetActor(0x13) + 0xc) = 0xc0 << 12;
    *(int *)(__MapActor_GetActor(0x13) + 0x3c) = 0x80 << 24;
    {
        unsigned short *t = *(unsigned short **)(__MapActor_GetActor(0x13) + 0x50);
        int v = 0x80;
        v <<= 8;
        t[0xf] = v;
    }
    __PlaySound(0x7c);
    __CutsceneWait(0x28);
    __Func_80921c4(0xf, 0xd8, 0x98);
    { P3; q1 = 0x80; q1 <<= 7; q2 = 0x1e; q0 = 0xf; __Func_8092adc(q0, q1, q2); }
    __ClearFlag(0x898);
    __SetFlag(0x89b);
}
