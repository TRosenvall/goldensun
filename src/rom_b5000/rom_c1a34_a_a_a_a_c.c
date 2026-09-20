/* Cluster Func_80c1df4..Func_80c1df4 extracted from goldensun/asm/rom_b5000/rom_c1a34_a_a_a.s.
 *
 * Total .text for this TU = 200 bytes (= 0xc8). Never attempted before batch 275.
 * No pins, no flags.
 *
 * THREE LEVERS, and the first two are new.
 *
 * 1. gcc FOLDS `(x & (1 << n)) == 0` INTO `((x >> n) & 1) == 0`. `fold` does it because the
 *    AND's operand is literally a LSHIFT_EXPR of 1. Assigning the shift to a named
 *    `int bit` first blocks the fold and restores the ROM's
 *    `mov r2, #1 / lsl r2, r0 / and r3, r2`. 25 differing to 21.
 *
 * 2. PUT THE EARLY-EXIT CONSTANT IN THE TEXTUALLY LAST BLOCK. Writing
 *    `if (n > 4) return -1;` emits the -1 inline and branches over it; the ROM branches
 *    AWAY and lets -1 fall through into the epilogue, which is what
 *    `if (n <= 4) { ... } else` gives. 21 differing to exact.
 *
 *    That is a companion to the recorded control-flow rules: those are about where a
 *    `goto` target lands, this is about which arm the fall-through constant sits in.
 *
 * 3. THE SEARCH LOOP IS `while (i < n && arr[i] != key) i++;`. The ROM's shape is
 *    guard, then a peeled element-0 test at an immediate offset, then the pointer
 *    materialised, then the loop top beginning with the increment -- and that spelling
 *    reproduced it INSTRUCTION-EXACT on the first compile. Note `for (i = 0; i < n; i++)
 *    if (...) break;` measured BYTE-IDENTICAL, so the two are interchangeable here; the
 *    `&&` form is simply the one that makes the peel legible.
 *
 *    The unsolved loop in src/non_matching/rom_b5000/c1ebc.c is the same shape and used a
 *    `for`, measuring 66-70. Worth re-attempting with this reading.
 *
 * Also measured: `b->state[i]` cached into a local rather than re-read is 51 differing and
 * 4 bytes short; `j` in the `for`-init rather than a statement is 11.
 */
struct S {
	unsigned char pad00[0x10];
	unsigned short keys[6];
	int flags[6];
	signed char state[6];
	unsigned char pad3a[6];
	unsigned char count;
};

extern struct S *iwram_3001e74;

int Func_80c1df4(int key)
{
	struct S *b;
	int n;
	int i;
	int j;
	int bit;

	b = iwram_3001e74;
	n = b->count;
	i = 0;
	while (i < n && b->keys[i] != key)
		i++;
	if (i != n) {
		j = 0;
		if (b->state[i] < 0) {
			b->state[i] = 1;
			b->flags[i] = 3;
			return 0x8001;
		}
		while (j <= 0x1f) {
			b->state[i] = (b->state[i] + 1) % 9;
			bit = 1 << b->state[i];
			if ((b->flags[i] & bit) == 0)
				break;
			j++;
		}
		b->flags[i] |= 1 << b->state[i];
		return b->state[i];
	}
	if (n <= 4) {
		b->state[n] = -1;
		b->keys[n] = key;
		b->flags[n] = 0;
		b->count = n + 1;
		return 9;
	}
	return -1;
}
