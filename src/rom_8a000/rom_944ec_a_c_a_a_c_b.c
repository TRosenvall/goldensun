/* Cluster Func_8095b8c..Func_8095b8c extracted from goldensun/asm/rom_8a000/rom_944ec_a_c_a_a_c.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/rom_8a000/rom_944ec_a_c_a_a_c_a.o and asm/rom_8a000/rom_944ec_a_c_a_a_c_c.o
 * in goldensun/stage1.ld.
 *
 * Returns an entry from .L9f0a4 selected by bit 2 of iwram_1800 -- which of two
 * ride variants is configured -- and writes it to two fields.
 *
 * UNPARKED, on one instruction, by NAMING THE TABLE POINTER.
 *
 * The whole difference was the operand order of a register-offset load:
 *
 *     rom    ldr r3, [r3, r1]     index is the base, table is the offset
 *     ours   ldr r3, [r1, r3]     table is the base, index is the offset
 *
 * The park carried a suggestion -- write the index as the pointer base -- and a
 * note that it had been tried and made things WORSE, three differing positions
 * instead of one, along with two other restructurings that also gave three.
 * All of them moved the whole expression.
 *
 * What works is smaller than any of them: assign the table to a local
 * `unsigned int *t` and index THAT. The expression is otherwise unchanged; only
 * which operand gcc treats as the base moves.
 *
 * This is the same family as the named-intermediate lever -- naming something
 * changes what gcc folds into what -- but the target is a load's base/offset
 * roles rather than an address computation. Worth trying on any lone
 * `ldr rD, [rA, rB]` whose two registers are the right values in the wrong
 * order.
 */
extern unsigned int iwram_3001800;
extern unsigned int L9f0a4[] __asm__(".L9f0a4");

void Func_8095b8c(void *p)
{
    unsigned int *t = L9f0a4;
    unsigned int v = t[(iwram_3001800 >> 2) & 1];

    *(unsigned int *)((char *)p + 0x18) = v;
    *(unsigned int *)((char *)p + 0x1c) = v;
}
