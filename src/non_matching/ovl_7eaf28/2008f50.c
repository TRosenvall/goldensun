/* OvlFunc_960_2008f50 -- 0x02008f50, asm/overlays/rom_7eaf28/ovl_314_c_c_c.s
 *
 * 131 of 131 lines, 40 differing.  Candidate: scratch/T8f50_B.c.
 *
 * THE RESIDUE IS CONCENTRATED, which is the useful part of this park.  Counting
 * differences by region:
 *
 *      prologue and the five flag remaps      3   (adjacency only)
 *      the actor loop, slots 8..0xc           2   (adjacency only)
 *      DMA queue append, first block         16
 *      DMA queue append, second block        19
 *
 * So 35 of 40 are in the two inline queue appends and the rest of the function
 * is right.  The flag remaps, the loop with its GetActor null test, the guarded
 * pair of writes at actor+0x18/0x1c, the sprite byte at +0x26, and both
 * REG_IME save/disable/restore pairs all reproduce -- the last using the
 * SET_IO(REG_IME, REG_ADDR_IME) idiom already established in
 * src/rom_c0/rom_3e58_c_b.c, which is what the ROM's `strh r5, [r5]` is.
 *
 * WHY THE APPENDS RESIST, and why this is a deliberate stop rather than a wall:
 * this tree already contains the same append, as ScheduleDmaTransfer in
 * src/rom_c0/rom_3650_c_b.c -- and that file is marked `// fakematch`.  It gets
 * there with REGISTER PINNING: `register void *d __asm__("r6")`, a matching pin
 * on r0, and an empty `__asm__ volatile("" : : "r"(queue))` to force the queue
 * pointer to be materialised before REG_IME is read.  Without those, gcc emits
 * the queue-base load AFTER the REG_IME load and allocates the task pointer
 * differently, which is exactly the 35.
 *
 * That technique would very likely match this function too.  It is not applied
 * here because a fakematch is a different kind of result from a match and is
 * worth choosing on purpose -- the existing file marks itself as one for the
 * same reason.  If the project decides the DMA-append family should be
 * fakematched consistently, this is the next candidate and the template is one
 * file away.
 *
 * TRIED: moving the count store ahead of the first task word, which is the
 * ROM's order (no change, gcc reorders it back); struct DmaQueue and
 * DmaTransfer taken from the existing user rather than hand-rolled.
 */
