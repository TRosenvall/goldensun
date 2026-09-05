// fakematch
/* OvlFunc_941_20084a8  --  0x020084a8
 *   [asm/overlays/rom_7c5efc/ovl_30_c_a_c_c_c_a_c_c_a.s, 1st of 2]
 *
 * 336 instructions of straight-line cutscene wrapped in one `if (__GetFlag)`.
 * Byte-exact: 896 bytes, 345 encodings and 95 relocations identical.  Third
 * member of the rom_7c5efc OvlFunc_941_* family, after OvlFunc_941_2009448
 * (ovl_30_c_c_c_c_c_a_b.c) and OvlFunc_941_2009760 (..._a_c.c); every lever
 * below is one of theirs, and the two new facts are numbered 5 and 6.
 *
 * Built at the TREE DEFAULT -O2.  No pattern rule in the Makefile matches
 * asm/overlays/rom_7c5efc/ovl_30_c_a_c_c_c_a_c_c_a% -- the only two rom_7c5efc
 * entries are the explicit CSE_CFLAGS lines for ..._a_c_b.o and ..._a_b.o -- so
 * `asm/%.o: src/%.c` applies and objcmp prints no `(built with: ...)` line for
 * a scratch-path screen.
 *
 * THE PROLOGUE IS `push {r5, lr}`, SO PINS ARE THE CURE.  r5 holds the message
 * base and nothing else; every repeated expensive constant is rebuilt at every
 * use.  Plain C is 345 lines against 344, 317 differing, with
 * `push {r5, r6, r7, lr}` plus a `mov r7, r8 / push {r7}` high-register spill:
 * cse1 commons 0xc0<<8 (eight sites), the 0x14ccc/0xa666 pool pair (four sites
 * each), 0xf4<<1 (three) and 0x80<<1 / 0x81<<1 (two each) into pseudos whose
 * live ranges straddle `bl`, and global-alloc must then find them callee-saved
 * registers.  Writing each value straight into a hard CALL-CLOBBERED argument
 * register kills it across the next call, so gcc has to rematerialise.
 *
 * 1. TWENTY-FOUR PINS, MINIMAL BY MEASUREMENT.  Twenty-eight pinned sites
 *    screened exact; stripping one at a time found seven inert
 *    (__Func_809218c(0xc,0xb8,0xc8<<1); __Func_8092adc at 0xd, 2, 3 and 1;
 *    __MapActor_Emote(0xc,0x80<<1,0x41); __MapActor_Emote(0xc,0x81<<1,0x46)).
 *    A greedy pass over those seven -- re-testing after every drop -- removed
 *    only FOUR: with 809218c(0xc) and 8092adc(0xd) gone, dropping 8092adc(2)
 *    is 30 differing, 8092adc(3) is 55 and 8092adc(1) is 81, because the four
 *    early 8092adc sites are one CSE chain and at least three of the four must
 *    stay pinned to keep 0xc0<<8 out of a callee-saved register.  The two
 *    Emote drops then succeeded, giving the fixpoint.  A second one-at-a-time
 *    sweep over the surviving 24 pins in the REDUCED shape found every one
 *    load-bearing, and the four late 8092adc sites were load-bearing in both
 *    shapes.  This is "N pins is a size, not a set" again: the seven inert
 *    sites are not seven removals, they are seven candidates.
 *
 * 2. INSIDE A PINNED FILL THE SHIFT'S POSITION IS SOURCE ORDER, AND THE
 *    UNIFORM FORM WINS.  All eight 0xc0<<8 sites are written
 *    `q1 = 0xc0; q0 = A; q1 <<= 8; q2 = 0;` even though the ROM emits two of
 *    them differently (r1/r2/lsl/r0 at the fourth, r1/r2/r0/lsl at the last);
 *    sched2 produces those orders from the uniform spelling.  Transcribing
 *    those two sites' emitted order instead is ALSO exact, so the two
 *    positions are genuinely indifferent -- but the uniform form is one rule
 *    instead of three.  The ONE-STATEMENT form (`q1 = 0xc0 << 8;`) is 256
 *    differing: it moves every shift to the head of its fill and loses the
 *    `mov r0` that the ROM emits between the movs and the lsl.
 *
 * 3. TWO `do { } while (0)` BARRIERS, ONE PER MESSAGE BASE.  sched2 hoists
 *    `ldr r5, =0x250d` five slots up, above __Func_80925cc and __CutsceneWait,
 *    and `ldr r5, =0x2512` five slots up over the __MapActor_Emote fill: r5 is
 *    the only call-saved value in the function, so its load is the only insn
 *    with no anti-dependence holding it down.  6 differing each.  `while (0) ;`
 *    is byte-identical; `if (0) ;` and a bare `;` are INERT (12 differing, both
 *    barriers lost), so what bounds the region is the loop note jump.c leaves
 *    behind, not a label.  Positive control: -fno-omit-frame-pointer widens the
 *    push and moves the count to 345, and tryc echoes the flag, so the sweep is
 *    live.
 *
 * 4. THE FIRST BARRIER COSTS ONE MORE PIN.  Ending the region right after
 *    __Func_80925cc(1, 1) leaves that call's argument group unscheduled, so it
 *    comes out r0/r1 against the ROM's r1/r0.  A PIN2 in the ROM's order closes
 *    it (2 differing) -- the same tax the sibling OvlFunc_941_2009448 paid on
 *    __Func_809280c(1, 0, 0).
 *
 * 5. NEW: THE MESSAGE-BASE PIN IS LOAD-BEARING HERE, WHERE IT WAS INERT IN THE
 *    SIBLINGS.  `register int m __asm__("r5")` is what keeps `add r0, r5, #3`
 *    and `add r0, r5, #4`; as a plain `int m` those two fold to
 *    `ldr r0, =0x2510` / `ldr r0, =0x2511` (2 differing).  The difference from
 *    OvlFunc_941_2009448, where the pin was inert, is WHERE the uses sit: only
 *    these two are inside the arms of an `if`/`else`, and gcse's cprop
 *    propagates a constant into a successor block.  Uses in the same block as
 *    the assignment are never rewritten, which is why m+1 and m+2 survive
 *    unpinned in all three files.  The barrier alone does not save them --
 *    dropping only the pin still gives the two pool loads.
 *
 * 6. NEW: ONE VARIABLE, TWO BASES, AND THE SECOND ONE IS WHY `m += 3` IS
 *    SPELLED THAT WAY.  r5 is reloaded with 0x2512 and used at +0, +1, +2 and
 *    then `add r5, #3` -- the DESTRUCTIVE two-operand form, not `add r0, r5,
 *    #3`.  gcc emits that only when m is dead after the add, so the last use
 *    must be written as `m += 3;` and not `__MessageID(m + 3);`.  sched2 then
 *    hoists the add into the preceding __MapActor_Emote fill, which is where
 *    the ROM shows it.  The same idiom appears as `m += 8;` in
 *    src/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_c_c_c_a_a_a_b.c.
 *
 * 0x941, 0x94d, 0x250d, 0x2512, 0x14ccc, 0xa666, 0x6666 and 0x3333 are all
 * BARE LITERALS.  Settled by relocations, not by line count: objcmp reports 95
 * relocations identical, and a symbol spelling for any of them would add an
 * R_ARM_ABS32 the reference does not carry.  None has an entry in message.sym,
 * area.sym or const.sym.
 *
 * The four __MapActor_GetActor(0) guards are plain `if (p != 0)` with 32-bit
 * member loads at +8 and +0x10; the trailing `bl OvlFunc_941_2008828` is a real
 * call before the epilogue, not a tail call.
 *
 * LANDING NEEDS A SPLIT.  The .s holds two functions and this is the FIRST;
 * overlays/rom_7c5efc/overlay.ld:31 names the single
 * ovl_30_c_a_c_c_c_a_c_c_a.o, which must become _a_a.o and _a_b.o.  Check
 * tryc.makefile_flags on both split names before landing -- no pattern
 * captures either today, but the split is what could expose one.
 *
 * MEASURED WORSE
 *   plain C, no pins, no barriers, no m pin ............... 317 differing
 *   19 pins only (repeated constants), no barriers ......... 33
 *   + singleton fills pinned, + barriers, + m pin ........... 4
 *   one-statement pinned fills (`q1 = 0xc0 << 8;`) ........ 256
 *   barrier as `if (0) ;` or as a bare `;` ................. 12
 *   greedy: drop 8092adc(2) after 809218c(0xc)+8092adc(0xd) . 30
 *   greedy: also drop 8092adc(3) ........................... 55
 *   greedy: also drop 8092adc(1) ........................... 81
 *   drop __MapActor_SetSpeed(0xd/2/3, 0x14ccc, 0xa666) pin .. 276 / 252 / 228
 */
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_WaitMovement(int slot);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int n);
extern void __Func_809218c(int slot, int a, int b);
extern void __Func_809228c(int slot, int a, int b);
extern void __Func_80922c4(int slot, int a, int b);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int slot, int a);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092adc(int slot, int a, int b);
extern int __Func_8092c40(int a, int b);
extern int __Func_8091c7c(int a, int b);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void OvlFunc_941_2008828(void);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_941_20084a8(void)
{
    unsigned char *p;
    register int m __asm__("r5");

    if (__GetFlag(0x941) != 0) {
        __SetFlag(0x94d);
        __CutsceneStart();
        { PIN3; q1 = 0x90; q2 = 0xc8; q0 = 0xc; q1 <<= 16; q2 <<= 17; __MapActor_SetPos(q0, q1, q2); }
        { PIN3; q1 = 0xc0; q2 = 0xc0; q0 = 0xc; q1 <<= 9; q2 <<= 8; __MapActor_SetSpeed(q0, q1, q2); }
        __Func_809218c(0xc, 0xb8, 0xc8 << 1);
        __MapActor_WaitMovement(0xc);
        __MapActor_SetAnim(0xc, 1);
        { PIN3; q1 = 0xc0; q2 = 0; q0 = 0xc; q1 <<= 6; __Func_8092adc(q0, q1, q2); }
        __Func_80925cc(0, 1);
        __CutsceneWait(0x1e);
        __Func_80933d4(0x80 << 8, 0x80 << 5);
        __Func_80933f8(0xc0 << 16, -1, 0xd8 << 17, 1);
        p = __MapActor_GetActor(0);
        if (p != 0)
            __MapActor_SetPos(0xd, *(int *)(p + 8), *(int *)(p + 0x10));
        { PIN3; q0 = 0xd; q1 = 0x14ccc; q2 = 0xa666; __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q2 = 0xe8; q1 = 0xa8; q2 <<= 1; q0 = 0xd; __Func_809218c(q0, q1, q2); }
        __MapActor_WaitMovement(0xd);
        __Func_8092adc(0xd, 0xc0 << 8, 0);
        p = __MapActor_GetActor(0);
        if (p != 0)
            __MapActor_SetPos(2, *(int *)(p + 8), *(int *)(p + 0x10));
        { PIN3; q0 = 2; q1 = 0x14ccc; q2 = 0xa666; __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q2 = 0xf4; q1 = 0x98; q2 <<= 1; q0 = 2; __Func_809218c(q0, q1, q2); }
        __MapActor_WaitMovement(2);
        { PIN3; q1 = 0xc0; q0 = 2; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
        p = __MapActor_GetActor(0);
        if (p != 0)
            __MapActor_SetPos(3, *(int *)(p + 8), *(int *)(p + 0x10));
        { PIN3; q0 = 3; q1 = 0x14ccc; q2 = 0xa666; __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q2 = 0xf4; q1 = 0xa8; q2 <<= 1; q0 = 3; __Func_809218c(q0, q1, q2); }
        __MapActor_WaitMovement(3);
        { PIN3; q1 = 0xc0; q0 = 3; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
        p = __MapActor_GetActor(0);
        if (p != 0)
            __MapActor_SetPos(1, *(int *)(p + 8), *(int *)(p + 0x10));
        { PIN3; q0 = 1; q1 = 0x14ccc; q2 = 0xa666; __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q2 = 0xf4; q1 = 0xb8; q2 <<= 1; q0 = 1; __Func_809218c(q0, q1, q2); }
        __MapActor_WaitMovement(1);
        { PIN3; q1 = 0xc0; q0 = 1; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
        __CutsceneWait(0x1e);
        { PIN2; q1 = 1; q0 = 1; __Func_80925cc(q0, q1); }
        do { } while (0);
        m = 0x250d;
        __MessageID(m);
        __ActorMessage(1, 0);
        __CutsceneWait(0x1e);
        __MapActor_DoAnim(3, 3);
        __CutsceneWait(0xa);
        __MessageID(m + 1);
        __ActorMessage(3, 0);
        { PIN3; q1 = 0x81; q0 = 2; q1 <<= 1; q2 = 0x46; __MapActor_Emote(q0, q1, q2); }
        __Func_809280c(2, 0, 0);
        __Func_809280c(0, 2, 0);
        __MessageID(m + 2);
        __Func_8092c40(2, 0);
        __Func_809280c(3, 0, 0);
        __Func_809280c(1, 0, 0);
        __CutsceneWait(0x1e);
        if (__Func_8091c7c(0, 0) == 0) {
            __MessageID(m + 3);
            __ActorMessage(1, 0);
        } else {
            __MessageID(m + 4);
            __ActorMessage(1, 0);
        }
        { PIN3; q1 = 0x80; q2 = 0x46; q1 <<= 1; q0 = 0xd; __MapActor_Emote(q0, q1, q2); }
        do { } while (0);
        m = 0x2512;
        __MessageID(m);
        __ActorMessage(0xd, 0);
        __Func_809259c(0, 2);
        __Func_809259c(1, 2);
        __Func_809259c(2, 2);
        __Func_809259c(3, 2);
        { PIN3; q1 = 0xc0; q0 = 0; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
        { PIN3; q1 = 0xc0; q0 = 1; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
        { PIN3; q1 = 0xc0; q0 = 2; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
        { PIN3; q1 = 0xc0; q0 = 3; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
        __MapActor_SetAnim(0xd, 2);
        __Func_80922c4(0xd, 0, -0x10);
        __MapActor_WaitMovement(0xd);
        __MapActor_SetAnim(0xd, 1);
        __MessageID(m + 1);
        __ActorMessage(0xd, 0);
        __MapActor_Emote(0xc, 0x80 << 1, 0x41);
        __MessageID(m + 2);
        __ActorMessage(0xc, 0);
        __MapActor_DoAnim(0xd, 3);
        __CutsceneWait(0x50);
        { PIN3; q0 = 0xc; q1 = 0x6666; q2 = 0x3333; __MapActor_SetSpeed(q0, q1, q2); }
        __Func_80922c4(0xc, -0xd, 0);
        __MapActor_WaitMovement(0xc);
        { PIN3; q1 = 0x80; q0 = 0xc; q1 <<= 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
        __MapActor_Emote(0xc, 0x81 << 1, 0x46);
        m += 3;
        __MessageID(m);
        __ActorMessage(0xc, 0);
        { PIN3; q2 = 0xd8; q1 = 0xa8; q2 <<= 1; q0 = 0xc; __Func_809218c(q0, q1, q2); }
        __CutsceneWait(0x28);
        __MapTransitionOut();
        __WaitMapTransition();
        __CutsceneWait(0x14);
        __CutsceneEnd();
        OvlFunc_941_2008828();
    }
}
