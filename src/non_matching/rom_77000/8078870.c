/* Func_8078870 (0x08078870) -- asm/rom_77000/rom_78414_c_c_a_c_a_c_c.s (2 functions).
 *
 * PARK HELD AT 5 differing encodings of 40, device-free, body UNCHANGED.  SIZE
 * EXACT (40 against 40 encodings, 39 instructions both sides), relocations
 * identical, per-opcode memory profile the reference's exactly: ldr=1 ldrb=1
 * ldrh=3.  Figure re-derived in batch 327 G.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_77000/8078870.c asm/rom_77000/rom_78414_c_c_a_c_a_c_c.s --func Func_8078870
 *
 * INSTALLED PATH, if it ever lands: src/rom_77000/rom_78414_c_c_a_c_a_c_c_b.c.
 * Split shape: TEXT-ONLY, tools/datacheck.py prints nothing.
 *   tools/split_s.py asm/.../rom_78414_c_c_a_c_a_c_c.s Func_8078870 --dry-run:
 *     _b.s Func_8078870 (46 lines), _c.s Func_80788c4 (79).  No _a.s -- the
 *     target is the FIRST function in the file.
 * PINS: 0.  No shim, no fakematch row, no flag group.  No device.
 *
 * OWNER RULING 4 (docs/owner-decisions.md) STANDS AND IS NOT REOPENED HERE.
 * The 2-of-40 body carries `*(volatile unsigned short *)` on ONE of THREE reads
 * of the same lvalue; that is a DEVICE, the 2 is a figure about the blocker, and
 * device-free that body is 7.  Nothing below re-proposes it.
 *
 * ---------------------------------------------------------------------------
 * THE RESIDUE IS THREE INDEPENDENT DEFECTS (unchanged map, still correct)
 *
 * All 5 differing encodings are in the prologue, indices 6,7,10,11,12 -- a
 * three-way permutation:
 *
 *      ref                       ours
 *  4   movs r2,#0x80             movs r2,#0x80
 *  5   ldr  r3,=0x1ff            ldr  r3,=0x1ff
 *  6   lsls r2,r2,#2       XX    adds r5,r0,#0
 *  7   adds r5,r0,#0       XX    lsls r2,r2,#2
 *  8   adds r7,r1,#0             adds r7,r1,#0
 *  9   movs r6,#0                movs r6,#0
 * 10   mov  r8,r2          XX    adds r5,#0xd8
 * 11   mov  sl,r3          XX    mov  r8,r2
 * 12   adds r5,#0xd8       XX    mov  sl,r3
 *
 *   D1  the 0x200 chain hoists AFTER the parameter copy   (indices 6,7,10)
 *       CURED by naming 0x200 as an `int` local before `p`.  Side effect: D3.
 *   D2  the 0x1ff chain hoists AFTER `p`'s init           (indices 11,12)
 *       CURED by naming 0x1ff as an `int` local before `p`.  Side effect: the
 *       index 24/25 register swap appears.
 *   D3  cse1 commons read 1 with read 2, so we emit ONE `ldrh` where the ROM
 *       emits TWO.  No device-free cure, and the cause is in the FRONT END.
 *
 * The four corners, all 40/40 encodings and 39 instructions:
 *   | masks                     | figure | ldrh | residue       |
 *   | both literal (THIS BODY)  |   5    |  3   | D1 + D2       |
 *   | `int` 0x200, lit 0x1ff    |   7    |  2   | D2 + D3       |
 *   | lit 0x200, `int` 0x1ff    |   9    |  3   |               |
 *   | both `int`                |   7    |  2   | D3 + the swap |
 * `int` 0x200 with a LITERAL 0x1ff is still the best MAP (prologue exact at
 * 4-10 AND indices 24/25/26 exact), even though its figure is worse -- the
 * batch-326 warning that a lower figure can be a worse starting point.
 *
 * ---------------------------------------------------------------------------
 * BATCH 327 G.  D1 AND D3 ARE NOW PROVEN MUTUALLY EXCLUSIVE **IN THE C
 * LANGUAGE**, WHICH IS WHY EVERY SPELLING SWEEP HAS BEEN FLAT.
 *
 * D1 and D2 are chain-ORDER defects: `.23.sched2` block 0 gives every insn in
 * the preheader priority 1, 2 or 3 and the ROM's order differs from ours only
 * at TIES, decided by `rank_for_schedule`'s final INSN_LUID rung
 * (haifa-sched.c:4113-4115, lower luid preferred):
 *     ours  140(3) 133(3) 125(2) 141(2) 6(1) 14(1) 17(1) 118(1) 120(1)
 *     ROM   140    133    141    125    6    14    118    120    17
 * i.e. the ROM needs LUID(141) < LUID(125) -- the 0x200 shift before the `a`
 * copy -- and LUID(17) after both high-register copies -- `p = a + 0xd8` last.
 * **Both are satisfied exactly when the masks are named locals assigned before
 * `p`, which is what the park's D1/D2 cures do.  The prologue is a LUID
 * question, not a cost question.**
 *
 * And D3 forbids it.  `build_binary_op`'s narrowing block applies `get_narrower`
 * to each operand (c-typeck.c:2354-2355) and then has exactly three cases
 * (:2385-2412): both operands narrowed from the same precision with the same
 * signedness; or one of them an INTEGER_CST that fits.  So:
 *   * a LITERAL mask narrows by the INTEGER_CST case, read 1 stays a plain
 *     `mem:HI` into an HImode pseudo, is NOT read 2's expression, and all three
 *     `ldrh` survive -- but it is not a pseudo, so the prologue order is wrong;
 *   * an `int` local does not narrow, read 1 becomes `(zero_extend:SI (mem:HI p))`
 *     character for character read 2's expression, and cse1 commons them;
 *   * `(unsigned short)m` WOULD narrow by the first case -- both operands 16-bit,
 *     both unsigned -- but the front end CONSTANT-FOLDS `(unsigned short)` of a
 *     constant-valued local, so the local stops existing.  That is exactly why
 *     the park's `(unsigned short)equipped` row collapsed BOTH masks (ldr=0),
 *     and my own `mask-ushort-cast` row is EXACTLY INERT at 5 with ldrh=3: the
 *     fold happening, visible as a no-op.
 *   * an `unsigned short` local does narrow (ldrh=3 measured) but then expand
 *     must zero-extend the mask pseudo for the SImode `and`, and `move_movables`
 *     hoists those extension insns to `loop_start`, i.e. AFTER `p` -- so the
 *     prologue reverts.  `.08.loop` shows it: insn 17 sets reg 35 = 512 at a low
 *     LUID, insn 23 sets `p`, and insns 125/126 are the shift pair carrying
 *     `REG_EQUAL (zero_extend:SI (subreg:HI (const_int 512) 0))`.
 *
 * **To be narrow the mask must be constant-foldable; to fix the prologue order
 * it must survive as a pseudo.  No C spelling of a 0x200 mask is both, so D1 and
 * D3 cannot both be cured.  This is a language-level bound, not a search
 * result** -- which is the honest reason 112 spellings across four batches have
 * been flat, and the reason this park should not be handed out again for
 * spelling work.
 *
 * MEASURED DEVICE-FREE IN BATCH 327 (crossfire depth 2, 9 NEW structural edits
 * that are not in the park's list, 28 valid rows, NOTHING BELOW 5):
 *   5   THIS BODY
 *   5   `p = 0xd8 + a;`                                       INERT
 *   5   `p = &a[0xd8];`                                       INERT
 *   5   `& (unsigned short)0x200` as the mask                  INERT (the fold)
 *   5   `info` declared first                                 INERT
 *   5   `return (int)(... & 0x1ff);`                           INERT
 *   6   `i = 0;` after `p = a + 0xd8;`
 *   6   `unsigned char *p = a + 0xd8;` (init at declaration)
 *   6   `kind == info[2]` instead of `info[2] == kind`
 *
 * CLOSED FROM SOURCE, carried forward (so nobody spends a round on them):
 *  * A type-based alias difference cannot separate read 1 from read 2 --
 *    `canon_hash`'s `case MEM` (cse.c:2252-2267) hashes only MEM plus the
 *    address and bails out only on MEM_VOLATILE_P / BLKmode.  The alias set is
 *    not in the hash at all.
 *  * An `unsigned short` PARAMETER cannot keep read 2 in HImode: `arm.h:2363`
 *    defines PROMOTE_PROTOTYPES 1 and `:611` PROMOTE_FUNCTION_ARGS.  Measured
 *    inert, consistent with that.
 *  * The park's old NEXT -- `regmove`'s `fixup_match_1` and the index 24/25 pair
 *    -- is a question about a defect THIS BODY DOES NOT HAVE.  With 0x1ff left a
 *    literal, indices 24/25/26 are byte-exact; the swap is a side effect of
 *    naming 0x1ff an `int`.
 *
 * ---------------------------------------------------------------------------
 * BATCH 330 A.  THE FIGURE IS UNCHANGED AND RE-DERIVED.  THE LEVER THAT CLOSED
 * THE OTHER THREE FUNCTIONS IN THIS FAMILY IS MEASURED HERE AND IS REFUTED, SO
 * NOBODY NEEDS TO SPEND A ROUND FINDING THAT OUT.
 *
 * Context worth having: the three parks of asm/rom_77000/rom_77320_a_c_c.s --
 * Func_8077f70, Func_807808c and Func_8078144, the same family, the same
 * division-and-clamp shape -- ALL LANDED this batch on one line each,
 * `__asm__ volatile ("");` placed between a store and the shift that follows
 * it.  A bare asm has pattern code ASM_INPUT, so gcc/haifa-sched.c:3580's guard
 * `if (code != ASM_OPERANDS || MEM_VOLATILE_P (x))` is unconditionally true for
 * it: :3585-3593 makes it depend on every prior use, setter and clobberer,
 * :3595 sets reg_pending_sets_all and :3780-3789 makes it the last setter of
 * every register, so the block is cut in two.  Each of those parks had PROVED a
 * priority bound and stopped; the barrier does not win the arithmetic, it
 * deletes the contest.
 *
 * THAT DOES NOT WORK HERE, AND THE REASON IS IN THIS PARK'S OWN DIAGNOSIS.
 * D1 and D2 are not an insn the scheduler SINKS past a barrier-able point; they
 * are TIES decided by `rank_for_schedule`'s final INSN_LUID rung
 * (gcc/haifa-sched.c:4113-4115), and the ROM's order is neither our schedule
 * nor the LUID order a barrier would freeze -- the ROM issues the two
 * priority-three insns first, then one priority-two insn whose LUID is the
 * HIGHEST of the group.  A barrier cannot invert a LUID; only the source can.
 * Measured, all at the reference length and with no memory-profile divergence:
 *
 *   * a bare barrier as the first statement of the guarded arm: EXACTLY INERT.
 *   * an extended `("" : : "r" (p))` barrier in the same place: EXACTLY INERT.
 *     Worth knowing WHY that one is interesting and still inert: gcc/cse.c
 *     :5741-5745 flushes the hash table only for `GET_CODE (PATTERN (insn)) ==
 *     ASM_OPERANDS && MEM_VOLATILE_P`, which a bare asm is NOT, so the extended
 *     form is the one that could in principle break D3's cse1 commoning without
 *     a volatile cast.  It does not: ldrh stays at the count this body already
 *     has, because with both masks left literal there is nothing to common.
 *   * EITHER barrier in the preheader, before the pointer init: WORSE, by four.
 *   * both masks named `int` locals, with or without either barrier: WORSE,
 *     by five, and all three spellings agree with each other.
 *   * naming only one mask, with or without either barrier: WORSE, by four or
 *     by five depending on which mask.
 *   * splitting the pointer init into a copy and a separate `+= 0xd8` add, so
 *     the add is its own statement that can be moved: EXACTLY INERT in four
 *     orderings, WORSE by one in a fifth.  This is the sharpest of the new
 *     rows, because it was the obvious attack on "the ROM wants the add LAST":
 *     the add's LUID is fixed by where the pointer is first NEEDED, not by
 *     where the statement sits, so source order cannot move it.
 *   * crossing the above with the mask namings: WORSE by five to six, never
 *     better than this body.
 *
 * SO THE PARK'S LANGUAGE-LEVEL BOUND SURVIVES A GENUINELY NEW INSTRUMENT.
 * Twenty-four new rows, nothing below this body.  Note that the earlier park
 * table's two cure figures did NOT reproduce for me: my spelling of the "name
 * the masks as locals" cures measures worse than that table records, by three
 * and by two.  Those superseded cure figures should be read as spelling-
 * dependent, not as a reachable rung; if someone revisits this, re-derive them
 * before building on them.
 *
 * REVISIT ONLY IF: owner ruling 4 is revisited (i.e. someone establishes the
 * location genuinely is volatile, in which case all three reads should be), or
 * someone finds a way to hold a non-foldable 16-bit-precision constant in a
 * pseudo before the loop.
 */
extern unsigned char *GetItemInfo(unsigned int id);

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
