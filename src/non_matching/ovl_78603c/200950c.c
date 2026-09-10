/* OvlFunc_885_200950c -- 0x0200950c, asm/overlays/rom_78603c/ovl_30_c_c_a_c_a_c_c_a.s
 *
 * TWO differing encodings of 239, with SIZE AND RELOCATIONS BOTH SILENT --
 * squarely the recorded ordering band. Candidate:
 * scratch_elev/b256/rich/final/ovl_30_c_c_a_c_a_c_c_a.c.
 *
 * BLOCKER CLASS: ORDERING AT EXPAND-TIME STACK-ARGUMENT EMISSION. At ONE of
 * six `__Func_8010704` sites the ROM materialises argument 6 before storing
 * argument 5. The other five sites are exact under the same spelling, which is
 * what makes this a site property rather than a callee property.
 *
 * ALIAS IS THE WRONG AXIS: `-fno-schedule-insns2` REGRESSES, 2 -> 70, so
 * sched2 is already producing the ROM's order.
 *
 * REFUTED, with measurements in the scratch NOTES.md: every pin width and
 * register at the site; 3 barrier placements; 4 prototypes; comma expressions;
 * named locals; 7 flags; an if/else restructure.
 *
 * WHAT IS WORTH TRYING NEXT, and was not: the stack-argument pin has a
 * discriminator found on this function's sibling -- it is exact where the ROM
 * REBUILDS both stack arguments and catastrophic (227) where the ROM COPIES
 * one out of a commoned high register. Check which case this site is before
 * reaching for a pin again.
 *
 * LANDING SHAPE IF CLOSED: src/overlays/rom_78603c/ovl_30_c_c_a_c_a_c_c_a.c.
 * `asmfacts.py` says WHOLE -- convert directly. One function, no data, no
 * split. `makefile_flags()` is empty with no wildcard hazard, and
 * overlays/rom_78603c/overlay.ld:28 MUST KEEP its asm/ path. No cross-file
 * `.L` symbol is referenced.
 */
