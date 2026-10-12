/* Func_8093304 (SetDialoguePortrait) -- 0x08093304,
 * asm/rom_8a000/rom_93304_a_a_a_a_a_a.s
 *
 * 29 of 37 stream encodings, 28 instructions against the ROM's 29 -- ONE SHORT,
 * so the 29 is PHASE, not distance.  Position-tolerant: 26 of 37 aligned-equal
 * (70.3%), 13 differing in 10 hunks.  Pin-free.  THIS PARK NOW HAS A BODY; it
 * was prose only until batch 338, and the 19-of-37 aligned figure below is what
 * its `Best C:` (scratch/L93304.c) measured.
 *
 * Verify with:
 *     python3 tools/objcmp.py src/non_matching/rom_8a000/8093304.c \
 *         asm/rom_8a000/rom_93304_a_a_a_a_a_a.s --func Func_8093304
 *     python3 tools/aligncmp.py src/non_matching/rom_8a000/8093304.c \
 *         asm/rom_8a000/rom_93304_a_a_a_a_a_a.s Func_8093304 -v
 *   Use the SECOND one while iterating.  The positional count read 27 for the
 *   parked body and 27 for five materially different variants of it.
 *
 * ---- batch 338, brief E: A DOCUMENTED BOUND IS WRONG ----
 *
 * docs/elevation.md:6394-6402 ("The named-pointer lever needs the offset to be
 * mutated afterwards") says of THIS function: "Where the offset is dead after
 * the store, gcc always folds it into the addressing mode and no spelling tried
 * recovers the separate `add`".  One spelling does.
 *
 * Write the join offset as a LITERAL rather than the offset variable:
 *
 *     parked   r3 = (unsigned int)base; r3 += off;    -> strh r1, [r5, r2]   FOLDED
 *     here     r3 = (unsigned int)base; r3 += 0x12f6; -> add r3, r5, r1
 *                                                        strh r2, [r3]       ROM's shape
 *
 * MECHANISM, with the citation.  THUMB_GO_IF_LEGITIMATE_ADDRESS accepts
 * REG+REG for any mode <= 4 bytes (arm.h:2135-2148) but REG+const only when
 * LEGITIMATE_OFFSET holds (arm.h:2149-2155) -- a 5-bit scaled offset for
 * HImode, max 62.  0x12f6 fails it, so `legitimize_address` force_regs the whole
 * sum at expand time and `combine` CANNOT re-fold: the only form it could
 * substitute back is `(plus reg const_int)`, which the predicate rejects, and
 * combine will not invent a second index register of its own.
 *
 * With the offset in a VARIABLE the expanded address is `(plus reg reg)`, which
 * the predicate accepts directly, and the `add` never exists to be preserved.
 * So the rule in elevation.md should read: the fold is avoidable, but only by
 * making the offset a constant the HImode addressing mode cannot hold.
 *
 * WHAT IS LEFT, and it is the other half of the same conflict.  The ROM carries
 * 0x12f6 in a REGISTER into the join -- `ldr r1,=0x12f6` on the sentinel path,
 * `ldr r1,=0x12f4 / add r1,#2` on the other, the derived-constant shape this
 * park already documents.  That needs the offset to be a mutated VARIABLE, which
 * is exactly what re-folds the store.  Measured both ways:
 *
 *     join offset a literal, arms uniform (`off=0x12f4; ...; off+=2`)   26 aligned
 *     join offset the variable, arms uniform                            15 aligned
 *     join offset the variable (the parked body)                         19 aligned
 *
 * gcc deletes the arms' `off += 2` as dead once the join stops reading it, so
 * the sentinel arm never materialises 0x12f6 and the derivation is lost.  The
 * residue is 2 real instructions: the sentinel arm's missing `ldr r1,=0x12f6`
 * and the non-sentinel arm's missing `add r1,#2`, plus a pool-offset shift and
 * the section-tail pad that both follow from being 4 bytes short.
 *
 * RECORDED INERT at 19 aligned (the parked figure), five spellings: mirroring
 * the ROM's register roles with reused `unsigned int` locals; the join written
 * as `a += (unsigned int)base`; as `*(short *)((unsigned int)base + a)`; as
 * `a + (unsigned int)base`; and the arm's `off += 2` moved after its store (18,
 * worse).  The three minimal forms of the literal fix all read 26, so the
 * role-mirroring is NOT what pays -- the literal is the whole gain.
 */
extern unsigned char *iwram_3001e8c;
extern unsigned int gState;
extern unsigned char L9fc28[] __asm__(".L9fc28");
extern int Func_8092ba8(int slot);
extern int GetSpriteVoice(int a);

void Func_8093304(unsigned int slot)
{
    unsigned char *base;
    unsigned int a;
    unsigned int b;
    unsigned int c;
    int id;

    base = iwram_3001e8c;
    if (slot == 0x80000000) {
        b = 0x12f4;
        a = 0x12f6;
        c = (unsigned int)base + b;
        b = 0;
        *(short *)c = b;
    } else {
        id = GetSpriteVoice(Func_8092ba8(slot));
        c = (unsigned int)&gState;
        a = 0x83;
        a <<= 2;
        c += a;
        c = *(unsigned char *)c;
        b = (unsigned int)L9fc28;
        b = *(unsigned char *)(b + c);
        a = 0x12f4;
        c = (unsigned int)base + a;
        a += 2;
        *(short *)c = id;
    }
    c = (unsigned int)base + 0x12f6;
    *(short *)c = b;
}
