/* Func_8078870 (0x08078870) -- asm/rom_77000/rom_78414_c_c_a_c_a_c_c.s (2 functions).
 *
 * STILL NON-MATCHING, 5 of 40 encodings -- this park's own body, unchanged; see
 * the coordinator's correction below.  SIZE EXACT (40 against 40), relocations
 * identical.  A device-bearing variant reaches 2 of 40 with crossfire.py's
 * memory screen reporting the REFERENCE's own profile exactly (ldr=1 ldrb=1
 * ldrh=3), so that 2 is not a figure bought by doing less work than the ROM --
 * but it IS bought by a device, and is recorded as a figure about the blocker.
 *
 * *** CORRECTED BY THE COORDINATOR, BATCH 322.  The brief reported "device-free
 * *** it is 5, i.e. the park's figure".  MEASURED: that is not so.  Three
 * *** distinct bodies, three distinct figures --
 * ***     this park's body (below), device-free ............ 5 of 40
 * ***     the brief's body WITH the volatile cast .......... 2 of 40
 * ***     the brief's body with the cast removed ........... 7 of 40
 * *** So the brief's body is better ONLY WITH THE DEVICE, and device-free it is
 * *** WORSE than what was already here.  THE BODY BELOW IS THEREFORE THIS PARK'S
 * *** ORIGINAL, UNCHANGED, at 5; the analysis above and below is the brief's and
 * *** is kept because it is correct and valuable.
 * ***
 * *** AND THE CAST IS A DEVICE, NOT THE DOCUMENTED LEVER.  The same lvalue
 * *** `*(unsigned short *)p` is read THREE TIMES in that loop -- twice plain and
 * *** once volatile -- so the qualifier says nothing true about the data; it
 * *** exists only to stop cse commoning the third read.  Contrast the lever in
 * *** docs/humanization.md, where pokefirered's ORIGINAL source cast a `u8 *`
 * *** pointer to `vu16` at the one access whose WIDTH was a real property, and
 * *** did so consistently.
 * ***     A VOLATILE CAST APPLIED TO ONE OF SEVERAL READS OF THE SAME LVALUE IS
 * ***     A DEVICE.  Applied consistently to an access whose width or ordering
 * ***     is a genuine property of the data, it is a lever.
 * *** The 2 is kept as a figure ABOUT THE BLOCKER: it says that once that third
 * *** read is not commoned, three of the five encodings close.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     scratch_elev/b322/F/p2_candidate.c \
 *     asm/rom_77000/rom_78414_c_c_a_c_a_c_c.s --func Func_8078870
 *
 * INSTALLED PATH, if it ever lands: src/rom_77000/rom_78414_c_c_a_c_a_c_c_b.c.
 * Split shape: TEXT-ONLY.  tools/datacheck.py prints nothing (no data section).
 * tools/split_s.py asm/.../rom_78414_c_c_a_c_a_c_c.s Func_8078870 --dry-run:
 *   _b.s Func_8078870 (46 lines), _c.s Func_80788c4 (79).  No _a.s -- the target
 *   is the FIRST function in the file.
 * PINS: 0.  No shim, no fakematch row, no flag group.
 *
 * ---------------------------------------------------------------------------
 * THE PARK'S VERDICT IS REFUTED.  It said:
 *
 *   "a diff that has collapsed to a fixed permutation of instructions with no
 *    dependence between them is done"
 *   "it is not reachable from the source: the copies into r8/r10 are emitted by
 *    reload at the point it chooses, not at any point the C names"
 *
 * Both halves are false.  The permutation is decided by ONE rung of
 * `rank_for_schedule` -- the LAST one, INSN_LUID -- and the LUID order IS
 * source-reachable.  The park measured nine source spellings and nine flag
 * settings and never read `.23.sched2`.
 *
 * THE ARITHMETIC (`-da -fsched-verbose=6`, prologue block, 9 insns, all leaves):
 *
 *     insn  what               prio  cost  dependents
 *     140   r2 = 0x80             3    1    141
 *     141   r2 = r2 << 2          2    1    118
 *     118   r8 = r2               1    1    --
 *     133   r3 = 0x1ff            3    2    120      <-- pool load, 2 cycles
 *     120   sl = r3               1    1    --
 *     125   r5 = r0               2    1    17       <-- reload's param copy
 *     17    r5 = r5 + 0xd8        1    1    --
 *     6     r7 = r1               1    1    --
 *     14    r6 = 0                1    1    --
 *
 * Two contests decide all five of the park's encodings, and BOTH fall through
 * priority, CLASS and dependent count to INSN_LUID:
 *   t=3, ready {120,14,6,141,125}: 141 and 125 both priority 2, both CLASS 3
 *     (neither depends on the last-scheduled 133), both exactly one dependent.
 *   t=7, ready {120,118,17}: all priority 1, all CLASS 3, all zero dependents.
 * Our chain order is `6 14 125 17 140 141 118 133 120`.  Simulating the ladder
 * over all ten cycles, the ROM's schedule is reproduced EXACTLY by the chain
 * order `6 14 140 141 118 133 120 125 17` -- i.e. the two constants' defining
 * insns must sit BEFORE the pointer's.
 *
 * WITH THE PARK'S LITERAL MASKS THAT IS IMPOSSIBLE, and this is the part worth
 * carrying: they are loop invariants, `move_movables` emits them with
 * `emit_insn_before (..., loop_start)`, and `-frerun-loop-opt` (on at -O2) runs
 * loop twice, so the second pass's hoist lands AFTER anything the first pass
 * created.  MEASURED, not assumed: making the pointer a giv instead (the
 * subscript inline, or `((unsigned short *)(a + 0xd8))[i]`) still puts its init
 * at chain position 3-4, BEFORE the invariants -- `.08.loop` of v2/a.c reads
 * `14, 140(giv), 148(0x200), 150(0x1ff)` -- and both spellings measure 5,
 * unchanged.  A SOURCE STATEMENT and a GIV INIT land in the same place relative
 * to hoisted invariants; only a source statement for the CONSTANT moves it.
 *
 * SO: NAME THE TWO MASKS AS `int` LOCALS, BEFORE THE POINTER.  `.08.loop`
 * becomes `14, 17(equipped), 20(idmask), 23(p)` and the WHOLE NINE-INSN
 * PROLOGUE, indices 4..12, GOES BYTE-EXACT.  They must be `int`: typed
 * `unsigned short` they are propagated away and the constants get re-hoisted to
 * the end of the preheader, and the prologue reverts (7, first diff at index 6
 * again -- v2/d.c).
 *
 * WHAT NAMING THEM COSTS, AND THE CURE.  `int` masks make the AND SImode, so
 * read 1 expands as `zero_extend:SI(mem:HI p)` rather than the literal mask's
 * `mem:HI` -- gcc narrows an AND to HImode only for a CONSTANT operand.  That
 * makes read 1 and read 2 (the GetItemInfo argument, `zero_extend:SI(mem:HI p)`
 * by argument promotion) the same expression in the same extended basic block,
 * so cse1 unifies them and we read memory twice where the ROM reads it three
 * times.  `.03.cse` counts it: 2 `mem:HI` against the park's 3.  Reads 1 and 3
 * are never at risk -- the call between them invalidates cse's memory table.
 * Cure: `*(volatile unsigned short *)p` on THE CALL ARGUMENT ONLY.  The pointer
 * already exists, which is the recorded precondition for the volatile-cast
 * lever.  Volatile on read 1 instead is 11 at 39 insns; volatile on all three
 * loses a relocation offset; volatile on read 3 as well is 39 at 36 insns.
 *
 * THE REMAINING 2 -- the GetUnit mechanism, third instance in this bank:
 *
 *     idx  rom                ours
 *      23  ldrh r3, [r5]      ldrh r0, [r5]
 *      24  mov  r0, sl        mov  r3, sl
 *      25  ands r0, r3        ands r0, r3     <-- identical
 *
 * combine leaves `(set 45 (and 46 36))` + `(set r0 45)`; regmove's
 * `fixup_match_1` rewrites the dest to the dying source 46 (`.15.regmove` shows
 * 45 -> 46), and 46 then carries the r0 copy-suggestion, so the LOAD lands in
 * r0.  The ROM instead ties the dest to the `sl`-to-low reload copy, which
 * reload emits as `mov r0, sl` because operand 1 of `*thumb_andsi3_insn` is
 * `%0`-matched AND commutative.  local-alloc's `combine_regs` is a SECOND route
 * to the same register: it refuses only when `reg_qty[ureg] < 0` (source not
 * block-local, or dies more than once) or `reg_qty[sreg] >= -1` (dest not
 * block-local).  `-fno-regmove` IS EXACTLY INERT HERE (2 of 40), which proves
 * the two routes are interchangeable and that killing one is not enough.
 *
 * MEASURED, ALL EXACTLY 2 (i.e. inert) against that last pair:
 *   `idmask & *(u16*)p` -- the swapped operand order SURVIVES to `.17.lreg`
 *     (checked), and reload's `%` commutative swap neutralises it;
 *   a block-local `m = idmask; return m & *(u16*)p;` (copy-propagated away);
 *   a function-scope `ret` written in two blocks;
 *   a shared `v` written in block 1 and block 3 (local-alloc'd anyway);
 *   `GetItemInfo(unsigned int)` -- its REAL definition's parameter type, from
 *     src/rom_77000/rom_78414_a_b.c, so this is a free declaration-correctness
 *     dividend rather than a lever;
 *   declaring `idmask` before `equipped`.
 * WORSE: `p` typed `unsigned short *` 3; ASSIGNING idmask before equipped 7;
 *   `(unsigned short)equipped` or `(unsigned short)(... & equipped)` 11 at 39.
 *
 * NEXT: the side that has to become ineligible for the dest/dying-source
 * combine is the LOAD, and it is born and dies inside one basic block.  That is
 * the same question GetUnit answered in the other direction
 * (src/rom_77000/rom_77320_a_a_c_c_a_b.c).
 */
extern unsigned char *GetItemInfo(int id);

int Func_8078870(unsigned char *a, int kind)
{
	int i;
	unsigned char *p;
	unsigned char *info;

	i = 0;
	p = a + 0xd8;
	do {
		if (*(unsigned short *)p & 0x200) {
			info = GetItemInfo(*(unsigned short *)p);
			if (info[2] == kind)
				return *(unsigned short *)p & 0x1ff;
		}
		i++;
		p += 2;
	} while (i <= 0xe);
	return 0;
}
