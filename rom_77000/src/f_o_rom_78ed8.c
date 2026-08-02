/* Func_78ed8 -- address of record `id` in the table at L844ec
 *
 * Records are 0xB4 bytes.
 *
 * The local `p` is load-bearing, not decoration. Written as
 * `return L844ec + id * 0xB4;` the compiler reuses r3 -- the register holding
 * the multiplier -- for the table address, where the ROM uses r2. Binding the
 * table to a local first keeps r3 live across the multiply and pushes the
 * address to r2, which is what the original did. Four other formulations
 * (array indexing, a struct-array type, a named multiplier, reversed operand
 * order) all produce r3.
 */
extern char L844ec[];

void *Func_78ed8(int id)
{
    char *p = L844ec;

    return p + id * 0xB4;
}
