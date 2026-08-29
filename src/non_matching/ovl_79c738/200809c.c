/* OvlFunc_909_200809c  [ovl_79c738]  --  0x0200809c
 *
 * Source asm: goldensun/asm/overlays/rom_79c738/ovl_30_c_c_a_a_a_a.s
 *
 * NOT BLOCKED -- NOT YET WRITTEN. This file exists to stop a sweep from
 * generating the wrong C for this function a fourth time.
 *
 * The GetEntrances sweep matches on "compares the gState halfword at +0x1C0
 * against N pooled constants, and has N+1 pool loads into r0". This function
 * satisfies that and is a DIFFERENT function: between the compare and the
 * return it calls __Func_808b868 and __GetFlag, and one of the values the
 * sweep counts as a return target is a pooled 0x84e -- a flag id, not a table
 * address. The generated C does not even compile:
 *
 *     parse error before `0x84e'
 *
 * That has now happened three times, in batches 15, 19 and 19 again. Each cost
 * a generate-and-screen cycle. The looseness of the sweep criterion is
 * deliberate -- it is what finds variants rather than exact clones, and it
 * found the 5- to 12-way forms this round -- so the fix is not to tighten it
 * but to leave this note where the next sweep's operator will see it.
 *
 * tools/rescreen_park.py will now pick this file up each round, which is the
 * mechanism that makes a parked note visible rather than merely written.
 *
 * WHAT IT ACTUALLY DOES, so far as reading it goes: for area 0x21 it loads a
 * table at .L29b4, passes it to __Func_808b868, then checks flag 0x84e and
 * branches on the result. Elevating it needs that read properly, not a
 * template.
 */
