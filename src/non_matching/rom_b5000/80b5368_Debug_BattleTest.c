/* Debug_BattleTest -- 0x080b56e0, from goldensun/asm/rom_b5000/rom_b5368_a.s.
 *
 * NON-MATCHING: 141 encodings of 162 differ (objcmp).
 * The instruction COUNT is right (162 against 162) but the SIZE is not -- 392
 * bytes against the reference's 388 -- so 141 is NOT a distance to exact: four
 * extra bytes early shift the alignment and most of the 141 is that shift, not
 * 141 independent defects.  Everything from the key-poll chain onward is the
 * ROM's instruction-for-instruction modulo r9/r10/r11 being one apart.
 *
 * The debug battle harness: the shipped ROM's encounter picker, reached only by
 * holding DOWN at boot.  Without DOWN it sets flag 0x162 and calls
 * BattleMain(0x101) forever; with DOWN it runs an auto-repeat picker over the
 * encounter id and the party preset, then launches the battle.
 *
 * WHAT IS ALREADY RIGHT.  The four-high-register prologue, the whole eight-test
 * `gKeyRepeat` chain with its exact masks and branch polarities, the
 * `(gKeyRepeat & 2) || mode != 0` short-circuit, the preset re-load guard, and
 * the tail layout.  Two structural levers got it here:
 *
 * 1. *** `volatile int gKeyRepeat` IS WHAT RE-LOADS IT BEFORE EVERY TEST. ***
 *    The ROM keeps the ADDRESS in r5 and does `ldr r3,[r5]` eight times in a row.
 *    A non-volatile int is loaded once.
 * 2. *** THE NO-DOWN-KEY PATH IS NOT AN `else`; IT IS THE CODE AFTER THE `if`. ***
 *    Writing `if ((gKeyHeld & 0x80) == 0) { _SetFlag(0x162); BattleMain(0x101);
 *    continue; }` puts that block inline and gives a short `bne`; the ROM has
 *    `bne` over a long `b` to a block at the very END of the function, which is
 *    what `if (gKeyHeld & 0x80) { <picker, never falls through> }` followed by the
 *    two statements produces.  Worth 8 bytes of length by itself.
 *
 * THREE BLOCKERS REMAIN, and they are independent.
 *
 * A. *** GCSE's CONSTANT PROPAGATION FOLDS THE SHARED FLAG ID 0x16a. ***  The ROM
 *    materialises it ONCE per outer iteration as `mov r5,#0xb5 / lsl r5,#1` and
 *    keeps it in r5 across five calls, feeding both `_SetFlag` and `_ClearFlag`
 *    from `mov r0,r5`.  gcc rebuilds `mov r0,#0xb5 / lsl r0,#1` at each use.
 *    THIS ONE IS PROVEN TO BE GCSE: compiled `-fno-gcse` this same file emits
 *    `mov r5,#181 / lsl r5,r5,#1` and both `adds r0,r5,#0`, exactly the ROM.
 *    cprop is entitled to do it -- rtx_cost of a CONST_INT is 0, so the constant
 *    always looks cheaper than the register -- and no spelling tried defeats it:
 *    `f = 0xb5; f <<= 1;` as two statements, hoisting `f` out of the outer loop,
 *    and dropping the local entirely are all identical.  A source route would
 *    have to make the value not a constant at cprop time.
 * B. 0x80 IS HOISTED OUT OF THE OUTER LOOP INTO r8.  The mask appears three times
 *    (`gKeyHeld & 0x80` at the top, `gKeyRepeat & 0x80` for +10, `gKeyHeld & 0x80`
 *    before `_SetFlag(0x16c)`) and loop.c lifts it to a callee-saved register,
 *    costing two insns at the top and turning `movs r2,#0x80` into `mov r2,r8`.
 *    The ROM materialises it twice inside the loop (r2 at the top test, r1 in the
 *    poll chain) and lets the third use share r1 -- i.e. plain in-block cse, no
 *    hoist.  Still hoisted under -fno-gcse, so this is loop invariant motion, a
 *    different pass from A.
 * C. THE SECOND EWRAM ADDRESS IS FOLDED INTO A SECOND POOL WORD.  The ROM holds
 *    `ewram_200046b` in r9 and derives the other address as r9 - 0x55 into r11
 *    (`mov r3,#0x55 / neg r3,r3 / add r3,r9` -- the negate is forced because r9
 *    is a HIGH register and thumb has no high-register `sub`).  That proves the
 *    two addresses were ONE value and an offset at RTL time.  Written as
 *    `p = ewram_200046b; q = (unsigned short *)(p - 0x55);` gcc folds
 *    symbol-minus-constant into a single `symbol-0x55` pool word, so both
 *    addresses are separate `ldr`s and only one high register is spent.  Written
 *    as the two absolute CONST_INT addresses (0x0200046b, 0x02000416) gcc does
 *    not hoist either one out of the loop at all -- 127 of 162 at 384 bytes, and
 *    the relocation for the pool word goes away, so that spelling is worse twice
 *    over.  0x02000416 has no name in wram.sym, and ADDING one would make things
 *    worse, not better: two symbols give two pool words, which is precisely what
 *    the ROM does not have.
 *
 * NO SHIM OF ANY KIND IS USED IN THIS FILE.  No .sym entry is required.
 *
 * THE .s HOLDS FOUR FUNCTIONS (Debug_LoadPresetParty, Func_80b5534,
 * Debug_BattleTest, Func_80b5864) and NO data, so landing any one of them needs
 * tools/split_s.py.
 *
 * Verify with (run it inside the build container; /opt/gcc296 is not on the host):
 *   python3 tools/objcmp.py scratch_elev/b290/A/Debug_BattleTest.park.c \
 *     asm/rom_b5000/rom_b5368_a.s --func Debug_BattleTest
 */
extern volatile int gKeyRepeat;
extern int gKeyHeld;
extern unsigned char ewram_200046b[];

extern void _GameInit(void);
extern void Func_800479c(void);
extern void ClearVRAM(void);
extern void ClearTasks(void);
extern void ClearHeap(void);
extern void ClearSprites(void);
extern void _SetFlag(int f);
extern void _ClearFlag(int f);
extern int WaitFrames(int frames);
extern void Func_80b5534(void);
extern void Func_80c2a08(void);
extern int Debug_LoadPresetParty(int n);
extern void _CalcStats(int id);
extern void BattleMain(int id);

void Debug_BattleTest(void)
{
	int mode;
	int id;
	int preset;
	int last;
	int f;
	unsigned char *p;
	unsigned short *q;

	mode = 0;
	_GameInit();
	for (;;) {
		f = 0x16a;
		Func_800479c();
		ClearVRAM();
		ClearTasks();
		ClearHeap();
		ClearSprites();
		_SetFlag(f);
		id = 0x101;
		if (gKeyHeld & 0x80) {
		last = -1;
		_ClearFlag(f);
		p = ewram_200046b;
		q = (unsigned short *)(p - 0x55);
		preset = 0;
		for (;;) {
			_ClearFlag(0x20);
			WaitFrames(1);
			for (;;) {
				if (gKeyRepeat & 0x10)
					id++;
				if (gKeyRepeat & 0x20)
					id--;
				if (gKeyRepeat & 0x40)
					id -= 10;
				if (gKeyRepeat & 0x80)
					id += 10;
				if (gKeyRepeat & 0x100)
					preset++;
				if (gKeyRepeat & 0x200)
					preset--;
				if (gKeyRepeat & 1)
					break;
				if (gKeyRepeat & 8)
					Func_80b5534();
				if (gKeyRepeat & 4)
					Func_80c2a08();
				if ((gKeyRepeat & 2) || mode != 0) {
					mode = 1;
					p[0] = 5;
				}
				if (preset != last) {
					_GameInit();
					Debug_LoadPresetParty(preset);
					last = preset;
				}
				WaitFrames(1);
			}
			if (gKeyHeld & 0x80)
				_SetFlag(0x16c);
			_CalcStats(0);
			*q = 0x1d;
			if (id == 0x1c)
				_SetFlag(0x16e);
			_SetFlag(0x162);
			BattleMain(id);
			Func_800479c();
			ClearVRAM();
			ClearTasks();
			ClearHeap();
			ClearSprites();
		}
		}
		_SetFlag(0x162);
		BattleMain(0x101);
	}
}
