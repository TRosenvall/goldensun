/* Func_807a664 -- 0x0807a664, asm/rom_77000/rom_79460_c_c_c_c_c_c.s
 *
 * NON-MATCHING: 31 encodings of 143 differ (objcmp).
 *
 * objcmp --func Func_807a664, verbatim:
 *   XX ENCODINGS differ in 31 place(s) (ref 143, ours 143)
 *      first at index 66: ref 1c04  ours 1c01
 * No SIZE line and no RELOCATIONS line: 316 bytes both sides, 143 encodings both
 * sides, all 10 relocations identical in type, symbol AND offset.  By the project's
 * rule (size and count both match) 31 is a TRUE DISTANCE.
 *
 * ONE CAVEAT ON THAT, so the next reader is not misled: a mnemonic-level count of the
 * two streams is 135 ROM instructions against our 134, with one compensating pool or
 * padding word, which is why objcmp's encoding totals still agree at 143.  The single
 * missing instruction is named under RESIDUE below.
 *
 * tryc --align says 27 instructions in disagreeing regions of 145, and that is the
 * figure that ranked every variant below -- objcmp's positional count moved the wrong
 * way twice.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_77000/807a664.c \
 *     asm/rom_77000/rom_79460_c_c_c_c_c_c.s --func Func_807a664
 *
 * THE SPLIT, derived independently of the brief and confirmed by datacheck:
 * the .s holds Func_807a664, Func_807a7a0 and a .rodata run of eight .incrom blobs
 * (.L84a8c .. .L8926c, 0x84a8c-0x89624).  NEITHER function reads any data label --
 * every pool word in both is ewram_2001078, gState, ewram_2000438, 0x6774, 0x222,
 * 0x952 or a plain 0.  The ten names on the EXPORTS line are already .global and
 * are read from OTHER files (.L84b1c, for instance, by rom_79460_c_c_c_a_c_c.s's
 * Func_80799b0).  So the boundary is crossed by NO label in either direction and
 * the split needs ZERO new exports; the whole .rodata run must stay with the
 * remaining .s (which keeps Func_807a7a0, itself parked at src/non_matching/
 * rom_77000/807a7a0.c).
 *
 * MIRROR OF THE PARKED Func_807a7a0: that one restores the party from the staging
 * buffer, this one saves it.  Its park was the source of the struct layout.
 *
 * FIVE CONSTRUCTS, each measured by a single drop against the variant before it
 * (objcmp differing / tryc --align):
 *
 * 1. LOOPS 1 AND 2 ARE INDEXED COUNT-UP LOOPS, NOT POINTER WALKS.  Writing
 *    `for (n = 0; n <= 14; n++) *p++ = u->items[n];` lets loop.c create the source
 *    cursor as a giv, so its preheader is the ROM's two-instruction
 *    `mov r2,r7 / add r2,#0xd8` and check_dbra_loop reverses the counter to the
 *    ROM's `mov r5,#0xe / sub r5,#1 / bge`.  Written `src = u->items;` with a
 *    pointer walk, that expression appears TWICE in the source (loop 1 and loop 3)
 *    and gcse hoists ONE u+0xd8 pseudo across the whole body; it then crosses the
 *    GetItemInfo call, takes a high register, and pushes s222 onto the stack --
 *    `sub sp,#8` against the ROM's `sub sp,#4`.  Only loop 3 derives u+0xd8 in the
 *    source, which is why the ROM computes it a second time after loop 2.
 *      pointer walk in loop 1     131 / 100  at 312 bytes (4 short)
 *      indexed loop 1             133 /  94  at 304 bytes -- and the spill is gone
 *    The positional count got WORSE here while the shape got right; the align
 *    figure and the `sub sp` are what say it is progress.
 *
 * 2. *** gState IS READ THROUGH A POINTER LOCAL. ***  `g = gState;` and then
 *    `*(short *)(g + 0x220)` / `(g + 0x222)`.  Written against the array directly,
 *    gcc folds both into one pool word `gState+544` and reaches the second with an
 *    offset of 2 -- five instructions cheaper than the ROM.  With the local, cse's
 *    use_related_value (cse.c:1637) keeps the bare symbol in a register and derives
 *    both addresses from it, giving the ROM's `ldr r3,=gState / mov r0,#0x88 /
 *    lsl r0,#2 / add r2,r3,r0` and the separately pooled 0x222 (0x220 is shiftable,
 *    0x222 is not).  This is the single biggest lever in the function and it is what
 *    makes the object the right SIZE.
 *      gState + 0x220 directly    133 /  94  at 304 bytes
 *      g = gState first            60 /  85  at 316 bytes -- size and count exact
 *
 * 3. LOOP 3 HAS ITS OWN COUNTER.  Sharing one `n` with loops 1 and 2 puts the
 *    counter in the register the ROM gives to `cnt`.  The ROM's loop-3 counter is
 *    r6 where loops 1 and 2 count in r5, so it is a second variable.
 *      one shared n                60 /  85
 *      separate j for loop 3       55 /  81
 *
 * 4. `cnt = 0;` IS WRITTEN BEFORE `src` AND `dst`, and the outer counter is `int`.
 *    Each is worth one instruction's worth of ordering; both are in the final file.
 *      cnt = 0 after the cursors   60 /  85
 *      cnt = 0 before them         60 /  84
 *      unsigned int i              60 /  85
 *      int i                       59 /  84
 *
 * 5. THE ZERO-FILL ADVANCES `base` ITSELF and keeps a SEPARATE countdown.
 *    `base += cnt; m = 15 - cnt; do { m--; *base++ = 0; } while (m != 0);`
 *    The ROM's `add r0, r3, r0` writes the sum back into the base register, so the
 *    base variable is the cursor.  Reusing `cnt` as the countdown as WELL is a
 *    regression -- the two halves do not compose:
 *      dst = base + cnt, separate m        53 /  78
 *      base += cnt,      separate m        50 /  69   <- in the file
 *      dst = base + cnt, cnt counts down   58 /  83
 *      base += cnt,      cnt counts down   57 /  83
 *      `do { *dst++ = 0; } while (--m)`    53 /  78
 *
 * 6. *** `v` IN LOOP 3 IS `int`, AND IT CASCADES. ***  This is worth more than
 *    everything above it put together and it moved the first differing encoding from
 *    index 17 to index 66 -- i.e. it fixed the whole gState block's register
 *    assignment as a side effect, not just its own loop.
 *      short v            50 /  69   (ldrsh with a zero register, count matches)
 *      unsigned short v   69 /  45   at 320 bytes (two instructions long)
 *      int v              31 /  27   <- in the file
 *
 * SHIMS: none.  No `register ... __asm__`, no `__asm__ ("")`, no flags.
 *
 * RESIDUE: ONE INSTRUCTION, and it is named.  A mnemonic histogram of both streams
 * (135 ROM instructions against our 134) differs in exactly three places:
 *   ldr   rom 9  ours 7      <- both are the pooled zeros, NOT a difference: gcc
 *   ldrh  rom 6  ours 8         prints a HImode pool word as `ldrh rD,.Lxx` and gas
 *                               assembles it to the same `ldr rD,[pc,#N]` as the
 *                               ROM's `ldr rD,=0` (batch 292's correction to 1b).
 *   lsl   rom 3  ours 2      <- the real one.
 * The ROM's loop-3 test is `ldrh r2,[r4] / lsl r3,r2,#16 / cmp r3,#0`; ours is
 * `ldrh r3,[r1] / cmp r3,#0`.  The `lsl` is a HImode compare against a value whose
 * upper half is UNKNOWN.  Ours comes straight out of a zero-extending `ldrh`, so
 * combine.c's simplify_comparison sees nonzero_bits(v) == 0xffff and rewrites
 * `(v << 16) != 0` back to `v != 0`.  Four spellings of the shift are therefore
 * byte-identical to no shift at all: `(short)v != 0`, `(unsigned short)v != 0`,
 * `v << 16`, and `v != 0` all measure 31 / 27 exactly.
 *
 * The corpus says where the ROM's form comes from and why it is out of reach here.
 * `ldrh / lsl #16 / cmp #0` occurs in 17 places across the 4,297 generated .s files;
 * the two in this very bank are src/rom_77000/rom_77320_c_c_a.c and _c_c_b.c, and
 * in both the shifted operand is an `int` that came out of a CLAMP (`if (r0 > K)
 * r3 = K; else if (r0 < 0) r3 = 0; else r3 = r0;`) and was stored through a
 * `short *` -- its upper half genuinely unknown, so the shift survives.  Our value's
 * only producer is the halfword load, so no source spelling can make its upper half
 * unknown while keeping the `ldrh`.  Every route that does make it unknown (a HImode
 * carrier, a signed source pointer, a separate int test variable) either restores
 * `ldrsh` plus a hoisted zero register or costs two instructions -- eleven variants,
 * all recorded above.
 *
 * ALSO MEASURED INERT (each against the 50 / 69 or 31 / 27 baseline):
 *   `base = cnt + base` to flip the commutative add's operand order   inert
 *   the tail written `*p++ = s220; *p++ = s222;`                      inert
 *   `__asm__ ("")` between the two tail stores                        50 / 70 (worse)
 *   s220/s222 declared first, or declared `int`                       inert
 *   s222 read before s220                                            52 / 70 (worse)
 *   loop 3 reading `base[j]` instead of `*src++`                      53 / 68
 *   `(v & 0xffff) != 0`                                              88 differ, 328 bytes
 *   `short h` carrier with an `int` test variable                     50 / 69
 *   `short *src`                                                      50 / 69
 *
 * REMAINING BLOCKERS, in the order they appear:
 *   a. loop 3's register assignment.  ROM src=r4, dst=r1, counter=r6; ours src=r1,
 *      dst=r2, counter=r4.  First differing encoding, index 66, is `mov r4,r0`
 *      against `mov r1,r0` -- the same three pseudos, rotated.
 *   b. the missing `lsl`, above.  Unreachable by arithmetic, not merely unbeaten.
 *   c. `add r0, r3, r0` against `add r0, r0, r3` in the zero-fill: one encoding, the
 *      commutative operand order, and `cnt + base` does not move it.
 *   d. `sub r5, r3, r5` against `sub r3, r3, r5`: the ROM gives `m` the register
 *      `cnt` just vacated.
 *   e. `ldr r0, =ewram_2000438` is one position earlier in the ROM and lands in r0
 *      rather than r2.
 */
extern unsigned short ewram_2001078[];
extern unsigned short ewram_2000438[];
extern unsigned char gState[];
extern unsigned char *GetUnit(unsigned int unit);
extern unsigned char *GetItemInfo(int item);
extern void Func_8079ae8(unsigned int pc);
extern void CalcStats(unsigned int pc);
extern void Func_807a628(int a, int b);
extern void SetFlag(int id);
extern void Func_807808c(int a);

struct Unit {
	unsigned char pad00[0xd8];
	unsigned short items[15];
};

void Func_807a664(void)
{
	unsigned short *p;
	unsigned short *src;
	unsigned short *dst;
	unsigned short *base;
	struct Unit *u;
	unsigned char *info;
	unsigned char *g;
	short s220;
	short s222;
	int i;
	int n;
	int j;
	int cnt;
	int m;
	int v;

	p = ewram_2001078;
	if (*p != 0x6774) {
		*p = 0x6774;
		p++;
		g = gState;
		s220 = *(short *)(g + 0x220);
		s222 = *(short *)(g + 0x222);
		for (i = 0; i <= 3; i++) {
			u = (struct Unit *)GetUnit(i);
			for (n = 0; n <= 14; n++)
				*p++ = u->items[n];
			for (n = 0; n <= 14; n++) {
				info = GetItemInfo(u->items[n]);
				if (info[2] != 6)
					u->items[n] = 0;
			}
			base = u->items;
			cnt = 0;
			src = base;
			dst = base;
			for (j = 0; j <= 14; j++) {
				v = *src++;
				if (v != 0) {
					*dst++ = v;
					cnt++;
				}
			}
			if (cnt <= 14) {
				base += cnt;
				m = 15 - cnt;
				do {
					m--;
					*base++ = 0;
				} while (m != 0);
			}
			Func_8079ae8(i);
			CalcStats(i);
		}
		*p = s220;
		p++;
		*p = s222;
		p++;
		*p = ewram_2000438[0];
		p[1] = ewram_2000438[1];
		Func_807a628(0, 0x10);
		SetFlag(0x952);
	}
	Func_807808c(1);
}
