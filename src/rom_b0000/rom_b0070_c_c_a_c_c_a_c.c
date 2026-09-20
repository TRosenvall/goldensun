/* Cluster Func_80b2e30..Func_80b2e30 extracted from goldensun/asm/rom_b0000/rom_b0070_c_c_a_c_c.s.
 *
 * Total .text for this TU = 168 bytes (= 0xa8). Never attempted before batch 275.
 * No pins, no flags, no volatile.
 *
 * ================ A `struct` INSTEAD OF `unsigned char *` IS A REGISTER LEVER ================
 *
 * This is the round's headline finding and it REFUTES A STANDING PARK CONCLUSION.
 *
 * With `unsigned char *` plus hand-written byte offsets, gcc-2.96's strength_reduce folds
 * the whole address into ONE pointer giv. With a typed struct member array it keeps the
 * ROM's BASE REGISTER + STEPPING INTEGER OFFSET split, which costs one more callee-saved
 * register and reproduces the ROM's push list.
 *
 * All four `unsigned char *` address spellings -- `(short *)(s+2)` indexed `0x1b6+i`, a
 * byte-offset base, the inline split, and two explicit stepping variables -- measured
 * IDENTICALLY at 72 lines and 76 differing against the ROM's 78. So did
 * -fno-strict-aliasing, -fno-gcse, -fno-rerun-cse-after-loop and
 * -fno-expensive-optimizations. The struct is exact.
 *
 * src/non_matching/rom_b0000/80b280c.c concludes "SO THIS IS THE ALLOCATION-ORDER CLASS ...
 * needs a differently configured gcc rather than a different C". That conclusion is WRONG,
 * and that park's own closing note -- a struct declaration for iwram_3001f2c, listed as
 * "NOT tried, and worth one screen" -- is the fix. Func_80b280c and Func_80b2b10 are the
 * same idiom (base `s+2`, offset `0xdb << 2`) and should be re-attempted; Func_80c1f50's
 * park carries the same "strength reduction creates a pointer the ROM does not have"
 * blocker and is a third candidate.
 *
 * So the earlier finding that all four cast spellings are identical was CORRECT -- for the
 * `unsigned char *` typing only. The variable that was never varied was the TYPE.
 *
 * THREE MORE LEVERS:
 *   - `s->count` RE-READ in the loop condition. Hoisting it into a local costs 16 bytes and
 *     66 differing -- the recorded do-not-cache rule, fourth function running.
 *   - TWO SEPARATE `_Sprite_SetAnim` CALLS, not one with a ternary animation argument: the
 *     ternary is 8 bytes longer, 71 differing, and loses a relocation.
 *   - `lang` read BEFORE the `arg0` guard; moved inside it is 9 differing.
 *
 * The two `vals[]` accesses 0x40 apart are ONE giv with a `#0x40` displacement -- gcc's own
 * giv combining, not a source feature. Declared `int` is 2 differing.
 */
struct State {
	unsigned char pad000[0x114];
	void *sprites[16];
	int vals[16];
	unsigned char pad194[0x36e - 0x194];
	short items[16];
	unsigned char pad38e[0x3a7 - 0x38e];
	signed char count;
	unsigned char pad3a8[2];
	signed char lang;
};

extern struct State *iwram_3001f2c;
extern void _Sprite_SetAnim(void *sprite, int anim);
extern int Func_80b27b0(int a, int b);

void Func_80b2e30(int arg0, int sel)
{
	struct State *s;
	int lang;
	int i;

	s = iwram_3001f2c;
	lang = s->lang;
	if (arg0 != 0) {
		for (i = 0; i < s->count; i++) {
			if (i == sel)
				_Sprite_SetAnim(s->sprites[i], 0x1e);
			else
				_Sprite_SetAnim(s->sprites[i], 1);
			s->vals[i] = 0x80 << 9;
			if (Func_80b27b0(s->items[i], lang) == 0)
				s->vals[i] = 0xb333;
		}
	}
}
