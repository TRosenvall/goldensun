// fakematch
/* ovl_30_c_c_c_c_c_c_c_c_c_a_a_b.c  --  OvlFunc_909_20088c0
 *   [the SECOND of TWO `.thumb_func_start`s in
 *    asm/overlays/rom_79c738/ovl_30_c_c_c_c_c_c_c_c_c_a_a.s, so a HAND SPLIT is
 *    required.  `tools/asmfacts.py` says `2 functions  split first`;
 *    `tools/split_asm.py ... OvlFunc_909_20088c0` says lines 198-1668,
 *    `carries data: no`, `remaining funcs: 1`, `label exports: none needed`.
 *    split_s.py's naming leaves OvlFunc_909_20086e0 in ..._a_a_a.s and this
 *    function in ..._a_a_b.s, which this file replaces; there is no _c part.
 *    overlay.ld line 41 (`..._a_a.o(.text)`) and line 54 (`..._a_a.o(.data)`)
 *    each become TWO lines naming `_a_a_a.o` and `_a_a_b.o`, and BOTH stay in
 *    the `asm/...o(SECTION)` form -- a line rewritten to `src/` is silently
 *    ignored and drops the function behind a green build.  `.func_end` expands
 *    to `.pool_aligned` (include/macros.inc:30), so each function's literal
 *    pool is dumped at its own end and the split is byte-neutral for pools.]
 *
 *   OK OvlFunc_909_20088c0 -- 3804 bytes, 1478 encodings and 371 relocations
 *      identical.  Measured six times, against a SINGLE-FUNCTION extract of
 *      the reference (`--func` filters the reference but not the candidate).
 *
 * objcmp prints no `(built with: ...)` line: adjust = set(), the tree default.
 * `makefile_flags()` is the EMPTY set -- no explicit rule names this stem, so
 * `asm/%.o: src/%.c` fires at plain -O2.  No .section/.data/.bss/.word/.byte
 * in the .s; all 39 `.L` symbols are branch targets DEFINED IN THIS FILE and
 * split_asm.py confirms `label exports: none needed`.  FAKEMATCH:
 * register-pin idiom.
 *
 * A two-variant cutscene: ~1417 instructions, 366 calls, four message bases,
 * and a 27-call block SHARED between the two variants.
 *
 * CROSS-JUMPING MERGES FORWARD; THE ROM MERGES BACKWARD.  Both variants end by
 * testing `__Func_8091c7c(0, 0)` and, on one answer, running the same block
 * (`.Lcb6`, asm lines 412-531) before dropping into the epilogue.  The ROM's
 * block order is A, C, B, rest -- the shared block sits between the then-block
 * and the else-block and the else-block reaches it with a BACKWARD `b .Lcb6`.
 * Writing the block TWICE and letting gcc's cross-jumper merge the copies is
 * the obvious move and it is wrong by 189 encodings: gcc-2.96 keeps the LATER
 * copy and turns the earlier one into a jump, giving A, (jump), B, C, rest --
 * 1458 instructions against 1417, with a 71-instruction ref-only hunk against a
 * 113-instruction ours-only one.  What is exact is a `goto` from the else arm to
 * a label INSIDE the then arm's `if` body (legal C: labels have function
 * scope).  Worth 189 -> 6 at an unchanged pin set, and it is the whole lever.
 * The else arm's `bne .Lf86 / b .Lcb6` is just Thumb's +-256-byte conditional
 * range, not a second mechanism.
 *
 * hi = 0, hiv = 0 AND GCC SPENDS SIX CALLEE-SAVED REGISTERS.  Plain C pushes
 * {r5, r6, r7, lr} and also saves r8, r10 and r11, against the ROM's
 * `push {r5, lr}`.  The ROM holds exactly ONE value callee-saved:
 * gOvl_0200a5c0, loaded once into r5 for the third argument of five
 * __Func_8092a1c calls.  The recorded cure applied unchanged -- the held value
 * gets a LOCAL (`s = gOvl_0200a5c0;`), every OTHER pinnable site gets a pin --
 * and `pinall` alone brings the push mask and the first ten instructions to
 * exact.  PINNING EVERYTHING DID NOT OVER-EVICT HERE, which is the first
 * counter-example in that series: 327 pins is 6 aligned and +4 bytes.
 *
 * WHAT CLOSED IT (aligned instruction diff, which is an edit distance -- the
 * raw encodings-differing count is meaningless until the sizes agree):
 *
 *   plain C, `goto` into the `if` body                        399  (+32 bytes)
 *   + all 327 pinnable sites pinned                             6  (+4 bytes)
 *   + the byte store's zero shared as `int z`                   5  (exact size)
 *   + the two __GetFlag(0x85f) conditions pinned                0
 *
 * THE BYTE STORE'S ZERO, WITH THE SIGN OPPOSITE TO THE SIBLING.
 * `b = __Func_8093554() + 0x55; *b = 0;` emits `mov r3, #0` for the store and a
 * SECOND `mov r3, #0` for the next call's fourth argument; the ROM has one,
 * live across the `strb`.  `int z; z = 0; *b = z;` and `z` as that argument
 * shares it.  ovl_30_c_c_c_c_c_c_c_c_c_c_c_a.c needed the same device for the
 * opposite reason -- there the ROM kept the zero in a CALLEE-saved register and
 * `z` had to be born at the top of the function; here the ROM keeps it in r3
 * and `z` is born at the store.  One rule, read in both directions.
 *
 * A CONDITION IS A PIN SITE, SECOND INSTANCE.  0x85f is tested twice and gcc
 * commons it into r5 (`ldr r5, =0x85f`, then `mov r0, r5` twice) -- which is
 * exactly why r5 was not free for gOvl.  No statement to hang a pin on, so the
 * statement-expression pin does it:
 * `if (({ PIN1; q0 = 0x85f; __GetFlag(q0); }) == 0)`.  Worth 5 -> 0, now at a
 * site with TWO occurrences rather than the sibling's six.
 *
 * `-fno-schedule-insns2` REGRESSES: 399 -> 832 on plain C and 0 -> 375 on the
 * match.  sched2 is already producing the ROM's order and alias is the wrong
 * axis -- the seventh function in this overlay to say so.  Flag NOT shipped.
 * No permutation sweep was needed; control flow, then two recorded
 * constant-placement levers, in that order.
 *
 * PIN-INDUCED MISCOMPILE RULED OUT.  Every pin is r0-r3, all caller-saved.  The
 * only callee-saved register in the emitted code is r5 and it has exactly ONE
 * definition, the gOvl_0200a5c0 pool word.
 *
 * ALL FIVE `b .Lxxx / .pool_aligned / .Lxxx:` SEQUENCES ARE POOL JUMPS, not
 * control flow (asm lines 423, 531, 935, 975, 1376) -- but two of them, 531
 * (`bl .L177e`) and 975 (`b .L12b2`), are a pool boundary AND real control flow
 * at once, so "a pool follows it" does not settle the question; the label's own
 * predecessors do.
 *
 * MEASURED WORSE / INERT (aligned diff against 1417 instructions):
 *
 *   spelling                                            aligned
 *   -------------------------------------------------  --------
 *   the shared block DUPLICATED, cross-jumped               189
 *   plain C, no pins                                        399
 *   CLASS-DROP of all 135 "every argument a bare
 *     mov #imm8" pins, as one set                            30
 *   CLASS-DROP of the 88 shifted-constant pins               189
 *   CLASS-DROP of the 103 pooled-constant pins               166
 *   CLASS-DROP of the 31 shift+pool pins                     91
 *   INERT: the 15 actor-field-load pins, dropped as one
 *     set; then 183 of 327 pins removed by a per-site
 *     fixpoint, twice, settling at the 144 kept here.
*
 * tryc.py REPORTS A FALSE 1030 ON THIS FILE AND IT IS THE DOCUMENTED DUPLICATE
 * LABEL CASE.  The `bail:` target becomes gcc's `.L9:` at the SAME address as
 * `.L7:`, the ROM's disassembly can only show one of them, and tryc's
 * line-for-line comparison slips by one from there on ("rom 1446 lines, ours
 * 1448, first diff at 411").  objcmp is byte- and relocation-identical.  Use
 * objcmp, not tryc, on any candidate that carries a `goto` into an `if` body.
 */
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __Func_800fe9c(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_Surprise(int slot, int id);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetIdle(int slot);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __Actor_SetSpriteFlags(void *a, int f);
extern int __Func_8091c7c(int a, int b);
extern void __Func_8091e9c(int a);
extern void __Func_8092158(int a, int b, int c);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809228c(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092a1c(int a, int b, void *s);
extern void __Func_8092adc(int a, int b, int c);
extern int __Func_8092c40(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern unsigned char *__Func_8093554(void);
extern int _MSG_1440;
extern unsigned char gOvl_0200a5c0[];
extern unsigned char *iwram_3001ebc;

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_909_20088c0(void)
{
    unsigned char *a;
    unsigned char *b;
    unsigned char *s;
    int z;
    __CutsceneStart();
    { PIN4; q0 = -1; q1 = -1; q2 = -1; q3 = 0; __Func_80933f8(q0, q1, q2, q3); }
    __WaitFrames(1);
    b = __Func_8093554() + 0x55;
    z = 0;
    *b = z;
    { PIN3; q0 = 0x37e0000; q1 = -1; q2 = 0xa6 << 18; __Func_80933f8(q0, q1, q2, z); }
    __WaitFrames(1);
    __MapActor_SetPos(0, 0, 0);
    if (({ PIN1; q0 = 0x85f; __GetFlag(q0); }) != 0) {
        { PIN4; q0 = 0x37e0000; q1 = -1; q2 = 0x2ba0000; q3 = 0; __Func_80933f8(q0, q1, q2, q3); }
        { PIN3; q0 = 0x13; q1 = 0xdb << 18; q2 = 0x27a0000; __MapActor_SetPos(q0, q1, q2); }
        { PIN3; q0 = 0x13; q1 = 0xc0 << 6; q2 = 0; __Func_8092adc(q0, q1, q2); }
        { PIN3; q0 = 0; q1 = 0x37e0000; q2 = 0x31e0000; __MapActor_SetPos(q0, q1, q2); }
    }
    __Func_800fe9c();
    __WaitFrames(1);
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x100;
    *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x28;
    __MapTransitionIn();
    __WaitMapTransition();
    if (({ PIN1; q0 = 0x85f; __GetFlag(q0); }) == 0) {
        __CutsceneWait(0x50);
        { PIN3; q0 = 0x13; q1 = 0x37e0000; q2 = 0x31e0000; __MapActor_SetPos(q0, q1, q2); }
        { PIN2; q0 = 0x9999; q1 = 0x1333; __Func_80933d4(q0, q1); }
        { PIN4; q0 = 0x37e0000; q1 = -1; q2 = 0x2ba0000; q3 = 1; __Func_80933f8(q0, q1, q2, q3); }
        { PIN3; q0 = 0x13; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q0 = 0x13; q1 = 0x37e; q2 = 0xae << 2; __Func_809218c(q0, q1, q2); }
        __CutsceneWait(0x50);
        { PIN4; q0 = 0x37e0000; q1 = -1; q2 = 0xa6 << 18; q3 = 1; __Func_80933f8(q0, q1, q2, q3); }
        __MapActor_WaitMovement(0x13);
        { PIN3; q0 = 0x13; q1 = 0x34a; q2 = 0xae << 2; __Func_80921c4(q0, q1, q2); }
        { PIN3; q0 = 0x13; q1 = 0x34a; q2 = 0x9f << 2; __Func_80921c4(q0, q1, q2); }
        { PIN3; q0 = 0x12; q1 = 0xe0 << 7; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
        { PIN3; q0 = 0x13; q1 = 0xdb << 2; q2 = 0x27a; __Func_80921c4(q0, q1, q2); }
        __MapActor_DoAnim(0x13, 3);
        __CutsceneWait(0x14);
        __MapActor_DoAnim(0x12, 3);
        __CutsceneWait(0xa);
        __MessageID(0x1437);
        { PIN3; q0 = 0x2012; q1 = 0; q2 = 0xa; __Func_8093040(q0, q1, q2); }
        __Func_80925cc(0x13, 2);
        __Func_8093040(0x13, 0, 0x14);
        __Func_80925cc(0x12, 1);
        { PIN3; q0 = 0x2012; q1 = 0; q2 = 0xa; __Func_8093040(q0, q1, q2); }
        __MapActor_DoAnim(0x13, 3);
        __CutsceneWait(0x28);
        { PIN3; q0 = 0x12; q1 = 0x105; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
        { PIN2; q0 = 0x2012; q1 = 0; __ActorMessage(q0, q1); }
        __Func_80925cc(0x12, 1);
        { PIN3; q0 = 0x2012; q1 = 0; q2 = 0xa; __Func_8093040(q0, q1, q2); }
        __MapActor_Surprise(0x13, 0x81 << 1);
        __CutsceneWait(0x3c);
        { PIN3; q0 = 0x13; q1 = 0xc0 << 6; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
        { PIN3; q0 = 0x12; q1 = 0xa0 << 7; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
        { PIN4; q0 = 0x37e0000; q1 = -1; q2 = 0x2ba0000; q3 = 1; __Func_80933f8(q0, q1, q2, q3); }
        { PIN3; q0 = 0; q1 = 0x37e0000; q2 = 0x31e0000; __MapActor_SetPos(q0, q1, q2); }
        { PIN3; q0 = 0; q1 = 0x9999; q2 = 0x4ccc; __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q0 = 0; q1 = 0x37e; q2 = 0x2d6; __Func_80921c4(q0, q1, q2); }
        __CutsceneWait(0x14);
        __MapActor_DoAnim(0, 3);
        __CutsceneWait(0xa);
        __Func_80925cc(0x12, 1);
        __ActorMessage(0x2012, 0);
        __Func_80933f8(0x37e0000, -1, 0xa6 << 18, 1);
        __Func_80921c4(0, 0x37e, 0xab << 2);
        a = __MapActor_GetActor(0);
        if (a != 0)
            __MapActor_SetPos(1, *(int *)(a + 8), *(int *)(a + 0x10));
        a = __MapActor_GetActor(0);
        if (a != 0)
            __MapActor_SetPos(2, *(int *)(a + 8), *(int *)(a + 0x10));
        if (__GetFlag(3) != 0) {
            a = __MapActor_GetActor(0);
            if (a != 0)
                __MapActor_SetPos(3, *(int *)(a + 8), *(int *)(a + 0x10));
        }
        { PIN3; q0 = 1; q1 = 0x9999; q2 = 0x4ccc; __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q0 = 2; q1 = 0x9999; q2 = 0x4ccc; __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q0 = 3; q1 = 0x80 << 9; q2 = 0x80 << 8; __MapActor_SetSpeed(q0, q1, q2); }
        __MapActor_SetAnim(1, 2);
        __MapActor_SetAnim(2, 2);
        __MapActor_SetAnim(3, 2);
        { PIN3; q0 = 1; q1 = -0x10; q2 = 0x10; __Func_809228c(q0, q1, q2); }
        __Func_809228c(2, 0x10, 0x10);
        if (__GetFlag(3) != 0)
            __Func_809228c(3, 0x20, 0x10);
        __MapActor_WaitMovement(2);
        __MapActor_SetAnim(1, 1);
        __MapActor_SetAnim(2, 1);
        __MapActor_SetAnim(3, 1);
        __CutsceneWait(0xa);
        { PIN3; q0 = 3; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
        { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
        { PIN3; q0 = 2; q1 = 0xc0 << 8; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
        __MapActor_Jump(0x12, 2, 0x14);
        { PIN3; q0 = 0x12; q1 = 0xe0 << 7; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
        { PIN3; q0 = 0x2012; q1 = 0; q2 = 0xa; __Func_8093040(q0, q1, q2); }
        { PIN3; q0 = 0x13; q1 = 0x80 << 5; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
        __MapActor_DoAnim(0x13, 3);
        { PIN3; q0 = 0x12; q1 = 0xa0 << 7; q2 = 0x28; __Func_8092adc(q0, q1, q2); }
        { PIN3; q0 = 0x12; q1 = 0xe0 << 7; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
        __MapActor_DoAnim(0x12, 4);
        { PIN3; q0 = 0x2012; q1 = 0; q2 = 0xa; __Func_8093040(q0, q1, q2); }
        __MapActor_Surprise(0x13, 0x81 << 1);
        __CutsceneWait(0x28);
        { PIN3; q0 = 0x12; q1 = 0xa0 << 7; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
        { PIN3; q0 = 0x12; q1 = 0x105; q2 = 0x28; __MapActor_Emote(q0, q1, q2); }
        __Func_8092c40(0x2012, 0);
        { PIN3; q0 = 1; q1 = 0xe0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
        { PIN3; q0 = 2; q1 = 0xa0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
        if (__Func_8091c7c(0, 0) != 0) {
bail:
            __MessageID((int)&_MSG_1440);
            { PIN3; q0 = 0x2012; q1 = 0; q2 = 0xa; __Func_8093040(q0, q1, q2); }
            { PIN3; q0 = 0x13; q1 = 0xc0 << 6; q2 = 0; __Func_8092adc(q0, q1, q2); }
            __MapActor_DoAnim(0x12, 4);
            __Func_8093040(0x2012, 0, 0xa);
            { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
            { PIN3; q0 = 2; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
            __MapActor_DoAnim(0, 3);
            __MapActor_SetAnim(1, 2);
            a = __MapActor_GetActor(0);
            if (a != 0)
                __MapActor_TravelTo(1, *(short *)(a + 0xa), *(short *)(a + 0x12));
            __MapActor_SetAnim(2, 2);
            a = __MapActor_GetActor(0);
            if (a != 0)
                __MapActor_TravelTo(2, *(short *)(a + 0xa), *(short *)(a + 0x12));
            if (__GetFlag(3) != 0) {
                __MapActor_SetAnim(3, 2);
                a = __MapActor_GetActor(0);
                if (a != 0)
                    __MapActor_TravelTo(3, *(short *)(a + 0xa), *(short *)(a + 0x12));
            }
            __MapActor_WaitMovement(2);
            __MapActor_SetPos(1, 0, 0);
            __MapActor_SetPos(2, 0, 0);
            __MapActor_SetPos(3, 0, 0);
            __SetFlag(0x85f);
            { PIN3; q0 = 0; q1 = 0x80 << 9; q2 = 0x80 << 8; __MapActor_SetSpeed(q0, q1, q2); }
            { PIN3; q0 = 0; q1 = 0x37e; q2 = 0xbc << 2; __Func_80921c4(q0, q1, q2); }
            *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x10;
            __MapTransitionOut();
            __WaitMapTransition();
            goto done;
        }
    } else {
        { PIN3; q0 = 0; q1 = 0x9999; q2 = 0x4ccc; __MapActor_SetSpeed(q0, q1, q2); }
        __Func_809218c(0, 0x37e, 0xab << 2);
        __CutsceneWait(0x50);
        __Func_80933d4(0x9999, 0x1333);
        __Func_80933f8(0x37e0000, -1, 0xa6 << 18, 1);
        __MapActor_WaitMovement(0);
        __MapActor_SetAnim(0, 1);
        a = __MapActor_GetActor(0);
        if (a != 0)
            __MapActor_SetPos(1, *(int *)(a + 8), *(int *)(a + 0x10));
        a = __MapActor_GetActor(0);
        if (a != 0)
            __MapActor_SetPos(2, *(int *)(a + 8), *(int *)(a + 0x10));
        if (__GetFlag(3) != 0) {
            a = __MapActor_GetActor(0);
            if (a != 0)
                __MapActor_SetPos(3, *(int *)(a + 8), *(int *)(a + 0x10));
        }
        { PIN3; q0 = 1; q1 = 0x9999; q2 = 0x4ccc; __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q0 = 2; q1 = 0x9999; q2 = 0x4ccc; __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q0 = 3; q1 = 0x80 << 9; q2 = 0x80 << 8; __MapActor_SetSpeed(q0, q1, q2); }
        __MapActor_SetAnim(1, 2);
        __MapActor_SetAnim(2, 2);
        __MapActor_SetAnim(3, 2);
        { PIN3; q0 = 1; q1 = -0x10; q2 = 0x10; __Func_809228c(q0, q1, q2); }
        __Func_809228c(2, 0x10, 0x10);
        if (__GetFlag(3) != 0)
            __Func_809228c(3, 0x20, 0x10);
        __MapActor_WaitMovement(2);
        __MapActor_SetAnim(1, 1);
        __MapActor_SetAnim(2, 1);
        __MapActor_SetAnim(3, 1);
        __CutsceneWait(0xa);
        { PIN3; q0 = 3; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
        { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
        { PIN3; q0 = 2; q1 = 0xc0 << 8; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
        __MapActor_Emote(0x12, 0x101, 0x3c);
        __MessageID(0x1442);
        __Func_8092c40(0x2012, 0);
        if (__Func_8091c7c(0, 0) == 1)
            goto bail;
    }
    { PIN3; q0 = 3; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xc0 << 8; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(3, 3);
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_DoAnim(2, 3);
    __MapActor_Emote(0x12, 0x105, 0x3c);
    __MessageID(0x1443);
    { PIN2; q0 = 0x2012; q1 = 0; __ActorMessage(q0, q1); }
    a = __MapActor_GetActor(0x14);
    __Actor_SetSpriteFlags(a, 0);
    a = __MapActor_GetActor(0x14);
    *(int *)(a + 0x18) = 0x80 << 8;
    *(int *)(a + 0x1c) = 0x80 << 8;
    a = __MapActor_GetActor(0x12);
    if (a != 0)
        __MapActor_SetPos(0x14, *(int *)(a + 8), *(int *)(a + 0x10));
    __WaitFrames(1);
    __MapActor_Jump(0x14, 6, 0);
    { PIN3; q0 = 0x14; q1 = 0x80 << 10; q2 = 0x80 << 9; __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092158(0x14, 0x37e, 0xa7 << 2);
    __CutsceneWait(0x28);
    { PIN3; q0 = 0x2012; q1 = 0; q2 = 0xa; __Func_8093040(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0x101; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0x101; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x101; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0x101; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    __MapActor_DoAnim(0x12, 4);
    __Func_8093040(0x2012, 0, 0xa);
    { PIN3; q0 = 1; q1 = 0x103; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xe0 << 8; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __Func_8092c40(0x4001, 0);
    { PIN3; q0 = 3; q1 = 0xa0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xa0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xc0 << 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    if (__Func_8091c7c(0, 0) == 1) {
        do {
            __Func_809259c(1, 2);
            __Func_80925cc(2, 2);
            __MessageID(0x1447);
            __Func_8092c40(0x4001, 0);
        } while (__Func_8091c7c(0, 0) != 1);
    }
    __MapActor_DoAnim(1, 3);
    __MessageID(0x1448);
    { PIN3; q0 = 0x4001; q1 = 0; q2 = 0xa; __Func_8093040(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xc0 << 8; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(0, 3);
    __MapActor_DoAnim(0, 3);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x12; q1 = 0x105; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(0x2012, 0, 0x14);
    { PIN3; q0 = 2; q1 = 0x81 << 1; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x12; q1 = 0xc0 << 6; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x12; q1 = 0x101; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x101; q2 = 0x28; __MapActor_Emote(q0, q1, q2); }
    __Func_8092adc(1, 0, 0x14);
    __Func_80925cc(1, 1);
    { PIN3; q0 = 0x4001; q1 = 0; q2 = 0xa; __Func_8093040(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0x80 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0x80 << 6; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0x81 << 1; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0x80 << 8; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0x4002, 0, 0xa);
    __Func_80925cc(1, 1);
    __CutsceneWait(0x28);
    __MapActor_DoAnim(1, 3);
    { PIN3; q0 = 0x4001; q1 = 0; q2 = 0xa; __Func_8093040(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xe0 << 8; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __Func_809259c(1, 1);
    __Func_8092c40(0x4001, 0);
    { PIN3; q0 = 3; q1 = 0xa0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xa0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xc0 << 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    while (__Func_8091c7c(0, 0) != 0) {
        __MessageID(0x144e);
        __Func_8092c40(0x4001, 0);
    }
    __CutsceneWait(0xa);
    { PIN3; q0 = 3; q1 = 0x80 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(1, 0, 0xa);
    { PIN3; q0 = 0; q1 = 0x80 << 6; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(1, 3);
    { PIN3; q0 = 2; q1 = 0x105; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0x80 << 8; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(2, 4);
    __MessageID(0x144f);
    { PIN3; q0 = 0x4002; q1 = 0; q2 = 0x14; __Func_8093040(q0, q1, q2); }
    __Func_80925cc(0x12, 1);
    { PIN3; q0 = 0x12; q1 = 0xa0 << 7; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0x12, 4);
    { PIN3; q0 = 0x2012; q1 = 0; q2 = 0xa; __Func_8093040(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xc0 << 8; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(3, 3);
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_DoAnim(2, 3);
    __CutsceneWait(0x28);
    { PIN3; q0 = 0x12; q1 = 0x105; q2 = 0x50; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x13; q1 = 0x80 << 5; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    { PIN2; q0 = 0x13; q1 = 0x81 << 1; __MapActor_Surprise(q0, q1); }
    __CutsceneWait(0x28);
    __Func_8093040(0x13, 0, 0xa);
    __Func_80925cc(0x12, 1);
    __CutsceneWait(0x28);
    { PIN3; q0 = 0x12; q1 = 0xe0 << 7; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x2012; q1 = 0; q2 = 0xa; __Func_8093040(q0, q1, q2); }
    __MapActor_DoAnim(0x13, 3);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0x12, 4);
    __MapActor_SetAnim(0x12, 4);
    { PIN3; q0 = 0x2012; q1 = 0; q2 = 0xa; __Func_8093040(q0, q1, q2); }
    __MapActor_Jump(0x14, 6, 0);
    a = __MapActor_GetActor(0x12);
    if (a != 0)
        __MapActor_TravelTo(0x14, *(short *)(a + 0xa), *(short *)(a + 0x12));
    __MapActor_WaitMovement(0x14);
    __MapActor_SetPos(0x14, 0, 0);
    __CutsceneWait(0x14);
    { PIN2; q0 = 3; q1 = 0x81 << 1; __MapActor_Surprise(q0, q1); }
    { PIN2; q0 = 0; q1 = 0x81 << 1; __MapActor_Surprise(q0, q1); }
    { PIN2; q0 = 1; q1 = 0x81 << 1; __MapActor_Surprise(q0, q1); }
    __MapActor_Surprise(2, 0x81 << 1);
    __CutsceneWait(0x28);
    __Func_80925cc(0x13, 2);
    __Func_8093040(0x13, 0, 0xa);
    __MapActor_DoAnim(0x12, 3);
    { PIN3; q0 = 0x2012; q1 = 0; q2 = 0x14; __Func_8093040(q0, q1, q2); }
    __MapActor_DoAnim(0x13, 3);
    __Func_80925cc(2, 2);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x4002; q1 = 0; q2 = 0xa; __Func_8093040(q0, q1, q2); }
    { PIN3; q0 = 0x13; q1 = 0xc0 << 6; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x12; q1 = 0xc0 << 6; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0x12, 4);
    { PIN3; q0 = 0x2012; q1 = 0; q2 = 0xa; __Func_8093040(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x103; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(0x4001, 0, 0xa);
    { PIN3; q0 = 0x12; q1 = 0xa0 << 7; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(0x12, 4);
    { PIN3; q0 = 0x2012; q1 = 0; q2 = 0xa; __Func_8093040(q0, q1, q2); }
    __MapActor_SetAnim(2, 4);
    __Func_8093040(0x4002, 0, 0xa);
    { PIN3; q0 = 0x12; q1 = 0xc0 << 6; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0x12, 3);
    { PIN3; q0 = 0x2012; q1 = 0; q2 = 0xa; __Func_8093040(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0x107; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0x107; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x107; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0x107; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x12; q1 = 0xe0 << 7; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0x12, 3);
    __CutsceneWait(0xa);
    __Func_8093040(0x2012, 0, 0xa);
    __Func_80925cc(0x13, 2);
    { PIN3; q0 = 0x13; q1 = 0x80 << 5; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0x13, 3);
    __CutsceneWait(0x14);
    s = gOvl_0200a5c0;
    { PIN2; q0 = 0; q1 = 0x10013; __Func_8092a1c(q0, q1, s); }
    { PIN2; q0 = 1; q1 = 0x10013; __Func_8092a1c(q0, q1, s); }
    { PIN2; q0 = 2; q1 = 0x10013; __Func_8092a1c(q0, q1, s); }
    { PIN2; q0 = 3; q1 = 0x10013; __Func_8092a1c(q0, q1, s); }
    { PIN3; q0 = 0x13; q1 = 0x9999; q2 = 0x4ccc; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x13; q1 = 0xd5 << 2; q2 = 0x286; __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0x13; q1 = 0xd5 << 2; q2 = 0x29a; __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0x13; q1 = 0xd8 << 2; q2 = 0xa8 << 2; __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0x13; q1 = 0x80 << 5; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0x4013, 0, 0x14);
    __MapActor_SetIdle(0);
    __MapActor_SetIdle(1);
    __MapActor_SetIdle(2);
    { PIN3; q0 = 0; q1 = 0x105; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x105; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0x105; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_DoAnim(2, 3);
    { PIN2; q0 = 0x13; q1 = 0x80 << 9; __Func_8092a1c(q0, q1, s); }
    __MapActor_SetAnim(1, 2);
    a = __MapActor_GetActor(0);
    if (a != 0)
        __MapActor_TravelTo(1, *(short *)(a + 0xa), *(short *)(a + 0x12));
    __MapActor_SetAnim(2, 2);
    a = __MapActor_GetActor(0);
    if (a != 0)
        __MapActor_TravelTo(2, *(short *)(a + 0xa), *(short *)(a + 0x12));
    if (__GetFlag(3) != 0) {
        __MapActor_SetAnim(3, 2);
        a = __MapActor_GetActor(0);
        if (a != 0)
            __MapActor_TravelTo(3, *(short *)(a + 0xa), *(short *)(a + 0x12));
    }
    __MapActor_WaitMovement(2);
    __MapActor_SetPos(1, 0, 0);
    __MapActor_SetPos(2, 0, 0);
    __MapActor_SetPos(3, 0, 0);
    { PIN3; q0 = 0; q1 = 0x80 << 9; q2 = 0x80 << 8; __MapActor_SetSpeed(q0, q1, q2); }
    __Func_80921c4(0, 0x37e, 0xbc << 2);
    *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x10;
    __MapTransitionOut();
    __WaitMapTransition();
    __SetFlag(0x321);
done:
    __Func_8091e9c(0x1d);
    __CutsceneEnd();
}
