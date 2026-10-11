/* HeightTile_A  --  0x08011e88   [rom_9000]
 *
 * *** LANDING.  BYTE-IDENTICAL.  PIN-FREE, DEVICE-FREE.  WAS PARKED AT 2 of 36. ***
 *   `objcmp --func`:  `OK HeightTile_A -- 72 bytes, 36 encodings and 0
 *   relocations identical`.  `--whole` agrees: `OK whole file -- 72 bytes,
 *   36 encodings and 0 relocations identical`.  35 instructions + one 2-byte
 *   pad on both sides.  `tools/shimcount.py`: no rows.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_9000/rom_11ce0_a_c_c_a_c_c_a.c \
 *     asm/rom_9000/rom_11ce0_a_c_c_a_c_c_a.s --whole
 *
 * SPLIT SHAPE: none needed; the split was already done by batch 324's
 * HeightTile_B landing.  `tools/piece.py` on the retired park prints
 *   asm/rom_9000/rom_11ce0_a_c_c_a_c_c_a.s -- 1 function(s) [HeightTile_A]
 * and `tools/datacheck.py` on that reference exits 0 silently (no data section,
 * no exports).  Install path src/rom_9000/rom_11ce0_a_c_c_a_c_c_a.c, no
 * `split_s.py` step, no Makefile flag group (generic `asm/%.o: src/%.c`).
 *
 * *** THIS CLOSES THE 16-MEMBER HeightTile FAMILY: HeightTile_0 .. HeightTile_F
 * are now 16 of 16 landed.  HeightTile_A was the last one left. ***
 *
 * ===========================================================================
 * WHAT CLOSED IT, AND WHY EVERY EARLIER PROBE MISSED IT
 * ===========================================================================
 *
 * The park's residue was exactly indices 26 and 27 of the high arm:
 *
 *     ref   26  adds r0, r3, #0      27  muls r0, r2
 *     ours  26  adds r0, r2, #0      27  muls r0, r3
 *
 * with r3 = `t - 8` and r2 = the sample difference on BOTH sides -- so the two
 * encodings were never a register-allocation fact at all.  They are which
 * MULTIPLY OPERAND gcc copies into the destination.  `*thumb_mulsi3`
 * (`config/arm/arm.md:1116-1129`) emits `mov %0, %1 ; mul %0, %0, %2`, so the
 * copied value is always PATTERN OPERAND 1, and the park's reading -- "the ROM
 * reads md operand 1 out of the bias register and we read it out of the sample
 * register" -- was a correct OBSERVATION with the wrong verdict attached: it
 * concluded the cause was local-alloc's REG_ALLOC_ORDER and the preference loop
 * (`global.c:1097-1110`), and built a whole coupling argument on top of that.
 *
 * *** THE DECIDER IS `expand_binop`'s COMMUTATIVE SWAP, optabs.c:647-669. ***
 *
 *     if (GET_RTX_CLASS (binoptab->code) == 'c' || ... mul optabs ...)
 *       {
 *         commutative_op = 1;
 *         if (((target == 0 || GET_CODE (target) == REG)
 *              ? ((GET_CODE (op1) == REG && GET_CODE (op0) != REG)
 *                 || target == op1)
 *              : rtx_equal_p (op1, target))
 *             || GET_CODE (op0) == CONST_INT)
 *           { temp = op1; op1 = op0; op0 = temp; }
 *       }
 *
 * It runs at EXPAND, so the operand order is already decided in `.00.rtl` and no
 * later pass touches it -- verified: `.00.rtl`, `.03.cse`, `.13.combine`,
 * `.15.regmove`, `.17.lreg` all print the SAME order for both multiplies, and
 * only `.19.flow2` substitutes hard registers.  That is why two batches of
 * register-level reasoning could not move these two encodings, and why 20
 * measured bodies came back "exactly inert": they were all varying the
 * allocator's inputs, and the decision is a tree-shape decision.
 *
 * THE EDIT, and it is one line:
 *
 *     -    a = *p << 19;
 *     -    a -= b;
 *     -    return b + ((t - 8) * a) / 8;
 *     +    a = *p << 19;
 *     +    return b + ((a - b) * (t - 8)) / 8;
 *
 * Writing the sample difference as the IN-EXPRESSION first operand `(a - b)`
 * (rather than as a named local assigned by `a -= b`) makes BOTH operands
 * non-trivial expressions, the swap condition above is false, and the tree order
 * survives into RTL: `(mult (a - b) (t - 8))`.  Operand 1 is then `t - 8`, which
 * is the ROM's `adds r0, r3, #0`.
 *
 * THE LESSON, for docs/elevation.md, because this park is a clean instance of a
 * shape that has cost this tree several batches:
 *   >> WHEN A MULTIPLY (OR ANY COMMUTATIVE BINOP) COPIES THE WRONG OPERAND, THE
 *   >> LEVER IS WHETHER EACH OPERAND EXPANDS TO A BARE PSEUDO OR TO AN
 *   >> EXPRESSION -- NOT THE SOURCE OPERAND ORDER AND NOT THE ALLOCATOR.
 *   >> `optabs.c:654-658` swaps exactly when op1 IS a REG and op0 IS NOT.  So
 *   >> `expr * var` and `var * expr` BOTH come out as `(mult var expr)`, and the
 *   >> only way to get `(mult expr var)` is to stop the var from being a bare
 *   >> pseudo -- i.e. spell it as an expression too. <<
 *   This also explains the park's own low arm, which was already exact:
 *   `(b - a) * t` has op0 an expression and op1 the bare parameter `t`, the swap
 *   fires, and `t` is copied -- which is what the ROM does there.
 *
 * MEASURED THIS BATCH, 30 bodies, every row 36 reference encodings (35 insns):
 *   `a = *p << 19; return b + ((a - b) * (t - 8)) / 8;`        ** 0 **  <- this
 *   `a = *p << 19; return b + ((t - 8) * (a - b)) / 8;`          13 at 36 insns, COUNT
 *   the park's body (`a -= b;` then `(t - 8) * a`)                2
 *   `c = t - 8;` named, `(c * a)`                                 2  (inert)
 *   `d = (t - 8) * a;` named product                              2  (inert)
 *   `return b + (a * (t - 8)) / 8;` (operand flip only)          32 at 36 insns, COUNT
 *   `c = t - 8;` named, `(a * c)`                                32 at 36 insns, COUNT
 *   `t -= 8;` then `(a * t)`                                     24
 *   `t -= 8;` then `(t * a)`                                     14 at 34 insns, COUNT
 *   `((*p << 19) - b) * (t - 8)` fully inlined                    6
 *   `(t - 8) * ((*p << 19) - b)` fully inlined                    4
 *   low arm written `t * (b - a)` instead of `(b - a) * t`        4
 * Note what the instruction counts say: the naive operand flip is NOT a distance
 * (36 instructions against 35), which is why the park's original "+1 instruction"
 * claim kept reappearing -- it is true of the flip, and false of the fix.
 */
int HeightTile_A(signed char *p, int t)
{
    int a;
    int b;

    a = *p << 19;
    p++;
    b = *p << 19;
    p++;
    if ((unsigned int)t <= 7)
        return a + ((b - a) * t) / 8;
    a = *p << 19;
    return b + ((a - b) * (t - 8)) / 8;
}
