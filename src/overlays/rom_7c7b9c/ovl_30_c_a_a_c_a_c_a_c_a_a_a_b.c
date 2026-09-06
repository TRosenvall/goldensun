// fakematch
/* OvlFunc_943_2008ca0  --  0x02008ca0
 * [asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_a_a.s, first of three]
 *
 * MATCH. 1024 bytes, 392 encodings and 105 relocations identical.
 * 374 instructions of straight-line cutscene behind one save-bit test, with
 * THIRTY-ONE shared symbols and FOUR values live in r8-r11.
 *
 * ============================================================
 * IS THE RESIDUE ABOUT THE HIGH REGISTERS?  NO -- AND THE HIGH
 * REGISTERS WERE NEVER THE PROBLEM.
 * ============================================================
 * The ROM holds four shifted constants across the whole body:
 *
 *     r11 = 0xb0 << 8 = 0xb000    2 uses
 *     r9  = 0xc0 << 6 = 0x3000    3 uses
 *     r10 = 0xd0 << 8 = 0xd000    4 uses
 *     r8  = 0xa0 << 7 = 0x5000    3 uses (the first is rebuilt inline)
 *
 * plus 0x4016 in r5 and 0x4017 in r6.  Six call-saved values, and r4 is
 * unavailable (`-fcall-used-r4`) and r7 is untouched.
 *
 * gcc reaches this assignment UNAIDED once -- and only once -- the competing
 * constants are pinned.  The FIRST screen, naive C with plain literals, is
 * 392 encodings against 392 with an IDENTICAL relocation list, and its
 * high-register traffic is the ROM's shape with the WRONG TENANTS: gcc hoists
 * 0x102, 0x101 and 0x8000 (which the ROM rebuilds at every use) and leaves
 * 0xb000 inline, spending SEVEN call-saved registers -- r5, r6, r7 and all
 * four high ones -- where the ROM spends six.  That is the batch-242
 * discriminator firing on the "one MORE" side, so the cure is in source, not
 * `-ffixed-r7`: r7 disappears by itself when the seventh tenant does.
 *
 * The pin is what evicts them.  A `register int qN __asm__("rN")` for r0-r2
 * puts the constant in a CALL-CLOBBERED register, so it cannot survive the
 * `bl` and gcc must rebuild it -- the batch-207 correction, used here not to
 * fix an argument order but to decide WHICH FOUR VALUES GET THE HIGH
 * REGISTERS.  With the ROM's rebuilt constants pinned and the ROM's hoisted
 * four left as plain `int` locals, r8/r9/r10/r11 land on the ROM's values on
 * the first try, and the residue drops 273 encodings -> 16 instructions.
 *
 * Every one of those 16 was ONE SHAPE and it is not an allocation problem
 * either: at 8 sites the ROM fills `mov r0, #imm / mov r1, rHIGH` and gcc
 * emits the copy first.  sched2 pulls a reg-reg copy out of a call-saved
 * register ahead of an equal-priority immediate REGARDLESS OF SOURCE ORDER --
 * the sites where the ROM itself puts r1 first matched with no help at all.
 * A `__asm__ volatile ("" : : "r" (q0))` between the two restores it.
 *
 * So: the high registers took no lever of their own.  No hard-register pin on
 * r8-r11 exists in this file.  The final residue was argument fill ORDER at
 * sites that merely happen to read a high register, and one pool load that
 * had to be dragged in front of a call.
 *
 * ============================================================
 * NEW: THE BARRIER IS AVAILABLE WHERE THE ROM SPILLS r8-r11
 * ============================================================
 * docs/elevation.md says, twice, that the volatile-asm barrier is unavailable
 * where the ROM uses r8-r11 -- "Zero means the barrier is available. Non-zero
 * means it is not, whatever the residue looks like" (batch 207, measured on
 * OvlFunc_961_2008120 and OvlFunc_901_2008c1c).  THIS FUNCTION STRIKES THAT AS
 * STATED.  It has 23 high-register references across four distinct values and a
 * full `mov r5, r8 / push {r5, r6}` spill prologue, and ELEVEN barriers land it
 * byte-exact.
 *
 * The batch-207 mechanism still holds; its boundary was drawn one step too
 * early.  A barrier hurts when splitting the block SHORTENS a live range the
 * ROM's allocation depends on.  Here the four values are live across forty-odd
 * calls, so no single split can shorten them -- each barrier falls strictly
 * inside every one of the four ranges.  The refinement:
 *
 *   ORDER MATTERS, NOT PRESENCE.  Pin first so the ALLOCATION is the ROM's,
 *   then barrier for ORDER.  The barrier is unsafe only while the allocation
 *   is still gcc's own -- which is the state both batch-207 specimens were in.
 *
 * ============================================================
 * NEW: THE BARRIER COUNT IS A HARD BUDGET, SET BY MINIPOOL REACH
 * ============================================================
 * An empty `__asm__ volatile ("")` emits NOTHING, and `shorten_branches` still
 * charges it a full instruction's length.  On a function this long that pushes
 * the minipool's first entry out of reach of the end-of-function barrier, and
 * `arm_reorg` dumps the pool before the epilogue with a real `b` over it.
 * Measured, by adding no-op barriers to the shipped file:
 *
 *     11 volatile asms (shipped)   pool after `bx r0`   376 lines, exact
 *     12                           exact
 *     13                           exact
 *     14                           pool dumped mid-function, +`b`, 12 differ
 *     15, 16, 17                   the same 12 differ
 *
 * The cliff is between 13 and 14 with the code stream unchanged at 376 lines.
 * Three intermediate candidates were lost to this before it was understood:
 * the SAME barrier that fixed a site read as "makes it worse" purely because
 * it tipped the pool.  The recorded reading is "a stray jump-to-next-label is
 * usually a POOL DUMP" -- that entry names an `ldrh` as the early pool user.
 * A BARRIER COUNT is a second cause, and unlike the `ldrh` it is one you added.
 *
 *   Diagnostic: residue jumps to a first-diff in the LAST few instructions and
 *   the length goes +2 with a bare `b Lx / Lx:` at the tail.  Take a barrier
 *   out somewhere else -- the budget is fungible across the whole function.
 *
 * That is what the "second 80933f8 block" line in the table below is doing:
 * its two barriers were freed to pay for the two the `__CutsceneWait` site
 * needed.
 *
 * ============================================================
 * MEASURED-WORSE, all against the 376-line reference
 * ============================================================
 *   plain C, no pins, plain literals               273 encodings (392/392)
 *   full pin set, no order barriers                 16   (all 8 sites, x2)
 *   + `-fno-schedule-insns2`                        44   sched2 IS what makes
 *                                                        a pin an ORDER pin
 *   pin set + 8 order barriers, no pool budget      14   pool dumped
 *   `a = 0x4016` before an unpinned __CutsceneWait    2
 *   `a` after the call / `register int a __asm__("r5")` / a barrier on q0
 *      alone -- three independent spellings          2   IDENTICAL, all three
 *   barrier on `a` alone (no q0 pin)                 3
 *   ORR site: `*p = 1 | *p` / `*p |= 1` /
 *      `int m = 1; *p = m | *p`                      2   IDENTICAL, all three
 *      -- `unsigned char m` is the only spelling that works, exactly as the
 *         "ORR-destination lever needs an unsigned char" note predicts
 *   dropping the local `g` (0x80 << 9 stored twice)  27, and 3 lines SHORT
 *
 * ============================================================
 * THE PIN SET: 17 blocks, minimal, and it strips cleanly
 * ============================================================
 * 57 pin blocks were screened one at a time: 40 individually inert, 17
 * load-bearing.  Unlike the ovl_30_c_c_b.c case, THE UNION OF ALL 40 INERT
 * REMOVALS ALSO PASSES, and greedy stripping to fixpoint from the front and
 * from the back converge on the SAME 17.  So "individually inert" was safe to
 * take as a set here; that is a property of the function, not a rule -- the
 * union was screened, not assumed.
 *
 * Also stripped after measurement: both `__MapActor_SetBehavior` pointer pins
 * (inert), the q0 barrier in the first __Func_80933f8 block (inert; its q1
 * barrier is load-bearing), one of the eight order barriers (inert), and
 * `p + 0x23` in favour of `p[0x23]` (0x23 > the ldrb imm5 limit, so gcc emits
 * the ROM's `add r0, #0x23` either way).
 *
 * WHAT DOES NOT NEED A PIN.  Every `OvlFunc_943_200b9ec(x)` -- one argument --
 * and every ascending all-cheap fill.  The pins cluster where the ROM fills
 * r1 or r2 before r0, and at the three sites whose later argument is a POOL
 * LOAD (the batch-208 rule).
 *
 * TWINS: `tools/solved_twins.py` reports zero.  The template is the sibling
 * two functions along in the same .s, src/overlays/rom_7c7b9c/
 * ovl_30_c_a_a_c_a_c_a_c_a_a_b.c, found by grepping for __Func_80933f8; its
 * two-barrier __Func_80933f8 fill transplanted unchanged, and its
 * `__Func_809280c(0, 0x14, 0xa)` pin is character-for-character the same call.
 *
 * Message base 0x1d26 is a bare literal and 0x1d40 is the linker symbol
 * `_MSG_1d40` -- read off the reference's R_ARM_ABS32 list, not guessed.
 */
extern int  __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_Surprise(int slot, int a);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Func_808e118(void);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void OvlFunc_943_2008bf0(void);
extern void OvlFunc_943_200b9ec(int a);
extern void OvlFunc_943_200ba00(int a, int b);
extern unsigned char ActorCmd_ARRAY_943__0200c464[];
extern unsigned char gScript_943__0200c49c[];
extern int _MSG_1d40;

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_943_2008ca0(void)
{
    unsigned char *p;
    int a, d, b, c, e, f, g;

    if (__GetFlag(0x911)) {
        __CutsceneStart();
        __Func_808e118();
        __Func_809280c(0, 0x14, 0xa);
        __Func_80933d4(0x19999, 0x3333);
        { PIN4;
          q0 = 0xbe;
          q1 = 1; __asm__ volatile ("" : : "r" (q1));
          q2 = 0xb1; q3 = 1; q2 <<= 18; q1 = -q1; q0 <<= 16;
          __Func_80933f8(q0, q1, q2, q3); }
        __Func_8093530();
        { register int q0 __asm__("r0"); q0 = 0x28;
          __asm__ volatile ("" : : "r" (q0));
          a = 0x4016; __asm__ volatile ("" : : "r" (a));
          __CutsceneWait(q0); }
        __Func_80925cc(0x16, 1);
        __MessageID(0x1d26);
        OvlFunc_943_200b9ec(a);
        { PIN3; q1 = 0x81; q2 = 0x3c; q0 = 0x14; q1 <<= 1;
          __MapActor_Emote(q0, q1, q2); }
        __Func_809259c(0x14, 2);
        OvlFunc_943_200b9ec(0x14);
        __Func_80925cc(0x16, 1);
        __Func_8092adc(0x16, 0xa0 << 7, 0);
        OvlFunc_943_200b9ec(a);
        __Func_80925cc(0x14, 1);
        b = 0xb0 << 8;
        OvlFunc_943_200ba00(0x14, b);
        OvlFunc_943_200b9ec(0x14);
        c = 0xc0 << 6;
        d = 0x4017;
        { PIN2; q0 = 0x17; __asm__ volatile ("" : : "r" (q0)); q1 = c; OvlFunc_943_200ba00(q0, q1); }
        __MapActor_SetAnim(0x17, 3);
        OvlFunc_943_200b9ec(d);
        { PIN3; q0 = 0x16; q1 = 0x101; q2 = 0x28;
          __MapActor_Emote(q0, q1, q2); }
        __Func_8092adc(0x16, 0x80 << 8, 0x14);
        OvlFunc_943_200b9ec(a);
        OvlFunc_943_200ba00(0x17, 0);
        __MapActor_DoAnim(0x17, 4);
        OvlFunc_943_200b9ec(d);
        { PIN3; q1 = 0x80; q2 = 0x28; q0 = 0x14; q1 <<= 1;
          __MapActor_Emote(q0, q1, q2); }
        __Func_809259c(0x14, 2);
        OvlFunc_943_200b9ec(0x14);
        __MapActor_DoAnim(0x16, 3);
        OvlFunc_943_200b9ec(a);
        e = 0xd0 << 8;
        { PIN2; q0 = 0x14; __asm__ volatile ("" : : "r" (q0)); q1 = e; OvlFunc_943_200ba00(q0, q1); }
        __MapActor_SetAnim(0x17, 3);
        __MapActor_DoAnim(0x14, 3);
        __CutsceneWait(0x3c);
        { PIN3; q1 = 0x83; q2 = 0x28; q0 = 0x16; q1 <<= 1;
          __MapActor_Emote(q0, q1, q2); }
        f = 0xa0 << 7;
        OvlFunc_943_200ba00(0x16, f);
        __MessageID((int)&_MSG_1d40);
        __Func_809259c(0x16, 1);
        OvlFunc_943_200b9ec(a);
        { PIN3; q2 = 0x28; q0 = 0x14; q1 = 0x101;
          __MapActor_Emote(q0, q1, q2); }
        __Func_809259c(0x14, 2);
        OvlFunc_943_200b9ec(0x14);
        { PIN3; q1 = 0x84; q0 = 0x16; q1 <<= 1; q2 = 0x14;
          __MapActor_Emote(q0, q1, q2); }
        __Func_8093040(a, 0, 0x14);
        __MapActor_Emote(0x17, 0x81 << 1, 0x3c);
        OvlFunc_943_200b9ec(d);
        OvlFunc_943_200ba00(0x16, 0x80 << 8);
        __MapActor_DoAnim(0x16, 3);
        __Func_8093040(a, 0, 0x14);
        { PIN3; q1 = 0x81; q2 = 0x28; q0 = 0x14; q1 <<= 1;
          __MapActor_Emote(q0, q1, q2); }
        __Func_809259c(0x14, 2);
        OvlFunc_943_200b9ec(0x14);
        { PIN2; q0 = 0x16; __asm__ volatile ("" : : "r" (q0)); q1 = f; OvlFunc_943_200ba00(q0, q1); }
        __MapActor_SetAnim(0x16, 4);
        OvlFunc_943_200b9ec(0x16);
        { PIN3; q0 = 0x14; q1 = b; q2 = 0; __Func_8092adc(q0, q1, q2); }
        { PIN3; q0 = 0x17; __asm__ volatile ("" : : "r" (q0)); q1 = c; q2 = 0x28; __Func_8092adc(q0, q1, q2); }
        __Func_8092adc(0x17, 0, 0);
        { PIN3; q2 = 0x14; q0 = 0x14; __asm__ volatile ("" : : "r" (q0)); q1 = e; __Func_8092adc(q0, q1, q2); }
        __Func_80925cc(0x16, 2);
        __CutsceneWait(0x14);
        OvlFunc_943_200b9ec(a);
        { PIN2; q1 = 0x81; q0 = 0x17; q1 <<= 1;
          __MapActor_Surprise(q0, q1); }
        __MapActor_Surprise(0x14, 0x81 << 1);
        __CutsceneWait(0x28);
        __MapActor_DoAnim(0x16, 3);
        OvlFunc_943_200b9ec(a);
        { PIN2; q0 = 0xcccc; q1 = 0x1999; __Func_80933d4(q0, q1); }
        __Func_80933f8(0xb6 << 16, -1, 0xbe << 18, 1);
        { PIN3; q2 = 0x6666; q0 = 0x17; q1 = 0xcccc;
          __MapActor_SetSpeed(q0, q1, q2); }
        __MapActor_SetBehavior(0x17, ActorCmd_ARRAY_943__0200c464);
        { PIN3; q2 = 0x6666; q0 = 0x16; q1 = 0xcccc;
          __MapActor_SetSpeed(q0, q1, q2); }
        __MapActor_SetBehavior(0x16, gScript_943__0200c49c);
        { PIN3; q0 = 0x14; q1 = 0xcccc; q2 = 0x6666;
          __MapActor_SetSpeed(q0, q1, q2); }
        __Func_80921c4(0x14, 0xb6, 0xbe << 2);
        __Func_809259c(0x14, 2);
        { PIN3; q1 = 0x80; q2 = 0x3c; q0 = 0x14; q1 <<= 1;
          __MapActor_Emote(q0, q1, q2); }
        { PIN2; q0 = 0x14; __asm__ volatile ("" : : "r" (q0)); q1 = e; OvlFunc_943_200ba00(q0, q1); }
        __Func_8093040(0x14, 0, 0x14);
        __MapActor_DoAnim(0x14, 3);
        __MapActor_Jump(0x14, 4, 0);
        { PIN3; q2 = 0x28; q0 = 0x14; __asm__ volatile ("" : : "r" (q0)); q1 = c; __Func_8092adc(q0, q1, q2); }
        __Func_80933d4(0x80 << 9, 0x80 << 6);
        { PIN4;
          q0 = 0xd8; __asm__ volatile ("" : : "r" (q0));
          q1 = 1; q1 = -q1; q2 = 0x3160000; q3 = 1; q0 <<= 16;
          __Func_80933f8(q0, q1, q2, q3); }
        p = __MapActor_GetActor(0x14);
        {
            unsigned char m = 1;
            p[0x23] = m | p[0x23];
        }
        { PIN3; q1 = 0x13333; q0 = 0x14; q2 = 0x9999;
          __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q0 = 0x14; q1 = 0xb6; q2 = 0x30e;
          __Func_80921c4(q0, q1, q2); }
        { PIN3; q2 = 0xca; q0 = 0x14; q1 = 0xc0; q2 <<= 2;
          __Func_80921c4(q0, q1, q2); }
        __Func_80921c4(0x14, 0xd8, 0xca << 2);
        OvlFunc_943_200ba00(0x14, e);
        __Func_80925cc(0x14, 2);
        OvlFunc_943_2008bf0();
        { PIN3; q0 = 0x14; q1 = 0xd8; q2 = 0x31e;
          __Func_80921c4(q0, q1, q2); }
        __MapActor_SetPos(0x14, 0, 0);
        g = 0x80 << 9;
        *(int *)(__MapActor_GetActor(0x14) + 0x18) = g;
        *(int *)(__MapActor_GetActor(0x14) + 0x1c) = g;
        __SetFlag(0x92 << 4);
        __CutsceneEnd();
    }
}
