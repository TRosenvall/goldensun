/* Cluster Func_8021360..Func_8021360 extracted from goldensun/asm/rom_15000/rom_20198_c_c_c_a_a_a.s.
 *
 * Slotted between rom_20198_c_c_c_a_a_a_a.o and the rest of stage1.ld.
 *
 * Picks a halfword from one of two tables by index, choosing the table on a
 * flag, and returns 0 for an out-of-range index.
 *
 * THE RANGE CHECK IS UNSIGNED -- the ROM uses `bhi`, so the index is unsigned
 * and a negative value is caught by the same test.
 *
 * THE FLAG TEST IS WRITTEN NEGATED, and that is the whole of the difference
 * between 1 of 21 and a match. The ROM's `bne` sends the FLAG-SET case to the
 * second table and falls through to the first, so the source reads
 * `if (!flag) return first; return second;`. Written the natural way round --
 * `if (flag) return second; return first;` -- gcc emits `beq` and the two arms
 * swap. The ROM always says which arm falls through.
 */
extern int _GetFlag(int id);
extern short L37206[] __asm__(".L37206");
extern short L37216[] __asm__(".L37216");

int Func_8021360(unsigned int i)
{
    if (i > 8)
        return 0;
    if (!_GetFlag(0x20))
        return L37206[i];
    return L37216[i];
}
