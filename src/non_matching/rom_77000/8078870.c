/* Func_8078870 (0x08078870) -- asm/rom_77000/rom_78414_c_c_a_c_a_c_c.s (2 functions).
 *
 * PARK HELD AT 5 of 40 encodings, device-free, body UNCHANGED.  SIZE EXACT
 * (40 against 40 encodings, 39 instructions both sides), relocations identical,
 * per-opcode memory profile the reference's exactly: ldr=1 ldrb=1 ldrh=3.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_77000/8078870.c \
 *     asm/rom_77000/rom_78414_c_c_a_c_a_c_c.s --func Func_8078870
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
 * THE RESIDUE IS THREE INDEPENDENT DEFECTS, NOT ONE.  THIS IS THE NEW MAP.
 *
 * The park and batch 322's brief treated it as "the prologue" plus "the GetUnit
 * pair".  Measured here it is THREE separable defects, and the park's stated
 * NEXT is a question about one the park's own body does not have.
 *
 * This body's 5 differing encodings are ALL IN THE PROLOGUE, indices 6,7,10,11,12
 * -- a three-way permutation:
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
 *       emits TWO                                        (indices 13,15,16,17,18)
 *       No device-free cure found.  The mechanism is below and it is in the
 *       FRONT END.
 *
 * The four corners, all 40/40 encodings and 39 instructions:
 *
 *   | masks                              | figure | ldrh | residue        |
 *   | both literal  (THIS BODY)          |   5    |  3   | D1 + D2        |
 *   | `int` 0x200, literal 0x1ff         |   7    |  2   | D2 + D3        |
 *   | literal 0x200, `int` 0x1ff         |   9    |  3   |                |
 *   | both `int`   (b322 p2_devicefree)  |   7    |  2   | D3 + the swap  |
 *
 * **`int` 0x200 with a LITERAL 0x1ff is the best map in the file**: its prologue
 * is exact at indices 4-10 AND its indices 24/25/26 are exact.  Its whole
 * residue is ONE scheduler contest (0x1ff's `mov sl,r3` against `p`'s
 * `adds r5,#0xd8`) plus D3.
 *
 * ---------------------------------------------------------------------------
 * D3's MECHANISM, LOCATED TO THE PASS AND READ OUT OF THE COMPILER
 *
 * `mem:HI` count per dump:
 *
 *               .02.jump  .03.cse  .07.gcse  .08.loop  .09.cse2
 *   literal        3         3        5         3         3
 *   `int` mask     3         2        3         2         2
 *
 * The loss is cse1, confirming the park.  But the CAUSE is in `c-typeck.c`, not
 * in cse: `build_binary_op`'s narrowing block (`shorten = -1` for bitwise ops at
 * `c-typeck.c:2023`, applied at `:2351-2412`).  Two of its three cases matter:
 *
 *   * `:2396` -- the other operand is an `INTEGER_CST` that fits in the narrower
 *     type, so the AND is done in `unsigned short`.  RTL:
 *     `(and:SI (subreg:SI (reg:HI 37) 0) (const_int 512))` -- read 1 is a plain
 *     `mem:HI` into an HImode pseudo, which is NOT read 2's expression, so cse1
 *     cannot common them and all three `ldrh` survive.
 *   * `:2385` -- BOTH operands variable and narrowed from the same precision with
 *     the same signedness.  Also narrows.
 *
 * An `int` mask hits neither, so read 1 expands as `(zero_extend:SI (mem:HI p))`
 * -- character for character read 2's expression, read 2 being the `GetItemInfo`
 * argument, zero-extended by promotion.  cse1 commons them and emits
 * `adds r3,r0,#0` where the ROM has its second `ldrh`.  Same length, which is
 * why the figure stays 40/40 and only the indices move.
 *
 *   **THE COUPLING, stated as the bound it is: a narrow AND requires the mask to
 *   be an INTEGER_CST or an equally-narrow variable; an `unsigned short` mask
 *   does narrow the AND (ldrh=3 measured) but then expand must zero-extend the
 *   mask pseudo for the SImode `and`, and those extension insns are emitted
 *   INSIDE the loop and hoisted by `move_movables` to `loop_start`, i.e. AFTER
 *   `p`'s init -- so the prologue reverts.  Narrow AND <=> constant-foldable mask
 *   <=> hoisted late.**
 *
 * Dump evidence for the last clause, `unsigned short equipped` at `.08.loop`:
 * insn 17 sets reg 35 = 512 (low LUID, good), insn 23 sets `p`, and THEN insns
 * 125/126 are `reg 55 = reg 35 << 16` / `reg 40 = reg 55 >> 16` carrying
 * `REG_EQUAL (zero_extend:SI (subreg:HI (const_int 512) 0))`.  Those two collapse
 * to the single `movs/lsls` pair later, at insn 126's position -- after `p`.
 *
 * ---------------------------------------------------------------------------
 * CLOSED FROM SOURCE (so nobody spends a round on them)
 *
 *  * **A type-based alias difference cannot separate read 1 from read 2.**
 *    `canon_hash`'s `case MEM` (cse.c:2252-2267) hashes only `MEM` plus the
 *    address and bails out only on `MEM_VOLATILE_P` / `BLKmode`.  The alias set
 *    is not in the hash at all.
 *  * **An `unsigned short` parameter cannot keep read 2 in HImode.**
 *    `arm.h:2363` defines `PROMOTE_PROTOTYPES 1` and `:611` defines
 *    `PROMOTE_FUNCTION_ARGS`, so the argument is always promoted to SImode.
 *    Measured inert, consistent with that.
 *  * **The park's stated NEXT -- "the side that has to become ineligible for the
 *    dest/dying-source combine is the LOAD", about `regmove`'s `fixup_match_1`
 *    and the index 24/25 pair -- is a question about a defect THIS BODY DOES NOT
 *    HAVE.** With 0x1ff left a literal, indices 24/25/26 are byte-exact; the
 *    swap is a SIDE EFFECT of naming 0x1ff as an `int`, not a property of the
 *    function.  `-fno-regmove` being exactly inert at 2 of 40 is still true and
 *    still interesting, but it is evidence about the device-bearing body.
 *
 * MEASURED DEVICE-FREE (all 40/40 encodings, 39 instructions)
 *
 *   5   THIS BODY
 *   5   `unsigned short *p` instead of `unsigned char *p`            INERT
 *   5   reuse the PARAMETER `a` instead of `p`                       INERT
 *   5   `unsigned short` 0x200 with a literal 0x1ff
 *   5   `GetItemInfo(unsigned short)` prototype                      INERT
 *   5   `GetItemInfo((unsigned short)*(unsigned short *)p)`           INERT
 *   5   assign idmask before equipped / assign `p` before both        INERT
 *   7   `int` 0x200 + `int` 0x1ff                            ldrh=2
 *   7   `int` 0x200, literal 0x1ff                           ldrh=2
 *   7   `unsigned short` BOTH masks                          ldrh=3
 *   7   `int` masks + `(p + 0)` on the call read             ldrh=2
 *   7   `unsigned int` 0x200, literal 0x1ff                 ldrh=2
 *   9   `int` 0x1ff only                                     ldrh=3
 *   9   `unsigned short` 0x200 + `int` 0x1ff                ldrh=3
 *   9   parameter reuse crossed with both `int` masks        ldrh=2
 *  10   `int` 0x200 + `p` assigned first
 *  11   `(unsigned short)equipped` as the mask -- 39 encodings, COUNT; ldr=0,
 *       because the cast re-enables the constant fold and BOTH masks collapse
 *  12   `int` masks + idmask-assigned-first / p-assigned-first
 *  39   `(unsigned short)idmask` at the return -- 36 encodings, RELOC
 *  42   the call-argument read written as a GIV, `*(unsigned short *)(a + 0xd8 +
 *       i * 2)` -- keeps ldrh=3 but costs FOUR instructions (43 against 39), the
 *       giv becomes a second induction variable.  Refuted.
 *
 * Crossfire depth 3 over {use-equipped, use-idmask, proto-ushort,
 * arg-cast-ushort, idmask-first, p-first}: 37 rows, nothing below 5.
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
