/* Func_801219c (IsPositionOnMap) -- 0x0801219c
 *                                  (asm/rom_9000/rom_1219c_a_a_a.s)
 *
 * MATCHING.  0 of 50 encodings -- byte-identical: 104 bytes against 104, 50
 * encodings against 50, 1 relocation against 1.  MEASURED batch 326, brief G.
 * Previous park figure: 4 of 50 (batch 323 brief I).
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/rom_9000/rom_1219c_a_a_a.c asm/rom_9000/rom_1219c_a_a_a.s --func Func_801219c
 *
 * SPLIT SHAPE: none.  asm/rom_9000/rom_1219c_a_a_a.s holds exactly one
 * .thumb_func_start (Func_801219c) and datacheck.py reports nothing, so this
 * converts WHOLE -- no split_s.py run, no linker-script change.
 * PINS: 0.  DEVICES: 0.  No register pin, no inline asm, no per-file flag.
 *
 * ==== WHAT CLOSED THE LAST 4, AND WHY THE PARK'S DIAGNOSIS WAS ONE CAUSE
 *      REPORTED AS TWO ====
 *
 * The park's four differing encodings were indices 29, 35, 37, 38:
 *
 *     idx 29   rom  asrs r4, r3, #4     park  asrs r2, r3, #4
 *     idx 35   rom  adds r3, r4, r3     park  adds r3, r3, r2
 *     idx 37   rom  adds r1, r1, r3     park  adds r3, r3, r1
 *     idx 38   rom  ldrb r3, [r1, #2]   park  ldrb r3, [r3, #2]
 *
 * and it read them as "(a) the ROM reuses x's register for x/16" plus "(b) the
 * final layer+index accumulates into layer's register" -- two allocation
 * facts.  They are ONE SOURCE-SHAPE FACT, and (a) follows from (b).
 *
 * Look at the operand ORDER rather than the registers.  Both adds in the ROM
 * put the NON-SHIFTED term first (`r4` = x/16 at idx 35, `r1` = layer at
 * idx 37); both adds in the park put the shifted term first.  That is
 * `expr.c:7340`:
 *
 *     /@ Put a constant term last and put a multiplication first.  @/
 *     if (CONSTANT_P (op0) || GET_CODE (op1) == MULT)
 *       temp = op1, op1 = op0, op0 = temp;
 *
 * -- and, reached first here, the "associate the constant outside" branch at
 * `expr.c:7312-7325`, whose else-arm is
 *
 *     op0 = gen_rtx_PLUS (mode, XEXP (op1, 0), op0);          /@ :7324 @/
 *
 * which puts `XEXP (op1, 0)` -- the MULT -- ahead of op0 unconditionally
 * whenever op0 is not itself a MULT.
 *
 * THE GATE IS THE WHOLE POINT: that entire block is only reachable when
 * `modifier == EXPAND_SUM || modifier == EXPAND_INITIALIZER` AND
 * `mode == ptr_mode` (`expr.c:7290-7292`; otherwise `goto binop`).  An
 * INDIRECT_REF/ARRAY_REF expands its address with EXPAND_SUM, so EVERY sum
 * written inside `layer[...]` is subject to the reordering.  A plain
 * assignment statement is EXPAND_NORMAL, falls through to `goto binop`, and
 * `expand_binop` keeps the written operand order -- and uses the LHS pseudo as
 * the destination.
 *
 * So hoisting the address arithmetic out of the subscript and into a `+=` on
 * `layer` itself fixes all four encodings in one edit:
 *   - idx 37 becomes `(plus layer idx4)` with layer's own pseudo as the dest,
 *     giving `adds r1, r1, r3`, and idx 38 then addresses off r1;
 *   - idx 35's inner sum is no longer under EXPAND_SUM either (it is the
 *     operand of the `+=`), so `x / 16` stays first;
 *   - idx 29 follows: with the index pseudo dying one insn earlier, x/16 gets
 *     r4 back.  The register was never the lever.
 *
 * MEASURED (10 variants, tools/sweep_variants.py, production flags):
 *   layer += (x/16 + (z/16)*128) * 4;  then layer[2] ....... 0   (this body)
 *   int i = x/16 + (z/16)*128; layer += i*4; layer[2] ...... 0
 *   int i = ...; i = i*4; layer += i; layer[2] ............. 0
 *   int i = x/16 + (z/16)*128; layer[i*4 + 2] .............. 7
 *   int i = (x/16 + (z/16)*128)*4; layer[i + 2] ............ 7
 *   x = x/16; first, crossed with each of the three above .. 8
 *   layer += (x/16)*4 + (z/16)*512 ........................ 11
 *   layer += ((z/16)*128 + x/16)*4 ........................ 11
 * All three zeros confirmed byte-identical by tools/objcmp.py; the figures
 * above are the sweep's positional counts at dsize=0 rel=ok throughout.
 *
 * THE PARK'S RECORDED NEGATIVE STANDS AND IS A DIFFERENT EDIT.
 * `q = layer + (...)*4; return (q[2] != 0xff) - 1;` really is 7: a NEW pointer
 * is a NEW pseudo, so the add's destination is q's register, not layer's, and
 * idx 37 stays wrong.  `layer +=` is the form that matters -- same pseudo in
 * and out.  (Park's three "exactly tie at 4" rows were therefore inert for the
 * right reason: none of them moved the arithmetic out of the subscript.)
 */
extern char *iwram_3001e70;

int Func_801219c(int *pos)
{
    unsigned char *layer;
    char *m;
    int x, z;

    x = pos[0] / 0x10000;
    z = (pos[2] - pos[1]) / 0x10000;
    m = iwram_3001e70;
    if (m == 0)
        return 0;
    layer = *(unsigned char **)(m + (0xc8 << 1));
    layer += (x / 16 + (z / 16) * 128) * 4;
    return (layer[2] != 0xff) - 1;
}
