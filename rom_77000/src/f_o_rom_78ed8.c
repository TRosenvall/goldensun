/* Func_78ed8 -- record lookup by id
 *
 * STATUS: 2 bytes off under gcc-2.96. The instruction sequence is right;
 * the ROM loads the table address into r2, gcc-2.96 picks r3 -- the register
 * it just used for the 0xB4 multiplier. Four C formulations were tried
 * (pointer arithmetic, array indexing, an explicit temporary, an integer
 * cast) and all four choose r3, so the original source said something
 * structurally different that kept r3 live. Not yet found.
 */
extern char L844ec[];

void *Func_78ed8(int id)
{
    return L844ec + id * 0xB4;
}
