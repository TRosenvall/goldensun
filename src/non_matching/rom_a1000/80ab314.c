/* Func_80ab314 (RunDjinnDetail, 0x080ab314) -- 296 instructions.
 * NON-MATCHING, 227 encodings of 307 UNDER THE TREE'S PRODUCTION FLAGS (ref 720 bytes / 307
 * encodings against ours 716 / 305).  That is what objcmp and parkcheck measure and it is what
 * this header claims.  Zero shims in both classes in this landing form.
 *
 * (Claim line first on purpose: parkcheck reads the FIRST `N encodings of M`.)
 *
 * IT REACHES 22 OF 307 -- A TRUE DISTANCE -- UNDER TWO NON-DEFAULT FLAGS, AND BOTH ARE OWNER
 * DECISIONS.  With `-fno-gcse -fno-expensive-optimizations`: size 720 = 720, encodings 307 = 307,
 * the pool in the same order (every `ldr [pc,#imm]` displacement matches) and all 47 relocations
 * at identical offsets.  `--align` 25 of 308.  GCSE_CFLAGS already exists; a group for the second
 * flag does not, and that is the decision.
 *
 * THE TWO FLAGS ARE A COUPLED PAIR AND EITHER ALONE IS A REGRESSION: none 225 (716 bytes),
 * -fno-gcse 89 (716), -fno-expensive-optimizations 280 (712), BOTH 22 (720, the ROM's size).
 * gcse's cprop kills a pseudo and removes the ROM's `ldr r3,=0xc32 / mov r8,r3` preheader -- the
 * whole length deficit -- while flag_expensive_optimizations gates reload_cse_regs, which keeps a
 * constant in a register across both stores and rewrites the ROM's `add r3,r1 / strb r2,[r3]`.
 *
 * A .sym PROPOSAL IS RECORDED AND WITHHELD: `_MSG_c30 = 0x0c30`.  Its control is TWO-SIDED and is
 * the strongest form -- `ldr r5, =0xc30` pools a SHIFTABLE constant (0xc3 << 4) while the SAME
 * function builds 0xc80 (0xc8 << 4) as `mov r1,#0xc8 / lsl r1,#4`, so shiftable constants here are
 * provably not pooled; as a symbol cse's use_related_value derives +1 with `add r5,#1`, and
 * dropping it is 22 -> 267.  It is withheld because it does NOT COMPLETE the function (22 remain)
 * and because those 22 depend on an undecided flag group.  Admit it when the function closes.
 * The measurement copy carrying the `.equ` shim is scratch_elev/b294/D/t4_measure.c and must
 * never land.
 *
 * HEAD split of a two-function file; no label crosses the boundary either way and the eleven pool
 * words are emitted by the assembler at .func_end, so no export is needed.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_a1000/80ab314.c \
 *     asm/rom_a1000/rom_aa538_c_c_c_a_a.s --func Func_80ab314
 * NON-MATCHING: 22 of 307 encodings differ, and 22 IS A TRUE DISTANCE --
 * size 720 = 720, encodings 307 = 307 (296 instructions + 11 pool words),
 * the pool in the SAME ORDER (every `ldr [pc,#imm]` displacement matches) and
 * the RELOCATION LIST IDENTICAL, all 47 entries at the same offsets.
 *
 * REQUIRES TWO NON-DEFAULT FLAGS AS A COUPLED PAIR (see FLAGS below):
 *     -fno-gcse -fno-expensive-optimizations
 *
 * Reference: asm/rom_a1000/rom_aa538_c_c_c_a_a.s, lines 10-319.
 *
 * Verify with (objcmp reads flags from the Makefile, so it cannot score this
 * candidate; the numbers above come from assembling both sides by hand):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/tryc.py src/non_matching/rom_a1000/80ab314.c \
 *     --ref asm/rom_a1000/rom_aa538_c_c_c_a_a.s --align \
 *     -fno-gcse -fno-expensive-optimizations        ->  25 of 308
 *
 * ---------------------------------------------------------------- THE SPLIT
 * The file holds exactly TWO functions (`grep -ci func_start` == 2):
 * Func_80ab314 at lines 10-319 and Func_80ab5e4 (RunDjinnMain, 2315 lines) at
 * 335-2655.  This target is the FIRST, so the split is a HEAD split.
 * NO LABEL CROSSES THE BOUNDARY IN EITHER DIRECTION.  The head defines and
 * references only .Lab3ce .Lab3ee .Lab43a .Lab45a .Lab474 .Lab48c .Lab4b2
 * .Lab4b8 .Lab4ec .Lab502 .Lab530 .Lab59a; the tail's ~100 labels, its
 * 28-word jump table (lines 1231-1258) and its `.word 0x8000` (line 600) are
 * all inside its own body, and no tail label is named in the head.
 * DATACHECK CONFIRMED CLEAN: the head region carries no .word, .byte, .short,
 * .align or .pool at all -- the 11 pool words are emitted by the assembler at
 * `.func_end`, after the last instruction.  No export is needed either way.
 *
 * ------------------------------------------------------------------- FLAGS
 * A COUPLED PAIR, and either one ALONE IS A REGRESSION.  Measured byte-exact
 * on the final candidate (differing encodings / our size / ours-vs-307):
 *   (production flags)                       225   716 bytes   305 encodings
 *   -fno-gcse                                 89   716         305
 *   -fno-expensive-optimizations             280   712         301
 *   -fno-gcse -fno-expensive-optimizations    22   720 = ref   307 = ref
 * This is the recorded "single drops cannot find a coupled pair" trap in its
 * purest form: -fno-expensive-optimizations alone is 225 -> 280.
 *
 * WHY -fno-gcse.  gcse's constant propagation kills the 0xc32 pseudo:
 * `.07.gcse` says literally `CONST-PROP: Replacing reg 46 in insn 168 with
 * constant (const_int 3122 [0xc32])`.  With the pseudo gone the pool load
 * sinks into the loop and the ROM's `ldr r3,=0xc32 / mov r8,r3` preheader pair
 * cannot exist -- that pair is the ENTIRE length deficit.  GCSE_CFLAGS already
 * exists in the Makefile for seven files.
 *
 * WHY -fno-expensive-optimizations.  `flag_expensive_optimizations` gates
 * `reload_cse_regs` (reload1.c:7992) and regclass's second costing pass
 * (regclass.c:1115).  reload_cse_regs is what keeps 0xea6 alive in a register
 * across BOTH `iwram_3001e8c[0xea6]` stores, which turns the ROM's
 * `add r3,r1 / strb r2,[r3]` into a register-offset `strb r3,[r2,r7]` and
 * loses two instructions.  The Makefile mentions this flag twice as tried and
 * INERT elsewhere (lines 199 and 485); here it is worth 89 -> 22.  It has no
 * *_CFLAGS group yet, so landing this needs an owner decision.
 *
 * -------------------------------------------------------- LOAD-BEARING FORM
 * Byte-exact single drops from the final candidate (22), each measured alone
 * under the pair of flags above:
 *
 *   `(int)&_MSG_c30` -> the literals 0xc30 / 0xc31 .............  22 -> 267
 *   `g` from a plain `extern iwram_3001e8c` .....................  22 -> 291
 *   `volatile` dropped on gKeyRepeat / gKeyPress ................  22 -> 207 (299 enc)
 *   named `zero` dropped ........................................  22 -> 216 (309 enc)
 *   the 0x63 `int` carrier dropped ..............................  22 -> 150 (311 enc)
 *   the five `goto done` written as `break` .....................  22 ->  83
 *   `cursor = 0` back where a reader would put it ................  22 ->  41
 *   named `one` dropped .........................................  22 ->  29
 *
 * 1. _MSG_c30 IS A SYMBOL, AND THIS FUNCTION GIVES TWO-SIDED IN-FUNCTION
 *    CONTROL.  `ldr r5, =0xc30` pools a constant gcc can build in two
 *    instructions (0xc30 == 0xc3 << 4) -- and the SAME FUNCTION builds
 *    0xc80 == 0xc8 << 4 with `mov r1,#0xc8 / lsl r1,#4` for StartTask's
 *    priority.  So a shiftable constant in this function is NOT pooled, which
 *    makes 0xc30's pool word evidence rather than coincidence.  As literals
 *    gcc emits `mov r0,#0xc3 / lsl r0,#4` for 0xc30 and a SEPARATE pool word
 *    for 0xc31 (measured); as a symbol, cse's `use_related_value` derives
 *    `_MSG_c30 + 1` with `add r5,#1`, which is the ROM.  0xc32, 0xc39, 0x12b6,
 *    0x12f8 and 0xea6 are all non-shiftable, pool either way, and are left as
 *    literals -- they discriminate nothing, and they all reproduce.
 *    PROPOSAL: `_MSG_c30 = 0x0c30;` in message.sym.
 *
 * 2. THE TWO iwram NAMES ARE ONE POOL WORD PLUS A `sub`, AND THE SPELLING IS
 *    NOT THE OBVIOUS ONE.  The ROM does `ldr r3,=iwram_3001f2c / ldr r1,[r3] /
 *    sub r3,#0xa0 / ldr r3,[r3]`, then LATER loads `=iwram_3001e8c` as its own
 *    pool word.  So both symbols are in the pool and the first derivation is
 *    cse's `use_related_value` on `CONST (sym - 0xa0)`.  Two externs give two
 *    pool loads; `iwram_3001f2c[-0x28]` through an array declaration gives
 *    `mov r3,#0xa0 / neg r3,r3 / ldr r2,[r2,r3]` because thumb has no negative
 *    offset and LEGITIMIZE_ADDRESS reaches for reg+reg.  What works is
 *    `*(unsigned char **)((int)&iwram_3001f2c - 0xa0)` for the ONE early read
 *    and the plain `iwram_3001e8c` extern for the three later ones.
 *    NOTE FOR THE DOCS: the park in src/non_matching/rom_a1000/80a5b94.c says
 *    "cse cannot relate two distinct symbol_refs, so [they] are one array".
 *    That is right about two symbol_refs and WRONG as a rule for this shape --
 *    here ONE symbol_ref plus a CONST offset is what the ROM has, and the
 *    array reading is what fails.  The pool word assembles to 0x03001E8C
 *    either way, because wram.sym defines these absolutely.
 *
 * 3. `volatile` ON THE KEY WORDS IS REQUIRED AND IS WORTH 185 ENCODINGS AND
 *    EIGHT INSTRUCTIONS.  The ROM re-reads gKeyPress at all three of its tests
 *    and gKeyRepeat at both of its own.  Without `volatile`, gcse's available-
 *    expression pass deletes four of those loads (299 encodings against 307)
 *    -- including one ACROSS THE LOOP BACK EDGE, which then feeds the 0x60
 *    test from the register the 0x90 test loaded.  `extern volatile unsigned
 *    int gKeyPress;` is this tree's established declaration (rom_a1814_...,
 *    rom_a4f08_b.c, 80a7850.c) and it is semantically right for an input
 *    mirror; it is NOT scaffolding.
 *
 * 4. `break` FOR THE FIRST EXIT AND `goto done` FOR THE OTHER FIVE -- 83 -> 22.
 *    This is batch 293's expand_end_loop lever with a boundary that is new.
 *    stmt.c:2371-2490: the roll scans FROM THE TOP of the loop for jumps whose
 *    destination is the loop's own end_label, KEEPS UPDATING `last_test_insn`
 *    at each one, and stops 30 insns after the first.  It then rolls
 *    EVERYTHING from the loop start through that insn to the bottom.  So
 *      - `if (kr & 0x90) break;` with an EMPTY body makes the 0x90 test the
 *        only qualifying jump, and the rolled block is exactly
 *        [redraw, WaitFrames, the 0x90 test] -- the ROM's `.Lab4b8`, entered
 *        by `b .Lab4b8` and left by `beq .Lab43a`;
 *      - the arm body goes AFTER the loop, where `goto end_label` becomes a
 *        fall-through: that is the ROM's `add r7,#1 / bl Func_80aa538 /
 *        bl _PlaySound` sitting between the test and `.Lab4ec`;
 *      - the other four exits must be `goto done`, NOT `break`, or their
 *        jumps qualify too and the roll swallows the 0x60 arm as well.
 *    Writing all five as `break` measures 83 and lays the gKeyPress tests
 *    first.  The rolled layout is also why the ROM has TWO gKeyRepeat loads
 *    that cse cannot merge: the 0x60 test's block is reached only by the
 *    backward branch, so it starts a new extended basic block.
 *
 * 5. `cursor = 0` HAS TO BE WRITTEN WHERE NO READER WOULD PUT IT -- 41 -> 22,
 *    and it is the whole r6/r7 question.  The ROM has box1 in r6 and cursor in
 *    r7; ours had them swapped, ~35 of the 41.  `allocno_compare`
 *    (global.c:598) cannot explain it: cursor carries ~44 loop-depth-weighted
 *    refs against box1's ~11 over a comparable live range, so cursor is
 *    allocated first however the source is written, and no declaration order,
 *    pointer type or reference count moves it (eight spellings measured, all
 *    inert).  What decides it is find_reg's PASS 0 (global.c): pass 0
 *    considers only registers already in `regs_used_so_far`, and local-alloc
 *    has put `win` (= base + 0x10c) in r6.  cursor therefore takes r6 in pass
 *    0 unless it CONFLICTS with r6's occupant.  Assigning `cursor = 0`
 *    immediately after `win` is formed makes cursor live across win's three
 *    dereferences, creates that conflict, and cursor falls to r7 in pass 1 --
 *    at which point box1 takes r6 in pass 0 for free.  THE LEVER IS "MAKE THE
 *    LOSER CONFLICT WITH THE LOCAL-ALLOC OCCUPANT", not "raise the winner's
 *    priority", and it is not reachable while gcse's cprop is on (the same
 *    move measured 0 under production flags, twice).
 *
 * 6. THE 0x63 HALFWORD STORE NEEDS AN `int` CARRIER *ONLY AT THE STORE* --
 *    150 -> 22, and it is the pool's structure, not one word.  A HImode store
 *    of a literal goes through the pool, so `*tile = 0x63` adds a 12th pool
 *    word -- which pushes the minipool over a barrier and SPLITS IT INTO THREE
 *    CHUNKS with 0xea6 duplicated (311 encodings, 13 words).  The ROM has one
 *    chunk of 11 words at the end.  Carrying the store value in an `int`
 *    (`full = 0x63; *tile = full;`) gives `mov r3,#0x63 / strh`.  The COMPARE
 *    must stay a literal: hoisting `full = 0x63` above the `if` and comparing
 *    against it measures 85 against 70 aligned.
 *
 * 7. NAMED `zero` AND `one`, AT EXACTLY THE ROM'S SITES.  r8 holds a zero for
 *    the 0x12f8 byte store, the three `strh` at obj+0x1a/+0x18/+0x14 and
 *    `*spr = 0`; r11 holds a one for the two Func_80ab1f4 5th args and
 *    `gKeyPress & 1`.  Every other 0 and 1 in the function (the WaitFrames
 *    arguments, the _CloseUIBox flags, both post-loop 0xea6 stores) is a fresh
 *    literal and must be written as one.  Materialisation ORDER sets the
 *    register: `one = 1;` before `kr = &gKeyRepeat;` puts one in r11 and kr in
 *    r9, which is the ROM; the other order swaps them (1 aligned instruction).
 *
 * 8. `kr = &gKeyRepeat` AS A NAMED POINTER LOCAL.  The ROM keeps the address
 *    in r9 across the whole outer loop.  Writing the global inline at either
 *    of the two sites re-materialises the pool load there: 22 -> 51 for the
 *    0x60 site, 22 -> 60 for the 0x90 site.  gcc does not hoist a
 *    single-constant load into a preheader on its own.
 *
 * 9. `i = 0;` BEFORE `msg = 0xc32;`, as separate statements with `for (; ...)`.
 *    The ROM's preheader is `ldr r3,=0xc32 / mov r7,#0 / mov r5,#0 /
 *    mov r8,r3`; with the init inside the `for` the r8 copy lands one slot
 *    early.  Worth 2 aligned instructions.  A `do { } while (i <= 6)` measures
 *    the same as the `for`.
 *
 * ------------------------------------------------------------- WHAT IS INERT
 * All of these measured 22 (or the stated worse figure) and are dead ends:
 *   declaration order of box1/cursor, of box2/box3, of one/zero, of full;
 *   `box1` as an `unsigned short *` with `box1[7]`;
 *   `cursor + 0xc32` vs `0xc32 + cursor` (and the same for 0xc39);
 *   naming the sum in a temp before _DrawSmallText or _Func_80175c0;
 *   naming the address for either 0xea6 store, or writing it
 *     `*(unsigned char *)(0xea6 + (int)iwram_3001e8c)`;
 *   `StartTask(f, 0xc80)` against `0xc8 << 4`;
 *   `pp -= 0x28` / an `int` address decremented / a plain CONST expression for
 *     `g` -- all three give the identical (correct) `sub r3,#0xa0`;
 *   -fregmove, -fno-regmove, -fno-cse-follow-jumps, -fno-cse-skip-blocks,
 *     -fno-strict-aliasing, -fno-strength-reduce, -fno-rerun-cse-after-loop
 *     (all on top of -fno-gcse: 48, i.e. no change);
 *   -fno-schedule-insns2 on top of -fno-gcse: 116, a bad regression.
 *
 * -------------------------------------------------------------- THE RESIDUE
 * 22 encodings in eight clusters, every one of them the RIGHT instruction with
 * the wrong scratch register, plus four adjacent-pair swaps:
 *   0x0e0  ldr r0,=0xc32          vs ldr r2   (+ the `adds r0,r7,r0` / `movs r2,#0` swap)
 *   0x0ee  ldr r1,=0xc39          vs ldr r3
 *   0x0f8  mov r2,r11 / str r2    vs mov r1,r11 / str r1
 *   0x10e  mov r3,r11 / str r3    vs mov r2,r11 (+ a swap with `movs r3,#14`)
 *   0x126  mov r1,r9 / ldr r2,[r1] vs mov r3,r9 / ldr r2,[r3]
 *   0x158  movs r2,#2 / negs / str vs the same three in r1
 *   0x170  movs r3,#1 / negs / str vs the same three in r2
 *   0x27c, 0x288  two adjacent-pair swaps in the tail
 * BLOCKER: reload's spill-register round robin.  reload1.c's
 * `allocate_reload_reg` walks `spill_regs` starting at `last_spill_reg`, so the
 * register a `mov rLow, rHigh` reload lands in is a PHASE of the reload history
 * and not a function of the source; every instruction before the first
 * divergence at 0x0e0 already matches, so the phase differs with no visible
 * cause upstream.  -fregmove (which is what would tie `adds r0,r7,r0`'s
 * destination to its dying source) is inert here, and regclass's second pass
 * is already off.  This is the recorded terminal "right instructions, wrong
 * registers" class; a `register ... __asm__` pin at each of the eight sites
 * would be eight shims for eight instructions and is not worth proposing.
 *
 * NEXT, if reopened: the only untried handle is a reload_reg phase shift, i.e.
 * finding a source form that adds or removes ONE earlier reload without
 * changing the instruction stream.  Nothing in this function looks like it has
 * room.  Do not re-measure the inert list.
 *
 * SHIMS IN THIS FILE: ZERO.  No `register ... __asm__` declaration and no
 * `__asm__(".equ ...")` line.  The measurement copy t4_measure.c adds exactly
 * ONE shim in the `.equ` class -- `__asm__(".equ _MSG_c30, 0x0c30");` -- so it
 * can be assembled standalone; that line MUST NOT LAND, because message.sym is
 * where the row belongs.
 */
extern int _MSG_c30;
extern unsigned char *iwram_3001f2c;
extern unsigned char *iwram_3001e8c;
extern volatile unsigned int gKeyRepeat;
extern volatile unsigned int gKeyPress;

extern void _Func_80164ac(int win);
extern void WaitFrames(int n);
extern void _Func_8016478(int win);
extern void _Func_801e7c0(int msg, int win, int x, int y);
extern void Func_80ab21c(int x, int y, int w, int h, int fill);
extern void Func_80ab2ec(int a, int b, int c, int d, int e, int f);
extern int _CreateUIBox(int a, int b, int c, int d, int e);
extern void _Func_801e318(void);
extern void _DrawSmallText(int msg, int win, int x, int y);
extern int *_Func_80175c0(int win, int msg);
extern void Func_80ab1f4(int win, int a, int b, int c, int d, int e);
extern int Func_80aa538(int v, int n);
extern void _PlaySound(int id);
extern int _Func_8017364(void);
extern void Func_80a1a40(int a, int b);
extern void Func_8003f3c(int id);
extern void _CloseUIBox(int win, int a);
extern void StartTask(void (*f)(void), int pri);
extern void Func_80a19a0(void);

int Func_80ab314(void)
{
	int box2;
	int box3;
	unsigned char *base;
	unsigned char *g;
	int ret;
	int prev;
	int box1;
	int cursor;
	int i;
	int *win;
	unsigned short *tile;
	int *spr;
	unsigned short *obj;
	volatile unsigned int *kr;
	int msg;
	int one;
	int zero;
	int full;

	base = iwram_3001f2c;
	g = *(unsigned char **)((int)&iwram_3001f2c - 0xa0);
	ret = 0;
	prev = 0;
	_Func_80164ac(*(int *)(base + 0x30));
	WaitFrames(1);
	win = (int *)(base + (0x86 << 1));
	cursor = 0;
	_Func_8016478(*win);
	_Func_801e7c0((int)&_MSG_c30, *win, 0, 0);
	_Func_801e7c0((int)&_MSG_c30 + 1, *win, 0, 0x10);
	Func_80ab21c(1, 1, 0xb, 3, 6);
	Func_80ab2ec(*(int *)(base + 0x30), 0, 0, 0x1c, 0xa, 6);
	box1 = _CreateUIBox(0, 9, 8, 0xa, 6);
	box2 = _CreateUIBox(8, 0xc, 0x16, 7, 2);
	box3 = _CreateUIBox(8, 9, 0x16, 3, 2);
	_Func_801e318();
	i = 0;
	msg = 0xc32;
	for (; i < 7; i++)
		_Func_801e7c0(i + msg, box1, 0, i * 8);
	one = 1;
	kr = &gKeyRepeat;
	zero = 0;
	do {
		_Func_8016478(box3);
		_DrawSmallText(0xc32 + cursor, box3, 0, 0);
		spr = _Func_80175c0(box2, 0xc39 + cursor);
		Func_80ab1f4(box1, 0, prev, 6, one, 0xf);
		Func_80ab1f4(box1, 0, cursor, 6, one, 0xe);
		prev = cursor;
		for (;;) {
			Func_80a1a40(-0xc,
			             (*(unsigned short *)(box1 + 0xe) + cursor) * 8 + 8);
			WaitFrames(1);
			if (*kr & 0x90)
				break;
			if (*kr & 0x60) {
				cursor--;
				cursor = Func_80aa538(cursor, 7);
				_PlaySound(0x6f);
				goto done;
			}
			if (gKeyPress & 8) {
				_PlaySound(0x71);
				ret = -2;
				goto done;
			}
			if (gKeyPress & 6) {
				_PlaySound(0x71);
				ret = -1;
				goto done;
			}
			if (gKeyPress & one) {
				if (_Func_8017364()) {
					cursor++;
					cursor = Func_80aa538(cursor, 7);
					_PlaySound(0x70);
					goto done;
				}
				_PlaySound(0x6f);
			}
		}
		cursor++;
		cursor = Func_80aa538(cursor, 7);
		_PlaySound(0x6f);
	done:
		tile = (unsigned short *)(g + 0x12b6);
		if (*tile != 0x63) {
			Func_8003f3c(*tile);
			full = 0x63;
			*tile = full;
		}
		iwram_3001e8c[0x12f8] = zero;
		_Func_8016478(box2);
		obj = (unsigned short *)*spr;
		obj[0xc] = zero;
		obj[0xd] = zero;
		obj[0xa] = zero;
		*spr = zero;
	} while (ret == 0);
	iwram_3001e8c[0xea6] = 1;
	_Func_80164ac(box3);
	_Func_80164ac(box2);
	WaitFrames(1);
	_CloseUIBox(box3, 1);
	_CloseUIBox(box1, 1);
	_CloseUIBox(box2, 1);
	_Func_801e318();
	if (ret == -2) {
		_Func_8016478(*(int *)(base + (0x86 << 1)));
		_Func_8016478(*(int *)(base + 0x30));
		_Func_8016478(*(int *)(base + 0x10));
		iwram_3001e8c[0xea6] = 0;
	}
	StartTask(Func_80a19a0, 0xc8 << 4);
	return ret;
}
