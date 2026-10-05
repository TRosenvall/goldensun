/* Func_80ad5b4 @ 0x080ad5b4  --  MATCHING, 0 of 30 encodings  (batch 326, brief C)
 *
 * Was parked at 4 of 30 since batch 322.  PINS: 0  (tools/shimcount.py reports
 * no shims).  DEVICES: 0.  FLAG GROUPS: 0.  No *.sym entry, no inline asm.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_a1000/rom_ad274_c_a_a_c_c.c \
 *     asm/rom_a1000/rom_ad274_c_a_a_c_c.s --func Func_80ad5b4
 *   -> OK Func_80ad5b4 -- 64 bytes, 30 encodings and 1 relocations identical
 *   --whole is GREEN too: the reference .s holds this function and nothing else.
 *
 * SPLIT SHAPE: none.  `tools/datacheck.py asm/rom_a1000/rom_ad274_c_a_a_c_c.s`
 * exits 0 (no data), and `tools/split_s.py ... Func_80ad5b4 --dry-run` says
 * "holds only Func_80ad5b4 and no data; convert it directly, no split needed".
 * Install path: src/rom_a1000/rom_ad274_c_a_a_c_c.c.  Exports: none.
 *
 * ========== THE WHOLE EDIT IS ONE STATEMENT ==========
 *
 * The park had the flag merge as a two-assignment if:
 *     v = b;
 *     if (flag != 0) { v = 0xffff8000; v |= b; }
 * and this file has it as a one-assignment ternary:
 *     v = (short)(flag != 0 ? (0x8000 | b) : b);
 *
 * ========== THE PARK'S CONFLICT WAS REAL; ITS TWO HORNS WERE NOT ==========
 *
 * The park stated the blocker as a conflict: the constant must be a LITERAL to
 * get a HImode pool fix (HImode's pool_range 64 is the only range below the 974
 * a const_int needs here -- batch 325 brief F proved no other narrow range on
 * Thumb accepts a const_int), but must be a VARIABLE to get *thumb_iorsi3's
 * destination tied to it, the ROM's `orr r3, r2`.
 *
 * THE SECOND HORN DOES NOT EXIST.  The literal form already puts the constant in
 * op0, in EVERY member of the family including the 11s.  Measured in .02.jump:
 *
 *     (ior:SI (subreg:SI (reg:HI 51) 0) ...)        <- reg:HI 51 IS the constant
 *
 * Read from optabs.c, in order:
 *   1. `:640` -- op1 is the constant, `rtx_cost > 2`, not a shift, and
 *      `preserve_subexpressions_p ()` returns 1 outright at -O2 (it short-circuits
 *      on flag_expensive_optimizations), so `op1 = force_reg (HImode, op1)`.
 *      THAT `(set (reg:HI 51) (const_int -32768))` IS the HImode pool fix.
 *   2. `:656-666` -- the commutative swap then fires, because `b` arrives as a
 *      `(subreg:HI (reg:SI ...))` (an `int` parameter being narrowed), so
 *      `GET_CODE (op1) == REG && GET_CODE (op0) != REG` is TRUE.  op0 becomes the
 *      constant.  The ROM's operand roles are FREE.
 *
 * fold-const.c:4813-4823 does swap a literal arg0 to arg1 for BIT_IOR_EXPR (and
 * PLUS/MULT/MIN/MAX/XOR/AND -- NOT minus, NOT the shifts), so the park was right
 * that the TREE cannot hold the constant first.  expand_binop swaps it back.
 *
 * ========== WHAT ACTUALLY SEPARATED THE 0s FROM THE 11s ==========
 *
 * The NARROWING, not the OR.  With `int v` assigned on TWO paths, gcc materialises
 * the HImode->SImode sign extension and never removes it:
 *
 *     ldrh r3, .L5 / orr r2, r2, r3 / lsl r3, r2, #16 / asr r2, r3, #16
 *
 * That is THREE defects off one cause: +2 instructions; the `strh` now reads r2
 * instead of r3; and the ROM's `mov r3, r2` (the `v = b` arm) is coalesced away
 * because `v` and `b` end up sharing a register.  11 differing.
 *
 * A COND_EXPR sets `v` ONCE.  convert_to_integer pushes the narrowing into both
 * arms, the extension is provably dead, and the else arm's copy into the single
 * result pseudo survives as `add r3, r2, #0` -- encoding 1c13, which is the ROM's
 * `mov r3, r2`.  This is the brief's own lever read from the other end:
 * nonzero_bits' REG case (combine.c:7987) tracks bits only for REG_N_SETS == 1,
 * so the SECOND assignment to the merge variable is what keeps the extension alive.
 *
 * > TRANSFERABLE.  When a park's merge variable is assigned on two paths and the
 * > residue contains an `lsl/asr` (or `lsl/lsr`) pair, the lever is to make it ONE
 * > assignment -- a ternary -- not to re-spell the operation.  The park had already
 * > measured eleven merge-variable spellings (d3/d6/e4/f1-f5/m5, 32-34 insns) and
 * > every one of them kept two assignments.
 *
 * ========== POOL ORDER, FROM .26.mach ON THIS BODY ==========
 *
 *     ;; SImode fixup for i18; addr  2, range (0,1020): `iwram_3001f2c'  -> 1022
 *     ;; HImode fixup for i66; addr 48, range (0,  64): 0xffff8000       ->  112
 *
 * add_minipool_forward_ref (arm.c:4817-4858) inserts before the first entry whose
 * max_address is strictly GREATER, so 112 sorts ahead of 1022 and the pool is
 * [0xffff8000][iwram_3001f2c] -- the ROM's, with the relocation back at 0x3c.
 * `ldrh r3, .L5` in the output assembles as the word load: MINIPOOL_FIX_SIZE
 * (HImode) is 4, which is also why the pool word is the sign-extended 0xffff8000.
 *
 * ========== MEASURED FAMILY (asm/rom_a1000/rom_ad274_c_a_a_c_c.s) ==========
 *     0   v = (short)(flag != 0 ? (0x8000 | b) : b)        <- THIS BODY
 *     0   cast duplicated inside each ternary arm
 *     0   `(short)(flag ? ... : ...)` without the != 0
 *     0   `(short)(flag != 0 ? (b | 0x8000) : b)`  (fold swaps it anyway)
 *     4   ternary stored DIRECTLY, no `v` local   -- WRONG PROGRAM: THREE `strh`
 *     4   same, cast per arm                      -- WRONG PROGRAM, same shape
 *    11   the if-form with a `(short)` cast (three spellings)
 *    11   `(unsigned short)` cast -- HImode fix and pool order RIGHT; lsl/lsr pair
 *    11   `(short)(0x8000 - b)`, a probe of the MINUS route (fold does not swap
 *         MINUS_EXPR, so a literal CAN be op0 there) -- same extension pair
 *    12   `(short)(0xffff8000 | b)`, two spellings
 *     4   the installed park body: SImode fix, pool transposed
 *
 * TWO THINGS THAT DO NOT FIT IN THE TABLE.
 *
 * 1. THE `int v` LOCAL IS LOAD-BEARING, and the two rows that drop it are the
 *    per-opcode-memory trap again: they read 4 -- the SAME FIGURE AS THE PARK --
 *    at identical encoding count, while emitting THREE `strh` where the ROM emits
 *    two, because gcc sinks the store into both ternary arms.  This independently
 *    confirms the park's d5/h5 "stores twice" rejection, and it is a reminder that
 *    a figure merely EQUAL to the base is not a safe inert.
 * 2. THE CAST'S SIGNEDNESS IS NOT ABOUT THE POOL WORD.  The park's d2 row
 *    attributed an unsigned failure to a `32768` pool word; the `(unsigned short)`
 *    body's .26.mach shows a HImode fix with pool word -32768 and the order
 *    CORRECT.  Its 11 is the zero-extension pair -- the same cause as the if-form.
 *
 * ========== KEPT FROM THE PARK, STILL TRUE AND STILL LOAD-BEARING ==========
 *  1. `off` is ONE variable, reused: the ROM builds 0x224 and bumps it with
 *     `add r6, #0x10`; `i` is likewise shifted and advanced in place.
 *  2. The return type is `int` with no return statement.  Declared `void`, the
 *     epilogue becomes `pop {r0} / bx r0`; gcc will not use r0 as the epilogue
 *     scratch when the return type is non-void (thumb_exit, arm.c:8306-8320).
 *  Note the park's third kept lever -- "the OR needs the constant as its
 *  destination, so split it into `v = 0xffff8000; v |= b`" -- is RETIRED by the
 *  expand_binop reading above.  It was a true observation about the if-form and a
 *  false requirement.
 *
 * ========== A TOOL CORRECTION THIS FUNCTION FORCED ==========
 * objcmp prints, on the PARK body:
 *     XX INSTRUCTION COUNT  ref 29, ours 30 ... a pad is absorbing the difference
 * That is a FALSE POSITIVE.  Both bodies are 64 bytes and both hold exactly 27
 * instructions (counted in the reference .s and in our own -S output).
 * `objcmp._insns` strips trailing zero encodings from the whole stream, POOL DATA
 * INCLUDED -- and a relocated pool word dumps as 00000000 in an unlinked object:
 *     ref : 27 insns | pad | .word 0xffff8000 | .word <reloc = 00000000>
 *     ours: 27 insns | pad | .word <reloc = 00000000> | .word 0xffff8000
 * so the reference's last word is stripped and ours is not.  The line was
 * reporting the POOL TRANSPOSITION -- the defect -- as a length defect.
 * `_insns` should not strip an encoding that carries a relocation, and should not
 * strip pool data at all.  Until it is fixed, ANY function whose last pool word is
 * relocated (or is a literal 0) raises this line spuriously -- and raises it on
 * exactly the bodies that are closest to matching.
 */

extern char *iwram_3001f2c;

int Func_80ad5b4(int i, int a, int b, int flag)
{
    char *base;
    int off;
    int v;

    base = iwram_3001f2c;
    off = 0x224;
    if (*(int *)(base + (i * 4 + off)) != 0) {
        i *= 2;
        off += 0x10;
        *(short *)(base + (i + off)) = a;
        i += 0x23c;
        v = (short)(flag != 0 ? (0x8000 | b) : b);
        *(short *)(base + i) = v;
    }
}
