/* WHOLE-FILE CONVERSION of asm/rom_b0000/rom_b0070_c_c_c_c_c_a.s -- BOTH of its
 * functions, NO SPLIT, NO LINKER CHANGE, NO DATA SECTION (datacheck.py).
 * Whole-file object compare (with the shim below):
 *   OK whole file -- 1064 bytes, 470 encodings and 57 relocations identical
 *     ok UI_SellMenu     217 encodings
 *     ok Func_80b362c    253 encodings
 *
 * `_MSG_75 = 0x0075` ALREADY EXISTS in message.sym:220.  NO NEW .sym ENTRY.
 * The `__asm__(".equ _MSG_75, 0x75");` line is a VERIFICATION SHIM for a
 * standalone objcmp run only (it makes the assembler fold the pool word instead
 * of leaving an R_ARM_ABS32).  REMOVE IT BEFORE LANDING -- this file is the
 * landing form and does not carry it.
 *
 * LEVERS (drop ladder in the batch report):
 *  - `i = 0;` WRITTEN BEFORE THE FIRST _CreateUIBox, not after _Func_80a1870,
 *    is what spills `ret` to sp+4: 205 of 217 -> 5.  Keeping `i` live across the
 *    three prologue calls makes it the 8th callee-saved-wanting value, and reload
 *    spills `ret`, which is why `e->f4 = ret` is read back as
 *    `add r0, sp, #4 / ldrb r0, [r0]`.  gcc still SCHEDULES the `mov r7, #0` down
 *    to just before `str r7, [sp]`, so the ROM's late zero is not evidence that
 *    the source assigns it late.
 *  - THE INIT ORDER `redraw = 1; ret = 0; cur = 0; i = 0;` is the last 5:
 *    6 of the 24 permutations are exact, and every exact one has `redraw = 1`
 *    before both zeros with `i = 0` not between them.  It decides which of r0/r2
 *    carries the CSE'd 0 and which the 1.
 *  - `else if (ret == -3)` IN Func_80b362c IS THE ORIGINAL'S OWN BUG AND IS
 *    LOAD-BEARING.  The ROM reads the spilled `ret` slot (`ldr r3, [sp, #4]`)
 *    for the -3 test while using r0 for the == 0 and == -4 tests, so the third
 *    test is on `ret` (still 0 there), not on the _CanRemoveItem result.
 *    Spelling it `r == -3` gives 236 of 253 AND THE WRONG SIZE.
 *  - `volatile` on gKeyPress/gKeyRepeat is required (54 of 193 without, measured
 *    on the file-mate Func_80b1a14).
 *
 * MEASURED INERT: the `struct Spr *` cast on the byte store is NOT load-bearing
 * at either site -- `((unsigned char *)s->spr)[5] = 0x12;` is also exact.  The
 * struct form is kept for the tree's convention only.  `i = 0;` placement is
 * inert inside Func_80b362c (exact either way); it only matters in UI_SellMenu.
 * Declaration order of the locals is inert in UI_SellMenu (all register-resident).
 */
struct Spr {
	unsigned char pad00[4];
	unsigned char f4;
	unsigned char f5;
};

struct State {
	unsigned char pad000[0x20];
	void *box20;
	unsigned char pad024[0x36e - 0x24];
	short items[9];
	void *spr;
	unsigned char pad384[0x390 - 0x384];
	unsigned short f390;
	unsigned char pad392[0x3a7 - 0x392];
	signed char count;
	unsigned char mode;
};

typedef struct { unsigned char pad00[0xd8]; unsigned short items[15]; } Unit;

extern struct State *iwram_3001f2c;
extern volatile int gKeyPress;
extern volatile int gKeyRepeat;
extern int _MSG_75;

extern void Func_80b010c(void);
extern void Func_80b0204(void);
extern void *_CreateUIBox(int a, int b, int c, int d, int e);
extern void _CloseUIBox(void *h, int n);
extern void *_Func_801eadc(int a, int b, void *c, int d, int e);
extern void Func_80b0a20(void *p, int a, int b);
extern void _Func_80a1870(void *box, int a, int b, int c, int d);
extern void _Func_80a195c(void);
extern void Func_80b0a6c(void *box, int x, int y);
extern void Func_80b11c4(void *box, int i, int z);
extern void Func_80b1dec(void *box, int unit);
extern void Func_80b11a4(void *box, int msg);
extern void Func_80b386c(void *box, int unit, int slot);
extern Unit *_GetUnit(int unit);
extern int _FindEmptyInventorySlot(int unit);
extern int _CanRemoveItem(int unit, int slot);
extern void _Func_8017658(int msg, int a, int b, int c);
extern int _Func_8017364(void);
extern void _Func_8019a54(void);
extern void _PlaySound(int sfx);
extern void WaitFrames(int n);

int Func_80b362c(int unit);

int UI_SellMenu(int *outUnit, int *outSlot)
{
	struct State *s;
	struct Spr *e;
	void *box1;
	int redraw;
	int i;
	int cur;
	int ret;
	int r;

	redraw = 1;
	ret = 0;
	cur = 0;
	i = 0;
	Func_80b010c();
	s = iwram_3001f2c;
	s->box20 = _CreateUIBox(0x10, 0xc, 0xe, 8, 2);
	box1 = _CreateUIBox(0, 0xe, 0xd, 3, 2);
	e = (struct Spr *)_Func_801eadc(s->f390, 0x80 << 23, box1, 0, ret);
	e->f5 = 4;
	e->f4 = ret;
	Func_80b0a20(&s->spr, -0x20, 0x70);
	s->spr = e;
	s->mode = 0xc;
	_Func_80a1870(box1, 2, 0, 8, ret);
	while (1) {
		if (redraw != 0) {
			redraw = 0;
			i = (i + s->count) % s->count;
			cur = s->items[i];
			Func_80b0a6c(box1, i * 24 - 12, 0);
			s->mode = 3;
			Func_80b11c4(box1, i, 0);
			Func_80b1dec(s->box20, cur);
		}
		WaitFrames(1);
		if ((gKeyPress & 1) != 0) {
			if (_FindEmptyInventorySlot(cur) == 0) {
				_PlaySound(0x71);
				continue;
			}
			_PlaySound(0x70);
			r = Func_80b362c(cur);
			if (r == -1) {
				((struct Spr *)s->spr)->f5 = 4;
				s->mode = 0xc;
				redraw = 1;
				continue;
			}
			*outUnit = cur;
			*outSlot = r;
			ret = 0;
			break;
		}
		if ((gKeyPress & 2) != 0) {
			_PlaySound(0x71);
			*outUnit = -1;
			*outSlot = -1;
			ret = -1;
			break;
		}
		if ((gKeyRepeat & 0x20) != 0) {
			_PlaySound(0x6f);
			i--;
			redraw = 1;
		}
		if ((gKeyRepeat & 0x10) != 0) {
			_PlaySound(0x6f);
			i++;
			redraw = 1;
		}
	}
	_Func_80a195c();
	_CloseUIBox(box1, 2);
	_CloseUIBox(s->box20, 2);
	WaitFrames(1);
	Func_80b0204();
	return ret;
}

int Func_80b362c(int unit)
{
	struct State *s;
	void *boxB;
	void *boxA;
	Unit *u;
	int ret;
	int i;
	int cnt;
	int redraw;
	int item;
	int r;

	s = iwram_3001f2c;
	u = _GetUnit(unit);
	cnt = 1;
	redraw = 1;
	ret = 0;
	boxA = _CreateUIBox(0xe, 8, 0x10, 4, 2);
	boxB = _CreateUIBox(0, 5, 0x1e, 3, 2);
	((struct Spr *)s->spr)->f5 = 0x12;
	s->mode = 0xc;
	i = 0;
	while (1) {
		if (redraw != 0) {
			redraw = 0;
			cnt = _FindEmptyInventorySlot(unit);
			if (i > cnt - 1)
				i = cnt - 1;
			item = u->items[i] & 0x1ff;
			Func_80b0a6c(s->box20, (i % 5) * 16, (i / 5) * 16 + 8);
			s->mode = 3;
			Func_80b386c(boxA, unit, i);
			Func_80b11a4(boxB, item + (int)&_MSG_75);
		}
		WaitFrames(1);
		if ((gKeyPress & 1) != 0) {
			r = _CanRemoveItem(unit, i);
			if (r == 0) {
				_PlaySound(0x70);
				ret = i;
				break;
			}
			if (r == -4)
				_Func_8017658(0xc96, 8, 1, 2);
			else if (ret == -3)
				_Func_8017658(0xc97, 8, 1, 2);
			_PlaySound(0x71);
			while (_Func_8017364() == 0)
				WaitFrames(1);
			_Func_8019a54();
			continue;
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
	}
	_CloseUIBox(boxB, 2);
	_CloseUIBox(boxA, 2);
	WaitFrames(1);
	return ret;
}
