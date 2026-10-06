/* OvlFunc_880_2008384  --  0x02008384, MATCHING.
 *
 * Was src/non_matching/ovl_7795e8/2008384.c at a positional figure of thirteen
 * encodings of thirty-two, which was PHASE: the streams were 28 against 29
 * sixteen-bit instructions, so the figure counted the offset.  Batch 332.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_7795e8/ovl_30_c_c_a_a_c_c_b.c \
 *     asm/overlays/rom_7795e8/ovl_30_c_c_a_a_c_c_b.s --whole
 *
 * SPLIT.  The piece asm/overlays/rom_7795e8/ovl_30_c_c_a_a_c_c.s holds two
 * functions, this one and OvlFunc_880_20083cc, so it splits two ways and this
 * is the _b part (there is nothing before it, so no _a is written):
 *   python3 tools/split_s.py \
 *     asm/overlays/rom_7795e8/ovl_30_c_c_a_a_c_c.s OvlFunc_880_2008384
 *   -> _b.s (this function), _c.s (OvlFunc_880_20083cc);
 *      overlays/rom_7795e8/overlay.ld rewritten.
 * OvlFunc_880_20083cc stays parked -- it is a 548-byte-frame, ~1000-line
 * function with no candidate, so there was nothing to gain by writing one TU
 * for both.
 * tools/datacheck.py on the piece: clean, no data section goes with the split.
 * PINS: 0 (tools/shimcount.py), no inline asm, no device.
 *
 * THE DIRECTION WAS "OURS LONGER": we were emitting an EXTRA instruction.
 *
 * THE ONE INSTRUCTION, AND WHY IT IS A CONSTRUCT AND NOT A SPELLING.
 * The ROM's tail is `... lsr r0, #0x1f / neg r0, r0`; the park's body gave
 * `... lsr r0, #0x1f / mov r3, #0x0 / sub r0, r3, r0`.  The park had the
 * observation right and the diagnosis wrong: it is not that "gcc will not use
 * negsi2 on a comparison result".  gcc never reaches negsi2 here at all,
 * because the negate is not a negate by the time the branchless comparison is
 * built.  Read in the .14.ce dump and then in ifcvt.c:
 *
 *   - `return -(a != b);` expands (see the .00.rtl dump) as a TWO-ARMED
 *     CONSTANT SELECT of -1 and 0 -- the negate is folded into the arms, there
 *     is no NEG insn anywhere.
 *   - if-conversion then runs noce_try_store_flag_constants (ifcvt.c:585).
 *     With ifalse 0 and itrue -1, `diff` is -1, which is -STORE_FLAG_VALUE, so
 *     the FIRST arm of its if/else chain fires: normalize 0, and the tail is
 *     `expand_binop (..., sub_optab, GEN_INT (ifalse), target, ...)` -- a
 *     MINUS against a materialised zero.  That is the extra instruction, and
 *     no rearrangement of `-(a != b)` can avoid it, which is exactly what the
 *     park's seven spellings measured.
 *   - write the arms as 1 and 0 instead and `diff` is +1 == STORE_FLAG_VALUE,
 *     whose tail is the add_optab branch with `ifalse` 0 -- i.e. nothing is
 *     emitted.  The branchless `neg / orr / lsr #31` store-flag survives, and
 *     the `-t` is then a plain NEGATE_EXPR that really does become `neg r0,r0`.
 *
 * SO THE ARMS MUST BE EXPLICIT.  `t = (gState.area != X);` on its own is NOT
 * enough: the 0 arm gets merged with the `return 0` above it, the then-block
 * stops being a constant-to-constant select, if-conversion declines, and the
 * comparison comes out branchy (`cmp / beq / mov #1`) -- which is the park's
 * sixth recorded spelling and the reason it saw "neg, but branchy".  Writing
 * both arms keeps two constant blocks for ifcvt to find.  Measured: five
 * two-armed shapes all reach zero (1/0 with `return -t`, 1/0 with `t = -t`,
 * the arms swapped under `==`, the negate through a second local, and the
 * comparison written as `(area ^ X) != 0`); every one-armed or ternary shape
 * stays at the park's figure.
 *
 * THE POOLED 2 IS A SYMBOL, AND THE PARK'S RELOCATION BLOCKER IS REFUTED.
 * The ROM has `ldr r2, =2` where `mov r2, #2` would do, which is the area.sym
 * tell, and `_AREA_02` was already defined.  The park then recorded that the
 * extra R_ARM_ABS32 "cannot pass make compare".  It can: an absolute symbol
 * definition emits no bytes, and 189 landed files in this tree already
 * reference `_AREA_*`.  objcmp cannot see that, because it compares an
 * unlinked object against a hand .s that spells the symbol as its literal, so
 * this was verified the documented way -- a copy of the reference with
 * `ldr r2, =2` rewritten to `ldr r2, =_AREA_02`, against which the body reads
 * OK whole file, 72 bytes, 32 encodings and 3 relocations identical.
 * Relocation parity is 2 against 3 BEFORE the rewrite and 3 against 3 after,
 * which is the admissibility test for a pooled-symbol spelling.
 *
 * WHAT IT DOES.  Returns 0 unless save flag 0x144 is set and the entrance id
 * at gState+0x23e is not 2, in which case it returns -1 when the area is not
 * _AREA_02 and 0 when it is: a three-way gate collapsed into a 0 / -1 answer.
 */

typedef struct {
    unsigned char pad[0x1c0];
    short area;
    unsigned char pad1c2[0x7c];
    short f23e;
} GlobalState;

extern GlobalState gState;
extern int _AREA_02;
extern int __GetFlag(int id);

int OvlFunc_880_2008384(void)
{
    int t;

    if (__GetFlag(0xa2 << 1) == 0)
        return 0;
    if (gState.f23e == 2)
        return 0;
    if (gState.area != (int)(&_AREA_02))
        t = 1;
    else
        t = 0;
    return -t;
}
