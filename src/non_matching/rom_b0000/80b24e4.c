/* Func_80b24e4 -- NON-MATCHING, 126 encodings of 204, size 480 against the ROM's
 * 484 (-4).  191 instructions.
 *
 * READ THE COUNT WITH THE POOL CAVEAT: ours is ONE INSTRUCTION SHORT, so everything
 * after it is displaced and the raw count is dominated by that.  The normalised,
 * aligned view is 25 INSTRUCTIONS IN DISAGREEING REGIONS OF 202, in three groups.
 * That normalised figure is NOT an objcmp number and must not be quoted as one.
 *
 * Blocker class: global_alloc PRIORITY INVERSION, and it is ARITHMETICALLY
 * UNREACHABLE FROM THIS SOURCE.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_b0000/80b24e4.c \
 *     asm/rom_b0000/rom_b0070_a_c_c_c.s
 * ONE function in the reference -- it CONVERTS WHOLE, no split.
 *
 * TWENTY OF THE 25 ARE A CLEAN TWO-WAY SWAP: the ROM puts the CSE'd `&s->sel`
 * address in r8 and `cur` in r10; we put `cur` in r8 and the address in r10.
 * Everything else about the function is right.
 *
 * WHY IT IS RECORDED AS A CLASS RATHER THAN CHASED.  From allocno_compare with the
 * measured pair -- `cur` 20 refs / 108 insns -> 4*20/108 = 0.741, the address pseudo
 * 4 refs / 29 insns -> 2*4/29 = 0.276 -- flipping the order needs EITHER `cur`'s
 * live length above ~290 instructions (THE WHOLE FUNCTION IS ~200) OR the address
 * pseudo at 8 refs in the same 29-instruction window.  Neither is expressible
 * without changing what the function does.
 *
 * The remaining 5: one missing `mov r1, r0` (the ROM copies _GiveItemTo's result
 * into a low register before `cmp`; with `int r; r = _GiveItemTo(...)` gcc coalesces
 * into r0 -- THE SAME ALLOCATION DECISION), and its 2-byte knock-on.
 *
 * MEASURED, normalised, in disagreeing regions of 202:
 *   as shipped                                                        25
 *   `cur = 0;` after `price = tbl[...]` AND i--/redraw in ROM order    25  <- BOTH
 *       needed; either alone is 29 / 32
 *   redraw = 1 before i--/i++                                          32
 *   cur = 0 / i = 0 left after price                                   29
 *   no `int r` local, call inlined into the if                         25  inert
 *   `cur = 0` deleted                                                  67, 203 lines
 *   i = 0; cur = 0; moved inside the guard                             41
 *   declaration order `cur` first                                      25  inert
 *   named `unsigned short *p = &s->sel` for the three entry accesses   84, 205 lines
 *   `if (price > coins) return;` early-return form                     25  inert
 *   price/item declaration order reversed                              36  <- this is
 *       what assigns sp+0 against sp+4
 *   -fno-gcse / -fno-strict-aliasing / -fno-rerun-cse-after-loop       25  all inert
 *   -fno-schedule-insns2                                               53
 *   -fno-expensive-optimizations                                       59
 *
 * ================================================================
 * SYMBOL TELL -- REPORTED, NOT ADDED, AND EXPLICITLY THE WEAKER OF A PAIR
 * ================================================================
 *
 *     _MSG_cc3 = 0x0cc3;
 *
 * The ROM does `ldr r5, =0xcc3 / mov r0, r5 / bl Func_80b0574 / ... / add r5, #1 /
 * mov r0, r5 / bl Func_80b0574`.  FOUR in-function controls -- 0xcc5, 0xc9e, 0xca6,
 * 0xca1 are plain `ldr rX, =` pool literals and ALL FOUR reproduce byte-exact as
 * bare integers.  Only the one with a `+1` needs the symbol.  Consumer is
 * Func_80b0574 / Func_80b04dc, which is _MSG_d1c's recorded consumer.  With the
 * symbol the two calls come out as `ldr r5, =_MSG_cc3 / mov r0, r5` plus
 * `add r5, #1`, the ROM's shape, and the relocation matches the reference exactly.
 *
 * WITHHELD BECAUSE IT DOES NOT COMPLETE THE FUNCTION -- the allocation inversion
 * above survives it.  Its sibling _MSG_d27 WENT IN this same batch on ONE control
 * where this has four, purely because that one completed its file.  Evidence quality
 * and completion are separate tests, and this pair is the clearest local instance.
 *
 * INCIDENTAL, AND IT RETIRED A BATCH-280 CLAIM: a global whose name starts with a
 * dot IS reachable from C.  `.Lb4146` (declared `.global .Lb4146` in
 * asm/rom_b0000/rom_b0070_c_c_c_c.s) is spelled
 * `extern short tbl[] __asm__(".Lb4146");` and objcmp confirms the relocation is
 * byte-for-byte the reference's R_ARM_ABS32 .Lb4146.  Verified again on .Lb4ab2 in
 * Func_80b3050.  Batch 280 had called Func_808d9a4's .L9e680 "one permanent
 * fakematch row nobody can remove ... which no C identifier can name"; that has been
 * retracted, and this function is where the mechanism was independently confirmed.
 *
 * No per-file Makefile flag override applies to this stem.
 */
struct State {
	unsigned char pad000[0x36e];
	short items[9];
	unsigned char pad380[0x39e - 0x380];
	unsigned short sel;
	unsigned char pad3a0[0x3a7 - 0x3a0];
	signed char count;
	unsigned char mode;
};

typedef struct {
	unsigned char pad000[0x118];
	int f118;
	signed char f11c;
	unsigned char pad11d[0x2a3];
} GlobalState;

extern struct State *iwram_3001f2c;
extern GlobalState gState;
extern volatile int gKeyPress;
extern volatile int gKeyRepeat;
extern short tbl[] __asm__(".Lb4146");
extern int _MSG_cc3;

extern void Func_80b0574(unsigned int msg);
extern void Func_80b04dc(unsigned int msg);
extern void Func_80b0a6c(void *box, int x, int y);
extern void Func_80b11c4(void *box, int sel, int item);
extern void Func_80b1470(void *box, int cur, int item);
extern void _Func_8019908(int a, int b);
extern int _GiveItemTo(int who, int item);
extern int _FindEmptyInventorySlot(int who);
extern void _Func_80788c4(int who);
extern void _Func_8079754(int a);
extern void _AddCoinsSpent(int n);
extern void _PlaySound(int sfx);
extern void WaitFrames(int n);

void Func_80b24e4(void *box, void *box2)
{
	struct State *s;
	int price;
	int item;
	int redraw;
	int i;
	int cur;
	int r;

	s = iwram_3001f2c;
	item = s->sel;
	redraw = 1;
	cur = 0;
	price = tbl[gState.f11c];
	i = 0;
	if (price <= gState.f118) {
		s->sel = 0xe4;
		_Func_8019908(0xe4, 2);
		Func_80b0574((int)&_MSG_cc3);
		_Func_8019908(s->sel, 2);
		Func_80b0574((int)&_MSG_cc3 + 1);
		while (1) {
			if (redraw != 0) {
				redraw = 0;
				i = (i + s->count) % s->count;
				cur = s->items[i];
				Func_80b0a6c(box, i * 24 - 12, 0);
				s->mode = 3;
				Func_80b11c4(box, i, s->sel);
				Func_80b1470(box2, cur, s->sel);
			}
			if ((gKeyPress & 1) != 0) {
				r = _GiveItemTo(cur, s->sel);
				if (r < 0) {
					_PlaySound(0x71);
					_Func_8019908(cur, 1);
					_Func_8019908(s->sel, 2);
					if (_FindEmptyInventorySlot(cur) == 0xf)
						Func_80b04dc(0xc9e);
					else
						Func_80b04dc(0xca6);
					continue;
				}
				_Func_80788c4(cur);
				_PlaySound(0x65);
				Func_80b0574(0xca1);
				_GiveItemTo(cur, s->sel);
				_AddCoinsSpent(-price);
				_Func_8079754(1);
				break;
			} else if ((gKeyPress & 2) != 0) {
				Func_80b0574(0xcc5);
				_PlaySound(0x71);
				break;
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
		s->sel = item;
	}
}
