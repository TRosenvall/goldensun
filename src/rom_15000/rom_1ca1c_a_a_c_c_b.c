/* Func_801ce48 -- 0x0801ce48, from goldensun/asm/rom_15000/rom_1ca1c_a_a_c_c.s.
 *
 * A countdown halfword at +0x574 of the UI record: decrement it, or reload it
 * to 2 when it has already reached zero.
 *
 * Preserves the original ROM layout when slotted after
 * asm/rom_15000/rom_1ca1c_a_a_c_c_a.o in goldensun/stage1.ld.
 *
 * TWO LEVERS, and the pair is the point.
 *
 *   THE COUNTER IS READ TWICE. The ROM's `ldrh r2, [r0] / mov r3, r2 /
 *   cmp r3, #0` is the CSEd-second-read signature from batch 178, and the
 *   decrement is `add r3, r2, #-1` off the FIRST read while the test used the
 *   copy. Everything through the pointer -- `if (*p == 0) ... else
 *   *p = *p + 0xffff;` -- emits it. A local would emit one `ldrh` and come out
 *   an instruction short.
 *
 *   THE RELOAD VALUE IS NAMED. `*p = 2` through a halfword pointer gets the 2
 *   POOLED (`ldr r3, =0x2`) where the ROM has `mov r3, #0x2`. That is the
 *   recorded halfword-store pooling rule, and naming the value in an int local
 *   first is its recorded fix. It was the single differing line of sixteen.
 *
 * `*p + 0xffff` rather than `*p - 1` is deliberate: the ROM builds the addend
 * as a pooled 0xffff and uses the three-operand `add r3, r2, r1`, which the
 * subtraction spelling does not produce.
 *
 * MEASURED (rom 16 lines):
 *   both reads through the pointer, `*p = 2` inline    16 lines, 1 differing
 *   the same with the 2 named first                    16, MATCH
 */
void Func_801ce48(int a)
{
	unsigned short *p;
	int n;

	p = (unsigned short *)(a + 0x574);
	if (*p == 0) {
		n = 2;
		*p = n;
	} else {
		*p = *p + 0xffff;
	}
}
