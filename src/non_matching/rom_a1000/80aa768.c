/* Func_80aa768  --  0x080aa768, from asm/rom_a1000/rom_aa538_c_c_a_a.s.
 * The Djinn screen's state machine: a 16-state dispatch around a real jump
 * table, driving set / remove / transfer of a djinni between party members.
 *
 * NON-MATCHING, 527 of 589 encodings differ
 *
 * MEASURED AT PRODUCTION FLAGS with tools/objcmp.py (the figure above is that
 * tool's, not aligncmp's and not a per-flag run):
 *     SIZE      ref 1308 bytes, ours 1312   -- NOT exact, 4 bytes over
 *     COUNT     ref 589 insns,  ours 590    -- NOT exact, one over
 *     aligncmp  346 aligned-equal (58.7% of ref), 319 differing/ins/del,
 *               89 hunks
 *     RELOCATIONS: 56 against 56, SAME SYMBOLS IN THE SAME ORDER since the
 *               first candidate.  The program shape is right; what is left is
 *               one literal-pool class and register/scratch choice.
 * Because neither size nor count is exact the 527 SATURATES and cannot rank
 * against another candidate -- see the four-way table below, where the LOWEST
 * raw figure belongs to the WRONG PROGRAM.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_a1000/80aa768.c \
 *     asm/rom_a1000/rom_aa538_c_c_a_a.s --func Func_80aa768
 *
 * SPLIT SHAPE -- re-confirmed with tools/split_s.py --dry-run this batch
 * (nothing was written; `git status` on asm/ and ld/ is clean):
 *     asm/rom_a1000/rom_aa538_c_c_a_a.s holds TWO functions
 *       ->  rom_aa538_c_c_a_a_a.s   Func_80aa56c   (219 lines)
 *       ->  rom_aa538_c_c_a_a_b.s   Func_80aa768   (648 lines)  <- the target
 *     the original .s is REMOVED and stage1.ld is rewritten by the tool.
 *     Run `make compare` after the split and BEFORE writing any .c.
 *   FINAL INSTALLED PATH (match):  src/rom_a1000/rom_aa538_c_c_a_a_b.c
 * tools/datacheck.py on the .s: NOTHING -- no text/data split beyond the
 *     function cut and no label needs `.global`.
 * tools/shimcount.py on this candidate: NOTHING -- the file is PIN-FREE.
 *     No `register ... __asm__`, no PIN macro, no barrier of any kind.
 *
 * ================================================================
 * WHAT LANDED, WITH FIGURES  (ref 589 insns / 1308 bytes throughout)
 * ================================================================
 *
 * Starting point: the recon at scratch_elev/b302e/RECON_Func_80aa768.c, which
 * had no candidate and no figure.  First compile of the structure it describes
 * read 574 of 589, size 1316, count 592 -- and the 56 relocations already
 * matched symbol-for-symbol, in order, which is the cheapest confirmation the
 * program shape is right before spending anything on allocation.
 *
 * 1. `char` IS UNSIGNED IN THIS TOOLCHAIN, so `*(char *)(p + 0x1c)` emitted
 *    `ldrb r3, [r7, #0x1c]` where the ROM has `mov r3, #0x1c / ldrsb r3,
 *    [r7, r3]`.  Three sites.  `signed char` is required, and it is not a
 *    one-instruction change: Thumb-1 `ldrsb` has NO immediate-offset form, so
 *    the signed spelling also forces the offset into a register, which is the
 *    ROM's two-instruction pair.  Worth noting for the next reader: the
 *    unsigned spelling is SHORTER, so it scores better on size while being
 *    the wrong program.
 *
 * 2. ONE VARIABLE PER SWITCH ARM WAS THE WHOLE BALLGAME -- 566 -> 474, and it
 *    is the finding of this attempt.  With the temporaries (`off`, `k`, `i`,
 *    `found`, `v`, `t`, `q`, `x`) declared at FUNCTION level and therefore
 *    shared between arms, gcc-2.96 gives each ONE pseudo whose live_length is
 *    the SUM of all its disjoint ranges.  That inflates the global-allocno set
 *    enough that the loop variable `n` -- the call result, live across the back
 *    edge -- won a callee-saved HIGH register (r8), which in Thumb-1 costs a
 *    `mov rlo, r8` before every `cmp n, #K` in the dispatch chain, fifteen of
 *    them, plus a third high-register save in the prologue (`push {r5,r6,r7}`
 *    against the ROM's `push {r6,r7}`) and a rotation of `ret`/`done` into
 *    r9/r10 against the ROM's r10/r8.
 *
 *    Re-declaring the temporaries INSIDE braces on each `case` dropped `n` to
 *    **r4 with caller-saves**, which is exactly the ROM: `sub sp, #8`,
 *    `str r4, [sp]` before each call and `ldr r4, [sp]` after, `mov r8` /
 *    `mov r10` for the two high quantities, and the prologue then agrees
 *    instruction for instruction.
 *
 *    THE MECHANISM, and it is the part worth transferring.  `-fcall-used-r4`
 *    is in this project's production CFLAGS, so r4 is call-clobbered here and
 *    NO call-saved LOW register can be free for `n` once r5/r6/r7 are taken by
 *    `state`, the +0x21a pointer and `p`.  gcc-2.96's global.c `find_reg`
 *    reaches a call-clobbered register only through its
 *    `accept_call_clobbered` retry, and that retry is gated on
 *    `CALLER_SAVE_PROFITABLE (n_refs, calls_crossed)` -- so the choice between
 *    "a high register plus a mov at every use" and "r4 plus a spill at every
 *    call" turns on the allocno's ref/live-length ratio, which is precisely
 *    what widening a temporary's scope destroys.  `allocno_compare` uses
 *    `floor_log2 (n_refs) * freq / live_length`; scope is the only one of the
 *    three a spelling can move this far.
 *
 *    THIS IS THE COMPLEMENT OF BATCH 304'S REUSE LEVER, NOT A CONTRADICTION.
 *    Batch 304 landed 17 functions by REUSING one variable so a later range
 *    INHERITS an earlier range's register.  That lever needs the count already
 *    exact and a donor already sitting in the ROM's target register.  Here
 *    neither held: the count was three over and the registers the ROM wants
 *    are r4-plus-a-stack-slot, which no donor can hand over.  The reuse lever
 *    RAISES an allocno's priority by giving it more refs; splitting by region
 *    LOWERS a competitor's by shortening it.  Ask which direction the ROM's
 *    allocation needs before reaching for either.
 *
 * 3. THE OFFSET AS A NAMED LOCAL, built in its own statement -- batch 303's
 *    lever, and it paid three times here even though `p` is a loaded POINTER
 *    and not a SYMBOL_REF, so the pool-folding half of that finding does not
 *    apply.  What applies is the ADDRESSING half.  Written inline,
 *        *(u16 *)(p + 0x208 + idx * 2)
 *    folds to `(p + 0x208) + idx*2` and gcc materialises the whole address
 *    (`add r3, r7 / add r3, r2 / ldrh r2, [r3]`); the ROM keeps the entire
 *    offset in one register and uses Thumb's reg+reg form
 *    (`add r3, r2 / ldrh r2, [r7, r3]`).  Hoisting the sum into its own
 *    statement -- `off = 0x208 + *(signed char *)(p + 0x1c) * 2;` and then
 *    `*(u16 *)(p + off)` -- produces the ROM's form exactly.  Parenthesising
 *    instead (`p + (0x208 + ...)`) is INERT: fold's `associate:` pulls the
 *    constant back out, which is the same trap batch 298 recorded.
 *    Same fix carried the +0x1d site (now 10 insns for 10, shape-exact) and
 *    the two search loops, where it additionally removed a three-instruction
 *    sign-extension: `*(signed char *)(q + v + 0xa0)` compiles to
 *    `ldrb / lsl #24 / asr #24` because the address became an immediate
 *    offset, while `off = v + 0xa0; *(signed char *)(q + off)` gives the ROM's
 *    single `ldrsb r3, [r2, r5]`.
 *
 * 4. STRICT ALIASING WAS DELETING A LOAD THE ROM PERFORMS.  In cases 10 and 3
 *    the ROM reads the same halfword TWICE with a word store between:
 *        ldrh r2,[r7,r3] / str r2,[r7,#8] / ldrh r2,[r7,r3] / strb r2,[r3]
 *    The obvious spelling -- `*(int *)(p + 8) = *(u16 *)(p + off);` then
 *    `p[0x21a] = *(u16 *)(p + off);` -- puts the store in `int`'s alias set
 *    and the load in `short`'s, so under -O2's `-fstrict-aliasing` the store
 *    does not kill the load and CSE deletes the second one.  The documented
 *    escape works: a COMPONENT_REF whose base is a UNION type returns alias
 *    set 0 (c-common.c's union type-punning clause), so
 *        union word { int i; };
 *        ((union word *)(p + 8))->i = *(u16 *)(p + off);
 *    keeps the `str` and restores BOTH `ldrh`.  Two sites, +2 insns, and they
 *    are the ROM's own two.
 *
 * 5. Two inner loops, two DIFFERENT source forms, and the asm says which.
 *    Case 3's counter loop shows `duplicate_loop_exit_test`'s signature -- a
 *    guard using an already-loaded count, then the test at the BOTTOM -- plus
 *    a strength-reduced offset giv (`add r2, #2`) and a hoisted count pointer.
 *    Per batch 301 that gate is the RTL signature of a while/for, so it is
 *    written `while (i < p[0x219]) { ...; i++; off += 2; }` and comes out
 *    shape-for-shape.  The search loop in cases 13/14 shows NEITHER: no
 *    duplicated guard, the `*(u8 **)(p + 0x184)` load re-executed every
 *    iteration, and the index arithmetic not reduced.  loop.c only sees loops
 *    delimited by its own NOTE_INSN_LOOP_BEG, so a loop built from `goto`
 *    is INVISIBLE to it -- and this bank writes goto loops (see the landed
 *    sibling src/rom_a1000/rom_aa538_c_c_a_b.c, whose two loops are both
 *    `goto`).  Writing the search as a goto loop reproduces the un-hoisted
 *    load, the absent strength reduction AND the entry-jump layout.  The outer
 *    `if (done == 0) goto top;` is the same choice; it is also why nothing
 *    anywhere in the 16 arms gets hoisted into a preheader.
 *
 * 6. Small ones that are already exact and should not be "fixed":
 *    - `lsl r1,r3,#3 / sub r1,r3 / lsl r1,#3 / add r1,#0x30` is gcc's own
 *      strength reduction of `idx * 56 + 0x30`.  Write the multiply.
 *    - `sub r3, r4, #3 / cmp r3, #1 / bls` is fold_range_test on
 *      `n == 3 || n == 4`.  The four-way `n == 3 || n == 4 || n == 8 || n == 9`
 *      gives exactly the ROM's range-test-then-two-equalities, because
 *      fold merges only the adjacent pair at the bottom of the OR tree.
 *    - `sub r2, #2` (0x254 from 0x256) and `add r2, #0x3a` (0x254 from 0x21a)
 *      are gcc deriving one pooled constant from another already in a
 *      register; they appear on their own, nothing to spell.  Batch 298's
 *      "gcc-2.96 never chains plain CONST_INTs, so a ROM deriving constants
 *      from a held value identifies a NAMED BASE" is TOO STRONG -- this
 *      function chains plain CONST_INTs in three places and two of them
 *      reproduce automatically from bare integer literals.
 *    - the switch's `cmp r5, #0xf / bls` is the tablejump's own unsigned
 *      range check; `int state` is correct and no unsigned type is needed.
 *    - cases 7/13, 6/12, 9/14 and 5/11 FALL THROUGH into each other, which is
 *      why the arms must appear in the order 0,1,2,10,15,8,3,7,13,6,12,9,14,
 *      4,5,11,default -- gcc emits arm bodies in SOURCE order and that order
 *      is readable straight off the ROM's block layout.
 *
 * ================================================================
 * THE FOUR-WAY TABLE, AND WHY THE LOWEST FIGURE IS THE WORST CANDIDATE
 * ================================================================
 *
 *   spelling                              objcmp  size   count  aligned
 *   this file (union, scoped, offsets)      527    1312    590    58.7%
 *   same, union dropped                     570    1308*   588    62.5%
 *   same, offsets inline at the 0x1d site   474    1312    590    61.8%
 *   union + offsets, `found` declared first 502    1316    592    59.3%
 *
 * (*) SIZE EXACT.  Dropping the union gives the ROM's byte count and the best
 * aligned figure, and it is the WRONG PROGRAM: it omits two `ldrh` the ROM
 * performs (item 4), which is batch 300's "a closer SIZE can be a wrong
 * program" for the third time.  The 474 spelling likewise scores best on raw
 * count while getting the +0x1d address form wrong.  This file is parked
 * because every address form, both re-reads, both loop forms and the whole
 * frame agree with the ROM; the remaining residue is enumerable and short.
 *
 * ================================================================
 * THE BLOCKER: AN HImode LITERAL STORE force_const_mem's, AND IT DUMPS THE
 * PENDING POOL MID-FUNCTION.  THIS IS 4 OF THE 4 EXCESS BYTES.
 * ================================================================
 *
 * `*(u16 *)(p + 0x220) = 2;` appears in four arms of case 3.  The ROM compiles
 * each as
 *       mov  r3, #2
 *       strh r3, [r2]
 * and this build compiles each as
 *       ldrh r3, .L76        @ a PC-relative literal-pool halfword
 *       strh r3, [r2]
 * -- same instruction COUNT, but it plants `.short 2` in the pool twice (the
 * function is long enough that gcc dumps the pending pool in the middle of
 * case 3, which is also what pushes our `iwram_3001f2c` pool word away from
 * the ROM's position and is the reason the aligned figure understates how
 * close the body is).
 *
 * WHAT RULES OUT THE ALTERNATIVES.  gcc did NOT split this into
 * `(set (reg:SI) (const_int 2))` + `(set (mem:HI) (subreg:HI ...))` and then
 * lose the register -- there is no `mov` anywhere near it; the constant
 * reaches the store as a POOL MEM, i.e. `force_const_mem` in HImode, so this
 * is expand/reload, not a later pass eating a register.  Measured, counting
 * `ldrh r3, .L` sites against `mov r3, #2` sites in the generated .s:
 *     production flags          4 pooled, 2 mov   (the 2 movs are elsewhere)
 *     -fno-gcse                 4 pooled, 4 mov   (the extra movs are
 *                                                   elsewhere too -- the four
 *                                                   stores are UNCHANGED)
 *     -fno-rerun-cse-after-loop 4 pooled, 2 mov
 *     -fno-cse-follow-jumps     4 pooled, 2 mov
 * So it is NOT gcse PRE commoning the four identical constants (batch 301's
 * "a value stored on three paths is gcse PRE's pseudo" does not apply), NOT
 * cse-after-loop and NOT cse-follow-jumps.  Every one of the four sites pools
 * under every flag tried.
 *
 * THIS IS A PARTIAL CORRECTION to elevation.md's "a plain `p[off] = 0;` always
 * pools" rule: the rule is right about the OUTCOME and wrong about the
 * MECHANISM it implies.  A sibling brief this batch hit the same class in
 * QImode; here it is HImode, the pending pool is dumped TWICE, and no
 * CSE-family flag moves it.  Thumb's `*thumb_movhi_insn` does carry a
 * `mov %0, #%1` alternative for an 8-bit immediate, so the pattern is not the
 * obstacle -- the constant never reaches it as a CONST_INT.  NEXT STEP for
 * anyone with the gcc source to hand: read arm.md's `movhi` expander for
 * TARGET_THUMB and find which arm calls force_const_mem for a MEM
 * destination; a source-level escape, if one exists, has to make the stored
 * value arrive as an SImode REGISTER, and four spellings of a plain literal
 * will not do it.
 *
 * ================================================================
 * THE REST OF THE RESIDUE, enumerated so nobody re-derives it
 * ================================================================
 *
 * a. +1 INSTRUCTION, case 3's counter loop.  The ROM builds the starting
 *    offset 0x144 as `sub r2, #0xd5` off the 0x219 pool constant STILL IN r2,
 *    because it put the hoisted count pointer in r0 (`add r0, r7, r2`).  Ours
 *    consumes the constant's register for the pointer (`add r0, r7, r0`) and
 *    then needs `mov r2, #0xa2 / lsl r2, #1`.  Pure allocation: the derivation
 *    gcc performs unprompted elsewhere in this same function (item 6) is
 *    unavailable once the donor register is destroyed.  Nothing source-level
 *    was found; the `while` form, the offset walk and the two-biv shape are
 *    all already the ROM's.
 *
 * b. -1 INSTRUCTION, the prologue.  The ROM materialises TWO zeros -- r4 for
 *    `ret` and `n`, a separate r1 for `done` -- and ours shares r4 for all
 *    three.  Three orderings of the `n = 0 / ret = 0 / done = 0` statements
 *    were measured; `n = 0` first is the best of them and is what is here.
 *    It changes which pair CSE groups but never produces the second zero.
 *
 * c. +2 INSTRUCTIONS, `mov ip, r3` once in each search loop.  The ROM puts
 *    `found` in r12 and the target byte `t` in r6, so `ldrb r6, [r3]` loads
 *    straight into its home; ours puts `t` in r12, and Thumb-1 `ldrb` cannot
 *    target a high register, so it needs the extra copy.  Both are global
 *    allocnos with calls_crossed == 0, so REG_ALLOC_ORDER {3,2,1,0,12,14,4,
 *    5,6,...} hands r12 to whichever is ranked first.  Declaring `found`
 *    before `t`, after `t`, and last of the block were all measured (502 /
 *    527 / 527); the ranking is driven by live_length, not by the declaration
 *    order tie-break, so this needs `found`'s range shortened or `t`'s
 *    lengthened and no spelling found doing either without changing the
 *    program.  Attribute it to global_alloc's `allocno_compare`, NOT to a
 *    scheduler.
 *
 * d. ~15 BLOCKS differ only in which LOW scratch register holds a `neg`'d -2
 *    or a shifted constant (ROM r2 where we have r1, ROM r1 where we have r3,
 *    and so on) with the instruction sequence identical.  These follow items
 *    a-c; they are not independent findings and should not be chased
 *    separately.
 *
 * e. `Func_80ad5b4(1, 0, 0xc8, 0)` in case 2: the ROM emits the argument
 *    registers 1, 2, 3, 0 and we emit 0, 1, 2, 3.  ONE SITE ONLY -- the second
 *    call to the same function, in case 3, already matches the ROM's
 *    arg-0-last order, because its arg 1 is an expression and
 *    `precompute_register_parameters` hoists it.  Callee confirmed to take
 *    exactly four `int`s (src/non_matching/rom_a1000/80ad5b4.c:65), and the
 *    tree shows gcc-2.96 producing BOTH orders: a 4-argument
 *    `_ClearUIRegion(0,0,0x1e,0x14)` comes out 0,1,2,3 while the 5-argument
 *    `_CreateUIBox(0xd,0,0x11,5,2)` comes out 1,2,3,0, both in
 *    asm/rom_a1000/rom_a1814_c_a_c_c_a_c_c_c_c_a.s.  So arg-0-last is real and
 *    reachable, but the thing that triggers it here is not a stack argument --
 *    this call has none and there is no `sub sp` for outgoing space
 *    (`sub sp, #8` is two caller-save slots: `n` at [sp] and the +0x256
 *    pointer at [sp,#4], both confirmed by the `str`/`ldr` pairs bracketing
 *    the calls).  Do NOT attribute this to sched1: `-da` at production flags
 *    gives 17.lreg 18.greg 19.flow2 20.ce2 23.sched2 25.jump2 26.mach with no
 *    sched1 dump.  Worth about 4 encodings.
 *
 * CHEAPEST NEXT MOVES, in order: (e) is a two-line experiment once someone
 * knows what flips the argument order; (c) is worth 2 and is a live-range
 * question with a small search space; (a) is worth 1.  The HImode pool is
 * worth the 4 bytes of size and the aligned figure, and it is the only item
 * here that looks like it needs the compiler source rather than a spelling.
 */
typedef unsigned char u8;
typedef unsigned short u16;

union word { int i; };

extern u8 *iwram_3001f2c;

extern void WaitFrames(int n);
extern void Func_80aad10(void);
extern unsigned int Func_80aa544(unsigned int a);
extern void Func_80ad5b4(int a, int b, int c, int d);
extern int Func_80ab5e4(int a);
extern int Func_80ae2f4(void);
extern int Func_80ab314(void);
extern void Func_80aafb8(u8 *a);
extern int Func_80ad6d4(int a);
extern int Func_80aaf58(u8 *a);
extern void _PlaySound(int id);
extern void _CalcStats(int a);
extern int _Func_807a498(int a, int b, int c, int d);
extern int _Func_807a350(int a, int b, int c);
extern int _Func_807a458(int a, int b, int c);
extern int _Func_807a3a8(int a, int b, int c);
extern void _SetDjinni(int a, int b, int c);
extern void _Func_80164ac(void *a);

int Func_80aa768(void)
{
	u8 *p = iwram_3001f2c;
	u8 *w;
	int state;
	int done;
	int ret;
	int n;

	w = *(u8 **)(p + 0x14);
	w[5] = 13;
	n = 0;
	ret = 0;
	*(u16 *)(w + 0xc) = 0;
	done = 0;
	Func_80aad10();
	WaitFrames(1);
	state = 2;
top:
	switch (state) {
	case 0:
		if (n < 0) {
			ret = -1;
			done = 1;
		}
		state = 2;
		break;
	case 1:
		break;
	case 2:
		Func_80aa544(0);
		Func_80ad5b4(1, 0, 0xc8, 0);
		n = Func_80ab5e4(0);
		state = 15;
		if (n == 10)
			break;
		state = 0;
		if (n < 0)
			break;
		*(u16 *)(p + 0x176) = *(signed char *)(p + 0x1c);
		state = 10;
		if (n == 7)
			break;
		state = 3;
		break;
	case 10: {
		int off;
		off = 0x208 + *(signed char *)(p + 0x1c) * 2;
		((union word *)(p + 8))->i = *(u16 *)(p + off);
		p[0x21a] = *(u16 *)(p + off);
		n = Func_80ae2f4();
		if (n == -2)
			done = 1;
		state = 2;
		break;
	}
	case 15:
		n = Func_80ab314();
		if (n == -2)
			done = 1;
		state = 2;
		break;
	case 8:
		state = 0;
		if (p[0x218] == 0)
			break;
		n = Func_80ab5e4(1);
		if (n == -2)
			done = 1;
		state = 4;
		if (n < 0)
			break;
		state = 9;
		break;
	case 3: {
		int off;
		int i;
		Func_80aafb8(*(u8 **)(p + 0x184));
		Func_80aa544(-8);
		off = 0x208 + *(signed char *)(p + 0x1c) * 2;
		((union word *)(p + 8))->i = *(u16 *)(p + off);
		p[0x21a] = *(u16 *)(p + off);
		Func_80ad5b4(0, *(signed char *)(p + 0x1c) * 56 + 0x30, 0x36, 0);
		n = Func_80ab5e4(1);
		i = 0;
		off = 0x144;
		while (i < p[0x219]) {
			*(u16 *)(off + p) += 8;
			i++;
			off += 2;
		}
		if (n == -2)
			done = 1;
		if (n < 0) {
			state = 2;
			break;
		}
		if (n == 3 || n == 4 || n == 8 || n == 9) {
			int o2 = 0x208 + *(signed char *)(p + 0x1d) * 2;
			p[0x21b] = *(u16 *)(p + o2);
		}
		if (n < 0) {
			state = 2;
			break;
		}
		if (n == 1) {
			state = 5;
			break;
		}
		if (n == 2) {
			state = 6;
			break;
		}
		if (n == 3) {
			*(u16 *)(p + 0x220) = 2;
			state = 7;
			break;
		}
		if (n == 4) {
			*(u16 *)(p + 0x220) = 2;
			state = 9;
			break;
		}
		if (n == 5) {
			state = 11;
			break;
		}
		if (n == 6) {
			state = 12;
			break;
		}
		if (n == 8) {
			*(u16 *)(p + 0x220) = 2;
			state = 13;
			break;
		}
		if (n == 9) {
			*(u16 *)(p + 0x220) = 2;
			state = 14;
			break;
		}
		break;
	}
	case 7:
		n = Func_80ad6d4(1);
		if (n == -2)
			done = 1;
		state = 3;
		if (n < 0)
			break;
	case 13: {
		u8 *q;
		int off;
		int k;
		int i;
		int found;
		int v;
		int t;
		_PlaySound(0x7e);
		n = _Func_807a498(p[0x21a], p[0x256], p[0x254], p[0x21b]);
		_CalcStats(p[0x21a]);
		_CalcStats(p[0x21b]);
		w = *(u8 **)(p + 0x14);
		w[5] = 13;
		_Func_80164ac(*(void **)(p + 0x30));
		Func_80aaf58(*(u8 **)(p + 0x184));
		v = (u16)(*(u16 *)(p + 0x176) % 10u);
		t = p[0x178];
		off = v + 0xa0;
		i = 0;
		found = 0;
		goto chk13;
	nxt13:
		i++;
	chk13:
		q = *(u8 **)(p + 0x184);
		if (i >= *(signed char *)(q + off))
			goto out13;
		k = (v * 10 + i) * 2;
		if (t != *(u8 *)(q + k))
			goto nxt13;
		found = i;
	out13:
		*(u16 *)(p + 0x174) = v + found * 10;
		w = *(u8 **)(p + 0x14);
		w[5] = 1;
		state = 0;
		break;
	}
	case 6:
		n = Func_80ad6d4(2);
		if (n == -2)
			done = 1;
		state = 3;
		if (n < 0)
			break;
	case 12:
		_PlaySound(0xaf);
		_Func_807a350(p[0x21a], p[0x256], p[0x254]);
		n = _Func_807a458(p[0x21a], p[0x256], p[0x254]);
		_CalcStats(p[0x21a]);
		w = *(u8 **)(p + 0x14);
		w[5] = 13;
		_Func_80164ac(*(void **)(p + 0x30));
		w = *(u8 **)(p + 0x14);
		w[5] = 1;
		state = 2;
		break;
	case 9:
		n = Func_80ad6d4(0);
		if (n == -2)
			done = 1;
		state = 3;
		if (n < 0)
			break;
	case 14: {
		u8 *q;
		int off;
		int k;
		int i;
		int found;
		int v;
		int t;
		_PlaySound(0x7e);
		_Func_807a498(p[0x21a], p[0x256], p[0x254], p[0x21b]);
		n = _Func_807a498(p[0x21b], p[0x257], p[0x255], p[0x21a]);
		_CalcStats(p[0x21a]);
		_CalcStats(p[0x21b]);
		Func_80aaf58(*(u8 **)(p + 0x184));
		v = (u16)(*(u16 *)(p + 0x176) % 10u);
		t = p[0x178];
		off = v + 0xa0;
		i = 0;
		found = 0;
		goto chk14;
	nxt14:
		i++;
	chk14:
		q = *(u8 **)(p + 0x184);
		if (i >= *(signed char *)(q + off))
			goto out14;
		k = (v * 10 + i) * 2;
		if (t != *(u8 *)(q + k))
			goto nxt14;
		found = i;
	out14:
		*(u16 *)(p + 0x174) = v + found * 10;
		w = *(u8 **)(p + 0x14);
		w[5] = 1;
		state = 2;
		break;
	}
	case 4: {
		int x;
		if (n == -1) {
			ret = n;
			state = 2;
			break;
		}
		x = *(u16 *)(p + 0x220);
		if (x & 1) {
			state = 8;
			break;
		}
		if (x & 2) {
			state = 7;
			break;
		}
		break;
	}
	case 5:
		n = Func_80ad6d4(3);
		if (n == -2)
			done = 1;
		state = 3;
		if (n < 0)
			break;
	case 11:
		_PlaySound(0x8b);
		_SetDjinni(p[0x21a], p[0x256], p[0x254]);
		n = _Func_807a3a8(p[0x21a], p[0x256], p[0x254]);
		_CalcStats(p[0x21a]);
		w = *(u8 **)(p + 0x14);
		w[5] = 13;
		_Func_80164ac(*(void **)(p + 0x30));
		w = *(u8 **)(p + 0x14);
		w[5] = 1;
		state = 2;
		break;
	default:
		done = 1;
		break;
	}
	if (done == 0)
		goto top;
	return ret;
}
