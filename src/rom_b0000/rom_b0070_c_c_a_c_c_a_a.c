/* WHOLE-FILE CONVERSION of asm/rom_b0000/rom_b0070_c_c_a_c_c_a_a.s -- both of its
 * functions, no split and no linker change.  Whole-file object compare:
 * cand 1024 bytes / ref 1024 bytes, TEXT BYTE-IDENTICAL, RELOCATIONS IDENTICAL.
 *
 *   UI_Sanctum      145 insns   360 bytes  152 encodings /  24 relocations
 *   Func_80b2b10    276 insns   664 bytes  284 encodings /  45 relocations
 *
 * It needs _MSG_d27, WHICH THIS COMMIT ADDS to message.sym -- it COMPLETES this
 * function and this file, which is the bar batch 280's three withheld entries
 * failed.  _MSG_d24 was already present.  No pins, no volatile on struct memory,
 * no barriers, no scaffolding of any kind: the only non-plain constructs are the
 * two _MSG_* references and one struct cast whose entire purpose is an alias set.
 *
 * ================================================================
 * AN ALIAS SET OF 0 ON A QImode STORE BLOCKS EVERY LATER LOAD FROM SCHEDULING
 * ABOVE IT -- AND A STRUCT-MEMBER BYTE STORE DOES NOT HAVE ONE, WHILE
 * `char *p; p[5] = 4;` DOES
 * ================================================================
 *
 * This was the last 4 encodings of Func_80b2b10 and NO SPELLING HUNT FINDS IT.
 * It was found by reading alias.c.  true_dependence is:
 *
 *     if (DIFFERENT_ALIAS_SETS_P (x, mem))  return 0;    // both must be non-zero
 *     ...
 *     if (mem_mode == QImode || GET_CODE (mem_addr) == AND)  return 1;
 *
 * so A QImode STORE WITH ALIAS SET 0 IS AN UNCONDITIONAL TRUE DEPENDENCE for any
 * load analysed after it -- aliasing is never consulted at all.  And
 * lang_get_alias_set in c-common.c returns 0 for ANY reference whose type is an
 * INTEGER_TYPE of char precision, so `s->spr[5] = 4` through an `unsigned char *`
 * gets set 0.  The SAME BYTE WIDTH through a struct member gets a real set --
 * verified in .19.flow2 on this function:
 *
 *   (set (mem:QI   (plus (reg r2) (const_int 5)) 0)   <- char* deref,  set 0
 *   (set (mem/s:QI (reg r3)                     10)   <- s->mode,      set 10
 *   (set (reg:QI r1) (mem:QI (reg r1)           18))  <- int spill,    set 18
 *
 * Changing `unsigned char *spr; s->spr[5] = 4;` to a `struct Spr *` cast and
 * `->f5 = 4` took the function 4 -> 0 and let sched2 hoist the
 * `add r1, sp, #4 / ldrb r1, [r1]` pair above the store, which is the ROM's order.
 * NINE other spellings all sat at 4: an `int t` split across the store, an
 * `unsigned char t`, `*(unsigned char *)&redraw` (that one costs `sub sp, #0x10`
 * and 245 differing), a named `unsigned char *rp`, a named
 * `unsigned char *p = s->spr`, and swapping the two statements (13 differing).
 * THE VARIABLE THAT MATTERED WAS THE TYPE OF THE STORE, NOT THE ORDER OF THE
 * STATEMENTS.
 *
 * ================================================================
 * A SHARED HARD REGISTER IS EVIDENCE OF DISJOINT LIVE RANGES, NOT OF A SHARED
 * VARIABLE -- and getting this backwards cost a whole candidate
 * ================================================================
 *
 * The ROM uses r8 for BOTH the `again` flag (=0, =1, cmp #0) and the 0xd27 message
 * base.  Writing that as ONE variable, reasoning from the shared register, is
 * exactly backwards: two pseudos with disjoint ranges share a hard register FOR
 * FREE, and merging them in the source inflated `again`'s refs and dropped it
 * below `cur` in allocno_compare.  Separating `int base;` from `int again;` moved
 * r8/r9/r10 from (cur, again, lang) to the ROM's (again, lang, cur) and took the
 * function from 34 instructions in disagreeing regions to 4, with no other change.
 * BATCH 280'S MERGE-LIVE-RANGES RULE NEEDS THIS CONVERSE STATED BESIDE IT.
 *
 * REG_ALLOC_ORDER FOR THE HIGH REGISTERS IS 8, 10, 9, 11 -- NOT ASCENDING.  From
 * arm.h in the image:
 *
 *     #define REG_ALLOC_ORDER { 3, 2, 1, 0, 12, 14, 4, 5, 6, 7, 8, 10, 9, 11, ... }
 *
 * So the highest-priority high-reg allocno gets r8, the second r10, the THIRD r9,
 * the fourth r11.  HANDOFF.md quotes this list truncated at "... 8, 10, ..." and
 * THE 9 AFTER THE 10 IS THE LOAD-BEARING PART -- without it a correct priority
 * reading looks self-contradictory.
 *
 * allocno_compare, exact, from global.c:
 * (double)(floor_log2(n_refs) * n_refs) / live_length * 10000 * size, ties on
 * allocno number.  n_refs and live_length are the ALLOCNO's sums (an allocno may
 * cover several pseudos) and n_refs is loop-depth-weighted -- so the numbers to
 * use are NOT the raw .12.life "used N times across M insns" line for one pseudo.
 *
 * THE TWO ROM STACK SLOTS ARE ORDINARY SPILLED SCALARS, NOT AN ARRAY OR STRUCT.
 * `str r1,[sp,#4]` / `add r1,sp,#4 / ldrb r1,[r1]` LOOKS like an addressable
 * aggregate; `int v[2]` makes gcc keep the constant 1 in callee-saved r6 across
 * both calls and fold it into the byte store (60-of-152 class wrong).  Plain
 * `int redraw; void *box2;` with all four callee-saved high registers already
 * spoken for makes reload spill them, and (subreg:QI (mem:SI sp+4)) simplifies to
 * (mem:QI sp+4) -- which Thumb cannot address off sp in QImode, hence the
 * `add r1, sp, #4`.  DECLARATION ORDER FIXES WHICH SLOT EACH GETS: `box2` first
 * gives the ROM's sp+8/sp+4; reversed, all six slot references invert.
 *
 * Smaller confirmations: branch polarity gave the if/else arm order;
 * `extern volatile int gKeyPress` is required for the second read (the tree's
 * established idiom); `extern GlobalState gState` as a STRUCT rather than
 * `unsigned char gState[]` keeps `ldr r3, =gState / ldr r3, [r3, #0x10]` instead
 * of folding to `=gState+16`; and `i = 0;` as a statement with an empty for-init
 * put the counter's zero ahead of cur/again and saved a redundant `mov r3, #0`.
 *
 * No per-file Makefile flag override applies to this stem; every flag probe was
 * inert or worse.
 */
struct Spr {
	unsigned char pad00[4];
	unsigned char f4;
	unsigned char f5;
};

struct State {
	unsigned char pad000[0xc];
	void *box2;
	unsigned char pad010[0x36e - 0x10];
	short items[9];
	void *spr;
	unsigned char pad384[0x390 - 0x384];
	unsigned short f390;
	unsigned char pad392[0x3a4 - 0x392];
	unsigned short f3a4;
	unsigned char pad3a6[1];
	signed char count;
	unsigned char mode;
	unsigned char pad3a9;
	signed char lang;
};

typedef struct { unsigned char pad00[0x10]; unsigned int f10; unsigned char pad14[0x2ac]; } GlobalState;

extern struct State *iwram_3001f2c;
extern GlobalState gState;
extern volatile int gKeyPress;
extern volatile int gKeyRepeat;
extern int _MSG_d24;
extern int _MSG_d27;

extern void Func_80b010c(void);
extern void Func_80b0204(void);
extern void Func_80b10cc(void);
extern void Func_80b28d4(unsigned int arg0);
extern void Func_80b2928(unsigned int arg0);
extern void Func_80b0a20(void *p, int a, int b);
extern int Func_80b280c(void);
extern int Func_80b27b0(int item, int lang);
extern int Func_80b2778(int item, int lang);
extern void Func_80b0a6c(void *box, int x, int y);
extern void Func_80b2e30(void *box, int sel);
extern void Func_80b2ed8(void *box, int item);
extern void Func_80b2da8(int item, int lang);
extern void Func_80b3050(int sel);
extern int Func_80b0664(int a);
extern int _SanctumMenu(int sel);
extern unsigned char *_MapActor_GetActor(int id);
extern void *_Func_8019da8(int a, int b, int c, int d);
extern void *_CreateUIBox(int a, int b, int c, int d, int e);
extern void _CloseUIBox(void *h, int n);
extern void *_Func_801eadc(int a, int b, void *c, int d, int e);
extern void _Func_801ec6c(int a, int b, int c, void *d, int e, int f);
extern void _Func_80a1870(void *box, int a, int b, int c, void *d);
extern void _Func_80a195c(void);
extern void _Func_8019908(int a, int b);
extern void _Func_8019a54(void);
extern void _AddCoins(int n);
extern void _PlaySound(int sfx);
extern void WaitFrames(int n);

int Func_80b2b10(void);

int UI_Sanctum(int arg0)
{
	struct State *s;
	unsigned char *a;
	void *box;
	unsigned char *e;
	int sel;

	sel = 0;
	box = 0;
	Func_80b010c();
	s = iwram_3001f2c;
	s->lang = 0;
	a = _MapActor_GetActor(arg0);
	s->f3a4 = **(unsigned short **)(*(int *)(a + 0x50) + 0x28);
	box = _Func_8019da8(s->f3a4, 0, 0, 0);
	if (box == 0) {
		box = _CreateUIBox(-5, 0, 5, 5, 2);
		if (box == 0) {
			box = _CreateUIBox(0, 0, 5, 5, 2);
			_Func_801ec6c(2, 0, 0, box, -4, -4);
		}
	}
	e = (unsigned char *)_Func_801eadc(s->f390, 0x80 << 23, box, 0, 0);
	e[5] = 1;
	e[4] = 0;
	Func_80b0a20(&s->spr, -0x20, 0x70);
	s->spr = e;
	Func_80b28d4(0xd21);
	s->box2 = _CreateUIBox(0x10, 0xb, 0xc, 4, 2);
	Func_80b10cc();
	while (1) {
		sel = _SanctumMenu(sel);
		s->lang = sel;
		if (sel == -1)
			break;
		Func_80b28d4((int)&_MSG_d24);
		if (Func_80b280c() == 0)
			Func_80b28d4((int)&_MSG_d24 + 1);
		else
			Func_80b2b10();
		s->lang = 0;
		Func_80b0a20(&s->spr, -0x20, 0x70);
		Func_80b28d4(0xd22);
	}
	Func_80b28d4(0xd23);
	_CloseUIBox(s->box2, 2);
	_CloseUIBox(box, 2);
	Func_80b0204();
	return 0;
}

int Func_80b2b10(void)
{
	struct State *s;
	void *box2;
	int redraw;
	void *box1;
	int lang;
	int again;
	int base;
	int i;
	int cur;
	int price;

	s = iwram_3001f2c;
	box2 = 0;
	redraw = 1;
	lang = s->lang;
	Func_80b28d4(0xd26);
	box1 = _CreateUIBox(1, 0xc, 0xd, 3, 2);
	((struct Spr *)s->spr)->f5 = 4;
	s->mode = redraw;
	_Func_80a1870(box1, 2, 0, 8, box2);
	box2 = _CreateUIBox(1, 0x10, 0x17, 3, 2);
	i = 0;
	cur = 0;
	again = 0;
	for (; i < s->count; i++) {
		cur = s->items[i];
		if (Func_80b27b0(cur, lang) != 0)
			break;
	}
	redraw = 1;
	while (1) {
		if (again != 0) {
			again = 0;
			Func_80b28d4(0xd26);
			redraw = 1;
			for (i = 0; i < s->count; i++) {
				cur = s->items[i];
				if (Func_80b27b0(cur, lang) != 0)
					break;
			}
		}
		if (redraw != 0) {
			redraw = 0;
			i = (i + s->count) % s->count;
			cur = s->items[i];
			Func_80b0a6c(box1, i * 24 - 12, 0);
			s->mode = 3;
			Func_80b2e30(box1, i);
			Func_80b2ed8(box2, cur);
		}
		if ((gKeyPress & 1) != 0) {
			WaitFrames(1);
			price = Func_80b2778(cur, lang);
			if (Func_80b27b0(cur, lang) == 0) {
				_PlaySound(0x71);
				continue;
			}
			_Func_8019908(cur, 1);
			_Func_8019908(price, 5);
			base = (int)&_MSG_d27;
			Func_80b28d4(base);
			if (Func_80b0664(0) != 0) {
				Func_80b2928(base + 2);
				again = 1;
				continue;
			}
			if ((unsigned int)price > gState.f10) {
				_PlaySound(0x71);
				Func_80b2928(base + 1);
				again = 1;
				continue;
			}
			_Func_8019908(cur, 1);
			Func_80b28d4(base + 3);
			_Func_8019a54();
			Func_80b2da8(cur, lang);
			Func_80b3050(i);
			_AddCoins(-price);
			Func_80b10cc();
			_Func_8019908(cur, 1);
			Func_80b28d4(base + 4);
			if (Func_80b280c() == 0)
				break;
			again = 1;
		} else if ((gKeyPress & 2) != 0) {
			_PlaySound(0x71);
			break;
		} else {
			if ((gKeyRepeat & 0x20) != 0) {
				_PlaySound(0x6f);
				redraw = 1;
				i--;
			}
			if ((gKeyRepeat & 0x10) != 0) {
				_PlaySound(0x6f);
				redraw = 1;
				i++;
			}
			WaitFrames(1);
		}
	}
	_Func_80a195c();
	_CloseUIBox(box2, 2);
	_CloseUIBox(box1, 2);
	WaitFrames(1);
	return 0;
}
