/* Func_80b2928 -- 0x080b2928, from goldensun/asm/rom_b0000/rom_b0070_c_c_a_c.s.
 *
 * The sanctum's spoken prompt: save the panel's style byte, look up the
 * speaker's voice, format the line, force style 0xd, open the box and wait for
 * the text to finish, then restore the style.
 *
 * Preserves the original ROM layout when slotted before
 * asm/rom_b0000/rom_b0070_c_c_a_c_c.o in goldensun/stage1.ld.
 *
 * THREE LEVERS, and the first is the read-count rule at its most emphatic.
 *
 *   THE PANEL POINTER IS WRITTEN OUT THREE TIMES, UNNAMED. The ROM loads
 *   `*(u8 **)(base + 0x380)` three times -- twice CSEd through r6 inside the
 *   first extended basic block, then rematerialised after the loop, because
 *   gcc-2.96's CSE does not reach across it. Naming that pointer ONCE collapses
 *   the function to 48 lines against 57 and destroys the allocation from the
 *   first instruction: r8 is never used and the frame shrinks. Writing the same
 *   unnamed expression three times reproduces all three loads AND the r6 CSE
 *   for free.
 *
 *   THE RETURN TYPE IS AN ORACLE IN BOTH DIRECTIONS. Declaring this `int`
 *   changes only the last two instructions -- `pop {r1} / bx r1` instead of the
 *   ROM's `pop {r0} / bx r0` -- because gcc reserves r0 for a returned value.
 *   So a ROM epilogue that pops into r0 is POSITIVE EVIDENCE OF A void
 *   FUNCTION, which is a cheap check to run before writing anything. The .s
 *   file's inherited comment calls this "the choice-returning form", which is
 *   wrong: it returns nothing.
 *
 *   CALL ORDER IN THE SOURCE IS EMISSION ORDER. gcc-2.96 does not reorder the
 *   `bl`s. The ROM runs the formatter before the layout call even though the
 *   near-twin sibling Func_80b28d4 has them the other way round; writing the
 *   sibling's order costs 36 differing. Only the surrounding arithmetic gets
 *   scheduled around them.
 *
 * MEASURED (rom 57 lines):
 *   three unnamed re-reads, void, formatter first        57 lines, MATCH
 *   `saved` typed `unsigned char` rather than `unsigned int`
 *                                                        57, MATCH (byte-identical)
 *   `for (;;) { if (...) break; ... }` for the wait      57, MATCH (byte-identical)
 *   the sibling's call order                             56, 36
 *   the panel pointer hoisted into a named local          48, 56
 *   declared `int` instead of `void`                     57, 2
 *
 * The last three are the discriminating perturbations: this screen is not
 * trivially green.
 */
extern unsigned char iwram_3001f2c[];
extern unsigned int _GetSpriteVoice(unsigned short id);
extern void _Func_8019a54(void);
extern unsigned int Func_80b2884(unsigned int arg0);
extern void _Func_8017658(unsigned int a, int b, int c, unsigned int d);
extern void WaitFrames(unsigned int nframes);
extern int _Func_8017364(void);

void Func_80b2928(unsigned int arg0)
{
	unsigned char *r5 = *(unsigned char **)iwram_3001f2c;
	unsigned int voice;
	unsigned int saved;

	saved = (*(unsigned char **)(r5 + (0xe0 << 2)))[5];
	voice = _GetSpriteVoice(*(unsigned short *)(r5 + (0xe9 << 2)));
	arg0 = Func_80b2884(arg0);
	(*(unsigned char **)(r5 + (0xe0 << 2)))[5] = 0xd;
	_Func_8019a54();
	_Func_8017658(arg0, 5, 0, (voice << 16) | 0x22);
	while (!_Func_8017364()) {
		WaitFrames(1);
	}
	WaitFrames(1);
	(*(unsigned char **)(r5 + (0xe0 << 2)))[5] = saved;
}
