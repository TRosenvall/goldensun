/* Func_801edec -- the 16-bit DMA fill, or an IWRAM-resident unpacker.
 *
 * MATCHING.  1 of 52 at OBJECT level and that one encoding is a relocated pool
 * word the linker fills with the ROM's own value; `make compare` is GREEN.  Park figure was 48 of 52 and it RE-DERIVED at 48 of 52
 * (ref 52 encodings / ours 41, ref 124 bytes / ours 100) -- because the parked
 * body CALLS a helper that does not exist: `DMA3_FILL16` is not in dma.h (the
 * park reproduces it in a COMMENT) so it compiled to
 * `R_ARM_THM_CALL DMA3_FILL16`.  Pasting the park's own inline function in:
 * 48 -> 40 (ref 52 / ours 49, 124 vs 116).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_15000/rom_1de5c_c_c_c_c_a_a_a_a.c \
 *     asm/rom_15000/rom_1de5c_c_c_c_c_a_a_a_a.s --func Func_801edec
 *
 * No split: the reference .s holds ONE function and no data (datacheck/split_s
 * have nothing to do).  No pins in this file; no flag group.
 *
 * PREREQUISITE: `DMA3_FILL16` is promoted to include/dma.h, as a MACRO.
 * docs/elevation.md forbids landing with a file-local copy of a shared helper,
 * which is why this is a prerequisite and not a tidy-up -- the same reason
 * DMA3_CLEAR_OFS and DMA3_FILL_OFS were promoted in batch 323.
 *
 * WHY A MACRO AND NOT AN INLINE FUNCTION.  This is the whole landing, and it is
 * a machine-description fact, not an optimisation one.
 *
 * All 40 remaining encodings were POOL LAYOUT.  Both streams hold the same 43
 * instructions; the reference splits its literal pool in TWO, dumping four
 * words mid-function after the `b` out of the then-arm (with a `.short 0x0000`
 * align pad) and three more at the end -- so `0x040000d4` (REG_DMA3SAD) appears
 * TWICE and there are two pads, +8 bytes, and every pc-relative offset and
 * branch target after the dump shifts.  The inline-function body emits ONE pool
 * at the end.
 *
 * `push_minipool_fix`'s own dump (arm.c:5380, in `<base>.c.26.mach`) said why:
 *
 *   ;; SImode fixup for i12; addr 0,  range (0,1020): `iwram_3001e8c'
 *   ;; SImode fixup for i21; addr 16, range (0,1020): 0xe0e0        <-- wrong
 *   ... all seven SImode, one pool
 *
 * `add_minipool_forward_ref` (arm.c:4820) sorts pool entries by
 * `max_address = fix->address + fix->forwards` ASCENDING, and `arm_reorg`
 * (arm.c:5497-5600) drops back to the LAST barrier that is still inside
 * `minipool_vector_head->max_address`.  With every fix at range 1020 that is the
 * end of a 116-byte function, so: one pool, in address order.
 *
 * The ROM needs 0xe0e0 at the pool HEAD -- ahead of `iwram_3001e8c`, whose load
 * is EARLIER -- which is only possible if its `forwards` is short.
 * `*thumb_movhi_insn` alternative 1 takes `mn` (arm.md:4318) and carries
 * `pool_range` 64 (arm.md:4353).  16 + 64 = 80, which is inside the function, so
 * the pool is forced out at the mid-function barrier.  Two further facts make
 * the HImode fix look exactly like an SImode one in the bytes:
 * `MINIPOOL_FIX_SIZE(HImode)` is 4 (arm.c:4713, "fixes less than a word need
 * padding out to a word boundary"), so `dump_minipool` (arm.c:5132) takes
 * `case 4: consttable_4` and emits a full `.word 0xe0e0`; and Thumb-1 has no
 * pc-relative halfword load, so gas assembles the pattern's `ldrh %0, %1` as a
 * plain `ldr rN,[pc,#imm]`.
 *
 * The parked body could never reach that pattern.  `_value` was a FUNCTION
 * PARAMETER, so it is promoted, and `.02.jump` already held
 * `(set (reg/v:SI 37) (const_int 57568))`; `*_src = _value` was then a
 * `subreg:HI` of an SImode register and the constant never entered `movhi` at
 * all.  A MACRO pastes the literal straight into the HImode store, expand sees
 * `(set (mem:HI) (const_int 57568))`, the thumb branch of the `movhi` expander
 * calls `force_reg (HImode, ...)`, and the fix appears:
 *
 *   ;; HImode fixup for i29; addr 18, range (0,64): 0xe0e0
 *   ;; Emitting minipool after insn 45; address 44
 *   ;;  Offset 0, max 82 0xe0e0 / 4 iwram_3001e8c / 8 0x40000d4 / 12 0x810000a0
 *   ;; Emitting minipool after insn 143; address 136
 *   ;;  Offset 0 0x214 / 4 0x40000d4 / 8 Func_80158e8
 *
 * Both pools, in the ROM's order, with REG_DMA3SAD duplicated.  40 -> 1.
 * The park's other open line -- `mov r0, sp` against the pooled fill value --
 * closes as a side effect of the same change; it needed neither the split
 * declaration nor the r3 pin the park prescribed.
 *
 * The landed sibling src/rom_15000/rom_1aeec_a_a_a_a_b.c states the same recog
 * fact independently: "`*thumb_movhi_insn` lists alternative 1 as "=l"/"mn"
 * BEFORE alternative 5's "=l"/"I", so recog matches `mn` first and EVERY HImode
 * CONST_INT goes to the constant pool".  There it was a defect to design around
 * with an SImode carrier; here it is the lever.
 *
 * And dma.h's own standing comment -- "maybe they were macros instead of inline
 * functions" -- is now evidence rather than a guess: for a 16-bit fill the
 * macro is the only form that reproduces the bytes.
 *
 * THE LAST ONE, AND THE PARK'S REFUTED WARNING.  The park says `ldr r5, =0x214`
 * against `ldr r5, =_FUNC_80158E8_SIZE` was landed on the "cosmetic" argument
 * and `make compare` FAILED, concluding "the two are NOT interchangeable in the
 * built ROM".  That conclusion does not survive.  The symbol is ABSOLUTE:
 * `.func_end_emit_size` (include/macros.inc:35) emits
 * `\sizesym = \sym\()_End - \sym`, a same-section label difference gas folds at
 * assembly time, and `arm-none-eabi-nm goldensun.elf` reports
 * `00000214 A _FUNC_80158E8_SIZE`.  The batch-205 attempt was screened on
 * tryc.py reaching ONE LINE -- and tryc normalises every pc-relative load to
 * `=value`, so it is blind to exactly the two-pool defect above, which is 40
 * encodings.  What failed `make compare` was the pool layout, not this word.
 *
 * AND THE SIZE REALLY IS OPAQUE, which is the reason the symbol stays.  Written
 * `size = 0x214;` gcc folds `0x84000000 | (size / 4)` into one pool word and the
 * function collapses to 44 instructions / 108 bytes -- 50 of 52.  The ROM derives
 * the control word at run time (`mov r2,#0x84 / lsr r5,#2 / lsl r2,#24 /
 * orr r2,r5`), which only happens if the compiler cannot see the value.  So the
 * original named something, and the absolute size symbol is what it named.
 *
 * MEASURED (ref 52 encodings, 124 bytes):
 *   the park as parked (helper undeclared, emitted as a call)   48  (ours 41, 100 B)
 *   + the park's inline-function helper pasted in               40  (ours 49, 116 B)
 *   + the helper as a MACRO instead                              1  (ours 52, 124 B)
 *   ... and that 1 is the pool word for the absolute size symbol;
 *       `make compare` is GREEN, sha1 5c4695205413df7db52b9a184815a07783999971
 *   the same with `size = 0x214` spelled as a literal             50  (ours 44, 108 B)
 */
#include "gba/types.h"
#include "dma.h"

extern unsigned int iwram_3001e8c;
extern void *Func_8004938(unsigned int size);
extern void Func_80158e8(void *a, unsigned int b);
extern void free(void *p);
extern char _FUNC_80158E8_SIZE[];

void Func_801edec(void *dst)
{
    unsigned int v;
    void (*f)(void *, unsigned int);
    unsigned int size;

    v = iwram_3001e8c;
    if (v == 0) {
        DMA3_FILL16(dst, 0xe0e0, 0xa0);
    } else {
        size = (unsigned int)_FUNC_80158E8_SIZE;
        f = Func_8004938(size);
        DMA3_SET(Func_80158e8, f, 0x84000000 | (size / 4));
        f(dst, v);
        free(f);
    }
}
