/* Cluster Func_80cd488..Func_80cd488 extracted from goldensun/asm/rom_c9000/rom_cd260.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/rom_c9000/rom_cd260_a.o and asm/rom_c9000/rom_cd260_c.o in
 * goldensun/stage1.ld.
 *
 * Copies the cached affine reference point from [iwram_3001eec]+0x77d0 and
 * +0x77d4 into REG_BG2X and REG_BG2Y.
 *
 * UNPARKED BY TWO THINGS, NEITHER OF WHICH IS THE ONE THE PARK NAMED. The park
 * blamed a `stmia r1!, {r3}` that gcc emits where the ROM has `str` then a
 * separate `add`, and recorded three formulations of the volatile pointer walk
 * that did not move it.
 *
 *   1. -fno-rerun-cse-after-loop. Found by tools/rank_parks.py --flags, which
 *      screens every park under every per-file build setting the tree uses.
 *      This takes it from two differing instructions to one.
 *   2. THE RETURN TYPE IS `void`, NOT `unsigned int`. The last instruction out
 *      was a `mov r0, #0` that the ROM does not have, because the ROM never
 *      sets r0 at all -- the function returns nothing. The park had it as
 *      `unsigned int ... return 0;`, which is a plausible guess that costs
 *      exactly one instruction and reads like codegen noise.
 *
 * Worth remembering as a shape: a lone trailing `mov r0, #imm` that the ROM
 * lacks is a RETURN TYPE question, not a scheduling one.
 */
extern unsigned char *iwram_3001eec;

/* Takes no arguments. Copies the cached affine reference point from
 * [iwram_1eec]+0x77D0 and +0x77D4 into REG_BG2X and REG_BG2Y.
 */
void Func_80cd488(void)
{
	unsigned int base = *(unsigned int *)&iwram_3001eec;
	volatile unsigned int *reg = (volatile unsigned int *)0x04000028;
	*reg = *(unsigned int *)(base + 0x77d0);
	reg = (volatile unsigned int *)((char *)reg + 4);
	*reg = *(unsigned int *)(base + 0x77d4);
}
