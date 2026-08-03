/* free  [rom_c0]  --  0x08002df0
 *
 * Source asm: goldensun/asm/rom_c0/rom_2dd8.s
 *
 * FreeScratch. Rewinds whichever of the two arenas the pointer belongs to back
 * to it. The arena is selected by bit 22 of the address, which is why the
 * index is `(p >> 22) & 4` -- a byte offset of 0 or 4 into a two-entry table,
 * not an element index.
 *
 * Blocker: REGISTER ASSIGNMENT, shifted by one position throughout. Six
 * instructions against six, and after taking the table base into a local the
 * ORDER is exact:
 *
 *     rom    ldr r4,=gPtrs / mov r1,#4 / lsr r2,r0,#22 / and r2,r1 / str r0,[r2,r4]
 *     ours   ldr r1,=gPtrs / mov r2,#4 / lsr r3,r0,#22 / and r3,r2 / str r0,[r1,r3]
 *
 * Same instructions, same sequence. The ROM allocates r4, r1, r2 where we
 * allocate r1, r2, r3 -- every register one place earlier in
 * REG_ALLOC_ORDER {3,2,1,0,4,...}. The ROM skips r3 entirely and reaches past
 * r0 to r4 for the base.
 *
 * THIS IS THE SAME RESIDUE AS FindEntityAtPosition
 * (src/non_matching/ovl_780898/200806c.c): instruction sequence exact, the
 * allocator handing out a different set of registers, and a global pointer
 * plus a computed index in both. Two unrelated functions failing the same way
 * is worth more than either alone -- whatever governs it is likely one
 * property of how the base pointer's live range is formed, not two separate
 * puzzles.
 *
 * TRIED: base in a local (this form); base and mask both in locals; the shift
 * split from the mask into its own statement; the mask named without a base
 * local; and `gPtrs[i >> 2]` as a real array index, which costs two extra
 * instructions because the byte offset has to be converted back.
 */
extern void *gPtrs[];

void free(void *p)
{
    char *base;
    unsigned int i;

    base = (char *)gPtrs;
    i = ((unsigned int)p >> 22) & 4;
    *(void **)(base + i) = p;
}
