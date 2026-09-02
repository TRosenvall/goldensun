/* FieldMove_NoTarget -- NON-MATCHING.
 * Blocker class: REGISTER-ALLOCATION PRIORITY, and it is QUANTIFIED rather than
 * guessed -- the two requirements are provably mutually exclusive.
 *
 * SIX instructions of 137, all inside the ten-instruction prologue. The line
 * counts agree, the literal pool agrees, and instructions 12 through 136 -- the
 * range check, the jump table, all sixteen arms, the whole of the two big cases
 * and the epilogue -- are IDENTICAL INCLUDING EVERY REGISTER.
 *
 * Dispatches a field move that needs no target: sixteen arms on a kind read out
 * of the field block, two of which do real work.
 *
 * THE RESIDUE, in full:
 *
 *     rom    mov r2, #0x1e / ldrsh r6, [r5, r2] / sub r3, #0x74 / ldr r1, [r3]
 *            mov r3, #0x1a / ldrsh r7, [r5, r3] / sub r3, r6, #0x1
 *     ours   sub r3, #0x74 / ldr r1, [r3] / mov r3, #0x1e / ldrsh r6, [r5, r3]
 *            sub r3, r6, #0x1 / mov r2, #0x1a / ldrsh r7, [r5, r2]
 *
 * THE MECHANISM, read out of gcc's own dumps. `global.c:allocno_compare` ranks
 * by `floor_log2(n_refs) * n_refs / live_length`. The whole function turns on
 * ONE comparison: the kind variable against the gState slot pointer. The slot
 * has 4 refs over a live length of 50, priority 1600. The kind has 3 refs over
 * a length that depends on where it is assigned:
 *
 *     kind assigned 2nd in the entry block   length 21   priority 1523
 *     kind assigned 3rd                      length 19   priority 1684 -- but see below
 *     kind assigned 4th (last)               length 18   priority 1666
 *
 * Only a length of 18 wins the comparison against 1600 in the direction that
 * puts kind in r6 and leaves the ROM's r7/r5/r8 assignment intact -- and that
 * requires kind to be assigned AFTER the style read.
 *
 * BUT THE EMISSION ORDER REQUIRES THE OPPOSITE. The ROM loads the 0x1e offset
 * before the `sub r3, #0x74` that builds the second base. With kind assigned
 * last, the pooled block address in r3 is dead before kind's load, so kind's
 * offset scratch takes r3 instead of r2, and the anti-dependency forces the
 * second base to schedule first.
 *
 * The two requirements are mutually exclusive at every statement order, and the
 * gap is exactly ONE RTL instruction. Measured on both sides of it:
 *   shortening the kind's live range -- copying it to a temp, re-reading it via
 *     gcse, typing it `short`, -fno-schedule-insns, --no-sched2, -O1,
 *     declaration order, an explicit `unsigned int` base chain
 *   lengthening the slot's range to 51 or more WITHOUT adding an instruction --
 *     naming the call results, naming the reloaded words, inverting the if/else,
 *     a named offset local
 * All inert.
 *
 * TWO LEVERS DID FIRE AND ARE WORTH KEEPING.
 *
 *   THE NEGATIVE-OFFSET GLOBAL. `*(T **)((unsigned char *)&iwram_3001f30 -
 *   0x74)` reproduces the ROM's `ldr r3, =iwram_3001f30 / sub r3, #0x74 /
 *   ldr r1, [r3]` verbatim on the first try, with no extra pool word.
 *
 *   THE `ldr rN, =0xffff / strh` SHAPE NEEDS AN int LOCAL ASSIGNED IN A
 *   DOMINATING BLOCK -- the function's FIRST statement. Assigned inside the arm,
 *   or inside the guarded body, gcc folds it back to a HImode const_int -1,
 *   commons it with the `mov #1 / neg` from the `!= -1` test, and emits a
 *   register store instead -- three instructions short. Hoisting the assignment
 *   to the top of the function was worth 27 aligned down to 9 on its own.
 *   The corpus template is src/rom_9000/rom_ea54_c_b.c. THIS IS THE MISSING
 *   COUNTER-EXAMPLE to the halfword-pool blocker note, which says the
 *   word-sized pool load for a halfword store is unreached: it is reached, by a
 *   dominating-block int local.
 *
 * MEASURED (rom 137 lines), the ordering sweep that isolates the mechanism:
 *   sentinel assigned inside the arm, order p/kind/m/style     27 aligned
 *   the same, sentinel inside the guarded body                 27
 *   sentinel hoisted to the function's first statement          9
 *   ... and then, with the sentinel hoisted, by assignment order:
 *     p, kind, m, style                                         9
 *     p, m, kind, style                                         9
 *     p, m, style, kind                                         6
 *     p, style, m, kind                                         6
 *     p, kind, style, m                                        22
 *     p, style, kind, m                                        22
 * FLAGS on the best form: -fno-schedule-insns 6, --no-sched2 10, -O1 20,
 *   --no-rerun-cse 9, -fno-cse-follow-jumps 9.
 *
 * WHAT IS RIGHT: everything else. The case-body order in the source is the
 * emission order (1,7,11,4,5,14,6,3,12,13,9,2,8,10,15,16); the epilogue's
 * `pop {r0}` confirmed `void`; and three of the callees take NO arguments --
 * declared `extern int f();` and called bare, because the ROM sets up no
 * argument registers for them.
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
