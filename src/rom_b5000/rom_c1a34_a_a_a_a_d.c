/* Cluster Func_80c1ebc..Func_80c1f50 extracted from goldensun/asm/rom_b5000/rom_c1a34_a_a_a_a.s.
 *
 * Total .text for this TU = 236 bytes (= 0xec). Func_80c1ebc was parked at 66 of 75 in
 * batch 274 and Func_80c1f50 at 30; both elevated in batch 276, so the whole file
 * converts. No pins, no flags.
 *
 * ALL FOUR OF THE c1ebc PARK'S BLOCKERS DISSOLVED, AND ONE OF THEM WAS MIS-DIAGNOSED.
 *
 * 1. THE SEARCH LOOP IS `while (i < n && arr[i] != key) i++;`. The park used a `for` and
 *    measured 66-70. This form reproduced BOTH loops instruction-exact on the first
 *    compile, and took the park's blocker 1 (the `0x128` field offset) with it. The form
 *    was established on the immediate neighbour Func_80c1df4 in batch 275
 *    (src/rom_b5000/rom_c1a34_a_a_a_a_c.c), whose own notes flagged this park as worth
 *    re-attempting with it. That note was right.
 *
 * 2. THE `u` POINTER IS A REGISTER-CLASS PROBLEM, NOT AN ALLOCATION-ORDER ONE. This is the
 *    park's blocker 3, and it is where the park was wrong: it read "we spend a
 *    callee-saved register" as an allocation-order defeat. Declaring `u` as `struct U *`
 *    instead of `unsigned char *` took 63 differing to 7 and dropped the push list from
 *    {r5,r6,r7,lr} to the ROM's {r5,r6,lr}.
 *
 *    The mechanism is CLASS, not order. With `unsigned char *u`, the peeled `u[0]` read
 *    compiles to a direct `ldrb [r6, #0]`, and Thumb's `ldrb` base must be a LOW register,
 *    so that single use pins `u`'s preferred class to LO_REGS for the whole function. With
 *    `u->name[k]` the peel goes through the materialised pointer, leaving `u` used only in
 *    `mov` and `add` -- HI_REGS is then admissible and gcc takes r12, which no low
 *    register has to be saved for.
 *
 *    That is a distinct lever from the batch-275 struct finding (which is about
 *    strength_reduce folding a base into one giv). Same `unsigned char *` cause, different
 *    pass, different symptom: there a dead base register, here a widened push list.
 *
 * 3. DO NOT NAME THE MASK. `b->flags[i] &= ~(1 << bit);` written straight is 2 differing;
 *    a named `int mask` swaps r2/r3 in the final read-modify-write. Ten RMW spellings
 *    measured: the two bare forms at 2, the other eight at 8.
 *
 * 4. RETURN TYPE `int` WITH A BARE `return;`. 2 differing to exact. The tell is in the
 *    epilogue: the ROM does `pop {r1} / bx r1`, not `pop {r0} / bx r0`, and a function that
 *    declares a return value is the only thing that makes r0 unavailable as the scratch for
 *    the pop. `return 0;` does NOT work -- it shifts the relocation. So the declaration
 *    carries the information even where no value is ever produced.
 *
 * FOR Func_80c1f50 THE STRUCT LEVER ALONE IS NOT ENOUGH -- the `goto` loop is what closes
 * it. Its park read "strength_reduce creates a pointer the ROM does not have", which was
 * correctly diagnosed, and the cure is the no-NOTE_INSN_LOOP_BEG lever rather than the
 * typing: struct + do/while measured 30, identical to the raw char-pointer do/while at 30,
 * while the `goto` form is 0. All three of struct, `goto` and the `int` return are
 * required. Ten inner-loop shapes measured: goto 0, goto-without-`w` 25, do/while 30,
 * while 36.
 */
struct S {
	unsigned char pad00[0x10];
	unsigned short keys[6];
	int flags[6];
	signed char state[6];
	unsigned char pad3a[6];
	unsigned char count;
};

struct U {
	unsigned char name[14];
	unsigned char pad0e[0x128 - 0x0e];
	unsigned char f128;
	unsigned char f129;
	unsigned char f12a;
};

extern struct S *iwram_3001e74;
extern struct U *_GetUnit(int id);

int Func_80c1ebc(int id)
{
	struct S *b;
	struct U *u;
	int n;
	int i;
	int k;
	int t;
	int bit;

	b = iwram_3001e74;
	n = b->count;
	u = _GetUnit(id);
	if (u->f129 != 0)
		return;
	t = u->f128;
	i = 0;
	while (i < n && b->keys[i] != t)
		i++;
	if (i == n)
		return;
	if (b->flags[i] == 0)
		return;
	k = 0;
	while (k <= 0xd && u->name[k] != 0)
		k++;
	bit = 0x20;
	if (k > 0)
		bit = u->name[k - 1] - 0x31;
	b->flags[i] &= ~(1 << bit);
}

int Func_80c1f50(int who)
{
	struct U *u;
	int i;
	int j;
	int v;
	int w;
	int val;

	i = 0;
	val = 0x31;
	for (; i <= 5; i++) {
		u = _GetUnit(i + 0x80);
		v = u->f12a;
		if (v == 1 && u->f128 == who) {
			j = 0;
			if (u->name[0] == 0) {
				u->name[0] = val;
				u->name[v] = 0;
			} else {
			loop:
				j++;
				if (j > 0xd)
					return;
				w = u->name[j];
				if (w != 0)
					goto loop;
				u->name[j] = val;
				u->name[j + 1] = w;
			}
			return;
		}
	}
}
