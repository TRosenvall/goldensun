/* Debug_FlagEditor @ 0x080291e4  [rom_15000]
 *
 * ADDRESS READ FROM THE LINKED ELF, not from the file's position or the
 * symbol's name -- an address-named symbol carries its own answer, a named one
 * does not:
 *
 *     arm-none-eabi-nm goldensun.elf | grep Debug_FlagEditor
 *     080291e4 T Debug_FlagEditor
 *     08015290 T _Debug_FlagEditor      <- the long-call veneer, emitted by
 *                                          src/rom_15000/exports.s and the
 *                                          symbol rom_8a000 calls
 *
 * Source asm: goldensun/asm/rom_15000/rom_23178_a_c_c.s (5 functions; this is
 * the SECOND).  Landing is a three-way split of that .s:
 *
 *     python3 tools/split_s.py asm/rom_15000/rom_23178_a_c_c.s Debug_FlagEditor
 *
 *     asm/rom_15000/rom_23178_a_c_c_a.s   Debug_WarpMenu_UI
 *     src/rom_15000/rom_23178_a_c_c_b.c   <- this file
 *     asm/rom_15000/rom_23178_a_c_c_c.s   Func_8029274, Func_80292c4,
 *                                         Func_802938c
 *
 * All three suffixes are FREE: asm/rom_15000 and src/rom_15000 hold
 * rom_23178_a_c_a, rom_23178_a_c_b and rom_23178_a_c_c, and nothing named
 * rom_23178_a_c_c_*.
 *
 * DATA CHECK (the neighbour is a debug-menu TU, so this was the first thing
 * looked at): the .s has exactly one `.align 2,0`, one `.word 1`, one `.pool`
 * and one `.pool_aligned`, and ALL FOUR sit inside Debug_WarpMenu_UI above
 * line 197.  There is no `.section` and no `.incbin` anywhere in the file, no
 * string table and no jump table.  Debug_FlagEditor's own range (193-264)
 * reaches every constant with a `mov` and every callee with a direct `bl`.
 * tools/split_asm.py agrees: "carries data: no", "label exports: none needed",
 * "three-way split : no labels cross between the remaining pieces".  The three
 * labels this function defines (.L29220, .L2922c, .L2923e) occur twice each in
 * the whole file -- definition plus its own single reference.
 *
 * stage1.ld names the object EXACTLY ONCE, in the rom_15000 .text list between
 * `asm/rom_15000/rom_23178_a_c_b.o(.text)` -- the Debug_WarpMenu TU, the
 * template for this one -- and `asm/rom_15000/rom_23178_b.o(.text)`; that one
 * line becomes three, in the order above.  NO .rodata/.data/.bss line mentions
 * this object; the only rom_23178 objects with .rodata lines are
 * rom_23178_c_c_b.o and rom_23178_c_c_c.o.
 *
 * NO FLAG GROUP.  No explicit or wildcard Makefile rule matches
 * src/rom_15000/rom_23178_a_c_c_b.c, so the generic `asm/%.o: src/%.c` rule
 * with plain GCC296_CFLAGS builds it -- which is what objcmp used.
 *
 * VERDICT
 *   OK Debug_FlagEditor -- 144 bytes, 63 encodings and 9 relocations identical
 * (objcmp against asm/rom_15000/rom_23178_a_c_c.s and against the post-split
 * single-function extract; both green.)
 *
 * WHAT IT DOES.  Opens a 0x1c x 0x14 box at column 1, draws the current page
 * through Func_80292c4, then loops: wait a frame, run the row picker
 * Func_802938c, leave on -1, redraw the page on 1, and reposition the cursor
 * sprite at (x*8 + 0x3a, y*8 + 0x14).  The .s's annotation comment calls it
 * "RunDjinnListScreen"; that is a shape guess and it disagrees with the
 * exported symbol, which is the authority here.
 *
 * ---------------------------------------------------------------------------
 * ONE LEVER, AND IT IS THE TEMPLATE'S OWN
 *
 * This is a genuine family member of src/rom_15000/rom_23178_a_c_b.c
 * (Debug_WarpMenu) -- same file, same UI idiom, same CreateUIBox /
 * Func_801c0dc / Func_801c154 / Func_801c17c / CloseUIBox skeleton -- and the
 * template's headline lever transplants unchanged and is the WHOLE residue.
 *
 * The plain translation is 63 encodings against 63 with NINE differing, all of
 * them in the CreateUIBox argument fill:
 *
 *     rom   add r6,sp,#0xc / mov r5,#0 / mov r3,#2 / str r5,[sp,#0xc]
 *           / mov r2,#0x1c / str r5,[r6,#4] / mov r1,#0 / str r3,[sp] ...
 *     ours  add r6,sp,#12  / mov r3,#0 / str r3,[sp,#12] / str r3,[r6,#4]
 *           / mov r3,#2 / str r3,[sp] / mov r2,#28 / mov r3,#20 ...
 *
 * The ROM's zero lives in r5, a CALL-SAVED register, and dies at the next
 * instruction -- nothing about its own live range asks for one.  r5 is what
 * holds the CreateUIBox handle for the rest of the function (`mov r5, r0`
 * immediately after the call).  That is the template's reading verbatim:
 * gcc-2.96 has no live-range splitting, one C variable is one pseudo, and
 * global-alloc gives that pseudo a call-saved register if ANY part of its range
 * crosses a call.  So the ROM is not saying "the source named a zero"; it is
 * saying WHICH VARIABLE the zero belongs to.
 *
 * Hence `int box = 0;` -- and with the zero in a call-saved register instead of
 * r3, the two `str`s no longer compete with the argument registers and gcc's
 * own scheduler interleaves them into the fill exactly as the ROM does.  One
 * initialiser, nine encodings, no other change.
 *
 * How the zero is spelled does not matter, only which variable owns it: the
 * shipped `int box = 0; cur[0] = 0; cur[1] = 0;` and
 * `int box; box = 0; cur[0] = box; cur[1] = box;` are byte-identical.
 *
 * WHAT IT IS *NOT*.  Two levers that this shape invites were measured and are
 * wrong here:
 *
 *   * The un-rotated-loop `goto` construct.  The ROM's `b .L2923e` jumps into
 *     the middle of the loop, which is the classic tell -- but a plain
 *     `for (;;) { WaitFrames; r = pick(); if (r == -1) break; ... }` produces
 *     that entry jump by itself, because gcc rotates the two statements that
 *     precede the break to the bottom.  Writing the control flow out with an
 *     explicit `goto entry;` into the loop body is 2 encodings WORSE.
 *   * `int cur[2]` is load-bearing and two separate `int`s are not a spelling
 *     of it.  The ROM stores the first through `sp` and the second through
 *     `[r6, #4]`, and reads them back the same way; two independent locals cost
 *     four instructions and 58 encodings.  The pair is one object because the
 *     source passes its address to Func_802938c.
 *
 * MEASURED (rom 144 bytes / 63 encodings, objcmp against the original .s):
 *
 *   `int box;` uninitialised                             63 enc, 9 differ
 *   two separate `int` cursor locals                     59, 58
 *   explicit `goto entry;` un-rotated loop               63, 2
 *   `int box = 0;`                                       MATCH
 *   `int page;` assigned in the body instead of at the
 *     declaration                                        MATCH (inert)
 *   `box = 0; cur[0] = box; cur[1] = box;`               MATCH (inert)
 *
 * OTHER NOTES
 *
 *   * No header declares this function and no other .c in the tree declares it;
 *     the only other mention is the `.export_func Debug_FlagEditor` veneer in
 *     src/rom_15000/exports.s, which needs the `.global` gcc already emits.
 *     Both exits return a value (`mov r0, #0` then `pop {r1} / bx r1`), so it
 *     is `int` here with an explicit `return 0;` -- unlike its sibling
 *     Func_80b7aac in this batch, the ROM DOES materialise the zero.
 *   * Func_80292c4, Func_802938c and Func_8029274 stay in the `_c` piece of the
 *     split and are reached by name; the nine relocations match.
 */
extern int CreateUIBox(int a, int b, int c, int d, int e);
extern void CloseUIBox(int box, int mode);
extern void Func_80292c4(int box, int page);
extern int Func_802938c(int box, int *page, int *cur);
extern void Func_801c0dc(void *a, void *b);
extern void Func_801c154(void *a, int x, int y);
extern void Func_801c17c(int handle);
extern void WaitFrames(int n);

int Debug_FlagEditor(void)
{
	int buf[3];
	int handle;
	int cur[2];
	int page = 8;
	int box = 0;
	int r;

	cur[0] = 0;
	cur[1] = 0;
	box = CreateUIBox(1, 0, 0x1c, 0x14, 2);
	Func_80292c4(box, page);
	Func_801c0dc(buf, &handle);
	for (;;) {
		WaitFrames(1);
		r = Func_802938c(box, &page, cur);
		if (r == -1)
			break;
		if (r == 1)
			Func_80292c4(box, page);
		Func_801c154(buf, cur[0] * 8 + 0x3a, cur[1] * 8 + 0x14);
	}
	Func_801c17c(handle);
	CloseUIBox(box, 2);
	return 0;
}
