/* Func_80b1a14  --  0x080b1a14, FIRST of the two functions in
 * asm/rom_b0000/rom_b0070_a_a_c_c_c_c_a.s.  EXACT.
 *   OK Func_80b1a14 -- 444 bytes, 193 encodings and 27 relocations identical
 * It is STILL exact when compiled in the combined two-function TU
 * (scratch_elev/b290/E/whole_accca.c: `ok Func_80b1a14  193 encodings`), so the
 * file only needs a SPLIT because its mate Func_80b1bd0 is not closed yet
 * (28 of 238 -- see Func_80b1bd0.park.c).  datacheck.py: NO DATA SECTION in this
 * .s, so the landing is a pure text change.
 *
 * No pins, no flags, no volatile on struct memory, no new symbols.
 *
 * THE ONE NON-OBVIOUS LEVER: `i = 0;` MUST BE WRITTEN BEFORE THE FIRST
 * _CreateUIBox, EVEN THOUGH THE ROM'S `mov r7, #0` SITS JUST BEFORE
 * `str r7, [sp]` AFTER _Func_80a1870.  12 of 193 -> 0.  Reading it:
 *   - `s` (r6) conflicts with hard reg 5 because the shared 5th argument
 *     constant 2 lives in r5 across the three prologue _CreateUIBox calls;
 *   - with `i = 0` written late, `i` does NOT conflict with r5, is allocated
 *     third (after the gKeyPress pointer and `s`), and takes r5 -- leaving r7
 *     for the gKeyRepeat address pointer;
 *   - written early, `i` is live across those calls, inherits the r5 conflict,
 *     and the two swap to the ROM's i=r7 / gKeyRepeat-ptr=r5.
 * `;; 8 regs to allocate: 111 32 35 152 33 36 34 101` in the .18.greg dump is
 * where this is visible: 111 is the gKeyPress pointer (r1, crosses no call),
 * 32 is `s`, 35 is `i`, 152 is the gKeyRepeat pointer.
 *
 * SECOND LEVER: `i--;` BEFORE `redraw = 1;` in both gKeyRepeat arms (14 -> 12).
 * The ROM's `mov r4,#1 / sub r7,#1 / mov r9,r4` puts the decrement between the
 * constant's birth and its use, which is source order, not scheduling.
 *
 * MEASURED INERT: all four declaration-order permutations of the five locals
 * (all register-resident -- batch 287's rule holds); a named `n = s->count`
 * local; `i = i - 1` / `i = i + 1`; a named `void *nul = 0` for the
 * _Func_80a1870 5th argument; and `((unsigned char *)s->spr)[5] = 4` instead of
 * the struct cast (the alias-set lever does NOT bite here).
 *
 * `volatile` on gKeyPress/gKeyRepeat IS required: 54 of 193 without it.
 *
 * `ldrsb r1, [r3, r1]` for s->count comes for free from reading the signed byte
 * straight into the int expression `(i + s->count) % s->count`, which is
 * docs/elevation.md's "Two signed byte reads, two instruction sequences"
 * confirmed at a new site -- and the SAME function reads s->lang at +0x3aa as
 * `ldrb / lsl #24 / asr #24` because that one is compared, not summed.
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

extern struct State *iwram_3001f2c;
extern volatile int gKeyPress;
extern volatile int gKeyRepeat;

extern void *_CreateUIBox(int a, int b, int c, int d, int e);
extern void _CloseUIBox(void *h, int n);
extern void Func_80b10cc(void);
extern void _Func_80a1870(void *box, int a, int b, int c, void *d);
extern void _Func_80a195c(void);
extern void Func_80b0a6c(void *box, int x, int y);
extern void Func_80b11c4(void *box, int i, int z);
extern void Func_80b1dec(void *box, int unit);
extern int _FindEmptyInventorySlot(int unit);
extern void _PlaySound(int sfx);
extern void WaitFrames(int n);
extern int Func_80b1bd0(int unit);
extern int Func_80b211c(int unit);

int Func_80b1a14(void)
{
	struct State *s;
	void *box1;
	int redraw;
	int i;
	int cur;

	s = iwram_3001f2c;
	redraw = 1;
	cur = 0;
	i = 0;
	s->box0c = _CreateUIBox(0, 9, 0xc, 4, 2);
	Func_80b10cc();
	s->box20 = _CreateUIBox(0x10, 0xc, 0xe, 8, 2);
	box1 = _CreateUIBox(0, 0xe, 0xd, 3, 2);
	((struct Spr *)s->spr)->f5 = 4;
	s->mode = 0xc;
	_Func_80a1870(box1, 2, 0, 8, 0);
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
		if ((gKeyPress & 1) != 0) {
			WaitFrames(1);
			if (_FindEmptyInventorySlot(cur) == 0) {
				_PlaySound(0x71);
				continue;
			}
			_PlaySound(0x70);
			if (s->lang == 1)
				Func_80b1bd0(cur);
			else
				Func_80b211c(cur);
			((struct Spr *)s->spr)->f5 = 4;
			s->mode = 0xc;
			redraw = 1;
		} else if ((gKeyPress & 2) != 0) {
			_PlaySound(0x71);
			_Func_80a195c();
			_CloseUIBox(box1, 2);
			_CloseUIBox(s->box20, 2);
			_CloseUIBox(s->box0c, 2);
			WaitFrames(1);
			return 0;
		} else {
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
			WaitFrames(1);
		}
	}
}
