/* Func_8079f10 -- 0x08079f10, asm/rom_77000/rom_79460_c_c_c_c_a_c_c_c_a_a.s
 *
 * NON-MATCHING: 186 encodings of 209 differ (objcmp).
 *
 * objcmp --func Func_8079f10, verbatim:
 *   XX SIZE  ref 444 bytes, ours 420
 *   XX ENCODINGS differ in 186 place(s) (ref 209, ours 200)
 *      first at index 10: ref 4698  ours 4693
 *   XX RELOCATIONS differ
 * 186 is NOT a distance -- we are nine instructions and 24 bytes short.  tryc --align
 * says 140 instructions in disagreeing regions of 217, and that is the working figure.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_77000/8079f10.c \
 *     asm/rom_77000/rom_79460_c_c_c_c_a_c_c_c_a_a.s --func Func_8079f10
 *
 * THE .s HOLDS ONE FUNCTION AND NO DATA SECTION (grep -ci func_start = 1, datacheck
 * silent), so this converts the file WHOLE when it closes.
 *
 * THE SIGNATURE IS READ OFF THE FRAME.  push{r5,r6,r7,lr} + push{r5,r6,r7} + push{r7}
 * is 0x20 bytes, so the ROM's `ldr r3,[sp,#0x20]` is argument FIVE, not a spill:
 * `int Func_8079f10(int a, int b, int c, int d, int e)`.  `pop {r1} / bx r1` says it
 * returns int.  With that signature the entry sequence through index 9 is
 * byte-identical.
 *
 * ONE CONSTRUCT ESTABLISHED, and it is worth 41 instructions:
 *
 *   *** THE BYTE GUARDS ARE NESTED `if`s, NOT `&&` CHAINS. ***  The ROM tests
 *   u+0x138, +0x139, +0x13a, +0x13b, +0x13c, +0x13d, +0x141 one byte at a time.
 *   Written as an `&&` chain over struct members, fold-const.c merges adjacent
 *   same-struct char field compares into ONE wide load -- `ldr r3,[r3]` for the four
 *   bytes at 0x138 and `ldrh r3,[r3]` for the pair at 0x13c -- and the function comes
 *   out 50 instructions short:
 *     `&&` over struct members     159 of 209, 340 bytes
 *     nested `if` per byte         200 of 209, 420 bytes   <- in the file
 *
 *   ITS SCOPE IS NARROWER THAN IT LOOKS, and this is the reusable part: the merge needs
 *   COMPONENT_REFs.  Rewriting the same tests as `*(ub + 0x138)` on an
 *   `unsigned char *` base makes `&&` and nested `if` produce BYTE-IDENTICAL output --
 *   both 200 of 209, both 140 in disagreeing regions.  So the lever is really "do not
 *   give fold two adjacent fields of one struct to compare", and a char-pointer
 *   spelling is an equally good way out.  Three files here measure the same:
 *     struct members + nested if   200 / 140
 *     char base     + nested if    200 / 140
 *     char base     + `&&`         200 / 140
 *
 * OTHER SHAPES CONFIRMED FROM THE ROM (all in byte-identical or near regions):
 *   - u+0x131 is `signed char` (ROM `ldrb / lsl #24 / asr #24` -- thumb has no
 *     `ldrsb rD,[rN,#imm]`), u+0x42 is `unsigned char` (`lsr #1`), u+0x38 is `short`.
 *   - the scaled term is `* 3`, which the ROM spells `lsl r3,r5,#1 / add r3,r5`.
 *   - the retry loop is a GUARDED do-while, which is the documented requirement for a
 *     count-up loop with a VARIABLE bound (`tries`) whose counter is otherwise unused:
 *     the ROM has `mov r6,#0 / cmp r6,r10 / bge <return 0>` and then `add r6,#1 /
 *     cmp r6,r10 / blt`, i.e. the guard and the latch, with no rotated entry test.
 *   - `x * e` is written INSIDE the loop and loop.c hoists it to the preheader, which
 *     is where the ROM's `ldr r3,[sp,#0x20] / mul r7,r3` sits.  The mul's operand order
 *     was read per the procedure -- the ROM has no `mov` at this site and the
 *     destination is the accumulator, so `x * e` with the accumulator on the left.
 *
 * SHIMS: none.
 *
 * BLOCKER: the nine missing instructions are reload_cse_move2add, and the pass is
 * named because the ROM's register choices are what disable it.
 *
 *   Ours materialises the successive byte offsets in ONE hard register and lets
 *   move2add chain them -- `mov r2,#156 / lsl r2,#1` then `add r2,r2,#1` six times,
 *   and `add r2,r2,#7` / `add r2,r2,#4` where the run skips.  The ROM materialises
 *   each offset FRESH in a DIFFERENT register: 0x131 -> r4, 0x13b -> r7, 0x13c -> r0
 *   (`mov #0x9e / lsl #1`), 0x13d -> r1, 0x141 -> r2, then 0x138 -> r4, 0x139 -> r7,
 *   0x13a -> r0.  move2add is per hard register, so rotating the register defeats it;
 *   the three shiftable offsets (0x13a, 0x13c, 0x140) each cost the ROM `mov + lsl +
 *   add` where our chain pays `add + add`, and that is most of the nine.
 *
 *   The rotation itself is downstream of a second difference: the ROM PRE-COMPUTES six
 *   addresses at the merge block before `cmp r7,#0x40` (`add r3,r6,r1 / add r1,r6,r7 /
 *   add r7,r6 / add r2,r6,r4 / add r0,r6,r4 / add r4,r6,r7`, with one parked in r12),
 *   so all six are live at once and must take distinct registers.  That is gcse PRE
 *   inserting the computations on the path that lacks them, because the same addresses
 *   are used in the d==3/d==4 chains.  Our build does not insert them, recomputes each
 *   address at its use, and therefore has one short-lived offset pseudo at a time.
 *
 *   TRIED AND MUCH WORSE: naming each offset in its own `int` local to force nine
 *   distinct pseudos -- 178 of 209 at 376 bytes, i.e. it spills.  That is the obvious
 *   route and it is closed; the next thing to try is making the ADDRESSES named
 *   pointer locals shared between the d==4 chain and the d==0x40 chain, which is what
 *   the ROM's PRE result looks like, rather than the offsets.
 */
struct Unit {
	unsigned char pad00[0x38];
	short f38;
	unsigned char pad3a[0x42 - 0x3a];
	unsigned char f42;
	unsigned char pad43[0x131 - 0x43];
	signed char f131;
	unsigned char pad132[0x138 - 0x132];
	unsigned char f138;
	unsigned char f139;
	unsigned char f13a;
	unsigned char f13b;
	unsigned char f13c;
	unsigned char f13d;
	unsigned char pad13e[0x140 - 0x13e];
	unsigned char f140;
	unsigned char f141;
};

extern struct Unit *GetUnit(int id);
extern int Func_8079ef8(int a);
extern int Func_8079d7c(int a);
extern int Func_807987c(int id, int i);
extern int Func_8079e9c(struct Unit *u, int a);
extern int RPGRandom2(void);

int Func_8079f10(int a, int b, int c, int d, int e)
{
	struct Unit *u;
	int tries;
	int x;
	int t;
	int s;
	int k;

	u = GetUnit(b);
	tries = 1;
	if (Func_8079ef8(d) != 0 && u->f38 != 0)
		return 0;
	if (d == 3) {
		if (u->f131 == 0)
			return 0;
	} else if (d == 4) {
		if (u->f138 == 0)
		if (u->f139 == 0)
		if (u->f13a == 0)
		if (u->f13b == 0)
		if (u->f13c == 0)
		if (u->f13d == 0)
		if (u->f141 == 0)
			return 0;
	}
	if (d == 0x40) {
		if (u->f131 == 0)
		if (u->f138 == 0)
		if (u->f139 == 0)
		if (u->f13a == 0)
		if (u->f13b == 0)
		if (u->f13c == 0)
		if (u->f13d == 0)
		if (u->f141 == 0)
		if (u->f140 == 0)
			return 0;
	}
	if (d == 0x1c) {
		if (u->f141 == 1)
			return 0;
	}
	x = Func_8079d7c(d);
	if (x > 0) {
		t = Func_807987c(a, c);
		t -= Func_807987c(b, c);
		t -= u->f42 >> 1;
		x += t * 3;
		if (Func_8079e9c(u, d) != 0)
			x += 0x19;
	} else {
		x = -x;
	}
	if (d == 0x43)
		tries = 3;
	k = 0;
	if (k < tries) {
		do {
			s = x * e / 100;
			if (s >= RPGRandom2())
				return 1;
			k++;
		} while (k < tries);
	}
	return 0;
}
