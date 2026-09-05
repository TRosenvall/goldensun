/* OvlFunc_885_2008170 -- overlay rom_78603c, ovl_30_c_c_a_c_a_a_c_a.
 *
 * A 777-instruction straight-line cutscene script.  MATCHED.
 *
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py scratch_elev/b228/f2008170/cand.c \
 *     scratch_elev/b228/f2008170/ref_sym.s --func OvlFunc_885_2008170
 *   OK OvlFunc_885_2008170 -- 2036 bytes, 797 encodings and 207 relocations identical
 *
 * ref_sym.s is byte-for-byte asm/overlays/rom_78603c/ovl_30_c_c_a_c_a_a_c_a.s
 * with ONE substitution, `ldr r5, =0xfbf` -> `ldr r5, =_MSG_fbf`.  See "THE
 * PHANTOM" below: against the unmodified asm path this candidate reports
 * exactly one differing encoding, the pool word, and one extra R_ARM_ABS32.
 *
 * ---------------------------------------------------------------------------
 * LANDING (message.sym addition REQUIRED -- the .c alone will not link)
 * ---------------------------------------------------------------------------
 *   1. message.sym: add   _MSG_fbf = 0x0fbf;
 *      beside `_MSG_fb0 = 0xfb0;` (line 33).  Then DELETE stage1.o by hand --
 *      message.sym is not a tracked dependency of it (Makefile:57), and the
 *      overlay dies with `undefined reference to _MSG_fbf` if you skip this.
 *   2. src/overlays/rom_78603c/ovl_30_c_c_a_c_a_a_c_a.c  <- this file
 *      remove asm/overlays/rom_78603c/ovl_30_c_c_a_c_a_a_c_a.s
 *   3. NO SPLIT NEEDED.  The .s holds exactly one function
 *      (.thumb_func_start line 8, .func_end line 804) and the linker script
 *      line is unchanged:
 *        overlays/rom_78603c/overlay.ld:23
 *            asm/overlays/rom_78603c/ovl_30_c_c_a_c_a_a_c_a.o(.text)
 *   4. NO Makefile rule needed.  The recurring rom_78603c/ovl_30_c_c_a_c_a%
 *      wildcard trap (Makefile:310, :322, :825, :4665) does NOT bite here:
 *      the wildcard has been narrowed to the single explicit target
 *      ovl_30_c_c_a_c_a_b, and tryc.makefile_flags() on this TU's .c path
 *      returns set() -- the tree default (-O2 GCC296_CFLAGS).  MEASURED, not
 *      inferred: objcmp against the ORIGINAL asm/ path and against the scratch
 *      ref path give the identical verdict, and objcmp prints no
 *      "(built with: ...)" line.
 *
 * ---------------------------------------------------------------------------
 * THE PHANTOM: a literal in the reference is NOT evidence against a symbol
 * ---------------------------------------------------------------------------
 * NEW FINDING (stated more sharply than docs/elevation.md line 943 does).
 * The reference disassembly in asm/ is SYMBOL-SUBSTITUTED FROM message.sym.
 * This one function reaches ten message ids; exactly one of them, 0xfb0, is
 * present in message.sym, and exactly one of them prints as a symbol:
 *
 *     line 272   ldr r0, =_MSG_fb0        <- _MSG_fb0 = 0xfb0 IS in message.sym
 *     line 541   ldr r5, =0xfbf           <- _MSG_fbf is NOT (yet)
 *     lines 23, 279, 293, 530, 534, 570, 679, 683: =0xfa6 =0xfb1 =0xfb2 =0xfbd
 *                                          =0xfbe =0xfc2 =0xfc6 =0xfc9, none in
 *
 * So `=0xfbf` in the .s means "not in message.sym yet", not "not a symbol".
 * The .s TEXT CHANGES RETROACTIVELY the moment the id is added, and tryc.py
 * normalises `=_MSG_*` to its value (tools/tryc.py:207) so the screen cannot
 * see the difference either way.  Only objcmp, which compares relocations,
 * shows it -- as a phantom.  Do not let that phantom talk you out of a symbol.
 *
 * WHY THE SYMBOL IS FORCED (mechanism).  The ROM holds 0xfbf in r5 across a
 * call and then reaches the two neighbours with `add r0, r5, #1` / `#2`.
 * Written as the literal `m = 0xfbf`, gcc constant-propagates and folds m+1
 * and m+2 into two MORE pool words: 798 encodings against 797, 2040 bytes
 * against 2036, 228 differing.  gcc cannot fold arithmetic on a link-time
 * address, so the address of an absolute symbol is the only spelling that
 * survives.  Direct sibling precedent, same overlay directory, same
 * __Func_8091c7c message-branch idiom: src/overlays/rom_78603c/
 * ovl_30_c_c_a_c_a_a_b.c uses `extern int _MSG_f76;` and m/m+1/m+2.
 *
 * ---------------------------------------------------------------------------
 * LEVERS THAT DECIDED THIS FUNCTION, and their mechanisms
 * ---------------------------------------------------------------------------
 * 1. WIDE PUSH READ BY CONTENT.  `push {r5, r6, lr}` -- r5 and r6 hold the
 *    actor's x and z, read once from the actor struct and passed to TWO
 *    __MapActor_SetPos calls.  They cross a call, so they are named, and they
 *    are named WITH their registers: plain `int x, z;` allocates them the
 *    other way round and costs 8 differing encodings.  (r5 is later recycled
 *    for the message id -- one saved slot, two roles.)
 *
 * 2. 63 ARGUMENT PINS, AND NOT ONE OF THEM IS REMOVABLE.  Greedy drop with an
 *    objcmp re-test after EVERY drop, then a fixpoint pass, in the final shape:
 *    0 removable of 63.  (min.py, the earlier tryc-scored pass, had already
 *    taken 4 of 67 off an earlier revision; those 4 are baked in here.)  This
 *    is the tight end of the documented range -- "N pins is a size, not a set"
 *    cuts both ways.
 *
 * 3. THE FILL IS THE DEFAULT ASCENDING ONE AT 60 OF 63 SITES.  Starting from a
 *    candidate that transcribed the ROM's emitted order at every site, 50 of 53
 *    such sites flip to plain ascending q0..q3 with NO byte changing.  That is
 *    the "transcribing the ROM's order often buys NOTHING" rule at scale:
 *    sched2 reconstructs the emitted order from the canonical spelling.
 *
 * 4. THE THREE SURVIVORS ARE ALL __Func_8092c40, AND THEY WANT THE DESCENDING
 *    FILL.  `q1 = 0; q0 = N;`.  Flipping just those three to ascending costs 6
 *    differing encodings at the right length.  This is the documented
 *    __Func_8092c40 lever, and this function adds three more sites to it.  It
 *    is binary, not graded: the OTHER two __Func_8092c40 calls in this function
 *    (`(5, 0)` and one `(0xd, 0)`) take no pin at all and are byte-identical
 *    as plain calls.
 *
 * 5. WHOLE-VALUE FILL IS REFUTED HERE; THE SHIFT SPELLING IS LOAD-BEARING.
 *    The ROM builds 0x8000, 0x4000, 0xc000, 0xb000, 0x100 and friends as
 *    `mov rN, #0x80 / lsl rN, #8`.  Writing the whole value (`q1 = 0x8000;`)
 *    pools it instead: 801 encodings against 797, 2044 bytes, 768 differing.
 *    So "whole value per statement" yields to the shifted-byte test.
 *
 * 6. INERT SCAFFOLDING DROPPED.  A named `int m` holding (int)&_MSG_fbf
 *    measures BYTE-IDENTICAL to writing (int)&_MSG_fbf inline at all three
 *    uses -- gcc CSEs the address into r5 either way -- so the local does not
 *    ship, even though the sibling above spells it with one.  Both are exact;
 *    if house style is preferred, `int m; m = (int)(&_MSG_fbf);` and m+1/m+2
 *    is the equally-exact alternative.
 *
 * ---------------------------------------------------------------------------
 * MEASURED-WORSE TABLE (all against ref_sym.s, ref = 797 encodings / 2036 B)
 * ---------------------------------------------------------------------------
 *   spelling                                       ours     differing
 *   -------------------------------------------------------------------
 *   THIS FILE                                      797/2036   0   EXACT
 *   m = 0xfbf (literal instead of the symbol)      798/2040 228
 *   uniform ascending fill, WHOLE values           801/2044 768
 *   uniform ascending fill, shifts kept            797/2036   6
 *   the three __Func_8092c40 pins ascending        797/2036   6
 *   plain `int x, z` (no r5/r6 register names)     797/2036   8
 *   any one of the 63 pins dropped                            not exact
 *
 * Against the UNMODIFIED asm/ path this file reports the phantom only:
 *   XX ENCODINGS differ in 1 place(s) (ref 797, ours 797)
 *      first at index 591: ref 00000fbf  ours 00000000
 *   XX RELOCATIONS differ   (ours has one extra R_ARM_ABS32 _MSG_fbf @0x5e0)
 * and tryc.py shows it as the single line
 *   -> rom ldr r5, =0xfbf                     ours ldr r5, =_MSG_fbf
 * which vanishes once _MSG_fbf is in message.sym.
 */
extern unsigned char iwram_3001ebc[];
extern unsigned char L20ac[] __asm__(".L20ac");
extern int _MSG_fb0;
extern int _MSG_fbf;

extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_Surprise(int slot, int a);
extern void __MessageID(int id);
extern void __PlaySound(int id);
extern void __Func_8010560(unsigned char *p, int a, int b);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern int __Func_8091c7c(int a, int b);
extern void __Func_8091e9c(int n);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_885_2008170(void)
{
    unsigned char *p;
    register int x __asm__("r5");
    register int z __asm__("r6");

    if (__GetFlag(0x801))
        return;
    __CutsceneStart();
    __MapActor_SetSpeed(0, 0x80 << 9, 0x80 << 8);
    __MessageID(0xfa6);
    __Func_80925cc(0xd, 1);
    __Func_80921c4(0, 0xe8, 0x108);
    __MapActor_SetAnim(0, 0);
    __Func_8092848(0, 0xd, 0x14);
    __Func_80925cc(0xd, 2);
    __Func_8093040(0xd, 0, 0xa);
    p = __MapActor_GetActor(0);
    x = *(short *)(p + 0xa) << 16;
    z = *(short *)(p + 0x12) << 16;
    __MapActor_SetPos(5, x, z);
    __MapActor_SetPos(1, x, z);
    { PIN3; q0 = 5; q1 = 0x80; q1 <<= 8; q2 = 0x80; q2 <<= 7; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x80; q1 <<= 8; q2 = 0x80; q2 <<= 7; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 5; q1 = 0xf8; q2 = 0x84; q2 <<= 1; __Func_809218c(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xd8; q2 = 0x84; q2 <<= 1; __Func_80921c4(q0, q1, q2); }
    __MapActor_SetAnim(0, 1);
    __MapActor_SetAnim(5, 1);
    __MapActor_SetAnim(1, 1);
    __CutsceneWait(4);
    { PIN3; q0 = 5; q1 = 0xb0; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xd0; q1 <<= 8; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(5, 4);
    __CutsceneWait(0xa);
    __Func_8093040(5, 0, 0x14);
    __Func_80925cc(0xd, 1);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xd; q1 = 0xc0; q1 <<= 6; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0xd, 0, 0xa);
    { PIN3; q0 = 1; q1 = 0xc0; q1 <<= 6; q2 = 0x28; __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(1, 2);
    __Func_8093040(1, 0, 0xa);
    __Func_80925cc(0xd, 2);
    { PIN3; q0 = 0xd; q1 = 0xa0; q1 <<= 7; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(0xd, 3);
    __Func_8093040(0xd, 0, 8);
    { PIN3; q0 = 5; q1 = 0xc0; q1 <<= 6; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    __Func_8093040(5, 0, 0xa);
    { PIN3; q0 = 0xd; q1 = 0xc0; q1 <<= 6; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0xd, 3);
    __CutsceneWait(0xa);
    __Func_8093040(0xd, 0, 6);
    { PIN3; q0 = 0; q1 = 0x101; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x101; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 5; q1 = 0x101; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xd0; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 5; q1 = 0xb0; q1 <<= 8; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(5, 2);
    __Func_8093040(5, 0, 0xa);
    { PIN3; q0 = 0xd; q1 = 0x81; q1 <<= 1; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    __MapActor_DoAnim(0xd, 4);
    __Func_8093040(0xd, 0, 0xa);
    __Func_8092adc(1, 0, 0);
    { PIN3; q0 = 5; q1 = 0x80; q1 <<= 8; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0x80; q1 <<= 1; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xd0; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 5; q1 = 0xb0; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0xa0; q1 <<= 7; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0xc0; q1 <<= 6; q2 = 0x28; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0xa0; q1 <<= 7; q2 = 0x28; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0xc0; q1 <<= 6; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 0xd; __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) == 0)
        __MessageID((int)&_MSG_fb0);
    else
        __MessageID(0xfb1);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0xd, 3);
    __CutsceneWait(0xa);
    __Func_8093040(0xd, 0, 0xa);
    __MessageID(0xfb2);
    __Func_80925cc(5, 2);
    { PIN3; q0 = 5; q1 = 0x80; q1 <<= 8; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __Func_8093040(5, 0, 6);
    { PIN3; q0 = 1; q1 = 0x103; q2 = 0x1e; __MapActor_Emote(q0, q1, q2); }
    __MapActor_Jump(1, 4, 0x1e);
    __Func_8092adc(1, 0, 0xa);
    __Func_8093040(1, 0, 6);
    __Func_8092848(0, 1, 0xa);
    __Func_8092848(0, 5, 0);
    __Func_809280c(0xd, 1, 0xa);
    __Func_809280c(0xd, 5, 0xa);
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_DoAnim(5, 3);
    __CutsceneWait(0xa);
    __MapActor_SetAnim(5, 1);
    __MapActor_SetAnim(1, 1);
    __MapActor_SetAnim(0, 0);
    { PIN3; q0 = 0; q1 = 0x80; q1 <<= 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x80; q1 <<= 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 5; q1 = 0x80; q1 <<= 7; q2 = 0x10; __Func_8092adc(q0, q1, q2); }
    { PIN2; q0 = 0xd; q1 = 0x81; q1 <<= 1; __MapActor_Surprise(q0, q1); }
    __Func_80925cc(0xd, 3);
    __CutsceneWait(0xa);
    __Func_8093040(0xd, 0, 6);
    { PIN3; q0 = 0; q1 = 0x80; q1 <<= 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x80; q1 <<= 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 5; q1 = 0x80; q1 <<= 1; q2 = 0x28; __MapActor_Emote(q0, q1, q2); }
    __MapActor_DoAnim(0xd, 4);
    __CutsceneWait(0xa);
    __Func_8093040(0xd, 0, 6);
    __Func_80925cc(0xd, 1);
    __Func_8093040(0xd, 0, 6);
    { PIN3; q0 = 5; q1 = 0xb0; q1 <<= 8; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __Func_8093040(5, 0, 6);
    { PIN3; q0 = 0xd; q1 = 0xc0; q1 <<= 6; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0xd, 3);
    __CutsceneWait(6);
    __Func_8093040(0xd, 0, 0xa);
    __MapActor_Jump(0, 2, 0);
    __MapActor_Jump(1, 2, 0);
    __MapActor_Jump(5, 2, 0xa);
    { PIN3; q0 = 0; q1 = 0xc0; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xd0; q1 <<= 8; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    __Func_8093040(1, 0, 6);
    { PIN3; q0 = 0xd; q1 = 0xa0; q1 <<= 7; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0xd, 3);
    __CutsceneWait(0x10);
    __Func_8092848(0, 5, 0x28);
    __Func_8093040(5, 0, 0xa);
    __Func_80925cc(0xd, 2);
    { PIN3; q0 = 0xd; q1 = 0xc0; q1 <<= 6; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0xd, 0, 6);
    { PIN3; q0 = 0; q1 = 0xc0; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 5; q1 = 0xb0; q1 <<= 8; q2 = 0x1e; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0x105; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x105; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 5; q1 = 0x105; q2 = 0x50; __MapActor_Emote(q0, q1, q2); }
    __MapActor_DoAnim(0xd, 4);
    { PIN2; q1 = 0; q0 = 0xd; __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) == 0)
        __MessageID(0xfbd);
    else
        __MessageID(0xfbe);
    __Func_8093040(0xd, 0, 0x14);
    __MessageID((int)&_MSG_fbf);
    __Func_8092adc(1, 0, 0xa);
    __Func_80925cc(1, 2);
    { PIN2; q1 = 0; q0 = 1; __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) == 0)
        __MessageID((int)&_MSG_fbf + 1);
    else
        __MessageID((int)&_MSG_fbf + 2);
    __Func_8093040(1, 0, 6);
    __MessageID(0xfc2);
    { PIN3; q0 = 5; q1 = 0x80; q1 <<= 8; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(5, 1);
    __Func_8092c40(5, 0);
    __CutsceneWait(4);
    if (__Func_8091c7c(0, 0) == 1) {
        __MapActor_Jump(5, 2, 0x14);
        __Func_8093040(5, 0, 0xa);
    } else {
        __MapActor_SetAnim(0, 3);
        __MapActor_SetAnim(1, 3);
        __MapActor_DoAnim(5, 3);
        __CutsceneWait(8);
        __MapActor_SetAnim(0, 0);
        (*(unsigned short *)(*(unsigned char **)iwram_3001ebc + (0xec << 1)))++;
    }
    __MapActor_DoAnim(0xd, 3);
    __CutsceneWait(0xa);
    __Func_8093040(0xd, 0, 0xa);
    { PIN3; q0 = 0; q1 = 0xc0; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xd0; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 5; q1 = 0xb0; q1 <<= 8; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_DoAnim(5, 3);
    __CutsceneWait(0xa);
    __MapActor_SetAnim(0, 0);
    __CutsceneWait(0x14);
    __Func_80925cc(0xd, 2);
    __Func_8092c40(0xd, 0);
    __CutsceneWait(4);
    if (__Func_8091c7c(0, 0) == 0)
        __MessageID(0xfc6);
    else
        __MessageID(0xfc9);
    __CutsceneWait(0xa);
    __Func_80925cc(1, 2);
    __Func_8092adc(1, 0, 0xa);
    __Func_8093040(1, 0, 6);
    { PIN3; q0 = 5; q1 = 0x80; q1 <<= 8; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(5, 4);
    __Func_8093040(5, 0, 6);
    __Func_80925cc(1, 2);
    { PIN3; q0 = 1; q1 = 0x80; q1 <<= 8; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x103; q2 = 0x1e; __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(1, 0, 0xa);
    { PIN3; q0 = 0xd; q1 = 0x80; q1 <<= 1; q2 = 0x28; __MapActor_Emote(q0, q1, q2); }
    __MapActor_Jump(0xd, 4, 0x28);
    { PIN3; q0 = 0; q1 = 0xc0; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xc0; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 5; q1 = 0xb0; q1 <<= 8; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0xd, 0xb000, 0xa);
    __PlaySound(0x9e);
    __Func_8010560(L20ac, 0x2b, 8);
    { PIN3; q0 = 0xd; q1 = 0x80; q1 <<= 9; q2 = 0x80; q2 <<= 8; __MapActor_SetSpeed(q0, q1, q2); }
    __Func_80921c4(0xd, 0xe8, 0xda);
    __MapActor_SetPos(0xd, 0, 0);
    { PIN3; q0 = 0; q1 = 0x101; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x101; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 5; q1 = 0x101; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    *(unsigned int *)(*(unsigned char **)iwram_3001ebc + (0xe0 << 1)) = 0x202;
    __MapTransitionOut();
    __WaitMapTransition();
    __Func_8091e9c(0xd);
    __CutsceneEnd();
}
