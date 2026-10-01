/* Func_80b0fa4 -- 0x080b0fa4, SECOND of the two functions in
 * asm/rom_b0000/rom_b0070_a_a_c_c_a_c_a.s (the first is Func_80b0aac, still asm).
 *
 * *** MATCHING.  objcmp: OK Func_80b0fa4 -- 296 bytes, 138 encodings and 8
 * *** relocations identical, at production flags (plain -O2, no per-file row).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_b0000/80b0fa4.c \
 *     asm/rom_b0000/rom_b0070_a_a_c_c_a_c_a.s --func Func_80b0fa4
 * (after landing, the same recipe against src/rom_b0000/rom_b0070_a_a_c_c_a_c_a_b.c)
 *
 * ============================================================================
 * BATCH 314: THE PARK'S NAMED BLOCKER WAS WRONG, AND THE LAST 3 ENCODINGS WERE
 * ONE OPERAND ORDER.
 * ============================================================================
 *
 * The park called the residue "RELOAD'S OUTPUT-RELOAD REGISTER FOR A HIGH-
 * REGISTER ADD" -- allocate_reload_reg's last_spill_reg rotation picking r7
 * because `arr` carries REG_DEAD there, with "no reach" from source because
 * `arr` has no later use to keep r7 busy.  THAT DIAGNOSIS IS REFUTED.  Reload
 * was never choosing freely: IT REUSES THE REGISTER OF INPUT OPERAND 1.
 *
 * The residue, three slots at indices 75..77:
 *     ref    lsl r3,r6,#1 / add r3,r3,r7 / mov r8,r3 / mov r3,#16 / mov fp,r3
 *     ours   lsl r3,r6,#1 / add r7,r7,r3 / mov r3,#16 / mov r8,r7 / mov fp,r3
 *
 * In BOTH streams rd == rn.  thumb addsi3 emits `add %0, %1, %2`, so the
 * destination follows operand 1, and operand 1 follows the PLUS's canonical
 * operand order:
 *     `arr + base`                 -> (plus (reg arr) (mult base 2))  -> rn = r7
 *     `base * 2 + (int)arr`        -> (plus (mult base 2) (reg arr))  -> rn = r3
 * The ROM wants the shift temp in rn, i.e. THE INDEX FIRST AND THE ADDITION
 * DONE IN INTEGER, NOT POINTER, ARITHMETIC.  `p = (short *)(base * 2 + (int)arr);`
 * is byte-exact.  The `mov r3,#16` slot is not independent: once the address
 * lives in r3 until `mov r8,r3`, sched2 can no longer hoist the constant past
 * it, so all three slots close together.
 *
 * WHY THE PARK'S SEVEN INERT SPELLINGS MISSED IT.  Every one of them keeps the
 * POINTER as the left operand -- `&arr[base]`, `base + arr`, `p = arr; p += base;`,
 * `(char *)arr + base*2`, `(int)arr + base*2`, a named index temp.  `base + arr`
 * looks like the flip but is not: C pointer arithmetic, and fold, normalise it
 * back to `arr + base`.  THE FLIP ONLY SURVIVES IF BOTH SIDES ARE INTEGERS.
 *
 * THREE SPELLINGS LAND, all byte-exact; the first is installed:
 *     p = (short *)(base * 2 + (int)arr);
 *     p = (short *)(base * 2 + (unsigned int)arr);
 *     p = (short *)((char *)(base * 2) + (int)arr);
 *
 * ============================================================================
 * THE OTHER LEVERS, ALL STILL LOAD-BEARING (unchanged from the park)
 * ============================================================================
 *
 * 1. THE LOOP IS A GUARDED do-while, NOT A while.  128 -> 38 came from the
 *    `arr`/`itm` restructure; the shape took 38 -> 13 AND FIXED THE INSTRUCTION
 *    COUNT (134 -> 138).  Written `while (base < n) { ...; if (k > 6) break; }`
 *    gcc makes the `k` test the loop's bottom test and puts `base < n` at the
 *    top; the ROM pre-checks `base < n` once before the loop and tests BOTH at
 *    the bottom, which only `if (cond) { do { ... } while (cond); }` produces.
 *
 * 2. THE LOOP CONDITION IS UNSIGNED AND THE ARROW TEST IS SIGNED, IN THE SAME
 *    FUNCTION, ON THE SAME TWO VARIABLES.  `bcs`/`bcc` on `base` vs `n` and
 *    `bhi` on `k` vs 6, but `bge` on `base + 7` vs `n`.  No single pair of
 *    types gives both, and making everything unsigned does not change the
 *    instruction count at all -- so the casts on the loop test are what the
 *    toolchain needs, worth 13 -> 10.
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
 * `n = s->f3a6` reads the signed byte as `ldrb / lsl #24 / asr #24` for free.
 *
 * ============================================================================
 * LANDING PREREQUISITES -- all checked in batch 314, nothing outstanding
 * ============================================================================
 *  * PIN-FREE.  tools/shimcount.py exits 0 with no findings.  No register pins,
 *    no "+r" barrier, no .equ, no volatile.  NO fakematch.txt ROW.
 *  * NO per-file flag override: objcmp's cflags_for() returns plain -O2, so NO
 *    Makefile row either.
 *  * TEXT/TEXT SPLIT, NO DATA.  tools/datacheck.py on the reference is SILENT
 *    (exit 0, no findings) -- re-run in batch 314 with the fixed .lcomm-aware
 *    datacheck, not inherited from the park.
 *  * tools/split_s.py --dry-run asm/rom_b0000/rom_b0070_a_a_c_c_a_c_a.s Func_80b0fa4:
 *        would write ..._a.s  (1 function, 576 lines)   <- Func_80b0aac stays asm
 *        would write ..._b.s  (1 function, 145 lines)   <- this function
 *        would REMOVE ..._a_c_a.s, would rewrite stage1.ld
 *    Landed file: src/rom_b0000/rom_b0070_a_a_c_c_a_c_a_b.c
 *  * NO NEW `.global` ANYWHERE.  The file carries zero .global lines today and
 *    needs none: the retained _a.s calls INTO this function
 *    (`bl Func_80b0fa4`, _a.s-relative line 101), which becomes an ordinary
 *    undefined external once this side is C and the C definition is global.
 *    This function references nothing in Func_80b0aac.
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
		p = (short *)(base * 2 + (int)arr);
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
