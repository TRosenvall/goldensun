/* Func_80b1bd0 -- 0x080b1bd0, the SOLE member of
 * asm/rom_b0000/rom_b0070_a_a_c_c_c_c_a_c.s.  EXACT: 540 bytes, 238 encodings
 * and 29 relocations identical (objcmp --whole).  Batch 334 brief B.
 *
 * Converted whole; no split, no linker change, no flag group, no pins.
 *
 * THE LEVER WAS A SIBLING'S PROTOTYPE, AND IT WAS ONE ARGUMENT.
 * The park sat at 28 of 238 for five batches with twenty-five spellings of a
 * `r != -1` test measured inert, because it declared
 * `void Func_80b1f4c(int unit, int slot);`.  The real Func_80b1f4c is landed
 * and EXACT in src/rom_b0000/rom_b0070_a_a_c_c_c_c_c_c.c and takes THREE
 * parameters, `(int unit, int slot, int qty)`; a second caller,
 * src/rom_b0000/rom_b0070_a_a_c_c_c_a_c.c, passes the third argument plainly.
 * Writing `Func_80b1f4c(unit, i, r)` closes it.
 *
 * WHY THAT IS EXACTLY THE ONE MISSING INSTRUCTION.  The reference's
 * `adds r2, r0, #0` between the call and the compare is ONE instruction doing
 * TWO jobs.  Passing the result as the third argument puts
 * `(set (reg 2 r2) (reg 42))` in the taken arm, so reg 42 is referenced in two
 * basic blocks and global_alloc owns it with copy preferences {r0, r2}
 * (set_preference, global.c:1650-1667; find_reg tries
 * hard_reg_copy_preferences first, global.c:1067-1074).  r0 now conflicts --
 * the arm reloads `unit` into r0 while reg 42 is still live -- so the
 * preference falls through to r2, the copy is emitted once, and the argument
 * move `mov r2, r2` is deleted as a noop.  reg 42 does not cross the `bl`, so
 * the call_used_reg_set exclusion (global.c:986-990) never fires and r2 stays
 * legal.
 *
 * The park's own account -- that local-alloc pass 1 coalesces the copy and
 * that REG_ALLOC_ORDER would find "r3 already holding the -1" -- could not
 * have been the story: in `.17.lreg` the -1 is still an IMMEDIATE operand of
 * cbranchsi4, and `mov r3,#1 / neg r3,r3` are RELOAD-generated insns that
 * first appear in `.18.greg`.  There is no r3 quantity at allocation time.
 *
 * KEPT FROM THE PARK, both still load-bearing:
 *  - DECLARATION ORDER `struct State *s; void *boxB; void *boxA; Unit *u;` is
 *    the slot lever: later declaration takes the lower slot, which is what
 *    puts u at sp+8, boxA at sp+0xc, boxB at sp+0x10 and unit at sp+0x14.
 *  - `item + (int)&_MSG_75` is the pooled-small-constant tell: 0x75 fits
 *    `add r6, #0x75` as a literal, so `ldr r3, =0x75 / add r6, r3` proves the
 *    operand is a SYMBOL.  `_MSG_75 = 0x0075` is message.sym:362 -- no new
 *    `.sym` entry.  The park carried a one-line assembler-directive shim
 *    equating that symbol to its value; it existed only so a STANDALONE
 *    objcmp run would fold the pool word, and it is NOT part of this landing.
 *    The landed `_MSG_182` precedent in
 *    src/rom_b0000/rom_b0070_c_c_c_c_c_b.c carries no such line either -- its
 *    generated `.s` has a plain `.word _MSG_182` at line 106 and the stage1
 *    partial link resolves it.
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

extern Unit *_GetUnit(int unit);
extern void *_CreateUIBox(int a, int b, int c, int d, int e);
extern void _CloseUIBox(void *h, int n);
extern int _FindEmptyInventorySlot(int unit);
extern void Func_80b0a6c(void *box, int x, int y);
extern int Func_80b19cc(int raw);
extern void Func_80b110c(void *box, int item, int val, int n);
extern void Func_80b11a4(void *box, int msg);
extern int Func_80b1e80(int unit, int slot);
extern void Func_80b1f4c(int unit, int slot, int qty);
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
			Func_80b1f4c(unit, i, r);
		Func_80b04dc(0xcaa);
		if (_FindEmptyInventorySlot(unit) == 0)
			break;
	}
	_CloseUIBox(boxA, 2);
	return ret;
}
