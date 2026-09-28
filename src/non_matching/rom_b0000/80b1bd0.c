/* Func_80b1bd0 -- 0x080b1bd0, SECOND of two in asm/rom_b0000/rom_b0070_a_a_c_c_c_c_a.s.
 *
 * NON-MATCHING: 28 encodings of 238 differ (objcmp).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py <this> \
 *     asm/rom_b0000/rom_b0070_a_a_c_c_c_c_a_c.s --func Func_80b1bd0
 *
 * SIZE AND RELOCATION COUNT ARE ALREADY RIGHT IN KIND: 540 bytes both, 238
 * encodings both.  THE 28 ARE ONE MISSING INSTRUCTION PLUS ITS 2-BYTE SHIFT.
 * The reference holds 232 instructions, this candidate 231, and a normalised
 * text diff of the two bodies differs in exactly two lines:
 *
 *     ref    bl Func_80b1e80 / mov r3,#1 / mov r2,r0 / neg r3,r3 / cmp r2,r3
 *     ours   bl Func_80b1e80 / mov r3,#1 /             neg r3,r3 / cmp r0,r3
 *
 * Everything after 0x1cc therefore shifts by two bytes, which is what inflates
 * the count; the relocation list is identical in symbol and order and differs
 * only by that 2 from `Func_80b1f4c` onward.
 *
 * NAMED BLOCKER: local-alloc pass 1 COALESCES THE CALL-RESULT COPY.
 * `(set (reg 42) (reg 0 r0))` is present and correct in .17.lreg (insn 547) and
 * reg 42 is confined to one basic block, so block_alloc handles it; the copy
 * gives its quantity a `qty_phys_copy_sugg` of r0, `find_free_reg
 * (just_try_suggested = 1)` succeeds because r0 dies at that very copy, and
 * flow2 deletes the resulting `mov r0, r0`.  For the ROM's r2 the quantity has
 * to MISS pass 1 -- either by crossing a call (which excludes all of
 * call_used_reg_set in pass 1, so the accept_call_clobbered retry walks
 * REG_ALLOC_ORDER {3,2,1,0,...}, finds r3 already holding the -1, and lands on
 * r2) or by spanning basic blocks so global_alloc owns it and never sees the
 * copy suggestion.  Neither is reachable from a source shape that emits the
 * ROM's other 231 instructions.
 *
 * NO FLAG REACHES IT.  -fno-regmove, -fno-gcse, -fno-cse-follow-jumps,
 * -fno-rerun-cse-after-loop, -fno-schedule-insns2 and -fno-strength-reduce all
 * leave 231 instructions and `cmp r0, r3`.  -fno-expensive-optimizations gives
 * 233 and still no copy; -fno-omit-frame-pointer gives 285.
 *
 * INERT, thirteen spellings, all at 28 and all still 231 instructions: no local
 * at all; `if (-1 != r)`; `if (!(r == -1))`; `if (r == -1) {} else ...`;
 * `(unsigned)r != 0xffffffff`; a named `m1 = -1` compared against; a
 * `do { } while (0);` between the call and the test; a block-scoped `int q`;
 * `if (r == -1) goto noset;` with a label; and reusing `r` for the following
 * _FindEmptyInventorySlot result.  PER docs/elevation.md's "N spellings tie is
 * only evidence if the spellings differ in STRUCTURE", these are all
 * assign-then-compare and so are NOT yet evidence that the shape is
 * unreachable.  Two that DO differ structurally were worse and are wrong:
 * `if (r >= 0)` (236 encodings, 4 bytes short) and a trailing `gKeyPress;`
 * volatile read (240 encodings, 4 bytes long).
 *
 * WHAT IS ALREADY LANDED HERE, and worth keeping if this is reopened:
 *  - DECLARATION ORDER `struct State *s; void *boxB; void *boxA; Unit *u;` is
 *    the SLOT LEVER and is worth 34 -> 28: later declaration takes the LOWER
 *    slot, so u must be declared last of the three to reach the ROM's
 *    u=sp+8 / boxA=sp+0xc / boxB=sp+0x10 / unit=sp+0x14.  This is the first
 *    measurement of the direction of that rule in this bank.
 *  - `item + (int)&_MSG_75` is the POOLED-SMALL-CONSTANT tell: 0x75 fits
 *    `add r6, #0x75` as a literal, so `ldr r3, =0x75 / add r6, r3` proves the
 *    operand is a SYMBOL.  `_MSG_75 = 0x0075` ALREADY EXISTS in
 *    message.sym:220 -- NO NEW .sym ENTRY.
 *  - VERIFICATION SHIM, THIS ONE LINE AND NOT PART OF ANY LANDING:
 *    `__asm__(".equ _MSG_75, 0x75");`  It only makes the assembler fold the
 *    pool word in a standalone objcmp run instead of leaving an R_ARM_ABS32
 *    (the reference is hand-written asm and has a plain word there).  The same
 *    situation was landed for _MSG_182 in src/rom_b0000/rom_b0070_c_c_c_c_c_b.c.
 *  - the four gKeyRepeat arms, the two `while` wrap loops, and the whole outer
 *    re-create-box loop are byte-exact as written.
 *
 * ITS FILE-MATE Func_80b1a14 IS EXACT and stays exact in the combined TU, so
 * closing this one instruction converts asm/rom_b0000/rom_b0070_a_a_c_c_c_c_a.s
 * WHOLE with no split and no linker change (datacheck.py: no data section).
 */
struct Spr {
	unsigned char pad00[4];
	unsigned char f4;
	unsigned char f5;
};

struct State {
	unsigned char pad000[0xc];
	void *box0c;
	unsigned char pad010[0x20 - 0x10];
	void *box20;
	unsigned char pad024[0x36e - 0x24];
	short items[9];
	void *spr;
	unsigned char pad384[0x3a7 - 0x384];
	signed char count;
	unsigned char mode;
	unsigned char pad3a9;
	signed char lang;
};

typedef struct { unsigned char pad00[0xd8]; unsigned short items[15]; } Unit;

extern struct State *iwram_3001f2c;
extern volatile int gKeyPress;
extern volatile int gKeyRepeat;
extern int _MSG_75;
/* VERIFICATION SHIM (objcmp only; _MSG_75 = 0x0075 ALREADY EXISTS in message.sym:220) */
__asm__(".equ _MSG_75, 0x75");

extern Unit *_GetUnit(int unit);
extern void *_CreateUIBox(int a, int b, int c, int d, int e);
extern void _CloseUIBox(void *h, int n);
extern int _FindEmptyInventorySlot(int unit);
extern void Func_80b0a6c(void *box, int x, int y);
extern int Func_80b19cc(int raw);
extern void Func_80b110c(void *box, int item, int val, int n);
extern void Func_80b11a4(void *box, int msg);
extern int Func_80b1e80(int unit, int slot);
extern void Func_80b1f4c(int unit, int slot);
extern void Func_80b04dc(int msg);
extern void _PlaySound(int sfx);
extern void WaitFrames(int n);

int Func_80b1bd0(int unit)
{
	struct State *s;
	void *boxB;
	void *boxA;
	Unit *u;
	int i;
	int cnt;
	int redraw;
	int ret;
	int item;
	int r;

	s = iwram_3001f2c;
	u = _GetUnit(unit);
	cnt = 1;
	boxA = _CreateUIBox(0xf, 8, 0xf, 4, 2);
	i = 0;
	while (1) {
		boxB = _CreateUIBox(0, 5, 0x1e, 3, 2);
		((struct Spr *)s->spr)->f5 = 0x12;
		s->mode = 0xc;
		redraw = 1;
		while (1) {
			if (redraw != 0) {
				redraw = 0;
				cnt = _FindEmptyInventorySlot(unit);
				if (i > cnt - 1)
					i = cnt - 1;
				item = u->items[i] & 0x1ff;
				Func_80b0a6c(s->box20, (i % 5) * 16, (i / 5) * 16 + 8);
				s->mode = 3;
				Func_80b110c(boxA, item, Func_80b19cc(u->items[i]), 1);
				Func_80b11a4(boxB, item + (int)&_MSG_75);
			}
			if ((gKeyPress & 1) != 0) {
				_PlaySound(0x70);
				ret = 0;
				break;
			}
			if ((gKeyPress & 2) != 0) {
				_PlaySound(0x71);
				ret = -1;
				break;
			}
			if ((gKeyRepeat & 0x20) != 0) {
				_PlaySound(0x6f);
				i--;
				i = (i + cnt) % cnt;
				redraw = 1;
			}
			if ((gKeyRepeat & 0x10) != 0) {
				_PlaySound(0x6f);
				i++;
				i = (i + cnt) % cnt;
				redraw = 1;
			}
			if ((gKeyRepeat & 0x40) != 0) {
				i -= 5;
				if (i < 0)
					i += 0xf;
				while (i >= cnt)
					i -= 5;
				_PlaySound(0x6f);
				redraw = 1;
			}
			if ((gKeyRepeat & 0x80) != 0) {
				i += 5;
				if (i >= cnt)
					i -= 0xf;
				while (i < 0)
					i += 5;
				_PlaySound(0x6f);
				redraw = 1;
			}
			WaitFrames(1);
		}
		_CloseUIBox(boxB, 2);
		WaitFrames(1);
		if (ret != 0)
			break;
		r = Func_80b1e80(unit, i);
		if (r != -1)
			Func_80b1f4c(unit, i);
		Func_80b04dc(0xcaa);
		if (_FindEmptyInventorySlot(unit) == 0)
			break;
	}
	_CloseUIBox(boxA, 2);
	return ret;
}
