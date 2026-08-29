/* ClearFlag  [rom_77000]
 * Source asm: goldensun/asm/rom_77000/rom_79338_a.s
 * (path updated: the .s was split or renamed after this was parked)
 *
 * Parked: logic faithful, does NOT byte-match (endgame permuter seed).
 * Candidate: work/loop8/ClearFlag.c
 * TODO(residual): logic + unsigned-shift confirmed correct. Remaining diff is
 * reg-alloc/scheduling ONLY (drain rolled back on compare-rom). Permuter should
 * close it. Sibling family: SetFlag (set), GetFlagNybble (get nibble).
 *
 * BATCH 98 -- THE RESIDUAL IS A SCHEDULING ONE AND IS NOW CHARACTERISED. 5
 * differing of 11, same length. The ROM computes the BIT before it touches
 * gFlags; we compute the index first:
 *
 *     rom   mov r3,#7 / and r3,r0 / mov r2,#1 / lsl r2,r3
 *           ldr r1,=gFlags / lsl r3,r0,#0x14 / lsr r0,r3,#0x17
 *     ours  mov r3,#7 / and r3,r0 / ldr r1,=gFlags
 *           lsl r0,#0x14 / lsr r0,#0x17 / mov r2,#1 / lsl r2,r3
 *
 * That ordering is what frees r3 for the ROM's THREE-OPERAND `lsl r3, r0, #20`;
 * with r3 still holding `flagID & 7` at that point we have to shift r0 in place.
 * So the two differences are one difference.
 *
 * Tried and identical at 5: naming the shift intermediate so the two shifts are
 * separate statements. Tried and WORSE at 12 lines / 8 differing: taking a
 * pointer to the byte first and masking through it.
 *
 * The source below already puts `bit` first; gcc reorders it. Nothing at the
 * statement level has reached that.
 */
extern unsigned char gFlags[];

void ClearFlag(int flagID) {
    int bit = 1 << (flagID & 7);
    int idx = ((unsigned)flagID << 20) >> 23;
    gFlags[idx] &= ~bit;
}
