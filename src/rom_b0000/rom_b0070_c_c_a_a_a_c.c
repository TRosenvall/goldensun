/* Cluster Func_80b280c..Func_80b280c extracted from goldensun/asm/rom_b0000/rom_b0070_c_c_a_a_a.s.
 *
 * Total .text for this TU = 120 bytes (= 0x78). Parked at 53 of 55 in batch 274;
 * elevated in batch 276 on the FIRST compile of the struct spelling.
 * No pins, no flags.
 *
 * THE PARK'S OWN CONCLUSION WAS WRONG, AND ITS OWN CLOSING NOTE WAS THE FIX.
 *
 * src/non_matching/rom_b0000/80b280c.c concluded "SO THIS IS THE ALLOCATION-ORDER CLASS
 * ... needs a differently configured gcc rather than a different C", having measured all
 * four `unsigned char *` address spellings and four `-fno-*` flags identical at 53. It
 * then listed, as NOT tried, a struct declaration for `iwram_3001f2c`. That was the whole
 * fix, and it is the same lever that landed Func_80b2e30 in batch 275
 * (src/rom_b0000/rom_b0070_c_c_a_c_c_a_c.c), where four char-pointer spellings and four
 * flags likewise measured identical at 76.
 *
 * THE MECHANISM. With `unsigned char *` plus hand-written byte offsets, the whole address
 * is one expression in `i`, so strength_reduce folds it into a SINGLE pointer giv and the
 * base register dies. A typed struct member array keeps the ROM's split -- base register
 * held across the loop, plus a stepping integer offset -- which costs one more
 * callee-saved register and is what reproduces the push list.
 *
 * So the half of the park's reading that survives is "gcc re-derives its own induction
 * variables and the source has no vote" -- true, but only of the char-pointer typing. The
 * source's vote is the TYPE, not the expression. When every expression spelling measures
 * identical, that is the variable you have not varied yet.
 */
struct State {
	unsigned char pad000[0x36e];
	short items[16];
	unsigned char pad38e[0x3a7 - 0x38e];
	signed char count;
	unsigned char pad3a8[2];
	signed char key;
};

extern struct State *iwram_3001f2c;
extern int Func_80b27b0(int a, int b);

int Func_80b280c(void)
{
	struct State *s;
	int count;
	int i;
	int key;

	s = iwram_3001f2c;
	key = s->key;
	count = 0;
	for (i = 0; i < s->count; i++) {
		if (Func_80b27b0(s->items[i], key))
			count++;
	}
	return count;
}
