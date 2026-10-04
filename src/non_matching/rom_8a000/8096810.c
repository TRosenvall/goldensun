/* FieldMove_NoTarget -- 0x08096810.  STILL NON-MATCHING.
 *
 * NON-MATCHING, 7 of 122 encodings, PIN-FREE, exact instruction count
 * (RE-MEASURED batch 322, brief D -- the park's figure is CORRECT).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_8a000/8096810.c \
 *     asm/rom_8a000/rom_944ec_a_c_c_a_a_a_a_a.s --func FieldMove_NoTarget
 *
 * The reference holds ONE function and no data (grep -c thumb_func_start = 1),
 * so when this lands it converts the whole file with NO SPLIT, to
 * src/rom_8a000/rom_944ec_a_c_c_a_a_a_a_a.c (path currently free).
 *
 * ========================================================================
 * BATCH 322 REWROTE THE DIAGNOSIS.  THE PARK WAS THREE-QUARTERS SOLVED AND
 * HAD FILED THE SOLVED PART IN ITS NEGATIVES.  READ THIS BEFORE ANYTHING.
 * ========================================================================
 *
 * The residue is TWO rungs in TWO DIFFERENT PASSES, not one mutually-exclusive
 * trade-off.  The park treated it as one.
 *
 * RUNG 1 -- A RELOAD SCRATCH REGISTER, AND IT IS ALREADY SOLVED.
 *
 * Both halfword reads are *thumb_extendhisi2_insn with (clobber (scratch:SI)):
 * thumb has no immediate-offset ldrsh, so each needs a register holding its
 * offset, and reload fills the scratch.  In this body reload gives the 0x1a
 * (style) read r2 and the 0x1e (kind) read r3.  The r3 choice is the whole of
 * rung 1: insns 18 and 20 (`sub r3,#0x74` / `ldr r1,[r3]`) USE r3 and insn 251
 * (`r3 = r6 - 1`) SETS it, so a kind read clobbering r3 is anti- and
 * output-dependent on that chain and sched2 cannot hoist it.  `.23.sched2`
 * shows exactly that: the kind read is not even in the ready list until t=6,
 * after `ldr r1,[r3]` retires.  Give it r2 and it is free, which is the ROM.
 *
 * WHICH READ GETS r2 IS DECIDED BY PLAIN RTL ORDER.  reload1.c:821 sets
 * `last_spill_reg = -1` and reload1.c:5003 starts `allocate_reload_reg`'s
 * round-robin from it, so THE FIRST ldrsh IN RTL ORDER TAKES spill_regs[0] =
 * r2 and the second takes r3.  The ROM gives the kind read r2.  **Therefore
 * the kind read MUST come FIRST in RTL, i.e. first in source order.**  That is
 * not a preference, it is forced, and it is the opposite of what this park
 * concluded after its order sweep.
 *
 * RUNG 2 -- ONE ADJACENT TRANSPOSITION IN global.c's allocno_order.  THIS IS
 * THE ONLY THING LEFT.
 *
 * Put the kind read first (either `p, kind, m, style` or `p, m, kind, style` --
 * they produce BYTE-IDENTICAL output) and the prologue goes from 7 differing to
 * THREE, with indices 5, 7, 8 and 9 all EXACT:
 *
 *     rom    mov r2,#0x1e / ldrsh r6,[r5,r2] / sub r3,#0x74 / ldr r1,[r3]
 *            mov r3,#0x1a / ldrsh r7,[r5,r3] / sub r3,r6,#1
 *     ord C  mov r2,#0x1e / ldrsh r7,[r5,r2] / sub r3,#0x74 / ldr r1,[r3]
 *            mov r3,#0x1a / ldrsh r6,[r5,r3] / sub r3,r7,#1
 *
 * EVERY INSTRUCTION AND BOTH SCRATCH REGISTERS ARE THE ROM'S.  The only defect
 * is that `kind` and `style` have swapped r6 and r7, and that costs 9 rather
 * than 3 because the swap also shows at kind's and style's uses in cases 9
 * and 2.
 *
 * `.18.greg` names the decision in one line.  The allocation order is
 * IDENTICAL between the two bodies except for ONE ADJACENT PAIR:
 *
 *     this body   ;; 10 regs to allocate: 45 71 46 32 33 43 34 44 54 35
 *     ord C       ;; 10 regs to allocate: 45 71 46 32 33 43 44 34 54 35
 *
 * reg 34 is `kind`, reg 44 is the gState slot pointer (`slot = g + 0x24a`,
 * set at insn 107, used at 110/126/179), and reg 35 is `style`.  r5 is taken
 * by `p` and r8 by reg 54, so r6 goes to whichever of 34 and 44 is allocated
 * FIRST and r7 to the other -- they conflict, so they cannot share.
 * Dispositions flip exactly: `34 in 6 / 44 in 7` here, `34 in 7 / 44 in 6`
 * under ord C.  **So the open question is: make reg 34 outrank reg 44 in
 * global.c:allocno_compare while the kind read stays first in RTL.**
 *
 * SO THE PARK NAMED THE RIGHT PAIR -- "the kind variable against the gState
 * slot pointer" -- AND THEN DREW THE WRONG CONCLUSION FROM IT.  Its claim that
 * the two requirements are "mutually exclusive at every statement order" is
 * true only of the SOURCE ORDERS it swept; it never noticed that the order it
 * rejected at 9 had already bought both scratch registers and the entire
 * prologue schedule.  This is the "rejected-because-worse edit that is HALF of
 * a two-part fix" shape that tools/crossfire.py's docstring warns about.
 *
 * ORDER SWEEP, RE-MEASURED WITH objcmp (the park's table used tryc line
 * counts, which is why its numbers read 6/9/22 against these):
 *     p, m, style, kind   (this body)      7
 *     p, style, m, kind                    7  (exactly inert -- same output)
 *     p, m, kind, style                    9  <-- schedule+scratch EXACT
 *     p, kind, m, style                    9  <-- byte-identical to the above
 *     p, kind, style, m                   22
 *     p, style, kind, m                   22
 *
 * MEASURED AND EXACTLY INERT ON TOP OF ord C (all 122 of 122 instructions, no
 * COUNT/MEM/RELOC flag) -- 40+ crossed rows, so rung 2 is in NONE of these
 * dimensions and the next agent should not re-sweep them:
 *   declaration order of kind and style; `short kind`; `short style`;
 *   `short v`; `short *slot` as `unsigned short *`; `char *g`;
 *   `slot = (short *)(gState + 0x24a)` instead of via `g`;
 *   the 0x24a load written through `g` instead of `slot`;
 *   either 0x24a STORE written through `g` instead of `slot`;
 *   a VOLATILE cast on either 0x24a store or on the load
 *     (`*(volatile short *)(g + 0x24a)`) -- all three still CSE back onto
 *     `slot`, so slot's reference count is NOT reachable from the source;
 *   naming the Func_808d5a4 result; naming the g+0x1f4 word;
 *   copying kind into a temp for the case-9 argument;
 *   moving the case-9 block-scoped locals to function scope;
 *   declaring Func_808df1c, Func_809ade8 or Field_Halt_Target int-returning
 *     (the batch-321 int-return lever: inert here).
 * MEASURED AND WORSE ON ord C: inverting the case-9 if/else, 17 with RELOC;
 *   reading 0x1a inline in case 2, 96; dropping the style read, 98 at 120
 *   instructions.
 *
 * ***  DO NOT USE `register ... __asm__` AS AN INSTRUMENT IN THIS FUNCTION. ***
 * It is not a probe here, it is a different program: pinning a call-crossing
 * local to a callee-saved register changes the prologue's push set and
 * therefore the INSTRUCTION COUNT.  Measured on ord C:
 *   register int kind __asm__("r6")    106 of 122 at 116 instructions (COUNT)
 *   register int style __asm__("r7")    14 of 122
 *   both pinned                       106 of 122 at 116 instructions (COUNT)
 * So "pin it and see" cannot confirm or refute rung 2, and a figure obtained
 * that way is not a distance.
 *
 * TWO BANK FACTS NOT RE-DERIVED HERE, both already established elsewhere:
 *   - THE DECLARATION LEVER IS CONTRAINDICATED FOR gState.  A typed member at
 *     offset 500 folds a pool word the ROM builds at run time (a measured
 *     51-line regression).  This function reads g+0x1f4 = 500.  Left as plain
 *     array indexing deliberately.
 *   - iwram_3001f30 is declared ELEVEN different ways across the tree against
 *     an unused `struct MapState` whose layout does not fit its users'
 *     offsets.  Left as `extern char *` here.
 *
 * ------------------------------------------------------------------------
 * WHAT THE PARK GOT RIGHT, REPRODUCED AND KEPT.  Instructions 12 through 136 --
 * the range check, the jump table, all sixteen arms, both big cases and the
 * epilogue -- are IDENTICAL INCLUDING EVERY REGISTER, and that is because of:
 *
 *   THE NEGATIVE-OFFSET GLOBAL.  `*(T **)((unsigned char *)&iwram_3001f30 -
 *   0x74)` reproduces `ldr r3, =iwram_3001f30 / sub r3, #0x74 / ldr r1, [r3]`
 *   verbatim with no extra pool word.
 *
 *   THE `ldr rN, =0xffff / strh` SHAPE NEEDS AN int LOCAL ASSIGNED IN A
 *   DOMINATING BLOCK -- the function's FIRST statement.  Assigned inside the
 *   arm or inside the guarded body, gcc folds it to a HImode const_int -1,
 *   commons it with the `mov #1 / neg` from the `!= -1` test and stores a
 *   register instead, three instructions short.  Hoisting it was worth 27
 *   down to 9 on its own.  Corpus template: src/rom_9000/rom_ea54_c_b.c.
 *   This is the counter-example to the halfword-pool blocker note: the
 *   word-sized pool load for a halfword store IS reachable, by a
 *   dominating-block int local.
 *
 *   The case-body order in the source is the emission order
 *   (1,7,11,4,5,14,6,3,12,13,9,2,8,10,15,16); the epilogue's `pop {r0}`
 *   confirms `void`; three callees take NO arguments, declared
 *   `extern int f();` and called bare, because the ROM sets up no argument
 *   registers for them.
 */
extern char *iwram_3001f30;
extern unsigned char gState[];

extern void Field_Move(void);
extern void Field_Lift(void);
extern void Field_Carry(void);
extern void Field_Force(void);
extern void Field_Douse(void);
extern void Field_Whirlwind(void);
extern void Field_Frost(void);
extern void Field_Ply(void);
extern void Field_Growth(void);
extern void Field_Catch(void);
extern void Field_Reveal(void);
extern void Field_Cloak(void);
extern void Field_Retreat(void);
extern void Field_Avoid(void);
extern void Field_Halt(void);
extern void Field_Halt_Target(int a);
extern void Field_MindRead(int id, int style);
extern void Func_809ade8(int id);
extern void Func_808df1c(int a, int b);
extern int Func_809ae3c();
extern int Func_808d5a4();
extern void Func_80970f8(int a, int b);
extern void Func_809ad90(int a);
extern void Func_80984c0(void);

void FieldMove_NoTarget(void)
{
    char *p;
    char *m;
    int kind;
    int style;
    int inval;

    inval = 0xffff;
    p = iwram_3001f30;
    m = *(char **)((unsigned char *)&iwram_3001f30 - 0x74);
    style = *(short *)(p + 0x1a);
    kind = *(short *)(p + 0x1e);
    switch (kind) {
    case 1:
        Field_Move();
        break;
    case 7:
        Field_Lift();
        break;
    case 11:
        Field_Carry();
        break;
    case 4:
        Field_Force();
        break;
    case 5:
        Field_Douse();
        break;
    case 14:
        Field_Whirlwind();
        break;
    case 6:
        Field_Frost();
        break;
    case 3:
        Field_Ply();
        break;
    case 12:
        Field_Growth();
        break;
    case 13:
        Field_Catch();
        break;
    case 9:
        {
            unsigned char *g;
            short *slot;
            int v;
            int a;

            g = gState;
            slot = (short *)(g + 0x24a);
            v = *slot;
            if (v != -1) {
                Func_809ade8(v);
                *slot = inval;
            }
            Func_808df1c(*(int *)(g + 0x1f4), kind);
            a = Func_809ae3c();
            if (Func_808d5a4()) {
                Func_80970f8(*(int *)(g + 0x1f4), a);
                Field_Halt_Target(a);
                Func_809ad90(a);
                *slot = a;
            } else {
                Field_Halt();
            }
        }
        break;
    case 2:
        if (*(short *)(m + 0xcb8) != 0)
            Func_80984c0();
        Field_MindRead(*(short *)(p + 0x18), style);
        break;
    case 8:
        Field_Reveal();
        break;
    case 10:
        Field_Cloak();
        break;
    case 15:
        Field_Retreat();
        break;
    case 16:
        Field_Avoid();
        break;
    }
}
