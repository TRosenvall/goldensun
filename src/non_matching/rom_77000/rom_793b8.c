/* GetFlagByte / SetFlagByte get/set a flag byte by 9-bit bitfield index.
 * Source asm: goldensun/asm/rom_77000/rom_79338_c_a.s
 * (path updated: the .s was split or renamed after this was parked)
 *
 * index = (id << 20) >> 23  (bits 3..11 of id), into the byte array
 * gFlags. b8 reads, c8 writes.
 *
 * non_matching: the LOGIC is exact, but the bytes differ only in instruction
 * SCHEDULING: gcc-2.96 here materializes the array base (ldr =gFlags)
 * FIRST and shifts the index in place on r0, whereas the ROM computes the
 * index using r3 as scratch (lsl r3,r0,#20; lsr r0,r3,#23) and loads the base
 * LAST. No C structuring tried (index temp, base pointer, declared before/after)
 * moves the pool load past the shifts. A clean decomp-permuter seed.
 *
 * REVISITED.  The park recorded only C-structuring attempts; the flag space had
 * never been tried and now has been, without moving it.  FIFTEEN settings, all
 * 3 of 5: SCHED2, -fno-schedule-insns, CSE, GCSE, ALIAS, STRENGTH, O1,
 * -fno-peephole, -fno-peephole2, -fno-expensive-optimizations,
 * -fno-cse-follow-jumps, -fno-force-mem, -fno-caller-saves, -fno-delayed-branch.
 *
 * FOUR SPELLINGS of the bit-field extraction were also screened and gcc
 * canonicalises all of them to the identical RTL, so the extraction form is not
 * the variable either:
 *      (id << 20) >> 23      (id >> 3) & 0x1ff
 *      (id & 0xff8) >> 3     a 3/9/20 bitfield struct passed by value
 *
 * WHAT THE DIFFERENCE ACTUALLY IS, stated precisely because five instructions
 * leave no room for ambiguity:
 *      rom    lsl r3, r0, #20 / lsr r0, r3, #23 / ldr r3, =gFlags
 *      ours   ldr r3, =gFlags / lsl r0, #20     / lsr r0, #23
 * The ROM uses THREE-operand shifts through r3 and loads the base LAST, reusing
 * the register it just used as shift scratch.  We use two-operand in-place
 * shifts and load the base FIRST.
 *
 * A three-operand shift means the source and destination pseudos did not
 * coalesce, which happens when the source is still live.  Nothing in five
 * instructions keeps `id` live, so either the original had a shape that did, or
 * -- more likely -- this is the same wall as the pool-load ordering recorded in
 * docs/elevation.md, where gcc emits pool loads early and the ROM emits them
 * late.  That shape has 25 sites and zero reproductions tree-wide.
 */
extern unsigned char gFlags[];

unsigned char GetFlagByte(unsigned int id) {
    return gFlags[(id << 20) >> 23];
}

void SetFlagByte(unsigned int id, unsigned char val) {
    gFlags[(id << 20) >> 23] = val;
}
