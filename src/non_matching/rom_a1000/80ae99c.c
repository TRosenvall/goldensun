/* Func_80ae99c -- 0x080ae99c  (asm/rom_a1000/rom_ae88c_c_c_c.s)
 *
 * BLOCKER: gcc IF-CONVERTS a two-arm constant selection that the ROM keeps as
 * a real branch, and the parameter saves then rotate. 39 of 40, two lines short.
 *
 *     rom   cmp r5,#0 / bne L0 / ldr r1,=0x392 / b L1
 *           L0: mov r1,#0xe5 / lsl r1,#2 / L1: add r3, r1
 *     ours  ldr r1,=0x392 / cmp r3,#0 / beq L0
 *           mov r1,#0xe5 / lsl r1,#2 / L0:
 *
 * We hoist the pooled 0x392 above the compare and conditionally overwrite it,
 * which is two instructions shorter and loses the join. Everything downstream
 * shifts, which is why the count is 39 rather than the handful of real
 * disagreements.
 *
 * MEASURED, both inert at 39:
 *   writing the arms so the ROM's fall-through (0x392) is the `if` body
 *   writing them as explicit `goto other; ... goto join;` around the two
 *     assignments, which is literally the ROM's control flow
 *
 * So the if-conversion is not reachable by control-flow spelling here. That is
 * worth recording against the block-layout reading, which is usually reliable:
 * it tells you which arm gcc will make the fall-through WHEN IT EMITS A BRANCH,
 * and says nothing when gcc decides not to emit one at all. Two constant
 * assignments with no other work in either arm is the shape where it declines.
 *
 * The remaining question is what makes the ROM's compiler keep the branch. One
 * asymmetry is visible and untested: the two constants have different COSTS --
 * 0x392 is a pool load and 0xe5 << 2 is a mov/lsl pair -- so the arms are not
 * interchangeable to a cost model, and forcing both to the same form (or making
 * one more expensive) might tip it. Not tried.
 */
extern int iwram_3001f2c;
extern void *_Func_801eadc(int id, int a, int b, int c, int d);

int Func_80ae99c(int a, int b, int c, int d)
{
    char *g;
    int off;
    unsigned char *r;

    g = (char *)iwram_3001f2c;
    if (d == 0)
        off = 0x392;
    else
        off = 0xe5 << 2;
    r = (unsigned char *)_Func_801eadc(*(unsigned short *)(g + off),
                                       0x80 << 23, a, b, c);
    if (r == 0)
        return -1;
    r[4] = 0;
    *(unsigned short *)(r + 0xc) = 0;
    r[5] = 1;
    return 1;
}
