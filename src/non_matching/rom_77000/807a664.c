/* Func_807a664 -- 0x0807a664, asm/rom_77000/rom_79460_c_c_c_c_c_c.s
 * NON-MATCHING, 8 encodings of 143.  A TRUE DISTANCE, and the closest park in the tree: 316 bytes, 143 encodings and all ten
 * relocations exact -- objcmp prints neither a SIZE nor a RELOCATIONS line.  DOWN FROM 31.
 * `--align` 18 of 145, from 27.
 *
 * (Claim line first on purpose: parkcheck reads the FIRST `N encodings of M`.)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_77000/807a664.c \
 *     asm/rom_77000/rom_79460_c_c_c_c_c_c.s --func Func_807a664
 *
 * NON-MATCHING: 8 encodings of 143 differ (objcmp).  Was 31.
 *
 * objcmp --func Func_807a664, verbatim:
 *   XX ENCODINGS differ in 8 place(s) (ref 143, ours 143)
 *      first at index 89: ref 1818  ours 18c0
 * Still no SIZE line and no RELOCATIONS line: 316 bytes both sides, 143 encodings both
 * sides, all 10 relocations identical in type, symbol AND offset.  TRUE DISTANCE.
 * tryc --align says 18 instructions in disagreeing regions of 145 (was 27).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_77000/807a664.c \
 *     asm/rom_77000/rom_79460_c_c_c_c_c_c.s --func Func_807a664
 *
 * THE SPLIT is unchanged and needs ZERO exports -- see the previous revision of this
 * header, and datacheck's per-function output.  The .rodata run stays with the remaining
 * .s, which keeps the (also parked) Func_807a7a0.
 *
 * ============================================================================
 * BATCH 294: THE MISSING `lsl` IS REACHED, AND IT COST NOTHING.  31 -> 15 -> 8.
 * ============================================================================
 *
 * The previous revision of this park concluded that the ROM's
 *     ldrh r2,[r4] / lsl r3,r2,#16 / cmp r3,#0
 * was "unreachable by arithmetic, not merely unbeaten", on the reasoning that our value's
 * only producer is a zero-extending halfword load, so combine's nonzero_bits can always
 * see nonzero_bits(v) == 0xffff and simplify_comparison's ASHIFT case always drops the
 * shift.  The reasoning about nonzero_bits is right.  The conclusion was wrong, because
 * THE LOAD IS NOT THE ONLY THING THAT SETS THE PSEUDO'S KNOWN BITS -- THE FORM OF THE
 * DESTINATION DOES TOO.
 *
 * 7. *** LOOP 3's VALUE IS A ONE-MEMBER u16 STRUCT, NOT AN int. ***  `struct ItemSlot
 *    { unsigned short id; }`, a local `v` of that type, `v.id = *src++;` and
 *    `if ((v.id << 16) != 0)`.
 *      int v, any of four spellings of the test    31 / 27
 *      struct ItemSlot v                          15 / 25   <- the lsl appears
 *
 *    THE MECHANISM, as far as I verified it -- and I am explicit below about where the
 *    verification stops, because my first write-up of it was wrong:
 *      - combine.c:10749, simplify_comparison's ASHIFT case, drops `(v << 16) != 0` exactly
 *        when `nonzero_bits (operand, SImode) & ~(0xffffffff >> 16) == 0`, i.e. when no bit
 *        at or above 16 is possible.  (`! equality_comparison_p` is 0 for NE/EQ.)  That part
 *        of the old diagnosis is confirmed against the source.
 *      - A one-member `unsigned short` struct local is a GENUINE HImode PSEUDO, not an
 *        SImode one.  `promote_mode` widens only INTEGER_TYPE, ENUMERAL_TYPE, BOOLEAN_TYPE,
 *        CHAR_TYPE, REAL_TYPE and OFFSET_TYPE; a RECORD_TYPE falls through to
 *        `default: break` and keeps its own mode.  So the promotion that makes a bare
 *        `unsigned short` local an SImode pseudo (arm.h:597, batch 293's finding 5) does not
 *        happen here.  VERIFIED in the -dr dump of probe2.c's q1:
 *            (insn 27 (set (reg:HI 37) (mem:HI (reg/v:SI 32))))   *thumb_movhi_insn -> ldrh
 *        and the compare reads
 *            (ashift:SI (subreg:SI (reg:HI 37) 0) (const_int 16))
 *        -- a PARADOXICAL SUBREG of a narrow pseudo, where the `int` spelling gives a plain
 *        SImode pseudo set by (zero_extend:SI (mem:HI)).
 *      - nonzero_bits' REG case answers 0xffff from `reg_last_set_nonzero_bits[regno]`
 *        through a fast path guarded by `reg_last_set_mode[regno] == mode`.  With a HImode
 *        pseudo and an SImode query that guard fails, and the answer comes from one of the
 *        fallbacks (get_last_value, or the global reg_nonzero_bits) instead.
 *      - WHAT I DID NOT DO: single-step which fallback fires.  Do not treat the last bullet
 *        as established.  It is also NOT true that the `int` spelling always loses the shift:
 *        in a call-free, loop-free standalone function an `int` carrier keeps it too
 *        (probe3.c r4).  The struct is what makes the shift survive IN THIS LOOP, where the
 *        cursor is incremented and the value is also stored; the isolated-function behaviour
 *        is different and is not the measurement that matters.
 *      - A `union { unsigned short h; }` member behaves identically (probe3.c r5).
 *      - It costs nothing: the struct is one halfword, it stays in a register, and the store
 *        `*dst++ = v.id` is the same `strh` as before.
 *
 *    The isolated proof is scratch_elev/b294/F/probe2.c q1, whose loop is
 *    INSTRUCTION-FOR-INSTRUCTION the ROM's, registers included:
 *        ldrh r2,[r0] / lsl r3,r2,#16 / add r0,#2 / cmp r3,#0 / beq / strh r2,[r1] /
 *        add r5,#1 / add r1,#2 / sub r4,#1 / cmp r4,#0 / bge
 *
 * 8. *** LOOP 3's RUNNING COUNT REUSES `n`, LOOPS 1 AND 2's COUNTER. ***  The ROM keeps
 *    the count in r5 -- the register loops 1 and 2 count in -- and gives loop 3's own
 *    index r6.  With a separate `cnt` we got that pair the other way round and NOTHING
 *    moved it: all 119 permutations of the five int declarations measure exactly 15
 *    (see INERT below).  Sharing the pseudo forces the register instead of asking for it.
 *      separate int cnt                           15 / 25
 *      the count reuses n                          8 / 18   <- in the file
 *
 * Constructs 1-6 are unchanged from the previous revision and are not restated; note only
 * that construct 6 ("`v` in loop 3 is `int`") is SUPERSEDED by construct 7 -- `int` beat
 * `short` and `unsigned short`, but a struct beats `int`.
 *
 * SHIMS: none.  Zero `register ... __asm__` declarations, zero `__asm__(".equ ...")`
 * lines, no flags, no pins.
 *
 * ADDED TO THE INERT LIST (each a single drop from the 8 / 18 file unless noted)
 *   ALL 119 PERMUTATIONS of `int i; int n; int j; int cnt; int m;`      15 each, from 15
 *     -- declaration order does not break an allocno tie in this function.  Measured
 *     exhaustively, not sampled; do not spend budget on decl order here again.
 *   base = (unsigned short *)((int)base + n * 2)                        inert
 *   base = (unsigned short *)(n * 2 + (int)base)                        inert
 *   base = (unsigned short *)((char *)base + n * 2)                     inert
 *   the doubled count in its own int local, written first in the add    inert
 *   base = &base[n]                                                     inert
 *   m seeded as `m = n; m = 15 - m;`                                    inert
 *   the tail pair as p[0]/p[1] instead of *p and p[1]                   inert
 *   a trailing p++ after the last tail store                            inert
 *   ewram_2000438 declared with a bound [2]                             inert
 *   -fno-regmove                                                        inert (diagnostic)
 *   the fill loop's store before its decrement                          9 (worse)
 *   the fill countdown reusing n as well (one pseudo for all three)      9 -- it DOES buy
 *     the ROM's `sub r5, r3, r5`, and loses the pooled zero's register and position
 *   dst = base + n instead of base += n                                10 (worse)
 *   `short *src` + the struct carrier                                  measured, worse
 *   loop 3 written as a source count-down (j = 14; j >= 0; j--)         inert
 *   --no-sched2                                                        55 (much worse:
 *     positive evidence sched2 is doing the right work here)
 *
 * TWO CORRECTIONS TO THE PREVIOUS REVISION, both load-bearing
 *  i. "The whole residue is one instruction" was never true of this park, and the brief
 *     that quoted it inherited the error.  What is true is that the MNEMONIC HISTOGRAMS
 *     differed in one place; the ENCODING residue at 31 was mostly loop 3's register
 *     assignment, which the old blocker list itself put first.  The two are different
 *     measurements and only the second is the distance.
 * ii. The old inert entry `short *src  50 / 69` and `short h carrier  50 / 69` were
 *     recorded against the pre-`int v` baseline.  Re-measured coupled with `int v` they
 *     give 50 / 69 again -- so those two entries were, unusually, right in isolation.
 *     The entry that was NOT right in isolation is `m` reusing `cnt`: recorded as a
 *     regression (57 / 83), it is worth 15 -> 9 once construct 7 has landed, and it is
 *     the only spelling that reaches the ROM's `sub r5, r3, r5`.
 *
 * THE REMAINING 8, all in two places, and none of them arithmetic
 *   a. the zero-fill's `add r0, r3, r0` against our `add r0, r0, r3` (1 encoding).  The
 *      RTL plus-operand order, not the source's: six spellings of the advance, including
 *      three that take the pointer out of pointer arithmetic entirely so the C front end's
 *      pointer_int_sum cannot force the pointer first, all emit the identical insn.
 *      -fno-regmove does not move it either, so it is not the tie-up pass.
 *   b. `sub r5, r3, r5` against `sub r3, r3, r5` plus the two fill-loop insns that follow
 *      it (3 encodings): the ROM gives `m` the register `n` has just vacated, we give it
 *      the register that held the constant 15.  Reusing `n` for `m` too gets the sub
 *      exactly right and then costs the pooled zero its register and its position, for a
 *      net loss of one.  A coupled lever with nothing to couple to yet.
 *   c. `ldr r0, =ewram_2000438` sits one position earlier in the ROM and in r0 rather
 *      than r2 (3 encodings, plus the two ldrh that read through it).  sched2 hoists it
 *      above `add r8, r2` in the ROM; --no-sched2 makes the whole function much worse, so
 *      this is the scheduler making a different choice, not a missing source shape.
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

struct ItemSlot {
	unsigned short id;
};

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
	int m;
	struct ItemSlot v;

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
			n = 0;
			src = base;
			dst = base;
			for (j = 0; j <= 14; j++) {
				v.id = *src++;
				if ((v.id << 16) != 0) {
					*dst++ = v.id;
					n++;
				}
			}
			if (n <= 14) {
				base += n;
				m = 15 - n;
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
