/* Func_8092ba8 -- 0x08092ba8, the whole of
 * goldensun/asm/rom_8a000/rom_92950_c_a_c_a_a.s, so no split was needed and the
 * linker script is unchanged.
 *
 * Returns the current animation frame of a map actor's sprite, or -1 when the
 * slot is empty or is not a plain actor. The slot index is masked to twelve
 * bits and scaled into the map state's actor pointer array at +0x14.
 *
 * TWO LEVERS, both about where a value has to be alive rather than how it is
 * spelled:
 *
 *   THE OFFSET BELONGS IN ONE REGISTER. Written
 *   `*(u8 **)(base + (((id & 0xfff) << 2) + 0x14))` gcc adds the scaled index
 *   to the base and then uses an immediate-offset load -- `add r2, r3 /
 *   ldr r2, [r2, #0x14]` -- where the ROM folds the 0x14 into the index and
 *   uses a register offset: `add r3, #0x14 / ldr r2, [r2, r3]`. Naming the
 *   offset in an int local produces the ROM's form. 8 differing to 6.
 *
 *   THE SENTINEL IS ASSIGNED BEFORE THE BASE, NOT BESIDE ITS USE. `v = -1`
 *   written after `base = iwram_3001ebc` lets gcc keep the result in r0 for
 *   the whole body and return it directly, which is one instruction SHORT of
 *   the ROM's `mov r0, r1` at the join. Moving the assignment to the top of the
 *   function overlaps its live range with the base pointer's, so the two cannot
 *   share r0 and the copy reappears.
 *
 * MEASURED (rom 25 lines):
 *   offset inline, sentinel beside its use        24 lines, 8 differing
 *   offset named                                  24, 6
 *   offset named, sentinel hoisted to the top     25, MATCH
 *   sentinel renamed and declared first instead   24, 6  (declaration order
 *                                                        alone does not do it)
 */
extern unsigned char *iwram_3001ebc;

int Func_8092ba8(int id)
{
	unsigned char *base;
	int off;
	unsigned char *a;
	unsigned char *k;
	unsigned char *s;
	int v;

	v = -1;
	base = iwram_3001ebc;
	off = ((id & 0xfff) << 2) + 0x14;
	a = *(unsigned char **)(base + off);
	if (a != 0) {
		k = a;
		k += 0x54;
		if (*k == 1) {
			s = *(unsigned char **)(a + 0x50);
			s = *(unsigned char **)(s + 0x28);
			v = *(short *)s;
		}
	}
	return v;
}
