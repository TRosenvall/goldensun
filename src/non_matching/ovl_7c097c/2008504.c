/* OvlFunc_936_2008504 -- asm/overlays/rom_7c097c/ovl_30_c_c_c_a_a_c_a_a_c.s
 *
 * BLOCKER: THE `neg` INTERLEAVE -- see src/non_matching/ovl_7c460c/2008c74.c
 *
 * 2 of 54, same length, first diff at position 39.  52 lines exact, including
 * both __CopyMapTiles sites (the two stack arguments are the same value here,
 * so ONE shared local is right and the stack-arg-PAIR lever is not wanted).
 *
 *     rom  mov r2,#8 / mov r1,#0 / neg r2,r2 / mov r0,#0
 *     ours mov r2,#8 / neg r2,r2 / mov r1,#0 / mov r0,#0
 *
 * Byte-for-byte the same residue as OvlFunc_939_2008c74 with a different
 * callee (__Func_809228c vs __Func_80922c4), which is what makes it a family
 * rather than a coincidence.  Everything measured on that function applies
 * here; the full table is in its park.  Best C: scratch/q8504.c.
 */
