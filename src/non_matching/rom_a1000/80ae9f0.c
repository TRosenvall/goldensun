/* Func_80ae9f0 -- asm/rom_a1000/rom_ae88c_c_c_c.s
 *
 * BLOCKER: REGISTER ROTATION on the parameters. 23 of 44, one line long.
 *
 * Picks a halfword from one of two offsets in the block at iwram_3001f2c
 * depending on the fourth argument, adjusts the third, calls _Func_801eadc
 * with five arguments (the fifth on the stack), and on success clears two
 * fields and sets a flag. Both arms, both offsets, the stack argument, the
 * failure return of -1 and the success path all reproduce.
 *
 * The residue is which register holds which parameter:
 *
 *     rom    d->r5  a->r4  c stays in r2 and is modified IN PLACE (`sub r2,#3`)
 *     ours   c->r4  a->r5  c copied out of r2 first
 *
 * and it propagates: the ROM's base advance is `add r3, r1` (in place,
 * clobbering the base) where ours is the three-operand `add r3, r2, r1`,
 * because our named `base` stays live across both arms.
 *
 * MEASURED:
 *   base addressed as `base + 0x392` in each arm       45 lines, 23 differ
 *   base ADVANCED in place, `base += 0x392;`           45 lines, 29 differ
 *
 * The second is the informative negative. The three-operand add looked like the
 * tell -- the ROM clobbers its base, so writing an in-place advance should
 * match -- and it makes the function SIX WORSE. Advancing the pointer keeps it
 * live longer in our version, not shorter, because the load then depends on the
 * advanced value rather than on a base-plus-offset the scheduler can fold.
 *
 * So the in-place add is a CONSEQUENCE of the ROM's register assignment, not a
 * cause of it -- the same relationship recorded on Func_8079664, where forcing
 * the ROM's runtime gState offset made that function six worse for the same
 * reason. Two instances now: when an addressing-mode difference sits downstream
 * of a register rotation, spelling the addressing mode does not fix the
 * rotation and usually costs lines.
 */
extern int iwram_3001f2c;
extern char *_Func_801eadc(int id, int flags, int a, int b, int c);

int Func_80ae9f0(int a, int b, int c, int d)
{
    char *base;
    char *p;
    int id;

    base = (char *)iwram_3001f2c;
    if (d == 0) {
        id = *(unsigned short *)(base + 0x392);
        c -= 3;
    } else {
        id = *(unsigned short *)(base + 0xe5 * 4);
        c -= 4;
    }
    p = _Func_801eadc(id, 0x80 << 23, a, b, c);
    if (p == 0)
        return -1;
    p[4] = 0;
    *(unsigned short *)(p + 0xc) = 0;
    p[5] = 1;
    return 1;
}
