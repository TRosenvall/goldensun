/* OvlFunc_930_2008b2c -- 0x02008b2c,
 * asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_a.s
 *
 * 95 vs 94 lines, 34 differing.  Candidate at scratch/L8b2c.c.
 * A pure straight-line script: 32 calls, no memory operations, no branches.
 *
 * BLOCKER, both halves the no-branch case:
 *   - two `__Func_8092adc` calls have r0 emitted INSIDE the split build of
 *     their second argument (`mov r1,#0xc0 / mov r2,#0 / mov r0,#0 /
 *     lsl r1,#7`), and the interleave lever needs a dominating block;
 *   - `0x81 << 1` is used at two calls and gets commoned into r5, adding a
 *     push the ROM does not have.
 *
 * Transcription itself was clean -- `tools/draft_script.py` produced the call
 * sequence and only the arity of two calls needed correcting against the
 * listing.  Nothing about the C is in doubt; the function is simply out of
 * reach of both levers.
 *
 * SELECTION NOTE.  This is the fourth straight-line function I have taken to a
 * park this session.  `tools/pool.py` prints `br`, and br == 0 with any
 * repeated constant or unguarded interleave site is a park before the first
 * screen.  Read the column.
 */
