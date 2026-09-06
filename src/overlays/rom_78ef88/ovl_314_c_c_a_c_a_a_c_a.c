// fakematch
/* ovl_314_c_c_a_c_a_a_c_a.c  --  OvlFunc_896_2008f8c  --  0x02008f8c
 *   [asm/overlays/rom_78ef88/ovl_314_c_c_a_c_a_a_c_a.s, the ONLY function in
 *    that .s -- no split is needed and none is proposed]
 *
 * 464 instructions of cutscene script: 118 calls, two counted `do` loops that
 * age five struct fields a frame at a time, one flag-guarded counter bump, and
 * a mid-function `.pool_aligned` dump reached by `b .L13c0`.  VERDICT:
 *
 *   OK OvlFunc_896_2008f8c -- 1220 bytes, 479 encodings and 119 relocations identical
 *
 * ...against BOTH a scratch copy of the reference and the REAL asm/ path.  Do
 * not confuse this with OvlFunc_895_2008f8c in
 * src/overlays/rom_78dee8/ovl_30_c_c_c_a_c_c_a_c.c -- same address, different
 * overlay, different function.  That file's write-up is the template this one
 * follows, and its levers were treated as SUFFICIENT NOT NECESSARY: every one
 * is re-measured in the table at the bottom, and two of them (the `unsigned
 * char one` bitfield spelling, the whole-value fill) came out with a different
 * boundary here.
 *
 * NO FLAG GROUP IS INVOLVED, AND THAT IS A POSITIVE FINDING.  No Makefile rule
 * mentions rom_78ef88 at all -- explicit or wildcard -- so the cross-dir
 * `asm/%.o: src/%.c` at Makefile:146 applies and the TU takes the tree default
 * `-O2 -mthumb -mthumb-interwork -mcpu=arm7tdmi -fno-builtin -nostdinc
 * -ffreestanding -fcall-used-r4`.  objcmp printed no "(built with: ...)" line
 * when screened through the real asm/ path, which is the check that catches an
 * inherited wildcard.  The ROM's `push {r5, r6, r7, lr}` with NO r4 is the
 * independent confirmation that -fcall-used-r4 is in force.
 *
 * THE PROLOGUE SAYS "NAMED LOCALS", NOT "PINS ONLY" -- read it by CONTENT.
 * `push {r5,r6,r7,lr}` plus `mov r7,r10 / mov r6,r9 / mov r5,r8 / push {r5,r6,r7}`
 * is SIX call-saved registers spent, so six values live across calls.  They are
 * r5 = the loop counter, r6 = the actor-0xc pointer, r7 = the loop's 0x28f
 * stride, r8 = the actor-8 pointer, r9 = a byte pointer read out of
 * `actor_c[0x50]`, r10 = `actor_c + 0x55`.  Pins are still needed for argument
 * ORDER on top of that -- 26 of them -- but a pins-only reading of this
 * function cannot start.
 *
 * NEW: ONE MATERIALISATION DOES NOT MEAN ONE VARIABLE, WHEN THE SECOND
 * ASSIGNMENT IS IN THE SAME BASIC BLOCK.  This is the exact converse of the
 * recorded "a re-loaded immediate after a join is TWO locals, not one", and it
 * runs the other way, so the pair of rules now bounds the question from both
 * sides.
 *
 * The ROM materialises zero exactly ONCE before the first loop:
 *
 *     ldr r3, [r6, #0x50] / mov r5, #0 / add r3, #0x26 / strb r5, [r3]
 *     ...  (80 instructions, 20 calls, no label)  ...
 *     mov r2, #0x55 / add r2, r6 / strb r5, [r2]
 *     ...  (60 instructions)  ...
 *   .L1148:  ... / add r5, #1 / bl __CutsceneWait / cmp r5, #0x5a / bne .L1148
 *
 * One `mov r5, #0` serving two byte stores AND arriving at the loop as the
 * counter's initial value reads as one variable.  Written that way -- `i = 0;`
 * up in the setup block, `*p9 = i; *p10 = i;`, then `do {...} while (i != 0x5a)`
 * -- it is 485 instructions against 479 and 373 encodings differ.  The six
 * extra instructions are reload copies: `i` now spans 120 insns, its
 * `floor_log2(n_refs) * n_refs / live_length` priority falls below the actor
 * pointer's, gcc hands r5/r6/r7 to the other three values and puts `i` in r8 --
 * where `strb`, `add #imm` and `cmp #imm` all need a LO register, so every use
 * pays `mov r2, r8` / `mov r8, r2`.
 *
 * Written as TWO variables -- an `int z = 0;` whose only job is the two byte
 * stores, and `i = 0;` immediately before the loop -- it is EXACT.  gcc emits
 * only ONE `mov r5, #0` anyway, because both assignments sit in the SAME basic
 * block (the reference has no label between insn 5 and `.L1148`, so the whole
 * 174-instruction opening is one block); the counter's init is folded into a
 * copy of a value already live, the copy is coalesced, and the `mov` disappears
 * -- while `i`'s live range stays short enough to win r5.  The second loop's
 * `mov r5, #0` at .L1184 IS emitted, because the first loop leaves 0x5a in the
 * register and that assignment is in a different block.
 *
 * So: a value the ROM materialises ONCE is one variable only if there is no
 * intervening basic-block boundary the compiler could have folded across.  If
 * the single `mov` and the later use are in the same block, count the SOURCE
 * things by their live ranges, not by the materialisations.
 *
 * Measured on the alternatives, all with the same 26 pins:
 *
 *   two locals, `i = 0` at the loop      exact          (this file)
 *   two locals, `i = z` at the loop      exact
 *   three locals (z, z2, i)              exact
 *   `z` typed `unsigned char`            exact
 *   ONE local, `i = 0` in the setup      485 insns, 373 differing
 *   ONE local, `i = 0` after `*p10 = 0`  483 insns, 276 differing
 *   two locals but no `z` at all         479 insns, 243 differing
 *
 * The type of `z` is INERT here, which is worth saying next to the recorded
 * HImode rule ("a pool load of a small constant where the ROM has a `mov` is a
 * TYPE question"): that rule is about which INSTRUCTION gets emitted, and this
 * is about which REGISTER the neighbour gets.  Only the variable COUNT matters.
 *
 * CORRECTION TO A RECORDED MECHANISM, read from the compiler rather than
 * inferred.  The entry "A parameter and a loop counter can be the SAME
 * variable" explains itself with "`allocno_compare` weights priority by
 * basic-block *frequency*, so merging the parameter with a loop counter
 * multiplies its priority".  `allocno_compare` in
 * /opt/camelot-gcc/gcc-2.96/gcc/global.c has NO frequency term:
 *
 *     pri1 = ((double) (floor_log2 (allocno[v1].n_refs) * allocno[v1].n_refs)
 *             / allocno[v1].live_length) * 10000 * allocno[v1].size;
 *
 * Loop depth does not enter it.  Merging a short-lived value with a loop
 * counter raises its priority by raising n_refs (and the doc's own line 5599
 * quotes the formula correctly), so the LEVER is right and its stated MECHANISM
 * is not.  That matters here because this function needed the merge run
 * BACKWARDS: splitting, not merging, is what gave the counter r5, and a
 * frequency-based model predicts the opposite.
 *
 * THE COUNTER BUMP GOES AFTER THE CALL -- the recorded lever, exactly.  The ROM
 * has `mov r0, #1 / add r5, #1 / bl __CutsceneWait`, increment BEFORE the call.
 * Writing `i++;` before `__CutsceneWait(1)` puts `add r5, #1` two slots too
 * early (6 encodings differ, SIZE and RELOCATIONS both silent).  Writing it
 * AFTER the call statement is exact -- gcc hoists it over the `bl` itself and
 * the argument wins the earlier slot.  `do { ... } while (++i != 0x5a)` is the
 * same thing spelled in the condition and is byte-identical.  With the bump
 * after the call the LOOP SPELLING stops mattering: `do/while`, `while` and
 * `for(;;)`-with-break are all exact; only a `goto` back edge is not (475
 * instructions, 449 differing).
 *
 * THE `|= 1` NEEDS THE QImode LOCAL *AND* IT MUST BE ASSIGNED BEFORE THE CALL.
 * The template's lever is `unsigned char one = 1;`, and the type half holds:
 * bare `p[0x23] |= 1`, an `int` local, and an inline
 * `__MapActor_GetActor(0xc)[0x23] |= 1` are all 477 instructions (ONE SHORT)
 * and 247 differing, because the `1` is commoned with the `*p9 = 1` byte store
 * 14 instructions earlier and the ROM builds that constant TWICE.  But the
 * POSITION is load-bearing too, which the template does not record:
 *
 *   one = 1;  p = GetActor(0xc);  p[0x23] |= one;        exact  (this file)
 *   one = 1;  p = GetActor(0xc);  p[0x23] = one|p[0x23]; exact
 *   p = GetActor(0xc);  one = 1;  p[0x23] |= one;        4 differing
 *
 * so the operand order the recorded "constant as the destination of an ORR"
 * lever is about is INERT here, and what decides it is whether the assignment
 * is separated from the use by the `bl`.  Same conclusion as the recorded "a
 * named local in the same basic block as the call": the intervening call is the
 * thing, not the statement gap.
 *
 * TWENTY-SIX PINS, NOMINATED BY FAMILY AND MINIMAL BY MEASUREMENT.  The
 * candidate set is the 25 sites carrying a repeated expensive constant, widened
 * by "same callee AND same argument shape as an already-nominated site" to 30,
 * plus site 96 for the `__Func_8092c40` descending fill: 31.  With all 31 the
 * function is already exact, so the sweep is pure minimisation -- every site
 * stripped individually under objcmp, greedily, re-tested after each drop, run
 * to a fixpoint FROM BOTH ENDS.  Both directions converge on the same 26.
 *
 * THE FAMILY RULE PAID FIVE FOR FIVE.  All five family-only nominations (19,
 * 33, 81, 111, 112) are REQUIRED, each worth exactly 2 encodings with SIZE and
 * RELOCATIONS silent -- pure ordering, invisible to any predicate over their
 * own argument lists.  Site 19 is `__Func_8092adc(9, 0xe0<<7, 0x50)`, whose
 * shift is used once; 33 is `(5, 0xc0<<8, 0)`; 81 is `(5, 0xc0<<7, 0x14)`; 111
 * is `(5, 0x80<<6, 0)`; 112 is `(9, 0xc0<<6, 0xa)`.  The sibling's write-up
 * reported two of three family extras as over-nominations; here none is.  The
 * five sites the sweep DID remove (37, 40, 68, 101, 108) are all CSE
 * nominations -- later uses of a value already pinned at its first use -- and
 * they are inert scaffolding that does not ship.
 *
 * THE RELOCATION LINE SORTS THE TWO PIN JOBS, again with no middle.  Dropping
 * each of the 26 one at a time:
 *
 *   13 ORDERING pins -- 19, 20, 32, 33, 35, 36, 51, 73, 80, 81, 96, 111, 112
 *           -- EXACTLY 2 encodings each (51 is 3), SIZE silent, RELOCATIONS
 *              SILENT
 *   13 CSE pins -- 7, 12, 15, 17, 27, 28, 31, 34, 38, 65, 66, 79, 86 -- 12 to
 *              448 encodings, RELOCATIONS ALWAYS differ, SIZE silent on 7 of
 *              the 13 (it fires only on 7, 12, 28, 38, 65, 66)
 *
 * Nothing landed in 4..9: the residues are 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
 * 3 and then 12, 21, 48, 92, 118, 149, 191, 256, 262, 297, 426, 442, 448.  SIZE
 * is silent on more than half the genuine CSE losses, exactly as recorded; the
 * relocation line is the discriminator, and it is free -- objcmp already prints
 * it, so no diff has to be read to classify a pin.
 *
 * SITE 96 IS `__Func_8092c40(0xb, 0)` WANTING THE DESCENDING FILL -- the
 * recorded lever, and the tell is binary here as it is everywhere else:
 * descending exact, ascending 2 encodings, no pin at all the SAME 2.  Both its
 * arguments are bare `mov #imm8`, so no constant-based rule can see it; it is
 * reachable only from the residue or from the callee name, and the callee name
 * is not evidence -- site 96 is the only `__Func_8092c40` in the function.
 *
 * UNIFORM WHOLE-VALUE ASCENDING FILL IS CORRECT.  Writing every pinned argument
 * as one ascending statement reproduces all of the ROM's emitted orders --
 * `mov r1 / mov r0 / lsl r1 / mov r2`, `mov r1 / mov r2 / mov r0 / lsl r1`,
 * `mov r2 / ldr r1 / mov r0 / lsl r2` -- with no per-site tuning at all.
 * Respelling every `a << k` as its whole value is byte-identical (so the shifts
 * are gcc's own arithmetic and the source's spelling is not recoverable);
 * filling in the ROM's own emitted register order at every site is 47 differing
 * and is WORSE, which is the recorded warning that the emitted order is a
 * readout, not an instruction.
 *
 * `iwram_3001ebc` IS A SCALAR POINTER, as it is in the template.  `extern
 * unsigned char iwram_3001ebc[]` folds the base into the relocation, comes out
 * four instructions short and 90 differing.  The ROM's `ldr r3, =iwram_3001ebc
 * / ldr r2, [r3]` is the pointer load, and 0xec << 1 is gcc's own build of the
 * 0x1d8 offset off the plain `*(unsigned short *)(iwram_3001ebc + 0x1d8) += 1`.
 *
 * 0x107d, 0x108d, 0x400c AND 0x5009 ARE PLAIN LITERALS, settled by relocation:
 * the reference object carries exactly 119 records, 118 `bl` and ONE
 * R_ARM_ABS32 for `iwram_3001ebc`, and message.sym has no entry in the 0x10xx
 * range.  The disassembly's own `ldr r0, =0x107d` agrees.
 *
 * MEASURED WORSE (all against the final 26-pin set, one change at a time):
 *
 *   | change                                   | insns | differing        |
 *   |------------------------------------------|-------|------------------|
 *   | `i++` before the call                    |  479  | 6                |
 *   | drop site 96's descending fill           |  479  | 2                |
 *   | ROM's own emitted register order at pins |  479  | 47               |
 *   | `p[0x23] |= 1` bare / int local / inline  |  477  | 247              |
 *   | `one = 1` after the GetActor call        |  479  | 4                |
 *   | no separate zero local                   |  479  | 243              |
 *   | counter init in the setup block          |  485  | 373              |
 *   | counter init after `*p10 = 0`            |  483  | 276              |
 *   | a separate counter `j` for the 2nd loop  |  481  | 300              |
 *   | 0x28f named as a local                   |  475  | 450              |
 *   | `goto` back edge for both loops          |  475  | 449              |
 *   | `iwram_3001ebc` as an array              |  475  | 90               |
 *   | whole-value fill instead of `a << k`     |  479  | exact (tie)      |
 *   | `while` / `for(;;)` loops                |  479  | exact (tie)      |
 *   | `do {...} while (++i != K)`              |  479  | exact (tie)      |
 *   | all 31 nominated pins                    |  479  | exact (tie)      |
 *
 * LANDING NEEDS NO LINKER-SCRIPT CHANGE.  The .s holds ONE function and no
 * data; `overlays/rom_78ef88/overlay.ld:30` is the only line in the tree naming
 * the object --
 *
 *     asm/overlays/rom_78ef88/ovl_314_c_c_a_c_a_a_c_a.o(.text)
 *
 * -- and it stays exactly as written, because Makefile:146's cross-dir rule
 * builds `asm/<bank>/X.o` from `src/<bank>/X.c` (shorter stem, so it beats
 * `%.o: %.s` at Makefile:95).  The overlay's `.data` block names only
 * ovl_314_c_c_c_c_a, _c_c_c_c_b, _c_c_c_c_c_b and _c_c_c_c_c_c, and this object
 * has neither .data nor .rodata -- the generated goldensun.map records both as
 * length 0 -- so there is nothing to remap.  Precedent in the same overlay:
 * ovl_314_c_c_a_b is already elevated with its ld line untouched.
 */
extern unsigned char *iwram_3001ebc;

extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __MessageID(int id);
extern void __PlaySound(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_Surprise(int slot, int a);
extern void __Func_800fe9c(void);
extern int __Func_8091c7c(int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8092950(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092b08(int a, int b);
extern void __Func_8092c40(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void OvlFunc_896_200c248(int a, int b);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_896_2008f8c(void)
{
    unsigned char *ac;
    unsigned char *a8;
    unsigned char *p9;
    unsigned char *p10;
    unsigned char *p;
    int i;
    int z;
    unsigned char one;

    __PlaySound(0x3d);
    __MapActor_SetAnim(0xa, 0x4);
    __MessageID(0x107d);
    OvlFunc_896_200c248(0xa, 0xa);
    __MapActor_SetAnim(0xb, 0x4);
    OvlFunc_896_200c248(0xb, 0x1e);
    { PIN3; q0 = 0x9; q1 = 0x81 << 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __MapActor_Jump(0x9, 0x4, 0xa);
    __MapActor_Jump(0x9, 0x6, 0x1e);
    OvlFunc_896_200c248(0x9, 0xa);
    __Func_80925cc(0xa, 0x1);
    { PIN3; q0 = 0xa; q1 = 0xb0 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    OvlFunc_896_200c248(0xa, 0x14);
    __Func_80925cc(0xb, 0x1);
    { PIN3; q0 = 0xb; q1 = 0xd0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    OvlFunc_896_200c248(0xb, 0x1e);
    { PIN3; q0 = 0x9; q1 = 0x81 << 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8092adc(0x5, 0x0, 0x0);
    { PIN3; q0 = 0x9; q1 = 0xe0 << 7; q2 = 0x50;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x5; q1 = 0x81 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    OvlFunc_896_200c248(0x5, 0x14);
    __Func_80925cc(0x9, 0x2);
    __MapActor_SetAnim(0x9, 0x4);
    OvlFunc_896_200c248(0x9, 0xa);
    ac = __MapActor_GetActor(0xc);
    a8 = __MapActor_GetActor(8);
    z = 0;
    p9 = *(unsigned char **) (ac + 0x50) + 0x26;
    *p9 = z;
    *(int *) (ac + 0x18) = 0x1999;
    *(int *) (ac + 0x1c) = 0x1999;
    *(int *) (a8 + 0x18) = 0x1999;
    *(int *) (a8 + 0x1c) = 0x1999;
    { PIN2; q0 = 0xc; q1 = 0x80 << 1;
      __Func_8092950(q0, q1); }
    { PIN3; q0 = 0xc; q1 = 0x1d70000; q2 = 0x91 << 17;
      __MapActor_SetPos(q0, q1, q2); }
    p10 = ac + 0x55;
    *p10 = z;
    *(int *) (ac + 0xc) = 0xa0 << 14;
    __CutsceneWait(0x1);
    OvlFunc_896_200c248(0xc, 0xa);
    { PIN3; q0 = 0x5; q1 = 0x80 << 1; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x9; q1 = 0x80 << 1; q2 = 0x1e;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x5; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x9; q1 = 0xb0 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xd0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xb0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __Func_80933d4(0x80 << 10, 0x80 << 7);
    { PIN4; q0 = 0x1d70000; q1 = -0x1; q2 = 0x1350000; q3 = 0x1;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __MapActor_SetPos(0x8, 0x1d70000, 0x91 << 17);
    __PlaySound(0xbe);
    __Func_8092b08(0xc, 0x2);
    i = 0;
    do {
        *(int *) (ac + 0xc) -= 0x1999;
        *(int *) (ac + 0x18) += 0x28f;
        *(int *) (ac + 0x1c) += 0x28f;
        *(int *) (a8 + 0x18) += 0x28f;
        *(int *) (a8 + 0x1c) += 0x28f;
        __CutsceneWait(0x1);
        i++;
    } while (i != 0x5a);
    *p10 = 5;
    __CutsceneWait(0x50);
    i = 0;
    do {
        *(int *) (ac + 0xc) -= 0x8000;
        __CutsceneWait(0x1);
        i++;
    } while (i != 0x3c);
    *p10 = 3;
    __CutsceneWait(0x1e);
    *p9 = 1;
    __MapActor_SetPos(0x8, 0x0, 0x0);
    __Func_8092b08(0xc, 0x1);
    one = 1;
    p = __MapActor_GetActor(0xc);
    p[0x23] |= one;
    __Func_8092950(0xc, 0x0);
    { PIN3; q0 = 0xc; q1 = 0x80 << 8; q2 = 0x80 << 7;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_80921c4(0xc, 0x1d7, 0x99 << 1);
    __CutsceneWait(0x28);
    __Func_80925cc(0xc, 0x2);
    OvlFunc_896_200c248(0x400c, 0x14);
    __Func_8092848(0x5, 0x9, 0x0);
    __CutsceneWait(0x14);
    __Func_809259c(0x5, 0x2);
    __Func_80925cc(0x9, 0x2);
    __CutsceneWait(0x28);
    __Func_809259c(0xa, 0x1);
    __Func_80925cc(0xb, 0x1);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0xa, 0x4);
    { PIN3; q0 = 0xa; q1 = 0xa0 << 7; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xa0 << 7; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    OvlFunc_896_200c248(0xa, 0x1e);
    __MapActor_Surprise(0xc, 0x81 << 1);
    __CutsceneWait(0x3c);
    __Func_80925cc(0xb, 0x1);
    __CutsceneWait(0xa);
    OvlFunc_896_200c248(0xb, 0x1e);
    { PIN3; q0 = 0xb; q1 = 0xd0 << 8; q2 = 0x1e;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0xb, 0x3);
    __CutsceneWait(0x14);
    OvlFunc_896_200c248(0xb, 0x1e);
    __MapActor_DoAnim(0xc, 0x3);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xb; q1 = 0xa0 << 7; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x9; q1 = 0xa0 << 7; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x5; q1 = 0xc0 << 7; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0xa, 0x3);
    __CutsceneWait(0x14);
    __MapTransitionOut();
    __WaitMapTransition();
    { PIN4; q0 = 0x84 << 17; q1 = -0x1; q2 = 0xe6 << 17; q3 = 0x0;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_800fe9c();
    __WaitFrames(0x1);
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x28);
    OvlFunc_896_200c248(0xa, 0x28);
    __Func_809259c(0x0, 0x3);
    __Func_80925cc(0x1, 0x3);
    __CutsceneWait(0x50);
    { PIN2; q1 = 0x0; q0 = 0xb;
      __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) != 0)
        *(unsigned short *) (iwram_3001ebc + 0x1d8) += 1;
    __Func_8093040(0x9, 0x0, 0x14);
    __MapTransitionOut();
    __WaitMapTransition();
    __Func_80933f8(0x1dd0000, -0x1, 0xa7 << 17, 0x0);
    __Func_800fe9c();
    __WaitFrames(0x1);
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x14);
    __Func_80925cc(0xa, 0x2);
    __Func_8092adc(0xa, 0xb0 << 8, 0xa);
    __MessageID(0x108d);
    OvlFunc_896_200c248(0xa, 0x14);
    { PIN3; q0 = 0x5; q1 = 0x80 << 6; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x9; q1 = 0xc0 << 6; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(0x9, 0x4);
    OvlFunc_896_200c248(0x5009, 0x28);
    __Func_80925cc(0xb, 0x1);
    OvlFunc_896_200c248(0xb, 0xa);
    __Func_809259c(0x5, 0x2);
    __Func_80925cc(0x9, 0x2);
}
