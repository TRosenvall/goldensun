/* Debug_WarpMenu  @ 0x08028f98  [rom_15000]
 *
 * Source asm: goldensun/asm/rom_15000/rom_23178_a_c.s (7 functions; this is the
 * SECOND).  Landing is a three-way split of that .s:
 *
 *     asm/rom_15000/rom_23178_a_c_a.s   Func_8028ef0
 *     src/rom_15000/rom_23178_a_c_b.c   <- this file
 *     asm/rom_15000/rom_23178_a_c_c.s   Debug_WarpMenu_UI, Debug_FlagEditor,
 *                                       Func_8029274, Func_80292c4, Func_802938c
 *
 * tools/split_asm.py reports "carries data: no", "label exports: none needed"
 * and no labels crossing between the remaining pieces, so the split is clean.
 * stage1.ld names the object exactly once, in the rom_15000 .text list between
 * `asm/rom_15000/rom_23178_a_b.o(.text)` and `asm/rom_15000/rom_23178_b.o(.text)`;
 * that one line becomes three, in the order above.  No .rodata/.data/.bss line
 * mentions this object.  No Makefile rule: the generic `asm/%.o: src/%.c` rule
 * with plain GCC296_CFLAGS is what this needs, and it is what objcmp used.
 *
 * The debug warp menu.  Opens a 0x1e x 5 box, draws the current map/entrance
 * row through Func_8028ef0, waits for the held keys to clear, then loops on
 * Debug_WarpMenu_UI: -1 warps to the chosen map, -2 backs out, anything else
 * becomes the new map id and re-draws the cursor row through Func_801c154.
 *
 * ---------------------------------------------------------------------------
 * THE WHOLE RESIDUE WAS ONE REGISTER, AND IT NAMES A VARIABLE
 *
 * The plain translation is 109 encodings against 109 with TWO bytes differing:
 *
 *     rom   mov r7, #0x0 / mov r2, r9 / strh r7, [r2]
 *     ours  mov r3, #0x0 / mov r2, r9 / strh r3, [r2]
 *
 * r3 is the FIRST entry of REG_ALLOC_ORDER (arm.h:989), so ours is what a
 * short-lived pseudo gets by default.  r7 is call-SAVED, and this zero dies at
 * the very next instruction, so nothing about its own live range asks for one.
 *
 * This is "THE VARIABLES *ARE* THE ALLOCATION" in the elevation notes: gcc-2.96
 * has no live-range splitting, one C variable is one pseudo, and global-alloc
 * gives that pseudo a callee-saved register if ANY part of its range crosses a
 * call -- even when the sub-range that matters dies immediately.  So the r7
 * does not say "the source named a zero"; it says WHICH VARIABLE the zero
 * belongs to.  Reading the dispositions of this function (r5 = the UI result,
 * r6 = the map id, r7 = the CreateUIBox handle) identifies it: r7 is `box`, and
 * the ROM's zero is `box`'s value before the call gives it its real one.
 *
 * Hence `int box = 0;`.  The initialiser is redundant to a reader and is the
 * entire lever: it puts the constant in the pseudo that later holds the window
 * handle, that pseudo crosses every call in the function, global-alloc gives it
 * a call-saved register, r5 and r6 are already taken by higher-priority
 * allocnos, and r7 is what is left.  The `strh` then stores out of it.
 *
 * How the zero is spelled does not matter -- CSE finds it either way -- only
 * WHICH VARIABLE OWNS IT does.  All four of these are byte-identical:
 *
 *     int box = 0; ... sel = 0;      <- shipped
 *     box = 0; sel = 0;
 *     box = 0; sel = box;
 *     sel = box = 0;
 *
 * MEASURED (rom 114 lines / 109 encodings, objcmp):
 *
 *   plain `sel = 0;`, box uninitialised            114 lines, 2 encodings
 *   the same + one zero shared with CreateUIBox's
 *     first argument (`z` used at both)            114, 2   (z dies before the
 *                                                   call, so it is still r3)
 *   the same + `-fno-rerun-cse-after-loop`         114, 7
 *   the same + `-fno-schedule-insns2`              114, 25
 *   `register int __asm__("r7") z = 0; sel = z;`   114, 18  (a hard-register pin
 *                                                   is not a call-crossing
 *                                                   pseudo and does not imitate
 *                                                   one)
 *   `sel = 0;` moved below the two gState reads    114, 35  (statement order is
 *                                                   the stream order here; the
 *                                                   two address pseudos swap)
 *   `*(short *)&sel = 0;`                          116, 95
 *   `map = 0; sel = map;`                          114, 2   (r6 is not r7)
 *   `int box = 0;`                                 MATCH
 *
 * The scan aimed this function at the "zero interleaved into a shifted build"
 * class, straight-line site.  IT WAS NOT THAT.  The bare call script reproduced
 * every interleave in the function first time -- the `mov r1,#0` for the ldrsh
 * sitting inside the `mov r1,#0xe5 / lsl r1,#1` gState build, the `mov r1,#0xa /
 * add r1,sp` threaded through the second load, the CreateUIBox argument order --
 * with no pins and no named split builds.  Nothing from the class cure is in
 * this file, and the one pin that was tried measured 18.
 *
 * OTHER NOTES
 *
 *   * No header declares this function.  The only other declaration in the tree
 *     is a use-site `extern void Debug_WarpMenu(void);` in
 *     src/rom_15000/rom_23178_a_b.c, which passes it to StartTask as a task
 *     entry point.  The definition returns a value (both exits do `mov r0, r5`),
 *     so it is `int` here; the two declarations never meet in one TU.
 *   * gState is reached through a local `unsigned char *` so the +0x1c8/+0x1ca
 *     stay real arithmetic (`mov r1,#0xe4 / lsl r1,#1 / add r3,r2,r1`) instead
 *     of folding into a `=gState+456` pool word.
 *   * Func_8028ef0, the neighbour above this one in the same .s, was attempted
 *     and is NOT solved: 70 lines against 70 with 20 differing, all of it one
 *     r2/r3 exchange plus the argument-move order at the two Func_801e9a0 call
 *     sites.  A uniform whole-value ascending fill at both calls is inert (20),
 *     naming the repeated stack argument 0xe is much worse (72 lines, 65), and
 *     splitting the `+ 0x99b` into its own statement changes nothing (20).
 *     That is the recorded four-member r2/r3 exchange class.  Its parameter is
 *     also sign-extended in the callee (`lsl/asr #16`), so if it is ever solved
 *     into THIS file its definition must keep an `int` parameter and do the
 *     narrowing internally, or the prototype coming into scope will make the
 *     call site above sign-extend and break this match.
 */
extern unsigned char gState[];
extern int gKeyHeld;

extern int CreateUIBox(int a, int b, int c, int d, int e);
extern void CloseUIBox(int box, int mode);
extern void Func_8028ef0(int box, int map, short *p);
extern void Func_801c0dc(void *a, void *b);
extern void Func_801c154(void *a, int b, int c);
extern void Func_801c17c(int a);
extern void WaitFrames(int n);
extern void _SetDestMap(int map, int entrance);
extern short Debug_WarpMenu_UI(int box, int map, short *cur, short *sel);

int Debug_WarpMenu(void)
{
	int buf[3];
	int handle;
	short sel;
	short cur;
	unsigned char *g;
	int box = 0;
	int map;
	int r;

	sel = 0;
	g = gState;
	map = *(short *)(g + (0xe4 << 1));
	cur = *(short *)(g + (0xe5 << 1));
	box = CreateUIBox(0, 7, 0x1e, 5, 2);
	Func_8028ef0(box, map, &cur);
	Func_801c0dc(buf, &handle);
	if (gKeyHeld != 0) {
		do {
			WaitFrames(1);
		} while (gKeyHeld != 0);
	}
	for (;;) {
		r = Debug_WarpMenu_UI(box, map, &cur, &sel);
		if (r == -1) {
			Func_801c17c(handle);
			CloseUIBox(box, 2);
			_SetDestMap(map, cur);
			return r;
		}
		if (r == -2) {
			Func_801c17c(handle);
			CloseUIBox(box, 2);
			return r;
		}
		Func_801c154(buf, 0x4a, sel * 14 + 0x3c);
		map = r;
		WaitFrames(1);
	}
}
