/* GetFlag  [rom_77000]
 * Source asm: goldensun/asm/rom_77000/rom_79338_a.s
 * (path updated: the .s was split or renamed after this was parked)
 *
 * Parked: logic faithful, does NOT byte-match (endgame permuter seed).
 * Candidate: tools/runs/run_20260606T194103Z/GetFlag-iter-9.c
 * TODO(residual): shift idiom is correct ((unsigned)(x<<20)>>23); pure reg-alloc /
 *   scheduling diff; ROM keeps the shift result in a fresh reg (`lsls r3,r0,#20;
 *   lsrs r0,r3,#23`) and places the table `ldr` between, vs in-place on r0 here.
 *   Main-tree fn -> permutable.
 *
 * RE-ATTEMPTED IN BATCH 84 with the levers batches 82 and 83 added, and the
 * floor is unchanged at 3 of 13. The whole difference is:
 *
 *     rom    lsl r3, r0, #20 / lsr r0, r3, #23 / ldr r3, =gFlags
 *     ours   lsl r0, #20     / ldr r3, =gFlags / lsr r0, #23
 *
 * i.e. gcc shifts in place on r0 and drops the table load into the slot that
 * frees up, where the ROM shifts through a fresh register and loads after.
 * Both `lsl` forms are the same instruction -- Thumb has only `LSL Rd, Rm, #imm`
 * and `lsl r0, #20` is the disassembler's shorthand for `lsl r0, r0, #20` -- so
 * this is purely which register the allocator picked, and the placement follows
 * from it.
 *
 * Measured, all 3 of 13 unless noted:
 *   a named `int` intermediate for `flagID << 20`            3
 *   the same as `unsigned int`, shifted in two statements    3
 *   a named intermediate for the index as well               3
 *   `unsigned int` parameter                                 3
 *   the table taken into a local `unsigned char *`           3
 *   the mask on the left of the `&`                          6   worse
 *   everything inlined into one expression                  11   worse
 *
 * So neither batch-82's "remove a local" nor batch-83's "name it at the right
 * width" reaches this one: there is no commutative operator to reorder and no
 * constant to re-type. It stays in the register-allocation class.
 */
extern unsigned char gFlags[512];

int GetFlag(int flagID)
{
    int bit;
    int val;
    bit = 1 << (flagID & 7);
    val = gFlags[(unsigned)(flagID << 20) >> 23] & bit;
    return (unsigned)(-val | val) >> 31;
}
