/* Func_80c1afc (0x080c1afc) -- STILL PARKED at 20 of 158, AND THIS PARK'S
 * DIAGNOSIS IS ONE OF THE FEW THAT SURVIVES RE-MEASUREMENT.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_b5000/80c1afc.c \
 *     asm/rom_b5000/rom_c1a34_a_a_a_a_b_a.s --func Func_80c1afc
 * Re-measured in batch 316c: 20 of 158, first difference at index 63, size and
 * relocations identical.  The body on disk produces the header's figure.
 *
 * *** THE ARITHMETIC REPRODUCES EXACTLY FROM .17.lreg, FIGURE FOR FIGURE. ***
 * Both numbers that matter were re-read off the dump rather than taken on
 * trust, because a sibling park's "unreachable by arithmetic" proof turned out
 * to have its two live lengths backwards.  This one does not:
 *     pseudo  38 (counter j)   refs=7  live=28  ->  floor_log2(7)*7/28 = 0.500
 *     pseudo 133 (offset giv)  refs=9  live=24  ->  floor_log2(9)*9/24 = 1.125
 *     pseudo  66 (table base)  refs=5  live=24  ->  floor_log2(5)*5/24 = 0.417
 * and .18.greg's own order confirms it: the allocation list is
 *     ;; 21 regs to allocate: 81 130 138 37 39 43 41 40 133 42 46 34 38 47 66 ...
 * with 133 ninth, 38 thirteenth, 66 fifteenth, and the dispositions
 *     133 in 5   38 in 6   66 in 7
 * All three cross the loop's two calls, so r0-r3 are out and r4 is out by
 * -fcall-used-r4, which is why REG_ALLOC_ORDER reaches r5 first for them.
 * The ROM needs 38 > 133 > 66, i.e. the counter above 1.125 and the giv
 * between 0.417 and 0.500.
 *
 * THE WINDOW, STATED AS NUMBERS (this is the part the park left implicit):
 *   - raise the counter: refs 8 -> 0.857, 9 -> 0.964, 10 -> 1.071,
 *     ELEVEN -> 1.179, the first value that clears the giv.  Refs are
 *     loop-depth weighted at (depth + 1), so +4 weighted refs is TWO MORE
 *     IN-LOOP USES OF j, which are two instructions the ROM does not have.
 *     Shortening the counter instead needs live_length <= 12 against a loop
 *     that is already 11 instructions plus its preheader.
 *   - lower the giv: refs 8 -> 1.000, 7 -> 0.583, SIX -> 0.500 (an exact tie
 *     with the counter, which would be decided by qsort on equal keys -- not
 *     something to rely on), 5 -> 0.417 (a tie with the BASE instead).  Its 9
 *     weighted refs decompose as init(1) + increment set+use(4) + the TWO
 *     `ldrh r0, [r6, r7]` loads(4), and the ROM shows both loads.
 *     Lengthening it instead needs live_length in 55..64 against a 24-insn
 *     range.
 * So the reachable set is empty unless an instruction count changes, which is
 * what the park said.  CLOSED, with a compiler-source citation and an
 * arithmetic impossibility argument rather than a tired author's note.
 *
 * MEASURED IN BATCH 316c, on top of the park's own inert list -- all still 20:
 *   `j = 0; do { ... j++; } while (j < 0x14);`            20
 *   `while (j != 0x14)` instead of `< 0x14`               21
 *   `j <= 0x13`                                           20
 *   caching tbl[j] for the first call only                20 (confirms the
 *                                                         park's "local copy")
 *   `0x600 + tbl[j]` instead of `tbl[j] + 0x600`          20
 *   `tbl[j + 0]` on the first read                        20
 *   an `int k` counter instead of `unsigned int j`        22
 *   an extra in-loop use of j (`if (j == 0x7fffffff)`)    95, +8 bytes,
 *                                                         relocations differ --
 *                                                         the direct test of
 *                                                         "two more uses of j",
 *                                                         and it costs exactly
 *                                                         what the park said
 * The last row is worth keeping: it is the park's prediction measured.
 *
 * ---- everything below is the park's own record and is unchanged ----
 *
 *
 * NON-MATCHING: 20 encodings of 158 differ (objcmp).
 * Size and instruction count both agree (344 bytes, 158 encodings against 158,
 * relocations identical INCLUDING the mid-function `.Lc73f8` pool word), so the
 * 20 is a true distance: every one of them is the SAME register swap or a
 * consequence of it.
 *
 * Builds a 32-entry candidate table of encounter ids whose difficulty sits in a
 * window around the party's average level, then picks one at random.
 *
 * WHAT THE RESIDUE IS.  In the 20-iteration `_GetEnemyInfo` / `_ClearFlag` loop
 * the ROM keeps the loop COUNTER in r5 and the address offset in r6; gcc puts
 * the offset in r5 and the counter in r6.  Everything downstream (the 0x17b scan
 * loop's id, the inner search's `best`/`bestval`) is renumbered off that one
 * swap.  The instruction sequence, the statement order, the pool layout and the
 * loop shapes are all already the ROM's.
 *
 * THE BLOCKER IS global.c's allocno_compare, AND IT IS ARITHMETIC, NOT A TIE.
 * The priority is
 *     floor_log2 (n_refs) * n_refs / live_length * 10000 * size
 * (/opt/camelot-gcc/gcc-2.96/gcc/global.c), n_refs being loop-depth weighted, so
 * from the .17.lreg dump of this very file:
 *     pseudo 38  (the counter `j`)   7 refs / 28 live ->  2*7/28  =  5000
 *     pseudo 133 (the offset giv)    9 refs / 24 live ->  3*9/24  = 11250
 *     pseudo 66  (the table base)    5 refs / 24 live ->  2*5/24  =  4166
 * and the three then take r5, r6, r7 in that order (REG_ALLOC_ORDER reaches r5
 * first once r0-r3 and r4 are ruled out by the two calls in the loop).  The ROM
 * needs 38 > 133 > 66, which requires the counter's ratio to exceed 1.125.  It
 * cannot: the giv's 9 refs are forced (init, increment set+use, and the TWO
 * `ldrh r0,[r6,r7]` loads the ROM itself shows -- the calls make a single cached
 * load impossible), and both live ranges are the same loop.  The counter would
 * need ELEVEN weighted refs, i.e. two more uses of `j` inside the loop body,
 * which would emit instructions the ROM does not have.  This is the
 * REG_ALLOC_ORDER park class with the formula filled in; it is not a spelling
 * that has been missed.
 *
 * THE FOUR LEVERS THAT ARE ALREADY IN THIS FILE, each a single change from the
 * previous best (objcmp differing counts):
 *
 * 1. *** `unsigned short val` WITH AN EXPLICIT `(short)` AT EVERY READ. ***  The
 *    clear loop is `arr[i].val |= 0xffff`, and the ROM's `ldrh / orr / strh`
 *    survives ONLY for an unsigned 16-bit field.  For a `short` field, fold
 *    distributes the truncation and `(short)0xffff == -1` collapses the whole
 *    statement to `mov r2,#1 / neg / strh` -- the load and the pool word go, the
 *    mid-function constant pool shrinks to one word and moves to the end of the
 *    function, and the size drops 8 bytes.  The three COMPARISONS of the same
 *    field are `ldrsh`, so they need the cast back: `(short)arr[i].val`.  That
 *    pair -- unsigned for the or, signed for the compare -- is the whole shape.
 *    Measured: 138 of 158 at 336 bytes with `short val`, 98 with unsigned and no
 *    casts, 25 with both.
 * 2. THE PARTY LOOP IS A GUARDED do-while OVER A SEPARATE COUNTER AND POINTER.
 *    `if (n > 0) { p = buf; i = n; do { ... } while (i != 0); }` gives the ROM's
 *    single `cmp r7,#0 / ble` plus `bne`, and keeps `n` alive in r7 for the
 *    divide while `i` and `p` get their own registers.  `for (i = 0; i < n; i++)`
 *    over `buf[i]` emits a second test and merges `i` with `n` (85 differing);
 *    `for (i = n; i > 0; i--)` gives `bgt` (92).
 * 3. `*p++` IN THE CALL ARGUMENT, not a separate `p++` statement.  Worth 2: it
 *    is what puts `add r6,#2` before `add r8,r3` (25 -> 23).
 * 4. THE INNER SEARCH LOOP NEEDS ITS OWN COUNTER `m`.  Reusing the `i` of the
 *    party and clear loops is 23; a separate `m` is 20.  (This is the mirror of
 *    the batch-286 "one variable shared by two loops" lever -- here the two
 *    ranges must be split, not shared.)
 *
 * WHAT WAS INERT (all single drops, all still 20 unless noted):
 *   _GetEnemyInfo declared void / int instead of `unsigned char *`      inert
 *   _ClearFlag declared int instead of void                             inert
 *   a separate index variable `k` walked beside `j`                     inert
 *   `int j` with an `(unsigned)` cast in the loop condition             inert
 *   a local copy of `tbl[j]` for the second call                        inert
 *   sharing `j` with the 0x17b scan loop                                inert
 *   sharing `j` with the clear loop                              WORSE (25)
 *   an `unsigned` clear-loop counter                             WORSE (27)
 *   `register unsigned int j asm("r5")`                          WORSE (92)
 *   spelling the table read as *(unsigned short *)((char *)tbl + j*2)
 *                                              WORSE (97, 340 bytes)
 *
 * The one-word `_TBL_`-style alias this file would otherwise need does not
 * exist: `.Lc73f8` is already `.global` in asm/rom_b5000/rom_c1a34_c.s, and the
 * tree's own idiom reaches it straight from C --
 * `extern unsigned short tbl[] __asm__(".Lc73f8");` -- exactly as
 * src/rom_b5000/rom_b8228_a_a_a.c names .Lc59a4 and friends.  No label.sym entry
 * is required.  NO SHIM OF ANY KIND IS USED IN THIS FILE.
 *
 * THE .s HOLDS TWO FUNCTIONS AND NO DATA.  The other one, Func_80c1c54, IS
 * exact (see Func_80c1c54.c), so this file is the only thing standing between
 * rom_c1a34_a_a_a_a_b.s and a whole-file conversion.
 *
 * Verify with (run it inside the build container; /opt/gcc296 is not on the host):
 *   python3 tools/objcmp.py scratch_elev/b290/A/Func_80c1afc.park.c \
 *     asm/rom_b5000/rom_c1a34_a_a_a_a_b_a.s --func Func_80c1afc
 */
struct U {
	unsigned char pad00[0xf];
	unsigned char lvl;
};

struct Cand {
	short id;
	unsigned short val;
};

extern void *Func_8004970(int size);
extern int Func_80b6a60(unsigned short *buf);
extern struct U *_GetUnit(int id);
extern int _GetFlagByte(int id);
extern unsigned char *_GetEnemyInfo(int id);
extern void _ClearFlag(int id);
extern int Func_80c1a34(int id);
extern unsigned int Random(void);
extern void free(void *p);
extern unsigned short tbl[] __asm__(".Lc73f8");

int Func_80c1afc(int *out)
{
	unsigned short buf[8];
	struct Cand *arr;
	int n;
	int sum;
	int cnt;
	int i;
	unsigned int j;
	int m;
	int best;
	int bestval;
	unsigned int id;
	int v;
	int ret;
	int k;
	unsigned short *p;

	cnt = 0;
	arr = Func_8004970(0x80);
	sum = 0;
	n = Func_80b6a60(buf);
	if (n > 0) {
		p = buf;
		i = n;
		do {
			sum += _GetUnit(*p++)->lvl;
			i--;
		} while (i != 0);
	}
	sum = sum / n;
	sum += (signed char)_GetFlagByte(0x3f8);
	if (sum <= 0)
		sum = 1;
	if (sum > 0x63)
		sum = 0x63;
	for (i = 0; i < 0x20; i++)
		arr[i].val |= 0xffff;
	for (j = 0; j < 0x14; j++) {
		_GetEnemyInfo(tbl[j]);
		_ClearFlag(tbl[j] + 0x600);
	}
	for (id = 0; id < 0x17c; id++) {
		v = Func_80c1a34(id);
		if (v < 0)
			continue;
		if (v > sum + 3)
			continue;
		best = -1;
		bestval = 0x3e7;
		for (m = 0; m < 0x20; m++) {
			if ((short)arr[m].val < bestval) {
				bestval = (short)arr[m].val;
				best = m;
			}
		}
		if (best < 0)
			continue;
		arr[best].val = v;
		arr[best].id = id;
		cnt++;
	}
	if (cnt > 0x20)
		cnt = 0x20;
	if (cnt != 0) {
		k = (cnt * Random()) >> 16;
		ret = arr[k].id;
		*out = sum - (short)arr[k].val;
	} else {
		*out = cnt;
		ret = 1;
	}
	free(arr);
	return ret;
}
