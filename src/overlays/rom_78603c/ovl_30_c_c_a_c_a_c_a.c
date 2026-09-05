// fakematch
/* OvlFunc_885_2008be0  --  0x02008be0
 *   [asm/overlays/rom_78603c/ovl_30_c_c_a_c_a_c_a.s, 1st of 1]
 *
 * 666 instructions of straight-line cutscene behind ONE save-flag guard, with
 * two two-armed __MessageID branches and a shared tail.  The .s holds exactly
 * one function and overlays/rom_78603c/overlay.ld:26 already names the single
 * .o, so landing needs NO split and NO linker-script change.
 *
 * THE WILDCARD TRAP IS ALREADY DISARMED FOR THIS ONE, AND THAT WAS MEASURED.
 * Three earlier split products under `rom_78603c/ovl_30_c_c_a_c_a%` each had to
 * be rescued with an explicit -O2 override, and the pattern was finally NARROWED
 * to the one file that wants -O1 (Makefile:823, `ovl_30_c_c_a_c_a_b.o`).  This
 * TU therefore falls to the tree default `asm/%.o: src/%.c` at -O2 and needs no
 * rule of its own.  Verified rather than assumed: objcmp run against the
 * ORIGINAL asm path prints NO `(built with: ...)` line and is byte-identical,
 * exactly matching the run against a scratch copy of the reference.  It is the
 * first new child under this prefix that needs no Makefile entry.
 *
 * ONE REGISTER, FOUR RANGES, AND THREE OF THEM NEED A NAME.  The prologue is
 * `push {r5, r6, r7, lr}` and every saved register holds a value, so this is a
 * PIN function with a wide push, not an allocation problem:
 *   r7  message base 0xf85, live across the whole body (`mov r0, r7`,
 *       then `mov r0, r7 / add r0, #0xa` and `#0xb` in the two arms).
 *   r6  the actor's z, `<< 16`, live across two __MapActor_SetPos calls.
 *   r5  FOUR values in sequence -- the actor's x, then message base 0xf91
 *       (`add r0, r5, #5` / `#6`), then the +0x5a bit masks, then
 *       gScript_885__02009ce0.
 * `m __asm__("r7")` and `n __asm__("r5")` are both required for the same reason
 * the sibling ovl_30_c_c_a_c_a_c_c_b.c records: as plain ints gcse's cprop
 * folds `m + 0xa` / `n + 5` back to literals and the ROM's `add` disappears
 * (m plain: 672 lines and 656 differing; n plain: the four `add`s become four
 * pool loads).  `y __asm__("r6")` is required too -- unnamed, gcc swaps x and z
 * between r5 and r6 and the two SetPos calls come out reversed (8 differing).
 * x is a PLAIN int and must stay one: inlining it into the two calls costs 634.
 *
 * ONE MORE r5 RANGE THAN THERE ARE NAMES, AND THE CURE IS TO REUSE `n`.  The
 * ROM sets bit 0 of actor+0x5a on two actors and the second store is
 * `orr r5, r3 / strb r5` -- the MASK's register is the destination, not the
 * loaded byte's.  gcc canonicalises commutative operands, so `p[0x5a] |= 1`,
 * `1 | p[0x5a]`, `p[0x5a] = p[0x5a] | 1u`, a separate `int k = 1` and an
 * `unsigned char` mask are ALL byte-identical to each other and all wrong by 2.
 * Writing the second store as an accumulate into the r5-bound `n` --
 * `n |= p[0x5a]; p[0x5a] = n;` -- is the only spelling that puts the mask in
 * the destination, and it is exact.  `n` is dead from `__MessageID(n + 6)`
 * onward, so the reuse is the ROM's own register reuse spelled out.  Note the
 * `&= 0xfe` pair two calls earlier needs NO help: gcc already picks the mask
 * register there, which is what makes the `|=` pair worth recording.
 *
 * gScript_885__02009ce0 NEEDS A NAME AND ITS OWN PIN SHAPE.  The ROM loads it
 * once into r5 and passes `mov r2, r5` twice; written straight into both calls
 * it is pooled twice (673 lines, 17 differing).  `unsigned char *s` is enough --
 * gcc picks r5 for it unaided -- but the two calls still want a PIN2 on r0/r1
 * so the `lsl r1, #9` lands after `mov r0`.
 *
 * FIFTY-THREE PINS, AND THE FILL IS THE ROM'S OWN INSTRUCTION ORDER.  The whole
 * body is ONE scheduling region (nothing but calls -- no loop anywhere), so
 * sched2 sees all 666 instructions at once and a uniform ascending q0..q2 fill
 * is wrong at 70 of 156 call sites.  The fills below are transcribed
 * instruction-for-instruction from the ROM's own setup group, `mov` and `lsl`
 * in the ROM's slots, which is what "these ROMs put the slot mov mid-group"
 * means in practice.  Unpinned the function is 673 lines with gcc hoisting
 * 0x13333, 0x9999, 0x101, 0x100f and 0x1005 into r8-r11 and paying four extra
 * pushes; 573 of 674 differ.
 *
 * TWO SITES WANT THE UNIFORM FILL INSTEAD, AND ONLY THOSE TWO.  Both are a
 * `__Func_80921c4` immediately following a `__Func_809218c` that shares its z:
 * the ROM emits `mov r1 / mov r2 / lsl r2 / mov r0 / lsl r1`, and transcribing
 * that order gives `mov r2 / mov r1 / ...` while unpinning gives
 * `... / lsl r1 / mov r0`.  All 60 valid orderings of the five fill statements
 * were compiled: 14 reproduce the ROM and every one of them writes q0 BEFORE
 * q1 -- the plain ascending `q0 = 1; q1 = 0xcc << 1; q2 = 0xa4 << 1;` is the
 * shortest.  So the lever is the SLOT argument going first, not the shift.
 *
 * TWO `do { } while (0)` BARRIERS, BOTH LOAD-BEARING.  With one scheduling
 * region covering everything, both `ldr r7, =0xf85` and `ldr r5, =0xf91` are
 * hoisted two and four instructions above their ROM slots.  A loop note ends
 * the region and pins each load.  `while (0) ;` is byte-identical -- a TIE --
 * and `if (0) ;` is INERT (10 differing), so these are LOOPS and not labels,
 * the distinction the elevation notes insist on.  Do NOT add a third barrier
 * at either 80921c4 site: it costs the PRECEDING call its scheduler and the
 * function goes from 2 differing to 12.
 *
 * MINIMISED TO A FIXPOINT.  All 152 candidate sites were pinned, then dropped
 * one at a time re-testing after each drop until no further drop held: 89 came
 * out, 53 remain, and a second full pass found none.  A `register unsigned
 * char *g __asm__("r0")` on the first __MapActor_GetActor was needed at the
 * 675-line stage and is INERT at the fixpoint -- it is dropped, and it is the
 * clearest evidence here that a one-at-a-time list is a set of CANDIDATES.
 * -fno-rerun-cse-after-loop is inert on this function and -fno-schedule-insns2
 * is far worse (125 differing), so sched2 is WANTED: no flag is involved.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __PlayMapMusic(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern int __Func_8091c7c(int a, int b);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8092a1c(int a, int b, unsigned char *c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern unsigned char gScript_885__02009ce0[];

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_885_2008be0(void)
{
    unsigned char *p;
    int x;
    register int y __asm__("r6");
    unsigned char *s;
    register int m __asm__("r7");
    register int n __asm__("r5");
    { PIN1; q0 = 0x808;
    if (__GetFlag(q0) == 0) {
        __CutsceneStart();
        __PlaySound(0x11);
        __SetFlag(0x808);
        do { } while (0);
        m = 0xf85;
        __MessageID(m);
        __Func_8093040(0xe, 0, 0xa);
        { PIN3; q0 = 0; q1 = 0x13333; q2 = 0x9999;
          __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q1 = 0x80; q0 = 0; q1 <<= 1; q2 = 0x1e;
          __MapActor_Emote(q0, q1, q2); }
        { PIN3; q1 = 0xc4; q2 = 0xa4; q0 = 0; q1 <<= 1; q2 <<= 1;
          __Func_80921c4(q0, q1, q2); }
        __Func_8092adc(0, 0x80 << 7, 0xa);
        p = __MapActor_GetActor(0);
        x = *(short *)(p + 0xa) << 16;
        y = *(short *)(p + 0x12) << 16;
        __MapActor_SetPos(5, x, y);
        __MapActor_SetPos(1, x, y);
        { PIN3; q0 = 5; q1 = 0x13333; q2 = 0x9999;
          __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q0 = 1; q1 = 0x13333; q2 = 0x9999;
          __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q1 = 0xbc; q2 = 0xa4; q0 = 5; q1 <<= 1; q2 <<= 1;
          __Func_809218c(q0, q1, q2); }
        { PIN3; q0 = 1; q1 = 0xcc << 1; q2 = 0xa4 << 1;
          __Func_80921c4(q0, q1, q2); }
        __MapActor_SetAnim(0, 0);
        __MapActor_SetAnim(5, 0);
        __MapActor_SetAnim(1, 0);
        { PIN3; q1 = 0x80; q0 = 1; q1 <<= 8; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        __Func_8092adc(5, 0, 0x14);
        { PIN3; q0 = 0; q1 = 0x101; q2 = 0;
          __MapActor_Emote(q0, q1, q2); }
        { PIN3; q0 = 1; q1 = 0x101; q2 = 0;
          __MapActor_Emote(q0, q1, q2); }
        { PIN3; q0 = 5; q1 = 0x101; q2 = 0x1e;
          __MapActor_Emote(q0, q1, q2); }
        { PIN3; q1 = 0x80; q0 = 0; q1 <<= 7; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        { PIN3; q1 = 0x80; q0 = 1; q1 <<= 7; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        { PIN3; q1 = 0x80; q2 = 0; q0 = 5; q1 <<= 7;
          __Func_8092adc(q0, q1, q2); }
        __Func_80933d4(0xc0 << 11, 0xc0 << 8);
        __Func_80933f8(0xd7 << 16, -1, 0x159 << 16, 1);
        __Func_8093530();
        __CutsceneWait(0x14);
        __PlaySound(0x3d);
        __MapActor_DoAnim(0xe, 4);
        __MapActor_SetAnim(0xe, 4);
        __Func_8093040(0xe, 0, 0x14);
        __Func_8092adc(0xf, 0, 0xa);
        __Func_8093040(0xf, 0, 0xa);
        __MapActor_DoAnim(0xe, 3);
        __Func_8093040(0xe, 0, 0xa);
        { PIN3; q1 = 0x80; q2 = 0x3c; q0 = 0xf; q1 <<= 7;
          __Func_8092adc(q0, q1, q2); }
        __Func_80925cc(0xf, 1);
        __Func_8093040(0xf, 0, 0x14);
        __MapActor_DoAnim(0xe, 3);
        __Func_8093040(0xe, 0, 0xa);
        __MapActor_DoAnim(0xf, 4);
        __Func_8093040(0xf, 0, 6);
        { PIN3; q1 = 0x80; q2 = 0; q0 = 0xe; q1 <<= 1;
          __MapActor_Emote(q0, q1, q2); }
        __Func_80925cc(0xe, 2);
        __CutsceneWait(0x14);
        __Func_8092adc(0xf, 0, 0xa);
        { PIN3; q0 = 0xf; q1 = 0x101; q2 = 0x28;
          __MapActor_Emote(q0, q1, q2); }
        __Func_8092adc(0xe, 0, 0x3c);
        { PIN3; q1 = 0x80; q0 = 0xe; q1 <<= 8; q2 = 0x28;
          __Func_8092adc(q0, q1, q2); }
        __Func_8092adc(0xe, 0, 0x28);
        { PIN3; q2 = 0xb4; q0 = 0xe; q1 = 0xe8; q2 <<= 1;
          __Func_80921c4(q0, q1, q2); }
        __Func_8092adc(0xe, 0, 0xa);
        __MapActor_DoAnim(0xf, 3);
        __CutsceneWait(0xa);
        { PIN3; q1 = 0xc4; q2 = 0xb4; q0 = 0xe; q1 <<= 1; q2 <<= 1;
          __Func_809218c(q0, q1, q2); }
        { PIN3; q2 = 0xb4; q0 = 0xf; q1 = 0xd8; q2 <<= 1;
          __Func_80921c4(q0, q1, q2); }
        { PIN3; q1 = 0xbc; q2 = 0xb4; q0 = 0xf; q1 <<= 1; q2 <<= 1;
          __Func_809218c(q0, q1, q2); }
        { PIN3; q1 = 0x80; q0 = 0; q1 <<= 7; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        { PIN3; q1 = 0x80; q0 = 1; q1 <<= 7; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        { PIN3; q1 = 0x80; q0 = 5; q1 <<= 7; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        { PIN4; q1 = 1; q3 = 1; q0 = 0x189 << 16; q1 = -q1; q2 = 0x153 << 16;
          __Func_80933f8(q0, q1, q2, q3); }
        { PIN3; q1 = 0xc4; q2 = 0xb4; q0 = 0xe; q1 <<= 1; q2 <<= 1;
          __Func_809218c(q0, q1, q2); }
        { PIN3; q0 = 0xf; q1 = 0xbc << 1; q2 = 0xb4 << 1;
          __Func_80921c4(q0, q1, q2); }
        __MapActor_SetAnim(0xe, 0);
        __MapActor_SetAnim(0xf, 0);
        { PIN3; q1 = 0xd0; q0 = 0xe; q1 <<= 8; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        { PIN3; q1 = 0xd0; q2 = 0x1e; q0 = 0xf; q1 <<= 8;
          __Func_8092adc(q0, q1, q2); }
        __Func_80925cc(0xe, 2);
        __Func_8093040(0xe, 0, 0xa);
        { PIN3; q1 = 0x81; q2 = 0x3c; q0 = 1; q1 <<= 1;
          __MapActor_Emote(q0, q1, q2); }
        __Func_80925cc(1, 1);
        __Func_8093040(1, 0, 0xa);
        __MapActor_DoAnim(0xf, 4);
        { PIN2; q1 = 0; q0 = 0x100f;
          __Func_8092c40(q0, q1); }
        if (__Func_8091c7c(0, 0) == 0)
            __MessageID(m + 0xa);
        else
            __MessageID(m + 0xb);
        { PIN3; q2 = 0xa; q0 = 0x100f; q1 = 0;
          __Func_8093040(q0, q1, q2); }
        { PIN2; q1 = 2; q0 = 1;
          __Func_80925cc(q0, q1); }
        do { } while (0);
        n = 0xf91;
        __MessageID(n);
        __Func_8093040(1, 0, 0x14);
        __Func_8092848(0xe, 0xf, 0x28);
        { PIN3; q1 = 0xd0; q0 = 0xe; q1 <<= 8; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        { PIN3; q1 = 0xd0; q0 = 0xf; q1 <<= 8; q2 = 0x14;
          __Func_8092adc(q0, q1, q2); }
        __Func_8093040(0xe, 0, 0x3c);
        __Func_80925cc(0xf, 1);
        __CutsceneWait(0xa);
        __MapActor_DoAnim(0xf, 3);
        __Func_8093040(0x100f, 0, 0xa);
        __Func_80925cc(5, 2);
        __MapActor_DoAnim(5, 3);
        __Func_8093040(0x1005, 0, 0x14);
        __Func_80925cc(0xe, 2);
        { PIN3; q1 = 0xa0; q0 = 0xe; q1 <<= 8; q2 = 0x14;
          __Func_8092adc(q0, q1, q2); }
        { PIN2; q1 = 0; q0 = 0xe;
          __Func_8092c40(q0, q1); }
        if (__Func_8091c7c(0, 0) == 0)
            __MessageID(n + 5);
        else
            __MessageID(n + 6);
        __Func_8092adc(5, 0, 0);
        { PIN3; q1 = 0x80; q2 = 0x14; q0 = 1; q1 <<= 8;
          __Func_8092adc(q0, q1, q2); }
        __Func_80925cc(0xe, 2);
        __Func_8093040(0xe, 0, 0xa);
        __Func_809280c(0xe, 1, 0x1e);
        __Func_809280c(0xe, 5, 0x1e);
        { PIN3; q2 = 0x50; q0 = 0xe; q1 = 0x105;
          __MapActor_Emote(q0, q1, q2); }
        __MapActor_DoAnim(0xe, 4);
        __MessageID(0xf98);
        __Func_8093040(0xe, 0, 6);
        { PIN3; q1 = 0x80; q0 = 0; q1 <<= 1; q2 = 0;
          __MapActor_Emote(q0, q1, q2); }
        { PIN3; q1 = 0x80; q0 = 1; q1 <<= 1; q2 = 0;
          __MapActor_Emote(q0, q1, q2); }
        { PIN3; q1 = 0x80; q2 = 0; q0 = 5; q1 <<= 1;
          __MapActor_Emote(q0, q1, q2); }
        __Func_809259c(1, 1);
        __Func_809259c(5, 1);
        __Func_80925cc(0, 1);
        __CutsceneWait(0x28);
        __Func_80925cc(5, 2);
        { PIN3; q1 = 0x80; q0 = 1; q1 <<= 7; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        { PIN3; q1 = 0x80; q0 = 5; q1 <<= 7; q2 = 0xa;
          __Func_8092adc(q0, q1, q2); }
        __Func_8093040(0x1005, 0, 0xa);
        __Func_80925cc(0xf, 2);
        __Func_8092adc(0xf, 0, 0xa);
        __Func_8093040(0x100f, 0, 0xa);
        { PIN3; q1 = 0x80; q2 = 0x14; q0 = 0xe; q1 <<= 8;
          __Func_8092adc(q0, q1, q2); }
        __MapActor_DoAnim(0xe, 4);
        __Func_8093040(0xe, 0, 0xa);
        __Func_80925cc(0xf, 2);
        __CutsceneWait(0xa);
        { PIN3; q1 = 0xb0; q0 = 0xe; q1 <<= 8; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        { PIN3; q1 = 0xd0; q0 = 0xf; q1 <<= 8; q2 = 0x14;
          __Func_8092adc(q0, q1, q2); }
        { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0xf; q1 <<= 8; q2 <<= 7;
          __MapActor_SetSpeed(q0, q1, q2); }
        __MapActor_SetSpeed(0xe, 0x80 << 8, 0x80 << 7);
        p = __MapActor_GetActor(0xe);
        p[0x5a] &= 0xfe;
        p = __MapActor_GetActor(0xf);
        p[0x5a] &= 0xfe;
        { PIN3; q1 = 0xc4; q2 = 0xbc; q0 = 0xe; q1 <<= 1; q2 <<= 1;
          __Func_809218c(q0, q1, q2); }
        { PIN3; q1 = 0xbc; q2 = 0xbc; q1 <<= 1; q2 <<= 1; q0 = 0xf;
          __Func_80921c4(q0, q1, q2); }
        __CutsceneWait(6);
        n = 1;
        p = __MapActor_GetActor(0xe);
        p[0x5a] |= n;
        p = __MapActor_GetActor(0xf);
        n |= p[0x5a];
        p[0x5a] = n;
        __MapActor_SetAnim(0xe, 0);
        __MapActor_SetAnim(0xf, 0);
        __CutsceneWait(0x14);
        __Func_80925cc(1, 2);
        __Func_8093040(1, 0, 0xa);
        __Func_8092adc(0, 1, 0x14);
        __MapActor_SetAnim(0, 3);
        __MapActor_DoAnim(1, 3);
        __PlaySound(0x11);
        __MapActor_SetAnim(1, 2);
        p = __MapActor_GetActor(0);
        if (p != 0)
            __MapActor_TravelTo(1, *(short *)(p + 0xa), *(short *)(p + 0x12));
        __MapActor_WaitMovement(1);
        __MapActor_SetPos(1, 0, 0);
        __MapActor_SetAnim(5, 2);
        p = __MapActor_GetActor(0);
        if (p != 0)
            __MapActor_TravelTo(5, *(short *)(p + 0xa), *(short *)(p + 0x12));
        __MapActor_WaitMovement(5);
        __MapActor_SetPos(5, 0, 0);
        s = gScript_885__02009ce0;
        { PIN2; q1 = 0x80; q0 = 0xe; q1 <<= 9;
          __Func_8092a1c(q0, q1, s); }
        { PIN2; q1 = 0x80; q0 = 0xf; q1 <<= 9;
          __Func_8092a1c(q0, q1, s); }
        __PlayMapMusic();
        __CutsceneEnd();
    }
    }
}
