/* Func_80f6148 (0x080f6148) -- NON-MATCHING, 20 encodings of 76.
 * Blocker class: register allocation ORDER -- a three-register permutation.
 *
 * Darkens two palette regions by one step per channel. Re-screened in batch 276
 * with the (u16) cast lever; RESIDUE 1 IS NOW SOLVED and the park's old
 * conclusion, "NEXT: nothing source-level", IS REFUTED. Encodings went from
 * 74/76 to 76 vs the ROM's 76, both mid-function pool skips now appear, and no
 * _CONST_1f symbol is needed. The function now lives alone in
 * asm/rom_f6000/rom_f6008_c_a_c.s after batch 275's split, so it needs no split
 * to land if the last 20 ever close.
 *
 * WHAT SOLVED RESIDUE 1 -- A (u16) CAST BEFORE THE MASK. `(u16)(t >> 21) & 0x1f`
 * makes the mask a HImode pool entry (*thumb_movhi_insn, pool_range 64), which
 * forces the pool into the middle of the function and produces the ROM's
 * `b`-over-pool at zero extra instructions. Validated in the same round on the
 * file-mate Func_80f61e8 (src/rom_f6000/rom_f6008_c_a_d.c).
 *
 * AND TWO ARTEFACTS ARE RETIRED WITH IT. docs/elevation.md called the
 * `(int)&_CONST_1f` reading "a convincing false lead", and const.sym's own
 * _CONST_1f entry rejected `unsigned short t; (t >> 5) & 0x1f` because it PRINTS
 * `ldrh`. Thumb-1 has no PC-relative `ldrh`, so gas assembles that to the
 * identical halfword -- the objection was reading a disassembly artefact, not a
 * difference. Anyone re-reading those two notes should read this one with them.
 *
 * TWO MORE SOURCE LEVERS LANDED, taking 58 to 20:
 *
 *   `b = 0x1f; b &= c;` AS TWO STATEMENTS, masking into the DESTINATION. This
 *   makes the mask pseudo the same pseudo as `b`, so set_in_loop > 1 and
 *   loop-invariant motion can no longer hoist it -- which is what gives the
 *   ROM's in-loop `mov r4, #0x1f / and r4, r2`. 58 -> 56.
 *
 *   `t = c << 16;` AS A NAMED VARIABLE, with `b` computed BETWEEN `r` and `g`.
 *   56 -> 34 -> 20, and with it the prologue `push {r5,r6,r7,lr}` and all three
 *   loop-carried registers (r5 = p, r6 = i, r7 = the pooled mask).
 *
 * RESIDUE 2 IS WHAT IS LEFT, and it is now small and precisely stated: 20
 * encodings, 10 per loop, a THREE-REGISTER PERMUTATION inside about 14
 * instructions. Ours reuses r3 for both channel extractions and spells
 * `sub r0, r3, #1` three-operand; the ROM keeps `t` in r3, `r` in r0, `g` in r1
 * and decrements all three in place. Same instruction count, same eight hard
 * registers, same peak liveness. The REG_ALLOC_ORDER reading still stands for
 * this part -- but it is no longer "every one of the 71 differing lines".
 *
 * MEASURED, about 55 spellings, all plateauing at 20 (rom 76 encodings):
 *   6 channel orders                        20 / 22 / 26 / 32 / 34 / 40 / 56
 *   6 clamp orders                          20 / 24 / 28 / 32 / 34 / 34
 *   `c` as u32 / int / u16                  20 / 20 / 26
 *   `t` as u32 / int                        20 / 20
 *   inline vs separated decrements          inline required; full separation
 *                                           costs the prologue, 54-56
 *   `b--` / `b -= 1` / `b = b - 1`          all 20
 *   store `((r<<10)|(g<<5))|b`               20
 *   store `b|(g<<5)|(r<<10)`                 44
 *   -fno-schedule-insns2                     63
 *   -fno-rerun-cse-after-loop                62
 *
 * NEXT: the three-register permutation, and it wants allocno_compare / find_reg
 * read against `.18.greg` rather than more spellings -- 55 of them are now on
 * file and the plateau is flat. Belongs with the other global_alloc parks
 * (Func_80a8578, Func_80cd52c, Func_80919d8, Func_80a6794, Func_808b090).
 */
#include "gba/types.h"

void Func_80f6148(void)
{
	u16 *p;
	u32 c;
	u32 t;
	int r;
	int g;
	int b;
	int i;

	p = (u16 *)0x5000140;
	i = 0;
	do {
		c = *p;
		t = c << 16;
		r = ((u16)(t >> 26) & 0x1f) - 1;
		b = 0x1f;
		b &= c;
		g = ((u16)(t >> 21) & 0x1f) - 1;
		b -= 1;
		if (r < 0)
			r = 0;
		if (g < 0)
			g = 0;
		if (b < 0)
			b = 0;
		i++;
		*p = (r << 10) | (g << 5) | b;
		p++;
	} while (i != 0x10);
	p = (u16 *)0x5000202;
	i = 0;
	do {
		c = *p;
		t = c << 16;
		r = ((u16)(t >> 26) & 0x1f) - 1;
		b = 0x1f;
		b &= c;
		g = ((u16)(t >> 21) & 0x1f) - 1;
		b -= 1;
		if (r < 0)
			r = 0;
		if (g < 0)
			g = 0;
		if (b < 0)
			b = 0;
		i++;
		*p = (r << 10) | (g << 5) | b;
		p++;
	} while (i != 0xef);
}
