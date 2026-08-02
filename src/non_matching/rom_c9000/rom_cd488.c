/* Func_80cd488  [rom_c9000]
 * Source asm: goldensun/asm/rom_c9000/rom_cd260.s
 *
 * Parked: logic faithful, does NOT byte-match (endgame permuter seed).
 * Candidate: tools/runs/run_20260607T010203Z/Func_80cd488-iter-3.c
 * TODO(residual): REG_BG2X then BG2Y shared-base reuse (ldr=REG_BG2X; str; add r1,
 */
extern unsigned char *iwram_3001eec;

/* Takes no arguments. Copies the cached affine reference point from
 * [iwram_1eec]+0x77D0 and +0x77D4 into REG_BG2X and REG_BG2Y.
 */
unsigned int Func_80cd488(void)
{
	unsigned int base = *(unsigned int *)&iwram_3001eec;
	volatile unsigned int *reg = (volatile unsigned int *)0x04000028;
	*reg = *(unsigned int *)(base + 0x77d0);
	reg = (volatile unsigned int *)((char *)reg + 4);
	*reg = *(unsigned int *)(base + 0x77d4);
	return 0;
}
