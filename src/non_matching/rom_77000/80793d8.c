/* IncFlagByte (IncrementSaveCounter) -- 0x080793d8,
 * asm/rom_77000/rom_79338_c_a.s
 *
 * 14 vs 13 lines, 11 differing.  Candidate at scratch/Linc.c.
 *
 * Shares GetFlag's index computation and its blocker (see
 * src/non_matching/rom_77000/8079338.c): the ROM's
 * `lsl r3, r0, #0x14 / lsr r0, r3, #0x17` keeps the intermediate in its own
 * register and no spelling reproduces that.
 *
 * Additionally the ROM copies the loaded byte before comparing it --
 * `ldrb r2,[r1,r0] / mov r3,r2 / cmp r3,#0xfe / bhi / add r3,r2,#1` -- so the
 * compare temp and the incremented value share r3 while the original stays in
 * r2.  A single `v` variable gives one register for all three.
 *
 * The saturation guard is `bhi`, i.e. UNSIGNED >, so the source test is
 * `v <= 0xfe` on an unsigned value, not a signed comparison.
 */
