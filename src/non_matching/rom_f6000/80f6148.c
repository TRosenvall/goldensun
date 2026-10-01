/* Func_80f6148 (0x080f6148) -- STILL PARKED at 20 of 76, BUT THE BLOCKER CLASS
 * WAS WRONG AND THE RESIDUE IS NOW A DIFFERENT, SMALLER, NAMED THING.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_f6000/80f6148.c \
 *     asm/rom_f6000/rom_f6008_c_a_c.s --func Func_80f6148
 * Re-measured in batch 316c: 20 of 76, first difference at index 5, size and
 * relocations identical.  The park's figure reproduces from the body on disk.
 *
 * *** THE PARK CALLED THIS "register allocation ORDER -- a three-register
 * permutation" AND SAID IT "WANTS allocno_compare / find_reg READ AGAINST
 * .18.greg RATHER THAN MORE SPELLINGS".  IT IS NOT global_alloc's ORDER. ***
 *
 * It is LOCAL-ALLOC's destination/dying-source COMBINE, and it is visible by
 * eye in the two `subs` forms:
 *     rom    lsrs r0, r3, #26 / ands r0, r7 / ... / subs r0, #1
 *     ours   lsrs r3, r1, #26 / ands r3, r7 / ... / subs r0, r3, #1
 * The ROM carries each channel in ONE register from extraction through mask
 * through decrement -- a TWO-operand `subs rN, #1`.  We compute the mask into a
 * shared temporary and decrement into a second register -- a THREE-operand
 * `subs rD, rN, #1`.  The register names are a CONSEQUENCE of that, not the
 * cause: the shared temp is why `t` is squeezed out of r3 and why the second
 * channel reuses the first channel's temp.
 *
 * AND THE IN-PLACE FORM IS SOURCE-REACHABLE.  Splitting the decrement off,
 * `r = (u16)(t >> 26) & 0x1f;` then `r -= 1;`, produces exactly the ROM's
 * two-operand `sub`.  The park filed this as "inline required; full separation
 * costs the prologue, 54-56" and that is right about the cost but wrong about
 * why: separation does not add pressure, it REMOVES it.  With single-pseudo
 * channel chains gcc needs one fewer callee-saved register and emits
 *     push {r5, r6, lr} / ldr r4, .L17+4 / ldrh r6, .L17 / mov r5, #0
 * against the ROM's `push {r5, r6, r7, lr}` with p/i/mask in r5/r6/r7 and the
 * blue channel in r4.  The ROM's peak liveness is EIGHT registers; the
 * separated form's is seven.  So the function needs the in-place chains AND the
 * ROM's eight-register pressure at the same time, and no spelling found here
 * delivers both.
 *
 * THAT IS THE NEW, PRECISE STATEMENT OF THE RESIDUE.  Not "allocation order":
 * "the in-place chain and the eighth live register are in conflict".
 *
 * MEASURED IN BATCH 316c (about 70 more variants on top of the park's 55):
 *
 *  SEPARATED DECREMENTS -- every placement, 54 to 74:
 *    r and g separated, b block before / between / after        56 / 56 / 56
 *    decrement order b,r,g and r,g,b                            56 / 56
 *    only r separated / only g separated                        54 / 56
 *    `r--` instead of `r -= 1`                                  56
 *    mask as its own statement (`r = (u16)(t>>21); r &= 0x1f;`)  74, -16 bytes
 *
 *  REGISTER PINS -- all 25 subsets of {t=r3, r=r0, g=r1, c=r2, b=r4} of size
 *  1-3, then every superset containing t,g,c up to all seven loop values:
 *    pin t,g,c                                   *** 18 *** (first diff at 6)
 *    pin t,r,g  /  pin t,r,g,c  /  +b,p,i         20 (first diff at 8)
 *    pin t alone / t,b                            26
 *    pin g, r, t+g, t+r, t+c, g+c ... (all else)  40-54
 *  So a THREE-PIN variant beats the park by two encodings.  It is recorded, not
 *  shipped: three fakematch-class pins for two encodings is a bad trade and it
 *  is still not a landing.  It is in scratch_elev/b316c/v_p4d/t_tgc.c.
 *  WHAT THE PINS TEACH is worth more than the two encodings.  `pin t,r,g` makes
 *  indices 5-7 and 12 exact, including the ROM's `subs r0, #1` -- so the r
 *  channel's in-place chain IS obtainable -- but it then breaks the store, which
 *  the base already had right: pinning r to r0 lets the `(r << 10)` accumulator
 *  combine with r and come out `lsls r0, r0, #10` against the ROM's
 *  `lsls r3, r0, #10`.  `pin t,g,c` keeps the store and loses the g chain.
 *  The two halves of the residue are reachable one at a time and not together.
 *
 *  STORE SPELLINGS, crossed against six pin sets -- COMPLETELY INERT, all 30
 *  combinations reproduce their pin set's figure exactly:
 *    `((r<<10)|(g<<5))|b`,  `(r<<10)|((g<<5)|b)`,  a named `int v` for the
 *    word, and three separate `v |=` statements.
 *
 *  LOOP-CARRIED PINS ON THE SEPARATED-DECREMENT BRANCH, to buy the eighth
 *  register back: pin p=r5,i=r6 is 44; +b=r4 is 36; +t=r3 is 32; +c is 32;
 *  adding r and g makes it 69-71 and EIGHT BYTES LONGER.  Best on that branch
 *  is 32, so the branch is dead -- the push list does not come back by pinning.
 *
 * NEXT, and it is now a specific question rather than "read the allocator":
 * find a spelling that keeps each channel as ONE pseudo through extract/mask/
 * decrement WITHOUT lowering peak liveness below eight.  Everything that makes
 * the chains single-pseudo also frees a register.  The park's own list of ~55
 * spellings plus the ~70 here is the evidence that it is not an easy spelling.
 *
 * -- scratch_elev/b316c/v_p4, v_p4b, v_p4c, v_p4d, v_p4e, v_p4f, v_p4g
 *
 * ---- everything below is the park's own record and is unchanged ----
 *
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
  *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_f6000/80f6148.c \
 *     asm/rom_f6000/rom_f6008_c_a_c.s --func Func_80f6148
 *
 * RECIPE ADDED IN BATCH 311.  This park carried an "N of M" figure with no way to
 * re-measure it, so its number could never be caught lying -- the dangerous half of
 * what parkcheck used to lump into one UNCHECKABLE verdict.  A tree-wide sweep found
 * eight such parks; this is one of them.  The figure above is NOT re-measured by the
 * act of adding this line: run it.
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
