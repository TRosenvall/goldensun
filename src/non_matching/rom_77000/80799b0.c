/* Func_80799b0 -- 0x080799b0, asm/rom_77000/rom_79460_c_c_c_a_c_c.s
 *
 * NON-MATCHING, 35 of 142 encodings  (batch 346 brief A, re-measured).
 *   INSTRUCTION COUNTS NOW EQUAL: ref 136, ours 136.  SIZE EQUAL: 296/296.
 *   ALIGNED RESIDUE 21 of 142 in 8 hunks  (tools/aligncmp.py).
 *   Was 108 of 142 / aligned 32 in 16 hunks, with ref 136 against ours 135.
 *
 * *** READ THIS BEFORE TRUSTING THE EQUAL COUNT. ***  The previous header said
 * "No SIZE line: 296 bytes both sides, 142 encodings both sides ... So 108 is a
 * true distance."  THAT WAS WRONG, and it is the trap batch 346's brief names:
 * objcmp's `142 of 142` is the STREAM length (pool words and pad included), and
 * directly above it sat `INSTRUCTION COUNT ref 136, ours 135`.  The parked body
 * was ONE INSTRUCTION SHORT with a `.short 0x0000` pad making the byte size
 * agree.  The counts below really are equal -- but see TWO DEFECTS, CANCELLING.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_77000/80799b0.c \
 *     asm/rom_77000/rom_79460_c_c_c_a_c_c.s --func Func_80799b0
 *
 * THE .s HOLDS ONE FUNCTION AND NO DATA SECTION (grep -ci func_start = 1, datacheck
 * reports nothing), so this converts the file WHOLE when it closes.  It reads
 * `.L84b1c`, which is already .global in asm/rom_77000/rom_79460_c_c_c_c_c_c.s and is
 * unaffected by any split of that file.  No pins, no devices.
 *
 * WHAT THIS BATCH CHANGED, and both changes are load-bearing:
 *
 * 1. `sel = (buf[ix2] > 9) ? ix2 : ix;` INSTEAD OF `sel = ix; if (...) sel = ix2;`
 *    This is the whole of the previous header's blocker (b) -- "the loop's success
 *    block is emitted by the ROM BETWEEN the `sel` selection and the
 *    `bl Func_80797ec` ... Five loop spellings did not move it."  It is not a loop
 *    spelling at all.  With the conditional expression the success block lands in
 *    the ROM's position and the ROM's `b .L79a46` over it appears, which collapsed
 *    SIX residue sites at once: 108 -> 88 of 142, aligned 32 -> 26, hunks 16 -> 15,
 *    and the only remaining unbalanced hunk became the single instruction below.
 *    The if/else form (`if (...) sel = ix2; else sel = ix;`) is EXACTLY EQUIVALENT
 *    (both read 26 in 15 hunks); the park's initialise-then-override form is not.
 *
 * 2. An explicit walking pointer `q` in the first max pass.  This equalises the
 *    instruction count and the byte size -- see the warning below.
 *
 * TWO DEFECTS, CANCELLING -- THIS IS NOT ONE MATCHING LENGTH.
 *   -1  the ROM's `adds r0, r5, #0` before `bl Func_80797ec` is still absent
 *   +1  the ROM's `ldmia r2!, {r3}` in the first max pass is `ldr r3, [r1, #0]`
 *       plus `adds r1, #4` here, because `q` replaced `buf[i]`
 * Reverting change 2 gives the CLEANER DIAGNOSIS (one defect, aligned 26 in 15
 * hunks, ref 136 / ours 135) and is kept in this batch's scratch as p3/s1.c.
 * Change 2 is kept because it is better on every measured number -- 35 vs 88 of
 * 142, aligned 21 vs 26, 8 hunks vs 15 -- but a future agent MUST NOT read the
 * equal count as "the length is closed".
 *
 * THE ONE REAL BLOCKER: a THREE-WAY HARD-REGISTER ROTATION, and the arithmetic
 * is now pinned down rather than asserted.
 *   ROM   i -> r0   walking pointer -> r2   ix -> r5   (ix2 r4, mx/mx2 ip, best r8)
 *   ours  i -> r2   walking pointer -> r1   ix -> r0
 * `ix` is copied into r0 for argument 0 of `Func_80797ec`, so when `ix` OWNS r0
 * the copy is a nop and vanishes.  That nop-elimination IS the missing
 * instruction.  REG_ALLOC_ORDER is {3,2,1,0,12,14,4,5,...} (arm.h:989, verified,
 * not recalled) and `ix` prefers r0 (greg: ";; 35 preferences: 0") because of
 * that argument copy, so it takes r0 the moment r0 is still free when its turn
 * comes.  In the ROM r0 is NOT free: three LO_REGS allocnos that conflict with
 * `ix` are placed first.  `ix` prefers LO_REGS, so r12/r14 are not available to
 * it even though gcc uses both as general registers here -- which is why it
 * lands on r5 and not on ip/lr.
 *
 * MEASURED PRIORITIES (`-da`, `.17.lreg` + `.18.greg` line 4, allocno_compare at
 * global.c:598-621 = floor_log2(n_refs)*n_refs/live_length*10000*size, no
 * frequency term, in-loop refs doubled by flow.c:5114-5116, ties on ASCENDING
 * allocno number):
 *   parked body   i (reg 40) 20 refs / 40 insns -> 2.00
 *                 ptr (120)   7 refs / 10 insns -> 1.40
 *                 ptr (125)   5 refs /  8 insns -> 1.25
 *                 sel (38)    3 refs /  6 insns -> 0.50
 *                 ix  (35)    7 refs / 54 insns -> 0.26
 *   greg order 55 62 108 107 [40] 120 125 42 32 34 38 50 129 36 37 41 [35] ...
 * `i` beats both pointers, so it takes r2 (r3 is held by the two 4.00-priority
 * scratch allocnos 55/62, which conflict with it), the pointer takes r1, and r0
 * is still free when `ix` arrives.
 *
 * THE PRIORITY FLIP IS REACHABLE AND IT IS NOT SUFFICIENT.  A separate counter
 * per max pass puts both counters at 3*9/18 = 1.50 and 3*11/22 = 1.50, still
 * above the pointer's 1.40 -- which is why the previous header's "separate
 * counter" rows were inert.  Hoisting each counter's `i = 0` ABOVE the two
 * `mx`/`ix` initialisers and using `while` instead of `for` adds two insns of
 * live range WITHOUT adding a ref: 3*9/22 = 1.227 and 3*11/26 = 1.269, both
 * BELOW 1.40, and the greg order duly flips to `... 120 44 124 40 42 ...`.
 * MEASURED: `i` moves off r2 (to r1) and `ix` STAYS ON r0; aligned 23 in 17
 * hunks, still ref 136 / ours 135.  So demoting `i` is not the lever -- what is
 * needed is a THIRD LO_REGS allocno conflicting with `ix` and ranking above it,
 * or something that prunes r0 out of `ix`'s own preference
 * (global.c:1630-1642, :897-909, :1016 -- a range overlapping hard r0).  That
 * variant is kept as p3/y3.c.
 *
 * BOTH CHEAP AXES CLOSED ON ALL THREE BODIES (tools/autosweep.py, batch 346):
 *   parked body  flags 8/8 inert or worse (best -fno-schedule-insns2 103 of 142,
 *                but aligned 32 -> 59, so NOT progress); anchor 23/23 inert.
 *                anchor20 reads 94 with dsize=-4: misalignment, not an
 *                improvement.  anchor18 reads 107 with `first` unmoved: phase.
 *   s1 body      flags 8/8 inert (-fno-schedule-insns2 83 of 142, aligned
 *                26 -> 52); anchor 22/22 inert.  anchor17 reads 87 with `first`
 *                unmoved at 14 -- re-measured by hand, aligned still 26: PHASE.
 *   THIS body    flags 8/8 inert (-fno-strict-aliasing exactly 35, i.e. no
 *                effect; -fno-schedule-insns2 63); anchor 24/24 inert.
 *
 * MEASURED INERT ON THIS BODY (all five BIT-IDENTICAL to it at 35 / aligned 21):
 *   inlining the conditional expression into the call's argument list;
 *   deleting the `Func_80797ec` prototype; declaring it `extern int F();`
 *   (unspecified args); giving `Func_80797fc` an `int` return type instead of
 *   `void`.  The sweep_decls.py argument-fill-order lever does not reach this.
 * MEASURED INERT ON THE s1 BODY (all BIT-IDENTICAL at 88 / aligned 26):
 *   eight declaration-order permutations of the twelve locals -- including
 *   `i` first, `i` last, `ix`/`ix2` first, and `x` before `buf` (the previous
 *   header's "all 24 permutations ... never flips the choice" survives, and
 *   the reason is now arithmetic: allocno_compare's priorities are not tied, so
 *   the ascending-allocno tie-break never runs);
 *   `i < 4` for `i <= 3`; `do/while` and `while` forms of the first max pass;
 *   `continue` instead of a nested `if` in the second;  `ix != i` for `i != ix`;
 *   naming `buf[i]` in a local (28, worse);  `ix = best` before `mx = best`
 *   (27, worse);  initialising mx/ix from the literal -1 instead of `best`.
 * MEASURED WORSE: sharing one counter between the max passes and the table walk
 *   (aligned 35 in 23 hunks);  `mx` reused as `mx2` (ours 134, 4 bytes short);
 *   reading `ix` after the call to lengthen its range (ours 139, +3);  routing
 *   argument 0 through a fresh local `x` (ours 138);  `&buf[i] != &buf[ix]`
 *   (ours 137);  naming `buf[ix2]` before the selection (ours 137);
 *   `*q++` with `q[-1]` in the first max pass (136/136 but aligned 44).
 *
 * THREE THINGS THIS ROM TELLS YOU, all three still in the source and all three
 * re-confirmed as byte-identical regions this batch:
 *
 * 1. *** Func_80797ec TAKES TWO ARGUMENTS. ***  The ROM computes r1 -- `mov r1,r5`,
 *    then `mov r1,r4` if buf[ix2] > 9 -- and never reads it again except that `bl
 *    Func_80797ec` follows.  Declared with one parameter, `sel` is dead and gcc
 *    deletes the whole selection, which is exactly the 8 instructions that were
 *    missing.
 * 2. Func_80797fc IS CALLED WITH AN UNINITIALISED MIDDLE ARGUMENT.  The ROM sets r0
 *    and r2 and leaves r1 untouched across `bl Func_80797fc`.  Passing an
 *    uninitialised `int x` reproduces it and costs nothing; the first 14 encodings
 *    match.
 * 3. `if (best != -1) return best;` IS DEAD CODE THAT THE ROM EMITS.  `best` is -1
 *    from the first instruction and never written before the test, yet the ROM spends
 *    `mov r0,#1 / neg r0,r0 / cmp r8,r0 / bne` on it.  cse loses the constant across
 *    three conditional branches and two calls, so the test survives while `return
 *    best` folds to `return -1`.
 *
 * SHAPES CONFIRMED CORRECT (byte-identical regions):
 *   - the requirement compare is `buf[j] < req[j] * 10`, and the ROM's
 *     `lsl r2,r3,#2 / add r2,r3 / lsl r2,#1` is `* 10` written as `* 10`.
 *   - the inner loop is rotated with its j == 0 test peeled and exits with j == 4;
 *     `for (j = 0; j <= 3; j++) ... break;` then `if (j == 4)` gives exactly that.
 *   - the table entry is 0x54 bytes: an `int` id then four requirement bytes.  The
 *     walk is `for (n = 0xca; n >= 0; n--)`, strength-reduced by loop.c into the ROM's
 *     two `sub #0x54` pointers with the base in r10 copied to r14.
 *   - the ROM's first max pass walks `ldmia r2!, {r3}` -- gcc's SImode
 *     post-increment load, from `buf[i]` with `i` the only index.  THIS BODY GIVES
 *     THAT UP on purpose (see TWO DEFECTS above); p3/s1.c keeps it.
 *
 * NEXT: the rotation, and only the rotation.  Two routes neither this batch nor
 * any earlier one has tried: (a) find a LO_REGS value that must be live across
 * both max passes and the selection -- a third conflicting allocno above `ix` in
 * priority is what the ROM has and we do not; (b) make `ix`'s range overlap hard
 * r0 so prune_preferences drops r0 from its preference list, which needs `ix`
 * live across an insn that SETS r0 and therefore probably a different placement
 * of the dead `best != -1` test rather than a new statement.
 * Do NOT re-screen: the flag axis, the anchor axis, declaration order, the
 * callee-prototype axis, or any loop spelling of the two max passes.
 */
struct Entry {
	int id;
	unsigned char req[4];
	unsigned char pad08[0x54 - 8];
};

extern void Func_80797fc(int id, int x, int *out);
extern int GetFlag(int id);
extern int Func_80797ec(int i, int sel);
extern struct Entry L84b1c[] __asm__(".L84b1c");

int Func_80799b0(int id)
{
	int buf[4];
	int best;
	int mx;
	int ix;
	int mx2;
	int ix2;
	int sel;
	int v;
	int i;
	int n;
	int j;
	int x;
	int *q;

	best = -1;
	if (id > 7)
		return 0;
	Func_80797fc(id, x, buf);
	if (GetFlag(0x20) != 0) {
		if (id == 0)
			return 0xc8;
		if (id == 1)
			return 0xc9;
	}
	if (id == 5)
		return 0xca;
	if (best != -1)
		return best;
	mx = best;
	ix = best;
	q = buf;
	for (i = 0; i <= 3; i++) {
		if (mx < *q) {
			mx = *q;
			ix = i;
		}
		q++;
	}
	mx2 = -1;
	ix2 = -1;
	for (i = 0; i <= 3; i++) {
		if (i != ix) {
			if (mx2 < buf[i]) {
				mx2 = buf[i];
				ix2 = i;
			}
		}
	}
	sel = (buf[ix2] > 9) ? ix2 : ix;
	v = Func_80797ec(ix, sel);
	for (n = 0xca; n >= 0; n--) {
		if (L84b1c[n].id == v) {
			for (j = 0; j <= 3; j++) {
				if (buf[j] < L84b1c[n].req[j] * 10)
					break;
			}
			if (j == 4) {
				best = n;
				break;
			}
		}
	}
	if (best == -1)
		best = 0;
	return best;
}
