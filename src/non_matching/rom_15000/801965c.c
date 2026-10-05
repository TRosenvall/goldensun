/* Func_801965c -- PARK STANDS, but CAUSE (i) IS NOW REACHABLE and the park's
 * negatives list contains the lever that closes it.  Batch 325 brief B.
 *
 *   10 differing encodings of 48.  ref 104 bytes, ours 104 -- EQUAL.
 *   Relocations identical.  45 instruction lines against the reference's 45.
 *   Re-measured in batch 325 brief B; identical to the batch-324 figure.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_15000/801965c.c asm/rom_15000/rom_1908c_c_a_a_a.s --func Func_801965c
 *
 * ============== WHAT THE 10 IS, read in the RTL rather than inferred
 *
 * The ROM's peeled load is `ldrh r3,[r6,r2]` -- Thumb's register-OFFSET form,
 * i.e. `(mem:HI (plus (reg blk) (reg off)))` -- and it forms the loop pointer
 * `add r2,r6,r2` only in the PREHEADER, after the guard branch.  Our body emits
 * `(set (reg 65) (plus (reg blk) (reg 64)))` and then `(mem:HI (reg 65))` from
 * `.00.rtl` onward (scratch_elev/b325/B/dump/965c), so the register-offset form
 * is never generated at all.  The park's "cse has commoned the peeled load's
 * address with the giv's start value" is the wrong pass: there is nothing to
 * common, because the address was already one pseudo in the expander's output.
 *
 * ============== REFUTED: THE ARRAY SPELLING IS NOT THE LEVER
 *
 * `((unsigned short *)blk)[0x758 + i]`, `[i + 0x758]` and a named
 * `unsigned short *hw` all read 10 flat -- and their `.02.jump` is BIT-IDENTICAL
 * to the base's (same md5 over every `(set ...)` pattern).  Fold erases the
 * grouping before RTL exists.  Index carriers `j = 0x758 + i` as `unsigned int`
 * and as `int` also read 10 flat.
 *
 * ============== REACHED: AN EXPLICIT BYTE-OFFSET CARRIER
 *
 *      for (i = 0; i < n; i++) {
 *              unsigned int j;
 *              j = (0xeb << 4) + i * 2;
 *              if ((out[i] = *(unsigned short *)(blk + j)) == 0)
 *                      break;
 *      }
 *
 * `.02.jump` then holds TWO `(mem:HI (plus reg reg))`, and the whole critical
 * block comes out in the ROM's shape --
 *   mov r2,#0xeb / lsl r2,#4 / ldrh r3,[r5,r2] / strh r3,[r7] / lsl r3,#16 /
 *   cmp r3,#0 / beq ... / add r2,r5,r2 / mov r4,#0
 * with `add` only AFTER the branch.  ** That is cause (i), 9 of the 10. **
 *
 * It reads 31 -- at 44 instruction lines against the reference's 45, and
 * crossfire flags the row INSNS.  ** SO THE 31 IS A MISALIGNMENT FIGURE, NOT A
 * DISTANCE: the body is exactly ONE INSTRUCTION SHORT, and the missing one is
 * the ROM's `mov r12,r5` ** -- which is the park's own cause (ii).
 *
 * > CAUSES (i) AND (ii) ARE ONE CAUSE.  The ROM has one more simultaneously live
 * > value than we do, which both parks its loop bound in a hi register
 * > (`mov r12,r5` / `cmp r0,r12`) and leaves the offset register alive for the
 * > register-offset load.  We have a spare low register and spend it on a
 * > precomputed pointer.  Confirmed in `.18.greg`: the base allocates pseudo 34
 * > before 35 (`8 regs to allocate: 69 71 77 37 65 34 35 33`) and the
 * > byte-offset body allocates 35 before 34 (`... 37 35 34 33 59`), which is the
 * > r5/r6 swap.
 *
 * ** AND THE PARK WROTE OFF THIS LEVER ON A LENGTH NUMBER. ** Its negatives read
 * "an explicit byte-offset `j` carrier 40".  I measure 31, and 31 is
 * misalignment.  The one edit that closes 9 of its 10 was in its rejected list.
 *
 * MEASURED on top of the byte-offset body, all EXACTLY INERT at 31 (crossfire,
 * depth 2, 11 subsets): `0xeb0` written plainly; `i << 1` for the offset; `j` at
 * function scope; `&blk[0x12b2]` for the store pointer; and every pair.
 * MEASURED and worse: bound in its own local `m = n - 1` 25; `n--; m = n;` 25;
 * a block-scoped copy of the bound inside the loop 31.  None produced any
 * hi-register `mov`.
 *
 * NEXT, named: supply a ninth simultaneously-live low-register value so
 * `global.c`'s `find_reg` parks the loop bound in a hi register.  The park's
 * cause-(i) framing ("the lever is not the loop") survives; its cause-(ii)
 * framing as a separate 1-encoding rotation does not.
 */

/* Func_801965c (0x0801965c) -- NON-MATCHING, 10 of 48 encodings.
 *
 *   SIZE ref 104 bytes, ours 104 -- EQUAL.
 *   RELOCATIONS IDENTICAL.  INSTRUCTION COUNT 49 against 49.
 *   So the 10 is a TRUE DISTANCE, not misalignment.  (The park this replaces
 *   read 39 of 48 at 112 bytes with shifted relocations; that 39 was dominated
 *   by literal-pool OFFSETS, every one of them a consequence of the length.)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/801965c.c \
 *     asm/rom_15000/rom_1908c_c_a_a_a.s --func Func_801965c
 *
 * ------------------------------------------------------------------ 39 -> 10
 *
 * TWO INDEPENDENT CAUSES WERE FIXED, both found by decomposing the diff into
 * runs rather than treating 39 as one problem.
 *
 * 1. THE +0x12b2 STORE NEEDED AN int-TYPED ZERO.  (39 -> 24, and it is what
 *    made the size and the relocations agree.)
 *
 *    `*(unsigned short *)(blk + 0x12b2) = 0;` CANNOT emit the ROM's
 *    `mov r3, #0`.  `*thumb_movhi_insn` (config/arm/arm.md:4318) lists
 *    `=l <- mn` as ALTERNATIVE 1 and `=l <- I` as ALTERNATIVE 5, and recog
 *    takes the first alternative that costs nothing -- so a HImode const_int 0,
 *    which `I` would happily build with `mov`, goes to the literal POOL
 *    instead.  Alternative 1's `pool_range` is 64 (against 1020 for SImode), so
 *    that fix sorts ahead of every SImode fix and forces a pool dump in the
 *    MIDDLE of the function with a branch over it: +1 insn, +1 `.short 0`
 *    pad, +1 pool word, +8 bytes, and every `ldr rN,[pc,#X]` in the function
 *    displaced.  The movhi expander's own CONST_INT->movsi escape hatch is
 *    gated on `! CONST_OK_FOR_THUMB_LETTER (..., 'I')`, so it never fires for 0.
 *
 *    Measured in scratch_elev/b324/D/probe/z.c: of six spellings of the store
 *    (plain, through a named `u16 *`, `(u16)0`, `s16` lvalue, int variable) ONLY
 *    the int variable emits `mov r2,#0` with no pool word.  So the ROM's
 *    `mov r3,#0` PROVES the stored zero reached expand as an SImode value.
 *
 *    The int must be SHORT-LIVED and must not cross the call: storing the loop
 *    index `i` instead measures 51, because `i` then lives in a callee-saved
 *    register and gcc does not rematerialise the zero.  The ROM's r3 is dead
 *    immediately after the `strh`, which is the same fact.
 *
 *    COROLLARY, and it corrects the old park: the ROM's TAIL zero,
 *    `ldr r3, =0x0`, is NOT an SImode literal -- an SImode 0 is `mov` (the same
 *    recog-alternative reasoning as `_MSG_182` in docs/owner-decisions.md).  It
 *    is the HImode POOLED zero, which we already emit: Thumb-1 has no
 *    pc-relative `ldrh`, so gas renders `ldrh r3,.L15` as `ldr r3,[pc,#8]`.
 *    And the old park's "gcc CSEs ours into a single pooled zero" was wrong --
 *    we emitted TWO pool words of 0 precisely because they landed in two
 *    different pool dumps and so could not be shared.
 *
 * 2. THE +0x12b2 POINTER HAD TO BE NAMED.  (24 -> 10.)
 *
 *    With the store spelled inline, the `0x12b2` pool constant is allocated
 *    r1 -- the `out` parameter's register.  That creates an anti-dependence
 *    which drags `mov r7, r1` up to position 2, and the sum then takes r3 and
 *    the zero r2: SEVEN encodings of prologue rotation.  Naming the pointer
 *    gives the constant r3 (reusing the dead `iwram_3001e8c`-address register,
 *    exactly as the ROM does) and the entire prologue falls into place.
 *
 *    Naming the OFFSET instead measures 40 with dirty relocations; the
 *    pointer is the thing to name.
 *
 * ------------------------------------------------------- WHAT THE 10 ACTUALLY IS
 *
 * Instruction counts are equal, so these are a rotation of one block:
 *
 *   rom   mov r2,#0xeb / lsl r2,#4 / ldrh r3,[r6,r2] / strh r3,[r7] /
 *         lsl r3,#16 / cmp r3,#0 / beq L0 / mov r12,r5 / add r2,r6,r2 /
 *         mov r4,#0
 *   ours  mov r3,#0xeb / lsl r3,#4 / add r2,r6,r3 / ldrh r3,[r2] /
 *         strh r3,[r7] / lsl r3,#16 / mov r1,#0 / cmp r3,#0 / beq L1 /
 *         mov r4,#0
 *
 *   (i) 9 of the 10.  We materialise `blk + 0xeb0` BEFORE the peeled first
 *       load; the ROM uses Thumb's register-offset form `ldrh r3,[r6,r2]` and
 *       forms the pointer only in the PREHEADER (`add r2,r6,r2`).  cse has
 *       commoned the peeled load's address with the giv's start value into ONE
 *       pseudo where the ROM has two.
 *   (ii) 1 of the 10.  `cmp r0,r12` vs `cmp r0,r5` -- the loop bound copied to
 *       a high register.  It shares the accounting with (i): we spend the spare
 *       slot on a `mov r1,#0` hoisted into the guarded block (so `beq` targets
 *       the tail), the ROM spends it on `mov r12,r5` (so `beq` targets the
 *       shared `L0: mov r1,#0 / b` block).
 *
 * MEASURED AND INERT at 10, all at 49/49 insns, dsize 0, relocations clean:
 *   inline `i * 2` subscript; `0xeb0` written plainly instead of `0xeb << 4`;
 *   the `while` form with `i++` at the bottom; `n--` moved before the call;
 *   `*flag = (unsigned short)z`.
 * MEASURED AND WORSE: a named `src` pointer 27; `*src++` 47; an explicit
 *   byte-offset `j` carrier 40; an explicit source-level peel of iteration 0
 *   51; a named `int off = 0x12b2` 40; `i < n - 1` without the decrement 44
 *   (loop.c's invariant hoist costs an insn); `n` as `int` 26; `out[i] = z`
 *   53; a `t = 0; out[i] = t` terminator 19; an explicit guard + `do/while` 39.
 * FLAG PROBES, as instruments only (lines against the ROM's 49):
 *   -fno-thread-jumps 49/17, -fno-cse-follow-jumps 49/17 (both exactly inert),
 *   -fno-schedule-insns2 49/29, -fno-rerun-cse-after-loop 47/32,
 *   -fno-strength-reduce 46/32, -fno-gcse 42/37, -fno-expensive-optimizations
 *   50/28.  None reaches the ROM's shape; cse2 ADDS two insns here.
 *
 * NEXT: cause (i).  The question is how to keep the peeled first load's address
 * and the loop giv's start value as TWO pseudos.  Every loop spelling tried
 * collapses to the same 10 (consistent with `check_dbra_loop` making loop form
 * one equivalence class, docs/elevation.md), so the lever is not the loop.
 */
extern unsigned char *iwram_3001e8c;
extern void BufferString(int a, int b);

int Func_801965c(int a, unsigned short *out, unsigned int n)
{
    unsigned char *blk;
    unsigned short *flag;
    unsigned int i;
    int z;

    blk = iwram_3001e8c;
    flag = (unsigned short *)(blk + 0x12b2);
    z = 0;
    *flag = z;
    BufferString(a, 1);
    n--;
    for (i = 0; i < n; i++) {
        if ((out[i] = ((unsigned short *)(blk + (0xeb << 4)))[i]) == 0)
            break;
    }
    out[i] = 0;
    return i;
}
