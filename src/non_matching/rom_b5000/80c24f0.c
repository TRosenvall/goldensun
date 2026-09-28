/* Func_80c24f0 -- 0x080c24f0, from goldensun/asm/rom_b5000/rom_c1a34_a_c.s.
 *
 * NON-MATCHING: 241 encodings of 265 differ (objcmp).
 *
 * objcmp --func, verbatim:
 *   XX ENCODINGS differ in 241 place(s) (ref 265, ours 265)
 *      first at index 7: ref 468b  ours b081
 * There is no SIZE line: 564 bytes both sides, 265 encodings both sides.  All
 * FOURTEEN relocations are present in the SAME ORDER with the same symbols, and
 * the `iwram_3001e74` pool word is at the IDENTICAL offset 0x224; the thirteen
 * call offsets drift by 2-4 bytes in the middle and re-converge.  So 241 is a
 * true distance -- but it is a HIGH count for ONE cause, because a single
 * allocation decision renames a register in almost every instruction.
 *
 * THE FUNCTION IS STRUCTURALLY COMPLETE.  Every block, branch, libcall, loop
 * shape and constant materialisation is the ROM's; read the residue below before
 * changing any statement, because the shape is not what is wrong.
 *
 * *** THE ROOT CAUSE, and it is ONE thing: `arg1` IS SPILLED TO THE STACK
 * INSTEAD OF LIVING IN r11. ***  Index 7 differs because our prologue carries
 * `sub sp,#4 / str r1,[sp]` where the ROM has `mov r11, r1`, and our r11 is then
 * spent on `bestIdx` at the end (`mov fp, r2` at our line 207).  The ROM's
 * mid-section holds SEVEN values in callee-saved registers -- j(r5), acc(r6),
 * blk(r7), u(r8), &info->fXX(r9), info(r10), arg1(r11) -- by SHARING r5 between
 * `&u->f128` and the count-loop counter and r6 between `found` and `acc`.  Ours
 * keeps `&u->f128` and the counter as separate allocnos, so `blk` is pushed from
 * r7 up to r8 and `u` from r8 to r10; every `k->a[...]` then costs a `mov r1,r8`
 * the ROM does not have, which is most of the 241.  arg1 has exactly two uses
 * and `mov r3,r11 / cmp r3,#0` costs the same two instructions as
 * `ldr r1,[sp] / cmp r1,#0`, so gcc is choosing the memory home on a TIE.
 * **The next lever to try is making those two live ranges share a name**, the
 * way batch 291's ResetPCs and InitEnemyUnit both needed (one counter variable
 * for three loops).  Reducing the local count from twelve to six already took
 * this from 245 at 568 bytes to 241 at the right size, so the direction is
 * confirmed; it has not gone far enough.
 *
 * ESTABLISHED, and each confirmed by a drop:
 *
 * 1. THE TWO TABLE SEARCHES ARE PLAIN `while` LOOPS WITH AN INNER `break`, NOT
 *    the rotated `while (1)` form.  `while (b->tbl[i] != u->f128) { if (++i > 5)
 *    break; }` gives the ROM's guard-plus-body layout; the rotated form that
 *    ResetPCs needed is 279 encodings against 265 here.  **The same two loop
 *    shapes are NOT interchangeable between functions -- test both.**
 *
 * 2. THE FIRST DIVISION IS UNSIGNED AND THE SECOND IS SIGNED, IN THE SAME
 *    FUNCTION.  `(unsigned char)(u->f0f / 10U)` gives `__udivsi3` plus the ROM's
 *    `lsl #24 / lsr #24`; `e->f4c * 3 / 10` on a `unsigned short` field gives
 *    `__divsi3`.  Both libcalls are correct with a CONSTANT divisor because
 *    Thumb-1 has no `umull`, so expand_divmod cannot use the multiply-high trick
 *    -- this is NOT batch 286's "a libcall with a constant divisor means the
 *    divisor was a variable".
 *
 * 3. `Random()` RETURNS `unsigned int`.  The ROM's `lsr r3,#16` after `r0*6` is a
 *    LOGICAL shift, so the product is unsigned; `int` gives `asr`.
 *
 * 4. THE 0x4e FIELD IS READ BOTH WAYS.  `ldrsh` feeds the comparisons and the
 *    Func_80c2470 argument, `ldrh` feeds the store into the ability slot, so the
 *    field is `short` and the store value is `*(unsigned short *)&e->f4e`.  This
 *    is batch 290's "one offset read unsigned for the store and signed for the
 *    compares is a real shape".
 *
 * 5. THE `blk` POINTER IS A NAMED LOCAL.  `k = &b->blk;` assigned before the
 *    first guard is what puts the `add r7, r0, r1` in the prologue; writing
 *    `b->blk.a[0]` throughout instead is 277 encodings at 592 bytes.  This is the
 *    OPPOSITE of batch 290's "DO NOT NAME THE SUB-STRUCT", and the tell is the
 *    same one: the ROM keeps base and blk in two registers at once
 *    (`strh r6,[r0,#0x3c]` beside `ldr r3,[r7,#8]`).
 *
 * 6. Thumb has NO immediate-offset `ldrsh`, so every `short` read needs the
 *    ROM's `mov r1,#0 / ldrsh r0,[r0,r1]` pair; that falls out of the type and is
 *    not a source shape to chase.
 *
 * MEASURED (ref 265 encodings, 564 bytes):
 *   first candidate, twelve locals                     245   (568 bytes)
 *   + six locals (counters and accumulators shared)    241   (564 bytes)
 *   both searches rotated to `while (1)`               269   (592 bytes)
 *   `int a1 = arg1;` copied at the top                       inert
 *   `b->blk.a[...]` instead of the named `k`           266   (592 bytes)
 *   -fno-gcse / -fno-cse-follow-jumps do NOT restore the ROM's in-loop
 *     `ldrb r3,[r4]` reload of u->f128        measured negative
 *
 * ONE SECONDARY RESIDUE worth recording: the ROM RELOADS `u->f128` inside the
 * first search loop where we reuse the guard's load.  Neither `-fno-gcse` nor
 * `-fno-cse-follow-jumps` changes that, so it is the FIRST cse pass reusing a
 * value across the loop entry, and it is worth two instructions.  It is not the
 * spill's cause -- the spill is present with or without it.
 *
 * No pins, no barriers, NO SHIM, no flag row, no .sym entry, no fakematch.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_b5000/c24f0_Func_80c24f0.park.c \
 *     asm/rom_b5000/rom_c1a34_a_c_c.s --func Func_80c24f0
 */
struct Blk {
	int a[3];
	unsigned short b[4];
};

struct Base {
	unsigned char pad00[0x10];
	unsigned short tbl[6];
	unsigned char pad1c[0x3c - 0x1c];
	unsigned short f3c;
	unsigned short f3e;
	unsigned char pad40[0x530 - 0x40];
	struct Blk blk;
};

struct Unit {
	unsigned char pad00[0x0f];
	unsigned char f0f;
	unsigned char pad10[0x128 - 0x10];
	unsigned char f128;
	unsigned char f129;
};

struct Info {
	unsigned char pad00[0x4c];
	unsigned short f4c;
	short f4e;
	short f50;
	unsigned short f52;
};

extern struct Unit *_GetUnit(int id);
extern char *iwram_3001e74[];
extern int _GetFlag(int id);
extern void _SetFlag(int id);
extern struct Info *_GetEnemyInfo(int id);
extern unsigned int Random(void);
extern unsigned int _RPGRandom(void);
extern int Func_80c2470(int v);

int Func_80c24f0(int id, int arg1)
{
	struct Unit *u;
	struct Base *b;
	struct Blk *k;
	struct Info *e;
	int i;
	int v;
	int t;
	int n;
	int best;
	int bi;

	u = _GetUnit(id);
	b = (struct Base *)iwram_3001e74[0];
	k = &b->blk;
	if ((unsigned int)id <= 7)
		return -1;
	if (u->f129 != 0)
		return -2;
	i = 0;
	v = 0;
	while (b->tbl[i] != u->f128) {
		if (++i > 5)
			break;
	}
	if (i != 6)
		v = i;
	if (b->f3e != 2) {
		if (v < b->f3c)
			b->f3c = v;
		if (k->a[2] != 0)
			b->f3e = 1;
	}
	k->a[2]++;
	if (_GetFlag(0x173) != 0)
		return 0;
	_SetFlag(u->f128 + 0x600);
	e = _GetEnemyInfo(u->f128);
	if (arg1 != 0) {
		if (e->f4c != 0) {
			v = 0;
			i = 0;
			while (i < (unsigned char)(u->f0f / 10U) + 1) {
				v += (Random() * 6 >> 16) + 1;
				i++;
			}
			t = e->f4c * 3 / 10;
			if (v < t)
				v = t;
			k->a[0] += v + e->f4c;
		}
		if (e->f52 != 0) {
			v = 0;
			i = 0;
			while (i < (unsigned char)(u->f0f / 10U) + 1) {
				v += (Random() * 4 >> 16) + 1;
				i++;
			}
			t = e->f52 * 3 / 10;
			if (v < t)
				v = t;
			k->a[1] += v + e->f52;
		}
	} else {
		k->a[0] += e->f4c;
		k->a[1] += e->f52;
	}
	if (e->f4e == 0)
		return 0;
	if (e->f50 == 0)
		return 0;
	i = 0;
	while (k->b[i] != e->f4e) {
		if (++i > 3)
			break;
	}
	if (i != 4)
		return 0;
	n = e->f50;
	if (arg1 != 0)
		n -= 2;
	if (n < 0)
		n = 0;
	if ((0x20000 >> n) <= (int)(_RPGRandom() & 0xffff))
		return 0;
	best = 0x40000000;
	bi = -1;
	for (i = 0; i <= 3; i++) {
		t = Func_80c2470(k->b[i]);
		if (t < best) {
			best = t;
			bi = i;
		}
	}
	if (Func_80c2470(e->f4e) > best)
		k->b[bi] = *(unsigned short *)&e->f4e;
	return 0;
}
