/* GiveDjinni -- 0x0807a1b4, from
 * goldensun/asm/rom_77000/rom_79460_c_c_c_c_a_c_c_c_a_c_a.s.
 *
 * Grants one Djinni: refuse if this element's count is already 10, refuse if
 * the bit is already set, otherwise bump the count and set the bit.
 *
 * Preserves the original ROM layout when slotted between
 * ..._a_c_a_a.o and ..._a_c_a_c.o in goldensun/stage1.ld.
 *
 * THE PARK SAID THE COPY WAS NOT REACHABLE AND ALL FOUR OF ITS EXPERIMENTS
 * ADDED A NAME. The ROM's `ldrb r4, [r0, r6] / mov r3, r4 / cmp r3, #9` is one
 * load feeding TWO source reads -- the comparison's and the increment's.
 * Deleting the named count local and writing `if (u[off] > 9) ...; u[off]++;`
 * restores it. The fix was FEWER names, not a differently-typed extra one.
 *
 * TWO CORRECTIONS TO THE PARK'S SUB-FINDINGS, both worth keeping:
 *
 *   ITS `unsigned`/`bhi` RULE IS OVER-STATED. With the local gone, a plain
 *   `if (u[off] > 9)` emits `bhi` by itself -- gcc knows an `ldrb` result is
 *   0..255 and picks the unsigned branch. The signed `bgt` the park saw came
 *   from routing the value through a named SIGNED local, which erases the
 *   range. "A ROM `bhi` after an `ldrb` tells you the local's type" holds only
 *   when there is a local.
 *
 *   `goto fail` IS A NEGATIVE HERE. With two plain `return -1` statements gcc
 *   already cross-jumps them into the ROM's shared block and places it BEFORE
 *   the success tail; an explicit `goto` to a trailing label pins the failure
 *   block after the success path and inverts the two. The recorded
 *   shared-exit lever is for when our output is MISSING the shared block, not
 *   when it already has it. 12 differing.
 *
 * MEASURED (rom 35 lines):
 *   the park, `unsigned int n = u[off]` carried across      34 lines, 27 differing
 *   no local, `u[off] > 9`, `u[off]++`                      35, MATCH
 *   the same written `u[off] = u[off] + 1`                  35, MATCH
 *   the same with `> 9U`, or `>= 10U`, or unsigned offsets  35, MATCH each
 *   two `goto fail` into a shared tail                      35, 12
 *   the named local kept for the compare, `u[off]++` for
 *     the store                                             34, 27
 *
 * That last row isolates it: as long as ANY named local carries the loaded
 * value gcc coalesces and the copy disappears. It is the read count in the
 * source, not the form of the write.
 */
extern void *GetUnit(int id);

int GiveDjinni(int id, int elem, int bit)
{
	unsigned char *u;
	int off;
	int idx;
	int m;

	u = (unsigned char *)GetUnit(id);
	off = elem + (0x8c << 1);
	if (u[off] > 9)
		return -1;
	idx = (elem << 2) + 0xf8;
	m = 1 << bit;
	if ((*(int *)(u + idx) & m) != 0)
		return -1;
	u[off]++;
	*(int *)(u + idx) |= m;
	return 0;
}
