/* Func_80b0fa4 -- 0x080b0fa4, SECOND of the two functions in
 * asm/rom_b0000/rom_b0070_a_a_c_c_a_c_a.s (the first is Func_80b0aac, still asm).
 *
 * NON-MATCHING: 3 encodings of 138 differ (objcmp).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py <this> \
 *     asm/rom_b0000/rom_b0070_a_a_c_c_a_c_a.s --func Func_80b0fa4
 *
 * 296 bytes both, 138 encodings both, relocations identical.  The three are all
 * in the loop PREHEADER, and they are one register choice:
 *
 *     ref    lsl r3, r6, #1 / add r3, r7      / mov r8, r3 / mov r3, #0x10 / mov r11, r3
 *     ours   lsl r3, r6, #1 / add r7, r7, r3  / mov r3, #16 / mov r8, r7   / mov fp, r3
 *
 * NAMED BLOCKER: RELOAD'S OUTPUT-RELOAD REGISTER FOR A HIGH-REGISTER ADD.
 * `p` is a global allocno in r8, and Thumb cannot target a high register with
 * `add rd, rn, rm`, so reload materialises `arr + base` in a low register and
 * adds `(set (reg 8 r8) (reg N))` -- insn 475 in .19.flow2, which reload itself
 * created.  It picks N = r7, the register global_alloc gave `arr`, because
 * `arr` carries REG_DEAD at that very insn and is therefore free; the ROM picks
 * r3, the dying shift temp.  That is allocate_reload_reg's `last_spill_reg`
 * rotation (reload1.c), and the documented lever for it -- "reorder statements
 * so a live value holds the unwanted register" -- has no reach here: `arr` has
 * no use after the preheader (the ROM reuses r7 for `info` inside the loop), so
 * nothing can keep r7 busy.  The order of `mov r8, ..` and `mov r3, #0x10`
 * follows from the same choice and is not independent.
 *
 * INERT ON THAT SITE, seven pointer spellings all at 3: `p = &arr[base]`,
 * `p = base + arr`, `p = arr; p += base;`, `p = (short *)((char *)arr + base*2)`,
 * `p = (short *)((int)arr + base*2)`, a named index temp, and `k = 0` moved
 * inside the guard.  Four prologue reorderings were all WORSE (16, 20, 21, 69),
 * as were `y = 0x10` before `p = arr + base` (12) and `p` computed before the
 * guard (8).
 *
 * THREE LEVERS ARE LANDED HERE AND EACH IS LOAD-BEARING:
 *
 * 1. THE LOOP IS A GUARDED do-while, NOT A while.  128 -> 38 came from the
 *    `arr`/`itm` restructure; the shape took 38 -> 13 AND FIXED THE INSTRUCTION
 *    COUNT (134 -> 138).  Written `while (base < n) { ...; if (k > 6) break; }`
 *    gcc makes the `k` test the loop's bottom test and puts `base < n` at the
 *    top; the ROM pre-checks `base < n` once before the loop and tests BOTH at
 *    the bottom, which only `if (cond) { do { ... } while (cond); }` produces.
 *    A ROM loop with a single pre-check and two bottom tests, the loop's own
 *    condition LAST, is a guarded do-while and no permutation of a `while`
 *    reaches it.
 *
 * 2. THE LOOP CONDITION IS UNSIGNED AND THE ARROW TEST IS SIGNED, IN THE SAME
 *    FUNCTION, ON THE SAME TWO VARIABLES.  `bcs`/`bcc` on `base` vs `n` and
 *    `bhi` on `k` vs 6, but `bge` on `base + 7` vs `n`.  No single pair of
 *    types gives both, and making everything unsigned does not change the
 *    instruction count at all (38, unchanged) -- so the casts on the loop test
 *    are what the toolchain needs, worth 13 -> 10.
 *
 * 3. `y += 0x20;` BEFORE `k++;` IS WORTH 10 -> 3.  It decides which of r2/r3
 *    carries the constant 1 and which the 0x20 at the bottom of the loop, and
 *    which side of the `strb` the constant's `mov` is scheduled on.  Six other
 *    orderings of those four increments were 10 or 12.
 *
 * ALSO LOAD-BEARING, from the 128 -> 38 step: a named `short *arr = s->arr;`
 * assigned immediately after `s`, and a named `itm = *p;` so the item halfword
 * is read ONCE and feeds both _GetItemInfo and _Func_801eb90.  Without them the
 * address folds into `s + base*2 + 0x26c` and the halfword is loaded twice.
 *
 * `n = s->f3a6` reads the signed byte as `ldrb / lsl #24 / asr #24` for free.
 * The .s holds two functions and NO DATA SECTION (datacheck.py), so landing
 * this one needs tools/split_s.py and a pure text change, nothing else.
 */
struct Txt {
	unsigned char pad00[4];
	unsigned char f4;
	unsigned char f5;
	unsigned char pad06[6];
	unsigned short fc;
	unsigned char pad0e[1];
	unsigned char f0f;
};

struct State {
	unsigned char pad000[0x26c];
	short arr[0x40];
	unsigned char pad2ec[0x392 - 0x2ec];
	unsigned short f392;
	unsigned short f394;
	unsigned char pad396[0x3a6 - 0x396];
	signed char f3a6;
};

typedef struct { short price; } ItemInfo;

extern struct State *iwram_3001f2c;

extern void _Func_8016478(void *box);
extern struct Txt *_Func_801eadc(int a, int b, void *box, int d, int e);
extern struct Txt *_Func_801eb90(int item, int a, void *box, int x, int y);
extern struct Txt *Func_80b0744(int price, void *box, int y, int d);
extern ItemInfo *_GetItemInfo(int item);

void Func_80b0fa4(void *box, int sel)
{
	struct State *s;
	struct Txt *e;
	short *arr;
	short *p;
	ItemInfo *info;
	int n;
	int base;
	unsigned int k;
	int y;
	int itm;

	s = iwram_3001f2c;
	arr = s->arr;
	n = s->f3a6;
	base = sel - sel % 7;
	if (box == 0)
		return;
	_Func_8016478(box);
	if (base != 0) {
		e = _Func_801eadc(s->f392, 0x80 << 23, box, 0xd8, -0x10);
		e->f4 = 0;
		e->f5 = 0x11;
		e->fc = 0;
	}
	if (base + 7 < n) {
		e = _Func_801eadc(s->f394, 0x80 << 23, box, 0xd8, 0x18);
		e->f4 = 0;
		e->f5 = 0xf;
		e->fc = 0;
	}
	k = 0;
	if ((unsigned int)base < (unsigned int)n) {
		p = arr + base;
		y = 0x10;
		do {
		itm = *p;
		info = _GetItemInfo(itm);
		e = _Func_801eb90(itm, 1, box, k * 32, 0);
		e->f0f = 0xfc;
		if (base == sel) {
			e->f5 = 9;
			e->fc = 0xa;
			e->f0f = 0xfd;
		}
		e = Func_80b0744(info->price, box, y, 0);
		e->f0f = 0xfb;
		y += 0x20;
		k++;
		p++;
		base++;
		if (k > 6)
			break;
		} while ((unsigned int)base < (unsigned int)n);
	}
}
