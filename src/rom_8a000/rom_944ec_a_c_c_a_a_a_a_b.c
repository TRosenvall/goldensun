/* FieldMove_Target  --  0x08096960
 *
 * The second half of goldensun/asm/rom_8a000/rom_944ec_a_c_c_a_a_a_a.s; the
 * first, FieldMove_NoTarget, stays in _a.s and is parked at
 * src/non_matching/rom_8a000/8096810.c.
 *
 * THE JOIN-SPLIT LEVER HAS A SWITCH-ARM FORM, and it is this whole function.
 * The repeated expensive value is `gState + 0x24a`, used in case 2 and case 9 --
 * MUTUALLY EXCLUSIVE ARMS, not two sides of a straight-line join. Three
 * spellings are all distinguishable in the output:
 *
 *   folded inline      one pool word `=gState+586`; the ROM has TWO pool words
 *                      and an add -- 11 aligned
 *   one shared local   live range spans the switch, allocno priority drops, and
 *                      the diff moves into the prologue -- 13 aligned
 *   one local PER ARM  defeats the fold AND keeps the range short -- exact
 *
 * Note the cost side runs backwards from the usual dominance rule: here naming
 * the value ADDS an instruction pair, and that pair is what the ROM has.
 *
 * THE SIBLING'S WALL IS NOT PRESENT HERE, and the reason is worth recording
 * against that park. FieldMove_NoTarget is blocked on a
 * floor_log2(refs)*refs/live_length tie between its kind variable and the
 * gState slot pointer that no statement order resolves. Here the slot pointer
 * lives entirely inside one switch arm and never coexists with `kind` in the
 * entry block, so there is no tie: the ROM's prologue assignment falls out of
 * the plain declaration order with the `p + 0x20` store written last, and it
 * matched on the first candidate. The sibling's blocker is therefore caused by
 * gState ALSO being read at 0x1f4 in the same arm, not by the switch shape the
 * two functions share.
 *
 * Two levers imported from that park both fired, which is a second independent
 * confirmation of each: the negative-offset global spelling for
 * `iwram_3001f30 - 0x74`, and an `int` local assigned in a DOMINATING BLOCK to
 * reach the `ldr rN, =0xffff / strh` shape -- hoisting `inval` to the first
 * statement is worth 8 aligned regions.
 *
 * The read-count rule holds too: case 2 re-reads `*(short *)(p + 0x1a)` for its
 * comparison and passes the named `style` to the call. Collapsing those into
 * one name costs 18 aligned. The source case order is the emission order.
 */
extern char *iwram_3001f30;
extern unsigned char gState[];

extern void Field_Move_Target(int style);
extern void Field_Lift_Target(int style);
extern void Field_Carry_Target(int style);
extern void Field_Force_Target(int style);
extern void Field_Douse_Target(int style);
extern void Field_Whirlwind_Target(int style);
extern void Field_Frost_Target(int style);
extern void Field_Ply_Target(int style);
extern void Field_Growth_Target(int style);
extern void Field_Catch_Target(int style);
extern void Field_Halt_Target(int style);
extern void Field_Reveal(void);
extern void Field_Cloak(void);
extern void Field_Retreat(void);
extern void Field_Avoid(void);
extern void Field_MindRead(int id, int style);
extern void Func_809ade8(int id);
extern void Func_809ad90(int style);
extern void Func_80984c0(void);

void FieldMove_Target(void)
{
    char *p;
    char *m;
    int kind;
    int style;
    int inval;

    inval = 0xffff;
    p = iwram_3001f30;
    kind = *(short *)(p + 0x1e);
    m = *(char **)((unsigned char *)&iwram_3001f30 - 0x74);
    style = *(short *)(p + 0x1a);
    *(char *)(p + 0x20) = 0;
    switch (kind) {
    case 2:
        {
            unsigned char *g;

            if (*(short *)(m + 0xcb8) != 0)
                Func_80984c0();
            g = gState;
            if (*(short *)(g + 0x24a) != *(short *)(p + 0x1a))
                *(char *)(*(int *)(p + 0x14) + 0x5b) = 1;
            Field_MindRead(*(short *)(p + 0x18), style);
        }
        break;
    case 1:
        Field_Move_Target(style);
        break;
    case 7:
        Field_Lift_Target(style);
        break;
    case 11:
        Field_Carry_Target(style);
        break;
    case 4:
        Field_Force_Target(style);
        break;
    case 5:
        Field_Douse_Target(style);
        break;
    case 6:
        Field_Frost_Target(style);
        break;
    case 12:
        Field_Growth_Target(style);
        break;
    case 9:
        {
            unsigned char *g;
            short *slot;
            int v;

            g = gState;
            slot = (short *)(g + 0x24a);
            v = *slot;
            if (v != -1) {
                Func_809ade8(v);
                *slot = inval;
            }
            Func_809ad90(style);
            *slot = style;
            Field_Halt_Target(style);
        }
        break;
    case 3:
        Field_Ply_Target(style);
        break;
    case 14:
        Field_Whirlwind_Target(style);
        break;
    case 13:
        Field_Catch_Target(style);
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
