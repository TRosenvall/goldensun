// fakematch
/* OvlFunc_885_2009760  --  0x02009760
 *   [asm/overlays/rom_78603c/ovl_30_c_c_a_c_a_c_c.s, 2nd of 2]
 *
 * 205 instructions of cutscene behind two nested flag guards, with a third
 * two-armed branch in the middle and a shared tail.
 *
 * BUILT AT -O2, AGAINST A WILDCARD THAT SAYS -O1.  This is the FIFTH instance
 * of the mis-scoped-wildcard trap and the SECOND in this directory: Makefile's
 * `rom_78603c/ovl_30_c_c_a_c_a%` pattern captures this split product on prefix
 * alone.  objcmp run against the ORIGINAL .s path reports `(built with: O1)`
 * and 42 of 218 encodings wrong; the same candidate on a scratch path gets the
 * tree default and is byte-identical.  The immediately preceding sibling
 * ovl_30_c_c_a_c_a_c_b.c already carries an explicit -O2 override for exactly
 * this reason (Makefile:310); this file needs one too.
 *
 * ONE REGISTER, TWO RANGES, AND ONLY THE FIRST IS PINNED.  The prologue is
 * `push {r5, lr}`, so the ROM keeps exactly one value at a time and rebuilds
 * every other repeat.  r5 carries the message id 0xe85 (`mov r0, r5`,
 * `add r0, r5, #2`, `add r0, r5, #3` in three different blocks) and then, once
 * that is dead, gScript_885__02009ce0 (`mov r2, r5` twice).  As a plain
 * `int m` gcse's cprop folds `m + 2` and `m + 3` back to literals and the
 * function comes out 211 lines and 183 differing with no r5 at all;
 * `register int m __asm__("r5")` is the cure and needs no build change.
 * The SECOND range is left FREE -- the script address is written straight into
 * both call arguments and gcc picks r5 for it by itself.  Naming it
 * (`unsigned char *s = gScript_...;`) is byte-identical, so it is dropped:
 * "one pin plus a free choice" with nothing to gain from the name.
 *
 * FOURTEEN PINS, MINIMAL BY MEASUREMENT.  With r5 spoken for, every repeated
 * constant must be rebuilt at each use: 0x839 and 0x82f at two sites each,
 * 0xa0<<8 at two, 0x80<<7 at two, 0x105 at two, 0x1000b at two.  Unpinned the
 * function is 215 lines with `push {r5, r6, r7, lr}` and 210 differing.  A pin
 * on a call-clobbered argument register makes the value dead across the next
 * `bl`, so gcc has no register to carry it in and must rematerialise.
 * Eighteen pinned sites were stripped one at a time; four were individually
 * inert -- the first __Func_8092adc(0, 0xa0<<8, 0), __SetFlag(0x82f),
 * __MapActor_Emote(1, 0x105, 0x78) and __SetFlag(0x839) -- and all four come
 * out together.  A second round over the surviving fourteen found every one
 * load-bearing, and the sweep is a FIXPOINT and not a subtraction: dropping
 * the first 0xa0<<8 site raises the cost of the SECOND one from 2 differing to
 * 27, exactly the non-independence recorded in "the one-at-a-time pin list is
 * a set of candidates".
 *
 * THE `do { } while (0)` IS LOAD-BEARING, 6 ENCODINGS.  Without it sched2
 * hoists `ldr r5, =0xe85` two slots up, above __MapActor_SetIdle;  the loop
 * note ends the scheduling region and pins the load to the ROM's slot.
 * `while (0) ;` is byte-identical, `if (0) ;` is INERT -- so this is the loop
 * and not a label.  Ending the region costs the PRECEDING call its scheduler,
 * which is why __Func_80925cc(0xb, 1) needs a pin of its own (2 encodings)
 * when its four siblings at (0xb, 2) do not.
 *
 * DO NOT TRANSCRIBE THE ROM'S SHIFT ORDER.  The two 0xa0<<8 fills are emitted
 * with the `lsl` in DIFFERENT slots (`mov r1 / mov r0 / lsl r1 / mov r2` and
 * `mov r1 / mov r2 / mov r0 / lsl r1`) and one uniform source form produces
 * both.  Transcribing the second site's own emitted order is also exact -- a
 * TIE, which is the cleanest confirmation available that sched2 owns the
 * difference.  Moving the shift to sit directly after its `mov` costs 2.  The
 * one-statement fill (`q1 = 0xa0 << 8;`) costs 34: these ROMs put the slot
 * `mov` mid-group, which only per-instruction statements can express.
 *
 * EVERY POOLED SMALL VALUE HERE IS A BARE LITERAL, AND objcmp SAYS SO.
 * 0x839, 0x82f, 0xe85, 0xe8b, 0x125, 0x117, 0x101, 0x105, 0x1000b and
 * 0x11b0000 are all pool loads -- const.sym's tell -- but the reference object
 * and the candidate carry the SAME 62 relocations, so none of them is a
 * symbol.  The only two symbols in the function are the two the disassembly
 * already names.
 *
 * LANDING NEEDS A SPLIT AND A MAKEFILE RULE.  The .s holds two functions and
 * this is the second; overlays/rom_78603c/overlay.ld:27 names the single .o.
 * The new C TU also needs an explicit -O2 rule, or the ovl_30_c_c_a_c_a%
 * wildcard will build it at -O1 and break compare.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __MapActor_SetIdle(int slot);
extern void __MapActor_RunScript(int slot, unsigned char *p);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern int __Func_8091c7c(int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092a1c(int a, int b, unsigned char *c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80933f8(int a, int b, int c, int d);
extern unsigned char gScript_885__02009ce0[];
extern unsigned char ActorCmd_ARRAY_885__02009bdc[];
#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_885_2009760(void)
{
    unsigned char *p;
    register int m __asm__("r5");

    { PIN1; q0 = 0x839;
    if (__GetFlag(q0) == 0) {
        { PIN1; q0 = 0x82f;
        if (__GetFlag(q0) != 0) {
            __CutsceneStart();
            __Func_80925cc(0xb, 2);
            __MessageID(0xe8b);
            __ActorMessage(0xb, 0);
            __CutsceneEnd();
        } else {
            __CutsceneStart();
            __MapActor_SetIdle(0xb);
            { PIN2; q1 = 1; q0 = 0xb;
              __Func_80925cc(q0, q1); }
            do { } while (0);
            m = 0xe85;
            __MessageID(m);
            __Func_8093040(0xb, 0, 0x14);
            { PIN3; q1 = 0x80; q0 = 0; q1 <<= 1; q2 = 0x1e;
              __MapActor_Emote(q0, q1, q2); }
            __Func_80933f8(0xc4 << 15, -1, 0x11b << 16, 1);
            { PIN3; q0 = 0; q1 = 0x5e; q2 = 0x125;
              __Func_80921c4(q0, q1, q2); }
            __Func_8092adc(0, 0xa0 << 8, 0);
            p = __MapActor_GetActor(0);
            if (p != 0)
                __MapActor_SetPos(1, *(int *)(p + 8), *(int *)(p + 0x10));
            { PIN3; q0 = 1; q1 = 0x6e; q2 = 0x117;
              __Func_80921c4(q0, q1, q2); }
            { PIN3; q1 = 0xa0; q0 = 1; q1 <<= 8; q2 = 0x28;
              __Func_8092adc(q0, q1, q2); }
            __Func_80925cc(0xb, 2);
            __CutsceneWait(0x28);
            { PIN2; q1 = 0; q0 = 0xb;
              __Func_8092c40(q0, q1); }
            if (__Func_8091c7c(0, 0) == 0) {
                __Func_80925cc(0xb, 2);
                __CutsceneWait(0x14);
                __MessageID(m + 2);
                __ActorMessage(0xb, 0);
                __SetFlag(0x82f);
            } else {
                __Func_80925cc(0xb, 2);
                __CutsceneWait(0x14);
                __MessageID(m + 3);
                __Func_8093040(0xb, 0, 0x28);
                __Func_809280c(0xb, 0, 0);
                __MapActor_SetAnim(0xb, 1);
                __MapActor_Jump(0xb, 4, 0x28);
                __MapActor_SetAnim(0xb, 6);
                { PIN3; q0 = 0xb; q1 = 0x101; q2 = 0x28;
                  __MapActor_Emote(q0, q1, q2); }
                __Func_8093040(0xb, 0, 0xa);
                __MapActor_SetAnim(0xb, 1);
                __CutsceneWait(0xa);
                __MapActor_DoAnim(0xb, 3);
                __Func_8093040(0xb, 0, 0xa);
                __MapActor_DoAnim(0xb, 3);
                { PIN2; q0 = 0; q1 = 0x1000b;
                  __Func_8092a1c(q0, q1, gScript_885__02009ce0); }
                { PIN2; q0 = 1; q1 = 0x1000b;
                  __Func_8092a1c(q0, q1, gScript_885__02009ce0); }
                __MapActor_RunScript(0xb, ActorCmd_ARRAY_885__02009bdc);
                __MapActor_SetIdle(0);
                __MapActor_SetIdle(1);
                { PIN3; q1 = 0x80; q0 = 0; q1 <<= 7; q2 = 0;
                  __Func_8092adc(q0, q1, q2); }
                { PIN3; q1 = 0x80; q0 = 1; q1 <<= 7; q2 = 0x3c;
                  __Func_8092adc(q0, q1, q2); }
                { PIN3; q0 = 0; q1 = 0x105; q2 = 0;
                  __MapActor_Emote(q0, q1, q2); }
                __MapActor_Emote(1, 0x105, 0x78);
                __SetFlag(0x839);
            }
            __MapActor_SetAnim(1, 2);
            p = __MapActor_GetActor(0);
            if (p != 0)
                __MapActor_TravelTo(1, *(short *)(p + 0xa), *(short *)(p + 0x12));
            __MapActor_WaitMovement(1);
            __MapActor_SetPos(1, 0, 0);
            __CutsceneEnd();
        }
        }
    }
    }
}
