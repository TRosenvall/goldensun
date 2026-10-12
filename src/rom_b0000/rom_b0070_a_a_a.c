/* Func_80b0070  (ClassifyShopItem) -- asm/rom_b0000/rom_b0070_a_a_a.s
 *
 * LANDS.  BYTE-IDENTICAL: 132 bytes, 59 encodings, 3 relocations, 55/55
 * 16-bit instructions.  Measured both --func and --whole.  Park claimed
 * 14 of 59; re-measured 14 of 59 at the head of batch 338 (both sides 59
 * encodings, 55 instructions, so that figure was a true distance), and this
 * body takes it to 0.
 *
 * PINS: 0.  tools/shimcount.py prints no rows.  No device, no flag group,
 * no fakematch row.  datacheck.py on the reference: clean, no data section.
 * The piece holds this function alone (tools/piece.py), so NO SPLIT.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_b0000/rom_b0070_a_a_a.c \
 *     asm/rom_b0000/rom_b0070_a_a_a.s --whole
 *
 * ---------------------------------------------------------------------------
 * WHAT THE FOURTEEN WERE, AND THE ONE-LINE FIX
 *
 * The park's diagnosis -- "register choice for a materialised zero", the
 * `mov r1, #0 / mov r10, r1` against our `mov r2, #0 / mov r10, r2` -- was
 * the FIRST difference and NOT the cause.  Reproduced exactly; refuted as a
 * blocker.  The whole fourteen was one statement, and the three scratch-register
 * pairs ahead of it fell out with it:
 *
 *   idx  8,10   the zero into sl           r1 vs r2
 *   idx 28,30   the constant 1 added to sl r2 vs r3
 *   idx 32,34   the 0x1ff pool load        r3 vs r2
 *   idx 36-43   the tail store's address   eight, see below
 *
 * The tail.  The ROM builds the whole OFFSET in one register and stores with
 * REGISTER-OFFSET addressing off the base:
 *
 *   rom   mov r1, sl / mov r2, #0x9b / lsl r3, r1, #1 / lsl r2, r2, #2
 *           / add r3, r3, r2 / ldr r2, [pool=0] / mov r1, r8
 *           / strh r2, [r1, r3]
 *   ours  mov r2, sl / lsl r3, r2, #1 / mov r2, #0x9b / add r3, r8
 *           / lsl r2, r2, #2 / add r3, r3, r2 / ldr r2, [pool=0]
 *           / strh r2, [r3, #0]
 *
 * i.e. ours folds the base in EARLY (`add r3, r8` third) and ends with an
 * immediate-offset store; the reference adds the base LAST and gets
 * `(mem (plus (reg base) (reg off)))`.
 *
 * REPARENTHESISING THE EXPRESSION DOES NOT DO IT.  All of these measure 14,
 * unchanged, first difference still at index 8:
 *
 *   14  base + (0x9b << 2) + n * 2            THE PARKED BODY
 *   14  base + ((0x9b << 2) + n * 2)          explicit grouping      INERT
 *   14  &base[(0x9b << 2) + n * 2]            array form             INERT
 *   14  base + (n * 2 + (0x9b << 2))          operands swapped       INERT
 *   14  base + ((0x9b << 2) + (n << 1))       shift for the *2       INERT
 *   14  base + (0x26c + n * 2)                literal for the shift  INERT
 *   14  base + (n * 2) + (0x9b << 2)          base+index first       INERT
 *   14  ((short *)(base + (0x9b << 2)))[n]    scaled index           INERT
 *
 * That is the whole re-association dimension and it is flat, because gcc folds
 * `POINTER + int` at expand into a single `plus` chain whatever the source
 * parentheses say -- `expand_expr` on `PLUS_EXPR` goes through
 * `expand_binop`/`force_operand` and the association is already fixed by
 * `fold` in the front end.
 *
 *   **0  int off = (0x9b << 2) + n * 2;  then  *(short *)(base + off) = 0;**
 *
 * NAMING THE OFFSET IN AN INT IS THE LEVER.  The sum then has its OWN pseudo,
 * so the MEM gcc hands to `emit_move_insn` is `(plus (reg base) (reg off))`
 * with two registers rather than a folded address tree -- and
 * `legitimate_address_p` for Thumb (`arm.c` `thumb_legitimate_address_p`,
 * `arm.md`'s `thumb_movhi_insn` predicates) accepts REG+REG directly, so no
 * reload flattens it.  Spelled as one expression the tree is
 * `plus(plus(base, 0x26c), mult(n,2))` and gcc emits the adds in tree order,
 * which puts the base first.
 *
 * This is the SAME SHAPE as batch 332 D's note on Func_8079664 -- "the base has
 * to be read into a POINTER LOCAL of its own ... not spelled as an arithmetic
 * expression on the symbol's integer value" -- running in the other direction:
 * there the BASE needed a name, here the OFFSET does.  The general statement is
 * that a Thumb REG+REG address needs both halves to own a pseudo, and an
 * arithmetic expression at the use site gives neither.
 *
 * Insensitive to where `off` is declared or how the sum is written once it is
 * named: an inner block `{ int off = ...; }`, `int off;` declared first,
 * declared last, and `n * 2 + (0x9b << 2)` all measure 0.  Declared last with
 * the sum in the reference's operand order is kept, as the plainest C.
 *
 * AUTOSWEEP (batch 338, recorded negatives -- do not re-screen):
 *   FLAG axis, 8 per-file groups on the PARKED body: -fno-gcse 14,
 *   -fno-strict-aliasing 14, -fno-strength-reduce 14, -fno-rerun-cse-after-loop
 *   14, -fno-rerun-loop-opt 14 (all inert); -O1 26, -fno-schedule-insns2 26,
 *   -ffixed-r7 65 (and that last one breaks the length).
 *   ANCHOR axis, 13 top-level boundaries: twelve inert at 14, and
 *   `do { } while (0);` after `return n;` measured 13 -- one better, and
 *   SUPERSEDED by this landing.  Noted only because an anchor in dead code
 *   moving a figure at all is worth someone's curiosity, not this park's.
 *
 * STILL TRUE FROM THE PARK, carried forward:
 *   The trailing store keeps its LITERAL zero -- the ROM pools the zero itself
 *   (`ldr r2, .Lb00e0` where `.Lb00e0` is `.word 0`), the halfword exception in
 *   const.sym running in the direction where the literal is correct.  Naming it
 *   in an int, which is the fix in Func_801eea0 and Func_80175c0, would be the
 *   error here.
 *   The signed-char compare is reloaded every iteration because the two calls
 *   may write through the pointer; assigning it to a local at the top of the
 *   body reproduces that and fixes the `&&`'s otherwise unsequenced operands.
 */
extern unsigned char *iwram_3001f2c;
extern int _Func_8078480(int id);
extern int _Func_8078ad0(int id, int b);

int Func_80b0070(void)
{
    unsigned char *base;
    short *out;
    int i;
    int n;
    int c;
    int off;

    base = iwram_3001f2c;
    n = 0;
    i = 0;
    out = (short *)(base + (0x9b << 2));
    do {
        c = *(signed char *)(base + 0x3a9);
        if (c == _Func_8078480(i) && _Func_8078ad0(i, 0) != 0) {
            *out = i;
            n++;
            out++;
        }
        i++;
    } while (i <= 0x1ff);
    off = (0x9b << 2) + n * 2;
    *(short *)(base + off) = 0;
    base[0x3a6] = n;
    return n;
}
