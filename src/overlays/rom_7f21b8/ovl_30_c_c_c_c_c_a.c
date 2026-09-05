/* OvlFunc_967_2008508  --  0x02008508
 *   [asm/overlays/rom_7f21b8/ovl_30_c_c_c_c_c_a.s, 1st of 1 -- NO SPLIT NEEDED]
 *
 * 931 instructions / 947 encodings / 2532 bytes of straight-line cutscene
 * script: one save-bit branch, one mid-scene if/else, three GetActor/TravelTo
 * guards.  The .s holds this function ALONE and carries no `.data` (the
 * `.o(.data)` line at overlays/rom_7f21b8/overlay.ld:40 is the linker naming a
 * section the object MAY contribute, not one it does -- the recorded rule).
 * The only other reference is overlay.ld:32 `...ovl_30_c_c_c_c_c_a.o(.text)`.
 * `asm/%.o: src/%.c` keeps the object at its asm/ path, so LANDING IS A
 * WHOLE-FILE REPLACEMENT WITH NO LINKER EDIT: add this .c, delete the
 * reference .s.  No Makefile pattern rule matches this stem, so the tree
 * default -O2 applies and objcmp prints no `(built with: ...)` line.
 *
 * A PIN FUNCTION, AND THE PROLOGUE SAYS SO.  `push {lr}` alone: nothing is kept
 * across a call in the ROM, so every repeated constant is rebuilt at every use
 * and only pins can work.  Here the CONSTANT-CSE LENGTH TELL FIRES CLEANLY
 * (unlike the template OvlFunc_962_2008240, where it cancelled): plain C is
 * 953 encodings / 2544 bytes against the ROM's 947 / 2532, and its prologue is
 * `push {r5, r6, lr}` followed by four more `mov`/`push` pairs spilling fp, sl,
 * r9 and r8.  820 of 947 differ.
 *
 * THE FIVE HELD VALUES, counted off plain C's `mov rN, <hi>` copies BY
 * DESTINATION: 0xc0 << 8 (16 copies, into r1 and r3), 0x80 << 7 (8), the two
 * pooled emote ids 0x105 and 0x101 (21 between them, into r1 and r2),
 * 0x80 << 8 (2) and -0x10 (2).  0x13333/0x9999 are held as a pair for the three
 * closing __MapActor_SetSpeed sites.  Every 8-bit `mov` constant is left alone
 * by CSE and needs nothing -- the pin candidates are exactly the sites with an
 * argument that is a shifted byte, a `neg`, or a pool load.
 *
 * THE UNIFORM ASCENDING FILL IS ENOUGH AT 54 OF 55 SITES.  One statement per
 * argument, ascending q0..q3, whole value per statement, at all 54 sites with a
 * complex argument: 820 differing -> 2, and the two survivors were the single
 * __Func_8092c40 site below.  sched2 relands every one of the ROM's transposed
 * emitted orders from that one spelling -- `mov r1 / mov r0 / lsl r1 / mov r2`,
 * `mov r1 / mov r2 / lsl r1 / mov r0`, `mov r1 / lsl r1 / mov r2 / mov r0`,
 * `mov r3 / lsl r3 / mov r2 / mov r1 / mov r0`.  TRANSCRIBING each fill into its
 * own ROM order instead costs 55 differing, which is the recorded trap in its
 * strongest form here: the uniform form is not an approximation, it is right.
 *
 * __Func_8092c40 WANTS THE DESCENDING FILL, the seventh function to show it,
 * and this one PINS DOWN THE MECHANISM.  The site is `__Func_8092c40(0xc, 0)`;
 * `q1 = 0; q0 = 0xc;` matches, the ascending fill costs 2 encodings and is
 * byte-identical to leaving the site unpinned (the tell is binary, as recorded).
 *
 * NEW -- THE DESCENDING FILL AND THE NO-PROTOTYPE LEVER ARE THE SAME LEVER, AND
 * THEY MEASURE BYTE-IDENTICAL.  Deleting `extern void __Func_8092c40(int, int)`
 * and writing the site as a plain `__Func_8092c40(0xc, 0)` with 44 pins is
 * EXACT, exactly as the 45-pin descending-fill form is.  Both spellings do one
 * thing: stop the prototype forcing r0 to be evaluated first.  That is why the
 * two levers have never both been needed on one site, and it explains the
 * "binary, not graded" shape of the tell.  This function is the clean control
 * because IT CONTAINS BOTH OF THE ROM'S ORDERS FOR THIS CALLEE -- the r1-first
 * `mov r1,#0 / mov r0,#0xc` above and an r0-first `ldr r0,=0x2002 / mov r1,#0`
 * later -- and the no-prototype form reproduces BOTH from one declaration
 * choice, while the prototyped form needs the descending pin at the first and
 * nothing at all at the second.  The prototyped, descending-pinned form ships
 * here: it keeps type checking on all 31 callees, and it is the form the six
 * earlier functions use.  Note that the file-sibling in this same overlay,
 * src/overlays/rom_7f21b8/ovl_30_c_c_c_c_c_b.c, made the opposite choice and
 * documents it; both are correct, and they are now measured to be equivalent.
 *
 * NEW -- THE CORPUS COUNT BEHIND "WRITE IT DESCENDING ON SIGHT".  Across all
 * 324 `bl __Func_8092c40` sites in asm/, 284 set r1 before r0 and 40 set r0
 * first: the descending fill is right 88% of the time.  A POOLED FIRST ARGUMENT
 * DOES NOT PREDICT THE ORDER -- of the 27 sites whose r0 is a pool load, 25 are
 * still r1-first.  (I formed that hypothesis from this function's own r0-first
 * pooled site and the corpus refuted it; the r0-first-with-pooled-r0 shape
 * occurs exactly twice in the ROM, here and in rom_79c738.)
 *
 * FORTY-FIVE PINS, NOT ONE REMOVABLE, AND THE SET IS UNIQUE.  Pinning all 55
 * complex-argument sites is also exact; ten of those pins measure inert and are
 * dropped, because scaffolding that measures inert must not ship.  A greedy
 * pass in ASCENDING site order and a greedy pass in DESCENDING site order from
 * the same 55-pin start converge on the IDENTICAL 45 -- so here "N pins is a
 * size" understates it, the minimum is a single set.  A fixpoint sweep in the
 * 45-pin shape then strips each survivor individually and every one fails: 2 to
 * 865 differing, with size changes of -8, -4, 0 and +4 bytes.
 *
 * WHICH TEN FALL, EXACTLY.  Eight are a NON-FIRST use of a shared value (the
 * second 0x80 << 7, the third 0x105, four of the sixteen 0xc0 << 8, the last
 * 0x101, the last 0x80 << 8); two are sites whose complex constant occurs
 * exactly ONCE in the whole function (0x2002 and 0xa0 << 8), where CSE has
 * nothing to common.  "Pin the first use" comes out exact: the first use of
 * every one of the five shared values is load-bearing and NO first use is
 * droppable.  The converse does not hold in either direction -- the SECOND
 * 0xe0 << 8 site is load-bearing, and so is the unique-constant
 * __MapActor_Emote(2, 0x107, 0x3c), which is the second job a pin does:
 * argument ORDER, not constant lifetime.
 *
 * INSIDE A PIN, THE SHIFT SPELLING IS FREE.  Three structurally different
 * spellings of every shifted-byte fill tie byte-for-byte: `q1 = 0xc0 << 8;`,
 * the folded `q1 = 0xc000;`, and the split `q1 = 0xc0; q1 <<= 8;`.  The pin
 * fixes the register and sched2 lands the `lsl` itself, so the recorded
 * "shift position is source order" rule has nothing left to bite on once the
 * shift is LAST in the fill.  The `<<` form ships because it says what the ROM
 * does.
 *
 * ALL EIGHT POOLED VALUES ARE BARE LITERALS, INCLUDING THE TWO MESSAGE BASES.
 * 0x2850, 0x2861, 0x2002, 0x9bf, 0x105, 0x101, 0x107, 0x13333 and 0x9999: none
 * is a shifted byte, so gcc pools each unaided and objcmp reports 308
 * relocations identical -- the function's only non-`bl` relocation is
 * R_ARM_ABS32 iwram_3001ebc.  Spelling the two message bases as `_MSG_2850` /
 * `_MSG_2861` symbols is 515 differing with the relocation list wrong, which is
 * the recorded tell working in the negative direction: the sibling
 * ovl_30_c_c_c_c_c_b.c DOES need `_MSG_2880`, and 0x2880 is 0x51 << 7 while
 * 0x2850 and 0x2861 are not shifted bytes.  Nothing belongs in message.sym for
 * this function.
 *
 * NAMED LOCALS ARE WORSE, AND THE PROLOGUE IS WHY.  Hoisting only the hottest
 * value into `int c0 = 0xc0 << 8;` while leaving the other 44 sites pinned is
 * 865 differing at 2524 bytes (-8) -- worse than plain C -- because the named
 * local is exactly what the ROM does not have: a value carried across a call.
 *
 * The `iwram_3001ebc` increment appears in BOTH arms of the middle if/else, in
 * the ROM's two different positions relative to __ActorMessage; written that way
 * plain C emits the pointer-then-0xec<<1 build with no help.  The two save-bit
 * branches are gcc's own layout: `if (__GetFlag(0x9bf) == 0)` puts the short
 * OvlFunc_967_2008eec arm first and jumps over it, and `.pool_aligned` dumps
 * appear where gcc's own minipools land.
 *
 * VERDICT (tools/objcmp.py, in-container, against the original asm/ path):
 *   OK OvlFunc_967_2008508 -- 2532 bytes, 947 encodings and 308 relocations
 *   identical
 */
extern unsigned char *iwram_3001ebc;

extern void OvlFunc_967_2008eec(void);

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern int __GetFlag(int id);
extern void __Func_807808c(int a);
extern int __Func_8091c7c(int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092304(int a, int b, int c);
extern void __Func_809233c(int a, int b, int c, int d);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_967_2008508(void)
{
    unsigned char *p;

    __CutsceneStart();
    __Func_807808c(1);
    { PIN3; q0 = 0; q1 = 0xc0 << 15; q2 = 0xb8 << 16;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0xb, 0, 0);
    { PIN3; q0 = 0xc; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __MessageID(0x2850);
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0xa);
    { PIN3; q0 = 0; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0; q2 = -0x10;
      __Func_8092304(q0, q1, q2); }
    __Func_80921c4(0, 0x68, 0x88);
    __CutsceneWait(0xa);
    { PIN4; q0 = 1; q1 = -0x10; q2 = 0x10; q3 = 0xc0 << 8;
      __Func_809233c(q0, q1, q2, q3); }
    { PIN4; q0 = 3; q1 = 0; q2 = 0x18; q3 = 0xc0 << 8;
      __Func_809233c(q0, q1, q2, q3); }
    { PIN4; q0 = 2; q1 = 0x10; q2 = 0x10; q3 = 0xc0 << 8;
      __Func_809233c(q0, q1, q2, q3); }
    __MapActor_WaitMovement(1);
    __CutsceneWait(0x14);
    __CutsceneWait(0xa);
    __Func_80925cc(2, 2);
    __CutsceneWait(0x14);
    __ActorMessage(2, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xb; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0xc, 0x80 << 7, 0);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0xb; q1 = 0x105; q2 = 0x46;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0xb, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xb, 4);
    __CutsceneWait(0x14);
    __ActorMessage(0xb, 0);
    __CutsceneWait(0x14);
    __Func_80925cc(0xb, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0xb, 0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xc; q1 = 0x81 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0xc, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xc, 4);
    __CutsceneWait(0x14);
    __ActorMessage(0xc, 0);
    __CutsceneWait(0x14);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xb; q1 = 0x105; q2 = 0x32;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0xb, 0);
    if (__GetFlag(0x9bf) == 0) {
        OvlFunc_967_2008eec();
    } else {
        __CutsceneWait(0x14);
        __Func_80925cc(0xb, 2);
        __CutsceneWait(0x14);
        __ActorMessage(0xb, 0);
        __CutsceneWait(0xa);
        __Func_809280c(1, 0, 0x32);
        __MapActor_Emote(0, 0x105, 0x3c);
        __CutsceneWait(0xa);
        __Func_8092adc(1, 0xc0 << 8, 0);
        __CutsceneWait(0x14);
        __ActorMessage(1, 0);
        __CutsceneWait(0xa);
        __MapActor_DoAnim(3, 4);
        __CutsceneWait(0x14);
        __ActorMessage(3, 0);
        __CutsceneWait(0xa);
        __Func_80925cc(2, 2);
        __CutsceneWait(0x14);
        __ActorMessage(2, 0);
        __CutsceneWait(0x14);
        { PIN3; q0 = 0xc; q1 = 0x101; q2 = 0x28;
          __MapActor_Emote(q0, q1, q2); }
        { PIN2; q1 = 0; q0 = 0xc;
          __Func_8092c40(q0, q1); }
        __Func_8091c7c(0, 0);
        __CutsceneWait(0x14);
        __Func_80925cc(0xc, 2);
        __CutsceneWait(0x14);
        __ActorMessage(0xc, 0);
        __CutsceneWait(0x14);
        __MapActor_DoAnim(0, 3);
        __CutsceneWait(0x1e);
        __CutsceneWait(0xa);
        __Func_80925cc(1, 2);
        __CutsceneWait(0x14);
        __ActorMessage(1, 0);
        __CutsceneWait(0xa);
        __MapActor_DoAnim(3, 4);
        __CutsceneWait(0x14);
        __ActorMessage(3, 0);
        __CutsceneWait(0xa);
        __MapActor_DoAnim(2, 3);
        __CutsceneWait(0x1e);
        __ActorMessage(2, 0);
        __CutsceneWait(0xa);
        __Func_809280c(2, 0, 0x1e);
        __Func_8092c40(0x2002, 0);
    }
    __MessageID(0x2861);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_809280c(1, 0, 0x14);
    if (__Func_8091c7c(0, 0) == 0) {
        __CutsceneWait(0x14);
        { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        __Func_8092adc(2, 0xc0 << 8, 0);
        __CutsceneWait(0x1e);
        __CutsceneWait(0xa);
        __MapActor_DoAnim(0xb, 3);
        __CutsceneWait(0x1e);
        __ActorMessage(0xb, 0);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    } else {
        __CutsceneWait(0x14);
        { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        __Func_8092adc(2, 0xc0 << 8, 0);
        __CutsceneWait(0x1e);
        __CutsceneWait(0xa);
        __MapActor_DoAnim(0xb, 4);
        __CutsceneWait(0x14);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
        __ActorMessage(0xb, 0);
    }
    __CutsceneWait(0xa);
    __Func_80925cc(0xb, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0xb, 0);
    __CutsceneWait(0xa);
    __Func_8092848(1, 0, 0);
    __Func_8092848(3, 2, 0);
    __CutsceneWait(0x28);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xb; q1 = 0x81 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0xb, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x32);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xb, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(0xb, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(0xb, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0xb, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 3; q1 = 0x80 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(3, 0);
    __CutsceneWait(0x14);
    __Func_8092adc(0xb, 0, 0);
    __CutsceneWait(0x14);
    __ActorMessage(0xb, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(0xc, 2);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xc; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    __ActorMessage(0xc, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xb, 4);
    __CutsceneWait(0x14);
    __ActorMessage(0xb, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(2, 2);
    __CutsceneWait(0x14);
    __ActorMessage(2, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xb; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __ActorMessage(0xb, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xc; q1 = 0x81 << 1; q2 = 0x32;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0xc, 0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xb; q1 = 0x101; q2 = 0x41;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8092adc(0xb, 0, 0);
    __CutsceneWait(0x14);
    __ActorMessage(0xb, 0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xc; q1 = 0x80 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0xc, 0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xc; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x28);
    __ActorMessage(0xc, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xb; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x32);
    { PIN3; q0 = 0xb; q1 = 0x83 << 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0xb, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xb, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(0xb, 0);
    __CutsceneWait(0xa);
    __Func_809259c(0, 2);
    __Func_809259c(1, 2);
    __Func_809259c(3, 2);
    __Func_80925cc(2, 2);
    __CutsceneWait(0x1e);
    __CutsceneWait(0xa);
    __Func_8092adc(0xb, 0, 0);
    __CutsceneWait(0x14);
    __ActorMessage(0xb, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(0xc, 2);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0xc; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    __ActorMessage(0xc, 0);
    __CutsceneWait(0xa);
    __MapActor_Emote(0xb, 0x101, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 1; q1 = 0xe0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __MapActor_DoAnim(1, 4);
    __CutsceneWait(0x14);
    __ActorMessage(1, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xc; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0xc; q1 = 0x105; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0xc, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(1, 2);
    __CutsceneWait(0x14);
    __ActorMessage(1, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xb; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x28);
    { PIN3; q0 = 0xb; q1 = 0x81 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0xb, 0);
    __CutsceneWait(0xa);
    __Func_8092adc(2, 0x80 << 8, 0);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 2; q1 = 0x107; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8092adc(2, 0xc0 << 8, 0);
    __CutsceneWait(0x1e);
    __MapActor_DoAnim(2, 4);
    __CutsceneWait(0x14);
    __ActorMessage(2, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xb, 4);
    __CutsceneWait(0x14);
    __ActorMessage(0xb, 0);
    __CutsceneWait(0xa);
    __Func_809259c(0, 2);
    __Func_809259c(1, 2);
    __Func_809259c(3, 2);
    __Func_80925cc(2, 2);
    __CutsceneWait(0x1e);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xb, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(0xb, 0);
    __CutsceneWait(0x14);
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(3, 3);
    __MapActor_DoAnim(2, 3);
    __CutsceneWait(0x1e);
    __CutsceneWait(0xa);
    __Func_80925cc(0xb, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0xb, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(1, 2);
    __CutsceneWait(0x14);
    __ActorMessage(1, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xb, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(0xb, 0);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(3, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(3, 0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xe0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(2, 0xa0 << 8, 0);
    __CutsceneWait(0x1e);
    __MapActor_DoAnim(0, 3);
    __CutsceneWait(0xa);
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(2, 3);
    __MapActor_DoAnim(3, 3);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 1; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetAnim(1, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(1, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(1);
    __MapActor_SetPos(1, 0, 0);
    __MapActor_SetAnim(3, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(3, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(3);
    __MapActor_SetPos(3, 0, 0);
    __MapActor_SetAnim(2, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(2, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(2);
    __MapActor_SetPos(2, 0, 0);
    __CutsceneWait(0xa);
    __CutsceneEnd();
}
