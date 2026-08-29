/* OvlFunc_945_200beec -- asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c.s
 *
 * BLOCKER: SPLIT SHIFTED BUILD -- see src/non_matching/ovl_7cb2c0/200be34.c
 *
 * 14 of 56, same length, first diff at position 8.  Byte-for-byte the same
 * residue as its sibling OvlFunc_945_200be34 in the same overlay -- same call
 * sequence with actor slot 0x12 instead of 0x10:
 *
 *     rom  mov r1,#0x96 / mov r0,#0x12 / lsl r1,#0x10 / ldr r2,=0x24a0000
 *     ours mov r1,#0x96 / lsl r1,#0x10 / ldr r2,=0x24a0000 / mov r0,#0x12
 *
 * Everything measured on 200be34 applies; the full table is in that park.  The
 * function is straight line (no labels), so the basic-block lever has nothing
 * to bite on.  Reachable in principle -- 51 of the 2987 generated .s files
 * contain the shape -- but no spelling has been found.
 *
 * Best C: scratch/wbeec.c.
 */
