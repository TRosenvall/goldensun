/* Task_Thunder -- asm/rom_8a000/rom_944ec_a_a_a_a_c_c_a.s (7 functions).
 *
 * NOT MATCHING: 18 differing of 117 encodings, LENGTH IDENTICAL. Candidate below.
 * INDICES 0-94 ARE EXACT; the whole residue is one pool load's position.
 *
 *   rom arm-1 tail   ldr r3, =0x2a01 / add r2, r7, r3 / mov r3, #0xc / b L7
 *   rom merged tail  strb r3, [r2] / ldr r2, =0x2a02 / mov r1, #0 / add r3, r7, r2 / strb r1, [r3]
 *
 * Ours hoists `ldr r1, =0x2a02` into BOTH arms, so cross-jumping (.25.jump2, which runs AFTER
 * .23.sched2) merges four insns instead of five. The second Func_8091200 relocation therefore
 * sits at 0xe8 against the reference's 0xe6.
 *
 * THREE LEVERS DID LAND, and the first two are worth keeping:
 *
 * 1. `int u = *p` ON AN `unsigned short *`, THEN `t = (short)u`, is the only spelling giving the
 *    ROM's `ldrh / sub #1 / lsl / asr / strh`. A `short *p` with `t = *p` gives `ldrsh` and is one
 *    instruction SHORT (91); `unsigned short u` gives `ldr =0xffff / lsl / lsr / add / asr` (99);
 *    `unsigned int u` gives `add rX, 0xffff` (92).
 * 2. HOISTING THE VALUE OUT OF A HALFWORD STORE fixed the whole Random block: writing the store as
 *    one expression schedules the address ahead of the multiply chain, where
 *    `sum = ...; w = (short *)(b + (0xfc << 5)); *w = sum;` does not. 31 to 23.
 * 3. `c = 0x80; *p = c;` is required -- the bare `*p = 0x80` pools, the HImode-store class, same
 *    as StartRain's `c1` in this same batch.
 *
 * STORE-TAIL SHAPES MEASURED: duplicated stores in both arms 18; a `goto` with a shared `q` and
 * `val` 23 (cross-jumping swallows the `add` into the tail); DISTINCT `q1`/`q2` 16 but 116 lines
 * and WRONG -- gcc notices `0x2a01 & 0xff == 1` and stores the LOW BYTE OF THE ADDRESS CONSTANT
 * instead of `mov r3, #1`, which is worth knowing as a hazard in its own right; a shared `val`
 * with a per-arm store 23; a named zero for the second store 24.
 *
 * AND A CORRECTION THIS FUNCTION PRODUCED, recorded in docs/elevation.md: `synth_mult` is NOT
 * capped at three operations. Probed directly here -- `x * 400` synthesises as the ROM's exact
 * five-operation `lsl/add/lsl/add/lsl`, and `x * 100` becomes `mov #100 / mul`. Plain `* 400` and
 * `* 100` are correct and NO composite spelling is needed. `lsr` rather than `asr` after both pins
 * the operands as unsigned.
 */
#include "dma.h"

extern unsigned char *iwram_3001ec8;
extern int _GetFlag(int id);
extern unsigned int Random(void);
extern void _PlaySound(int id);
extern void Func_8091200(void *p, int n);

void Task_Thunder(void)
{
	unsigned char *b;
	unsigned char *y;
	unsigned short *p;
	int u;
	int t;
	unsigned int ra;
	unsigned int rb;
	int c;
	int sum;
	short *w;

	b = iwram_3001ec8;
	p = (unsigned short *)(b + (0xfc << 5));
	y = (&iwram_3001ec8)[2];
	if (*(short *)p < 0)
		return;
	if (_GetFlag(0xb3 << 1)) {
		c = 0x80;
		*p = c;
	}
	u = *p;
	*p = u - 1;
	t = (short)u;
	switch (t) {
	case 0:
		if (*(short *)(b + 0x1f82) != 0) {
			ra = Random();
			rb = Random();
			sum = ((ra * 400) >> 16) - ((rb * 100) >> 16) + 0x96;
			w = (short *)(b + (0xfc << 5));
			*w = sum;
			if (*(short *)(b + 0x1f84) != 0)
				_PlaySound(0xac);
			else
				_PlaySound(0xab);
		}
	case 5:
	case 10:
		Func_8091200(b, 1);
		DMA3_COPY(b + (0xa8 << 5), y + (0xc4 << 5), 0xa80);
		y[0x2a01] = 0xc;
		y[0x2a02] = 0;
		break;
	case 1:
	case 6:
	case 11:
		Func_8091200(b + (0xa8 << 4), 1);
		y[0x2a01] = 1;
		y[0x2a02] = 0;
		break;
	}
}
