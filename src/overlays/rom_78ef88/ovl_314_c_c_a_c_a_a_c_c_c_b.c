/* ovl_314_c_c_a_c_a_a_c_c_c_b.c  --  OvlFunc_896_200a400  --  0x0200a400
 *   [asm/overlays/rom_78ef88/ovl_314_c_c_a_c_a_a_c_c_c.s, the THIRD and LAST of
 *    the three functions left in that .s when _c_c_b.s was cut out for
 *    OvlFunc_896_200978c.  Landing it needs a second split; see LANDING.]
 *
 * 248 encodings of cutscene script -- a 6-iteration `unsigned char` fade loop,
 * a six-argument tile blit through a stack-argument pair, a null-guarded
 * position copy, and a tail that stores through a global pointer.  VERDICT:
 *
 *   OK OvlFunc_896_200a400 -- 628 bytes, 248 encodings and 58 relocations identical
 *
 * ...against BOTH a scratch copy of the reference and the REAL asm/ path, each
 * re-run to confirm, AND under `-fno-gcse` as well as the tree default:
 *
 *   default flags   OK -- 628 bytes, 248 encodings and 58 relocations identical
 *   -fno-gcse       OK -- 628 bytes, 248 encodings and 58 relocations identical
 *
 * That second line is what makes this function free to land in either TU.
 * Harness: scratch_elev/b243/f2009d04/{sweep,sdiff,minimise,objcmp2}.py plus
 * a400/gen.py -- one container invocation per whole sweep.
 *
 * THE ONE THING THAT WAS ACTUALLY HARD: A GLOBAL POINTER'S DECLARATION AND ITS
 * USE ARE ONE CHOICE, AND THE WRONG PAIRING COSTS A WHOLE `ldr`.
 *
 *     rom     ldr r3, =iwram_3001ebc / movs r2, #0xe0 / ldr r3, [r3]
 *     ours    ldr r3, =iwram_3001ebc / ldr r3, [r3] / movs r2, #0xe0 /
 *             ldr r3, [r3]                                   <-- TWICE
 *
 * `extern unsigned char *iwram_3001ebc;` declares the symbol to BE a pointer
 * variable, so `*(unsigned char **) iwram_3001ebc` loads the variable and then
 * dereferences the result: two loads where the ROM has one.
 * `extern unsigned char iwram_3001ebc[];` (or `extern unsigned char
 * *iwram_3001ebc[];` with `iwram_3001ebc[0]`) makes the symbol an ADDRESS, and
 * the same expression is the ROM's single `ldr r3, [r3]`.  Both array forms tie
 * exactly; the scalar form is 2 aligned differing.
 *
 * This is the SAME lever as docs/elevation.md §"THE ELEMENT-TYPE FOLD LEVER,
 * SECOND FORM" (address-of-a-scalar versus a declared array) but a DIFFERENT
 * SYMPTOM, so the recorded tell does not find it: that section's tell is "a
 * folded `=sym+offset` where the ROM has a bare symbol, with the length one
 * instruction over".  Here nothing folds and the pool word is identical -- the
 * excess is an extra `ldr rN, [rN]`, and the length is one instruction over for
 * a different reason.  Both spellings of the declaration exist in the tree, so
 * the fix is not "use the array form" but "match the DECLARATION to the USE".
 *
 * NINETEEN PINS, AND THE SET IS MINIMAL ONLY WITH RESPECT TO A FLAG GROUP.
 * ############################################################################
 * ## NEW.  docs/elevation.md §"'N PINS' IS A SIZE, NOT A SET" records that a  ##
 * ## minimised set is not unique and that individually-inert pins are not     ##
 * ## jointly removable.  It does not say the set depends on the FLAG GROUP.   ##
 * ## IT DOES.                                                                 ##
 * ############################################################################
 *
 * Greedy stripping to a fixpoint from BOTH ENDS of the 35 nominated sites:
 *
 *   under the tree default   converges on 17: 10, 17.0-17.4, 18, 23, 28, 30,
 *                            32, 35, 36, 38, 40, 41, 44        -- EXACT
 *   under -fno-gcse          converges on 19: the same 17 plus 5 and 8 -- EXACT
 *
 * and the 17-pin set under `-fno-gcse` is 246 instructions, TWO SHORT, SIZE
 * 624 against 628 and 16 aligned differing.  Sites 5 and 8 are the two
 * `0x80 << 9` builds inside the fade loop; with gcse on, cprop already keeps
 * them where the ROM has them, so the pin looks inert -- with gcse off the
 * value is commoned and the pin is what stops it.  The 19-pin set is exact
 * under BOTH, so THIS FILE SHIPS THE -fno-gcse FIXPOINT, not the smaller one.
 *
 * Operationally: MINIMISE UNDER THE FLAG GROUP YOU WILL SHIP.  Minimising
 * under the default and then adopting a flag is how a matching function turns
 * into a 16-differing one with no source change to blame.
 *
 * THE PROLOGUE, BY CONTENT.  `push {r5, lr}` and `sub sp, #8`: ONE call-saved
 * register and an eight-byte frame.  The frame is the two stack arguments of
 * `__CopyMapTiles(0, 0x28, 0xd, 0x2e, 3, 3)`; the ROM stores ONE register
 * twice (`movs r3, #3 / str r3, [sp] / str r3, [sp, #4]`), which is what a
 * plain pair of `0x3` literals gives.  docs/elevation.md §"Each stack-argument
 * SITE needs its own pair of locals" and §"the `str` operands tell you which
 * stack arguments to name" both point at named locals; here the ROM's own `str`
 * operands say NOT to name them -- one freshly-built register serving both
 * stores is the shared-constant case, and `s1 = 3; s2 = 3;` ties rather than
 * helping.  No r4 in the push confirms -fcall-used-r4.
 *
 * ONE MATERIALISATION IS NOT ONE VARIABLE, third sighting.  r5 has TWO tenants
 * with disjoint live ranges -- the `unsigned char` fade counter `i`, then the
 * handle returned by `OvlFunc_896_200c260`.  As ONE variable it is 250
 * instructions, FOUR LONG, and 13 aligned differing; as two it is exact.  The
 * counter's type is load-bearing on its own: the ROM's
 * `add r3, r5, #1 / lsl r3, #24 / lsr r5, r3, #24` is the QImode truncation, and
 * an `int i` is 246 instructions and 5 aligned.
 *
 * MEASURED WORSE (all against the final 19 pins, one change at a time,
 * `aligned` = sequence-aligned differing instructions):
 *
 *   | change                                       | insns | aligned |
 *   |----------------------------------------------|-------|---------|
 *   | no pins at all                               |  248  |  99     |
 *   | all 35 nominated pins                        |  248  |  11     |
 *   | + a pin on the null-guarded SetPos (site 22) |  248  |  11     |
 *   | `extern unsigned char *iwram_3001ebc;`       |  248  |   2     |
 *   | `k = 0xe0 << 1;` named                       |  248  |   8     |
 *   | one r5 variable for counter and handle       |  248  |  13     |
 *   | `int i` instead of `unsigned char i`         |  246  |   5     |
 *   | `m = -0x1;` named for the two `neg`s         |  246  |  17     |
 *   | the 17-pin default-flag fixpoint, -fno-gcse  |  246  |  16     |
 *   |----------------------------------------------|-------|---------|
 *   | `extern unsigned char *iwram_3001ebc[];`     |  248  | exact   |
 *   | the deref written inline, no `q` local       |  248  | exact   |
 *   | `while` / `goto` loop spelling               |  248  | exact   |
 *   | `if (p)` instead of `if (p != 0)`            |  248  | exact   |
 *   | whole-value fill instead of `a << k`         |  248  | exact   |
 *   | named `s1`/`s2` for the stack pair           |  248  | exact   |
 *   | + a pin on `__Func_80921c4(0, 0xe8, 0x9c)`   |  248  | exact   |
 *
 * TWO SITES THAT MUST NOT BE PINNED, and they are the two the "any expensive
 * argument" rule does not reach anyway:
 *
 *   22  __MapActor_SetPos(1, *(int *)(p+8), *(int *)(p+0x10))  -- pinning the
 *       null-guarded copy forces `adds r3, r0, #0` and moves both loads past
 *       the `cmp`: 11 aligned.  The ROM reads the fields off r0 BEFORE
 *       overwriting r0 with the slot, which is exactly what the plain call does.
 *   19  __Func_80921c4(0, 0xe8, 0x9c) -- ALL THREE ARGUMENTS CHEAP.  The pin is
 *       a tie, so it is inert scaffolding and does not ship; the ascending
 *       order the ROM has is what gcc emits unaided.
 *
 * UNIFORM WHOLE-VALUE ASCENDING FILL IS CORRECT.  One ascending statement per
 * pinned site reproduces every emitted order the ROM has, including
 * `mov r0 / mov r1 / mov r2 / lsl r1 / lsl r2 / lsl r0` and its two rotations at
 * the other `__Func_8012330` sites, with no per-site tuning and no DESCENDING
 * site anywhere in this function.  Respelling every `a << k` as its whole value
 * is byte-identical, so the shifts are gcc's own arithmetic.
 *
 * LANDING NEEDS A SECOND SPLIT, ONE LINKER LINE BECOMING TWO, AND NO MAKEFILE
 * RULE.
 *
 * `asm/overlays/rom_78ef88/ovl_314_c_c_a_c_a_a_c_c_c.s` holds THREE functions --
 * OvlFunc_896_2009d04 (~516 instructions), _200a27c (~153) and this one (~238).
 * This is the LAST of the three, so
 *
 *     python3 tools/split_s.py \
 *         asm/overlays/rom_78ef88/ovl_314_c_c_a_c_a_a_c_c_c.s \
 *         OvlFunc_896_200a400
 *
 * writes `_c_c_c_a.s` (2009d04 + 200a27c) and `_c_c_c_b.s` (this function); the
 * "after" part is empty and is not written.  Both suffixes are free: nothing in
 * asm/ or src/ matches `ovl_314_c_c_a_c_a_a_c_c_c_*` today.
 *
 * No `.L` label crosses the boundary.  Every label in the file sits strictly
 * inside one function -- .L1ee6, .L2048, .L205e, .L2076, .L2232 and the one
 * `.pool_aligned` are all inside 2009d04; .L2410, .L244e and .L250e are all
 * inside this one; 200a27c has none -- so the splitter's label-crossing refusal
 * should not fire.
 *
 * EVERY line in the tree naming that .o, matched on full path, is exactly ONE:
 *
 *     overlays/rom_78ef88/overlay.ld:33
 *         asm/overlays/rom_78ef88/ovl_314_c_c_a_c_a_a_c_c_c.o(.text)
 *
 * There is no .data and no .rodata line, and none is needed: the overlay map
 * records `.data 0x0` and `.bss 0x0` for the object (lines 71-72) against
 * `.text 0x02009d04 0x970` (line 211).  So the whole remap is that one `(.text)`
 * line becoming two, in order.  Do the split first and check `make compare`
 * green BEFORE writing the .c, because a layout mistake and a bad
 * decompilation look identical at the end.
 *
 * NO MAKEFILE RULE.  This function is exact at the tree default, so `_c_c_c_b.c`
 * needs no flag group -- and because it is ALSO exact under `-fno-gcse` it
 * would be equally correct inside a GCSE_CFLAGS TU, which is what keeps the
 * eventual four-function collapse open if the other two are ever solved.
 */
extern void __CutsceneStart(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8019908(int a, int b);
extern void __Func_801776c(int a, int b);
extern void __Func_8091200(int a, int b);
extern void __Func_8091254(int a);
extern void __Func_8091e9c(int a);
extern void __Func_80921c4(int slot, int a, int b);
extern void __Func_8092848(int slot, int a, int b);
extern void __Func_8092adc(int slot, int a, int b);
extern int OvlFunc_896_200c260(int a, int b, int c, int d);
extern unsigned char iwram_3001ebc[];

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_896_200a400(void)
{
    unsigned char *p;
    unsigned char *q;
    int h;
    unsigned char i;

    __CutsceneStart();
    __PlaySound(0x8d);
    i = 0x0;
    do {
        __Func_8091200(0x4039d2, 0x1);
        __Func_8091254(0x8);
        __CutsceneWait(0x8);
        { PIN2; q0 = 0x80 << 9; q1 = 0x1;
          __Func_8091200(q0, q1); }
        __Func_8091254(0x8);
        __CutsceneWait(0x8);
        if (i == 0x1)
            { PIN3; q0 = 0x80 << 9; q1 = 0x80 << 9; q2 = 0x80 << 9;
              __Func_8012330(q0, q1, q2); }
        i++;
    } while (i != 0x6);
    __PlaySound(0x121);
    { PIN3; q0 = -0x1; q1 = -0x1; q2 = 0xe666;
      __Func_8012330(q0, q1, q2); }
    __CopyMapTiles(0x0, 0x28, 0xd, 0x2e, 0x3, 0x3);
    __CutsceneWait(0x14);
    h = OvlFunc_896_200c260(0xde, 0xe8 << 16, 0x80 << 13, 0x90 << 16);
    __CutsceneWait(0x28);
    __Func_8019908(h, 0x1);
    __Func_801776c(0x1078, 0x1);
    { PIN3; q0 = 0x5; q1 = 0x1330000; q2 = 0x1150000;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0x9; q1 = 0x1330000; q2 = 0x1150000;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0x1330000; q2 = 0x1150000;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0x1330000; q2 = 0x1150000;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0xe; q1 = 0x1330000; q2 = 0x1150000;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0x0; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_80921c4(0x0, 0xe8, 0x9c);
    __CutsceneWait(0xa);
    p = __MapActor_GetActor(0x0);
    if (p != 0)
        __MapActor_SetPos(0x1, *(int *) (p + 0x8), *(int *) (p + 0x10));
    { PIN3; q0 = 0x1; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_80921c4(0x1, 0xda, 0xac);
    __Func_8092848(0x1, 0x0, 0x0);
    __CutsceneWait(0x14);
    __PlaySound(0x91);
    { PIN3; q0 = 0x80 << 11; q1 = 0x80 << 11; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x80 << 9; q1 = 0x80 << 9; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0x28);
    { PIN3; q0 = 0x0; q1 = 0xd0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0x1, 0xa0 << 7, 0x32);
    __PlaySound(0x90);
    { PIN3; q0 = 0xc0 << 10; q1 = 0xc0 << 10; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    { PIN3; q0 = 0x0; q1 = 0x80 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0x1, 0x0, 0x32);
    { PIN3; q0 = 0x80 << 9; q1 = 0x80 << 9; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    __Func_8092adc(0x0, 0x0, 0x0);
    { PIN3; q0 = 0x1; q1 = 0x80 << 8; q2 = 0x32;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x0; q1 = 0xb0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0x1, 0xd0 << 8, 0x0);
    __PlaySound(0x90);
    { PIN3; q0 = 0xc0 << 10; q1 = 0xc0 << 10; q2 = 0x80 << 9;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __MapActor_Jump(0x0, 0x2, 0x0);
    __MapActor_Jump(0x1, 0x2, 0x14);
    __MapActor_Jump(0x0, 0x6, 0x0);
    __MapActor_Jump(0x1, 0x6, 0x28);
    q = *(unsigned char **) iwram_3001ebc;
    *(int *) (q + (0xe0 << 1)) = 0x40 << 2;
    __MapTransitionOut();
    __WaitMapTransition();
    __Func_8091e9c(0x2);
}
