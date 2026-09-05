// fakematch
/* OvlFunc_882_200adec  --  0x0200adec
 *
 * Cluster OvlFunc_882_200adec extracted from
 * goldensun/asm/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_a_a_c_c_c_c_a.s; the
 * remaining OvlFunc_882_200b1ac stays in the sibling `_b` piece.
 *
 * 363 instructions of straight-line cutscene behind two save flags, plus two
 * actor-flag blocks that are the whole difficulty.  Twenty-two pinned call
 * sites, four register pins and two `do { } while (0)` walls.
 *
 * THE PROLOGUE PICKS THE CURE, AND HERE IT PICKS BOTH.  `push {r5, r6, r7,
 * lr}` is a wide push, but read WHAT it keeps: r6 holds the byte mask 0xfe,
 * r5 holds -13 and r7 holds an actor pointer.  NO SCRIPT CONSTANT IS KEPT --
 * every repeated `0x80 << 9`, `0x557`, `0xca << 15` is rebuilt at each site.
 * So the constants want PINS and the three held values want NAMED LOCALS, in
 * one function.  Plain C hoists 0x80<<9, 0x80<<8, 0x557 and 0xb0<<8 into r8
 * and r10, widening the push: 374 lines against 369, 363 differing.
 *
 * r4 IS USED AND NOT PUSHED.  That is `-fcall-used-r4` working, not a missing
 * save: `r` is dead before the next `bl` at both sites.  gcc will not pick r4
 * on its own here (it takes r0), so `r` carries an explicit r4 pin.
 *
 * THE TWO ACTOR BLOCKS, and the four levers they needed:
 *
 *  1. THE LOADS SIT BETWEEN THE STORE AND THE ARITHMETIC.  The ROM emits
 *     `ldrb [r4,#9]` BEFORE `strb [r1]`, which is only legal if the load came
 *     first in the source -- both refs are alias set 0, so the dependence is
 *     unavoidable and its DIRECTION is the whole signal.  Naming each byte
 *     load into `y` / `y2` and storing afterwards is what reaches it.  This is
 *     the same shape recorded on OvlFunc_931_2008c0c in
 *     src/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_c_b.c; grepping that
 *     file for "these two loads sit between" saved the rediscovery.
 *
 *  2. `-13` MUST NOT NARROW.  Written as the literal `-13 & r[9]` inside the
 *     store expression, combine narrows it against the ldrb's nonzero_bits and
 *     emits `mov r5, #0xf3`.  The ROM has `mov r5, #0xd / neg r5, r5`, i.e. a
 *     full SImode -13.  A plain `int n` assigned AT ITS FIRST USE keeps it.
 *     Assigning `n` earlier (at the top of the cutscene block) is much worse --
 *     the mov lands 50 instructions early and shifts everything.
 *
 *  3. THE COMMUTATIVE `and` NEEDS A LIVE COPY AT SITE 1 AND A DEAD ONE AT
 *     SITE 2.  Site 1 is `mov r3, r6 / and r3, r2` (mask still live), site 2
 *     is `and r6, r3` (mask dies).  Writing `z = m; z = z & y;` gets the copy
 *     only if BOTH ends are hard registers: with `z` unpinned gcc retargets
 *     the ldrb into z's register and emits `and r2, r6`, two instructions
 *     SHORT of the ROM (367 lines against 369).  Pinning `z` alone leaves `m`
 *     rematerialised at site 2; pinning `m` alone loses the copy again.
 *
 *  4. ONE VARIABLE FOR BOTH BYTE LOADS blocks the second load being hoisted
 *     over the first `and`: the reuse is a WAR on the same pseudo, which the
 *     scheduler will not cross.  Two separate names leave `ldrb r2, [r4,#9]`
 *     three slots early.
 *
 * THE TWO WALLS, AND THEY ARE NOT INDEPENDENT.  With the blocks correct two
 * adjacent transpositions remain.  A wall after `*t = m` fixes the trailing
 * `strb r5` / `mov r0, #0` pair but hands the `ldr r4` / `ldrb [r7]` pair back;
 * a SECOND wall earlier in the block fixes that.  Only two placements for the
 * second wall reach exact -- after `t = p + 0x23` (kept) and after `y2 = *t`.
 * After the GetActor call, after the `r` load, or after `r[9] = n` each leaves
 * 2 differing.  `__asm__ volatile ("")` in the tail position measures the same
 * as the loop, so the structural marker is what ships.
 *
 * WHAT EACH REGISTER PIN IS WORTH, measured by removing it from the finished
 * file: `z` in r3 272 differing, `p0` in r0 350, `m` in r6 15, `r` in r4 7,
 * each wall 2.  A fifth pin, `y` in r2, was load-bearing while the call-site
 * pins were still at 61 and became inert once they were minimised -- the
 * batch-221 rule that a rejected or accepted lever must be re-measured after
 * the diagnosis moves, running the other way.
 *
 * THE PIN SET IS 22 OF 61, verified as a set.  Nineteen sites are load-bearing
 * one at a time; the other forty-two were stripped greedily and THREE of them
 * -- the `__Func_80933f8` sites at `0xbb << 16`, `0xdd << 16` and `0xb6 << 16`
 * -- broke once their neighbours were gone (25, 56 and 76 differing) and had to
 * go back.  Re-running the one-at-a-time sweep over the 22 survivors leaves
 * nothing removable.
 *
 * The four `__Func_80933f8` sites take a UNIFORM spelling
 * (`q0 <<= 16; q1 = -q1; q2 <<= 19;`) even though the ROM emits the third one
 * with `lsl r2` first; sched2 produces both orders from the one source form.
 * Transcribing each site's own order costs 2 differing.
 */
extern unsigned char gScript_882__0200cd1c[];

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_SetBehavior(int slot, int s);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void OvlFunc_882_200b1ac(void);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_882_200adec(void)
{
    unsigned char *p;
    register unsigned char *r __asm__("r4");
    unsigned char *t;
    unsigned char *u;
    register int m __asm__("r6");
    int n;
    int y;
    int y2;
    register int z __asm__("r3");
    register int p0 __asm__("r0");

    p0 = 0x840;
    if (__GetFlag(p0) != 0) {
        p0 = 0x841;
        if (__GetFlag(p0) == 0) {
            __CutsceneStart();
            { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0; q1 <<= 9; q2 <<= 8;
              __MapActor_SetSpeed(q0, q1, q2); }
            { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0x16; q1 <<= 9; q2 <<= 8;
              __MapActor_SetSpeed(q0, q1, q2); }
            { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0x1a; q1 <<= 9; q2 <<= 8;
              __MapActor_SetSpeed(q0, q1, q2); }
            { PIN3; q1 = 0x80; q2 = 0x80; q0 = 8; q1 <<= 9; q2 <<= 8;
              __MapActor_SetSpeed(q0, q1, q2); }
            { PIN3; q0 = 0; q1 = 0xd9; q2 = 0x557; __Func_80921c4(q0, q1, q2); }
            p = __MapActor_GetActor(0);
            if (p != 0)
                __MapActor_SetPos(0x16, *(int *)(p + 8), *(int *)(p + 0x10));
            { PIN3; q0 = 0x16; q1 = 0xeb; q2 = 0x557; __Func_80921c4(q0, q1, q2); }
            __Func_8092adc(0x16, 0xb0 << 8, 0);
            p = __MapActor_GetActor(0);
            if (p != 0)
                __MapActor_SetPos(0x1a, *(int *)(p + 8), *(int *)(p + 0x10));
            { PIN3; q0 = 0x1a; q1 = 0xc7; q2 = 0x557; __Func_80921c4(q0, q1, q2); }
            { PIN3; q1 = 0xd0; q0 = 0x1a; q1 <<= 8; q2 = 0;
              __Func_8092adc(q0, q1, q2); }
            { PIN3; q1 = 0xf7; q0 = 0x19; q1 <<= 16; q2 = 0x4ba0000;
              __MapActor_SetPos(q0, q1, q2); }
            __Func_8092adc(0x19, 0xc0 << 7, 0);
            p = __MapActor_GetActor(8);
            u = p + 0x23;
            r = *(unsigned char **)(p + 0x50);
            m = 0xfe;
            y = *u;
            z = m;
            z = z & y;
            n = -13;
            y = r[9];
            *u = z;
            z = n;
            z = z & y;
            z = z | 4;
            r[9] = z;
            p = __MapActor_GetActor(0);
            t = p + 0x23;
            do { } while (0);
            r = *(unsigned char **)(p + 0x50);
            y2 = *t;
            m = m & y2;
            y2 = r[9];
            n = n & y2;
            n = n | 8;
            *t = m;
            do { } while (0);
            r[9] = n;
            p = __MapActor_GetActor(0);
            if (p != 0)
                __MapActor_SetPos(8, *(int *)(p + 8), *(int *)(p + 0x10));
            { PIN3; q0 = 8; q1 = 0xdd; q2 = 0x569; __Func_80921c4(q0, q1, q2); }
            { PIN3; q1 = 0xb0; q2 = 0x3c; q0 = 8; q1 <<= 8;
              __Func_8092adc(q0, q1, q2); }
            __Func_80925cc(0x1a, 2);
            __MessageID(0xec6);
            __Func_8093040(0x1a, 0, 0x28);
            { PIN3; q1 = 0xca; q0 = 9; q1 <<= 15; q2 = 0x4ad0000;
              __MapActor_SetPos(q0, q1, q2); }
            { PIN3; q1 = 0x80; q0 = 9; q1 <<= 6; q2 = 0;
              __Func_8092adc(q0, q1, q2); }
            __Func_8093040(0x1009, 0, 0xa);
            { PIN3; q1 = 0xa0; q2 = 0; q0 = 0x1a; q1 <<= 8;
              __Func_8092adc(q0, q1, q2); }
            __Func_80933d4(0x13333, 0x2666);
            __Func_80933f8(0xca << 15, -1, 0x4ad0000, 1);
            { PIN3; q2 = 0xb333; q0 = 9; q1 = 0x16666;
              __MapActor_SetSpeed(q0, q1, q2); }
            __MapActor_SetBehavior(9, (int)gScript_882__0200cd1c);
            __CutsceneWait(0x3c);
            __Func_80933d4(0x9999, 0x1333);
            { PIN4; q0 = 0xbb; q1 = 1; q2 = 0xa6; q3 = 1; q0 <<= 16; q1 = -q1; q2 <<= 19;
              __Func_80933f8(q0, q1, q2, q3); }
            __Func_8093530();
            __CutsceneWait(0x28);
            __Func_80925cc(0x1a, 2);
            __Func_8093040(0x1a, 0, 0x14);
            __Func_80925cc(9, 2);
            __Func_8093040(0x4009, 0, 0x14);
            __Func_80933d4(0x80 << 10, 0x80 << 7);
            { PIN4; q0 = 0xdd; q1 = 1; q3 = 1; q0 <<= 16; q1 = -q1; q2 = 0x5690000;
              __Func_80933f8(q0, q1, q2, q3); }
            __Func_809280c(0, 8, 0);
            __Func_809280c(0x16, 8, 0);
            { PIN3; q1 = 0xc0; q0 = 0x1a; q1 <<= 6; q2 = 0x50;
              __Func_8092adc(q0, q1, q2); }
            { PIN4; q0 = 0xb6; q1 = 1; q2 = 0xaa; q3 = 1; q0 <<= 16; q1 = -q1; q2 <<= 19;
              __Func_80933f8(q0, q1, q2, q3); }
            { PIN3; q2 = 0xad; q0 = 8; q1 = 0xb6; q2 <<= 3;
              __Func_80921c4(q0, q1, q2); }
            __Func_809280c(8, 9, 0);
            __CutsceneWait(0x1e);
            __MapActor_DoAnim(8, 3);
            __CutsceneWait(0xa);
            __Func_809280c(0, 9, 0);
            __Func_809280c(0x16, 9, 0);
            __Func_809280c(0x1a, 9, 0);
            __MapActor_DoAnim(9, 3);
            __ActorMessage(9, 0);
            __Func_80925cc(0x1a, 2);
            __Func_8093040(0x1a, 0, 0xa);
            { PIN3; q1 = 0xe0; q0 = 9; q1 <<= 8; q2 = 0x28;
              __Func_8092adc(q0, q1, q2); }
            { PIN3; q1 = 0xc0; q2 = 0x14; q0 = 9; q1 <<= 6;
              __Func_8092adc(q0, q1, q2); }
            __MapActor_DoAnim(9, 3);
            __ActorMessage(9, 0);
            __Func_8092848(0x1a, 8, 0);
            __Func_8092848(0x16, 0, 0);
            __CutsceneWait(0x28);
            __Func_809280c(0, 9, 0);
            __Func_809280c(0x16, 9, 0);
            __Func_809280c(0x1a, 9, 0);
            __Func_809280c(8, 9, 0);
            __Func_80925cc(9, 2);
            __CutsceneWait(0x14);
            __Func_8093040(9, 0, 0xa);
            __MapActor_SetAnim(0, 3);
            __MapActor_SetAnim(0x1a, 3);
            __MapActor_SetAnim(0x16, 3);
            __MapActor_DoAnim(8, 3);
            *t |= 1;
            p = __MapActor_GetActor(8);
            p[0x23] |= 1;
            OvlFunc_882_200b1ac();
            p0 = 0x841;
            __SetFlag(p0);
            __CutsceneEnd();
        }
    }
}
