/* Func_8015f30  --  0x08015f30   "InitUiSystem"
 *
 * Was goldensun/asm/rom_15000/rom_15e8c_a_c_a_a_a.s, which held it alone.
 *
 * Brings up the whole UI layer:
 *     galloc_ewram(0xf, 0x12fc) allocates the UI block -- this is what
 *       iwram_1e8c points at, and 0x12fc is its size, so every +0xNNN offset
 *       in this module is bounded by it
 *     the block is DMA-cleared, then defaults are written:
 *       +0xea3 = 1 (dirty mask, see Func_80160fc), +0xea7 = 0xf, +0x12b6 = 0x63
 *     the first 0x500 bytes are filled with 0xf000f000, the empty-tile pattern
 *     Func_8015ef4 builds the node free list
 *     Func_8019d0c initialises the menu layer
 *     StartTask registers Func_80160fc as the per-frame flush at priority 0x480
 *     Func_80173f4 finishes setup
 * Func_8016018 below is the same sequence with one extra field and a different
 * finisher; use this one for the plain case.
 *
 * CORRECTION to the .s header this replaced, which said "the first 0x140 bytes
 * are filled". The count field of a DMA control word is in TRANSFERS, and 0x85
 * selects 32-bit ones, so 0x85000140 is 0x140 WORDS = 0x500 bytes. The other
 * word, 0x850004bf, is 0x4bf words = 0x12fc bytes, which is the whole block and
 * confirms the reading.
 *
 * A THIRD DMA-HELPER SIGNATURE: ONE SLOT SHARED BY TWO TRANSFERS. The recorded
 * pair is `mov r0, sp / str r3, [r0]` for DMA3_CLEAR/DMA3_FILL, versus a fill
 * value stored wherever gcc likes for DMA3_SET. This ROM does neither:
 *
 *     mov r5, sp
 *     str r3, [r5]
 *     mov r0, r5
 *
 * -- the address in a PSEUDO, the store through the pseudo, and only then a
 * copy into r0. That is a named volatile slot object plus a plain pointer
 * local, handed to DMA3_SET twice. Two DMA3_CLEAR/DMA3_FILL expansions instead
 * give `sub sp, #8` -- two slots -- and store through the r0 hardreg.
 * Two sub-levers, both MEASURED:
 *   - the `volatile` belongs on the OBJECT, not the pointer. Without it gcc
 *     folds *slot back to [sp] (2 differing); `vu32 *slot` over a plain object
 *     does not help (5 differing).
 *   - `slot = &value;` has a PLACEMENT. Before the bl it hoists
 *     `sub sp, #4 / mov r5, sp` into the prologue (6 differing); after it,
 *     exact.
 *
 * WHY HImode LITERALS ALWAYS POOL -- the mechanism under blocker 1b, READ from
 * gcc-2.96 rather than inferred. `*thumb_movhi_insn` (config/arm/arm.md:4318)
 * lists alternative 1, `"l" <- "mn"`, BEFORE alternative 5, `"l" <- "I"`.
 * The `n` constraint accepts any const_int, so reload takes alternative 1 at
 * zero reload cost and the constant goes to the minipool -- even for values `I`
 * would happily encode. CONST_OK_FOR_THUMB_LETTER (arm.h:1096) is 0..255, and
 * 0x63 qualifies, and still pools. It is an ALTERNATIVE-ORDERING artefact, not
 * a value-range one.
 *
 * AND A POOLED HALFWORD CONSTANT FAKES THE LABEL FALSE NEGATIVE. Alternative
 * 1's pool_range is 64, so the `ldrh rN, .LCn` drags the whole minipool up
 * before the epilogue and gcc emits a real `b .L` over it. An intermediate
 * candidate's residue read as `ldr r3, =0x63` PLUS a bare `b L0 / L0:` pair --
 * which looks exactly like the recorded jump-over-pool false negative, but was
 * a consequence of the constant, and fixing the constant deleted it. BEFORE
 * filing a trailing `b Lx / Lx:` as a false negative, grep the generated .s for
 * `ldrh rN, .L`.
 *
 * THE int INTERMEDIATE NEEDS A SECOND NAME: THE DESTINATION POINTER.
 * `int v = 0x63; *(unsigned short *)(p + 0x12b6) = v;` buys the `mov`, but the
 * extra pseudo makes sched2 hoist the address pool load above the preceding
 * `strb` and rotates r3 through the block -- 7 to 10 differing across five
 * placements. Naming the lvalue's pointer as well, IN ROM ORDER (pointer
 * statement first, then the value), pins the address computation to its own
 * statement and the block returns to ROM order.
 *
 * The recorded remedy from src/overlays/rom_7c460c/ovl_314_c_a_c_c_b.c --
 * "assign at the top so it is live across the calls" -- DID NOT TRANSFER here,
 * giving 9 differing with `mov r6, #0x63` and a grown prologue, because this
 * ROM materialises 0x63 at the point of use, in the r3 just freed by
 * `add r2, r4, r3`. The two cases are distinguishable from the ROM alone:
 * a value live across a call wants assignment at the top; a value materialised
 * into a just-freed register beside its own store wants pointer and value
 * named adjacently.
 *
 * r4 holds the block pointer, is never pushed, and is dead before
 * `bl Func_8015ef4` -- -fcall-used-r4 working, not a missing save.
 */

#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern unsigned char *galloc_ewram(int tag, int size);
extern int StartTask(void *fn, int pri);
extern void Func_8015ef4(void);
extern void Func_8019d0c(void);
extern void Func_80173f4(void);
extern void Func_80160fc(void);

void Func_8015f30(void)
{
    unsigned char *p;
    volatile u32 value;
    volatile u32 *slot;
    int v;
    unsigned short *q;

    p = galloc_ewram(0xf, 0x12fc);
    slot = &value;
    *slot = 0;
    DMA3_SET((void *)slot, p, 0x85000000 | (0x12fc / 4));
    p[0xea3] = 1;
    q = (unsigned short *)(p + 0x12b6);
    v = 0x63;
    *q = v;
    p[0xea7] = 0xf;
    *slot = 0xf000f000;
    DMA3_SET((void *)slot, p, 0x85000000 | (0x500 / 4));
    Func_8015ef4();
    Func_8019d0c();
    StartTask(Func_80160fc, 0x90 << 3);
    Func_80173f4();
}
