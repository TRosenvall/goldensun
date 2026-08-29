/* OvlFunc_948_2009df8 -- asm/overlays/rom_7d30e0/ovl_30_c_c_c_a_c.s
 *
 * BLOCKER: ARG INTERLEAVE (straight-line -- basic-block lever unreachable)
 *
 * 18 of 40 differing, and every one of them is a register-name cascade from a
 * single decision.  The arithmetic, the two guards, the shared r5 and both
 * stack slots are all exact; ROM and ours emit the same 40 instructions in a
 * different ORDER for the immediate argument setup:
 *
 *     rom  ldr r0,[r0,#8] ... asr r0,#0x14 / str r0,[sp]
 *          mov r1,#0x37 / mov r0,#0x35 / mov r2,#1 / mov r3,#1
 *     ours ldr r3,[r0,#8] ... asr r3,#0x14 / str r3,[sp]
 *          mov r0,#0x35 / mov r3,#1 / mov r1,#0x37 / mov r2,#1
 *
 * ROM computes the divide in r0 (free, since GetActor just returned there) and
 * only afterwards loads r0 with arg1; we allocate r3 for the divide because r0
 * is claimed for arg1 first.  Whichever register wins, the other three moves
 * permute with it.
 *
 * The lever that fixes this class -- assign the value in a block that DOMINATES
 * the call -- has nothing to bite on: the function is straight line from the
 * prologue to the first call, so the call site's own block is the only block.
 * Same shape as OvlFunc_942_20087dc.
 *
 * MEASURED (all 40 lines, all 18 differing unless noted):
 *   shared s0 + s1 locals                              18
 *   separate s0/s2 locals, one per call site           18
 *   s1 declared and assigned before s0                 20
 *   divide inlined into the argument list              20
 *   struct Actor *p/*q pointer locals                  20
 *   arg2 spelled as s1 (sharing arg6's local)          18
 *   prototype removed                                  18
 *   -fno-schedule-insns                                18
 *   -fno-rerun-cse-after-loop                          18
 *
 * The two spellings that move the count move it the WRONG way, which is the
 * tell that the 18 is a floor and not a near miss.
 */
