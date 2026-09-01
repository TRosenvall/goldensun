/* Func_80b06ec -- 0x080b06ec, from
 * goldensun/asm/rom_b0000/rom_b0070_a_a_c_a_c_c_c_a.s.
 *
 * Copies four groups of four bytes into a 2x2 tile layout -- offsets 0, 1,
 * 0x1e, 0x1f from a destination that advances by four each round -- abandoning
 * a group at its first zero byte, four rounds counted down with `bge`.
 *
 * Preserves the original ROM layout when slotted after
 * asm/rom_b0000/rom_b0070_a_a_c_a_c_c_c_a_a.o in goldensun/stage1.ld.
 *
 * THE LEVER IS THE ONE FROM BATCH 178, and this is its first use outside the
 * family that produced it. The ROM emits, at all four tests,
 *
 *     ldrb r2, [r0]
 *     mov  r3, r2
 *     cmp  r3, #0
 *
 * with r2 surviving to the `strb` and r3 dying at the compare. The park this
 * file replaces read those four `mov`s as a redundant copy and recorded the
 * body as FOUR LINES SHORT -- 38 against the ROM's 42 -- with the note that
 * getting them to appear was the whole problem. They are not copies. THE SOURCE
 * READS `*src` TWICE per byte, once to test it and once to store it, and gcc
 * CSEs the second read into the register copy. The park's `v = *src` is one
 * read and one `ldrb`; `if (*src != 0) { dst[0] = *src; ... }` is two, and
 * matches.
 *
 * MEASURED (rom 42 lines):
 *   the park's `v = *src` per byte                    38 lines, 32 differing
 *   nested ifs reading `*src` twice per byte          42, MATCH
 *   the same with `goto tail` instead of nesting      42, MATCH (byte-identical)
 *
 * The nested-if and goto forms are byte-identical, which confirms the park's
 * other reading was right too: every `beq` targets the LOOP TAIL rather than
 * the exit, so a zero skips the rest of its group and the destination still
 * advances.
 *
 * `.Lb3d40` and `.Lb413c` are data labels in rom_b0070_c_c_c.s, already
 * exported with `.global` there.
 */
extern unsigned char Lb3d40[] __asm__(".Lb3d40");
extern unsigned short Lb413c[] __asm__(".Lb413c");

void Func_80b06ec(int a, int b, int c)
{
	unsigned char *src;
	unsigned char *dst;
	int i;

	src = Lb3d40 + a * 32;
	dst = (unsigned char *)(b + Lb413c[c] + 2);
	i = 3;
	do {
		if (*src != 0) {
			dst[0] = *src;
			src++;
			if (*src != 0) {
				dst[1] = *src;
				src++;
				if (*src != 0) {
					dst[0x1e] = *src;
					src++;
					if (*src != 0) {
						dst[0x1f] = *src;
						src++;
					}
				}
			}
		}
		i--;
		dst += 4;
	} while (i >= 0);
}
