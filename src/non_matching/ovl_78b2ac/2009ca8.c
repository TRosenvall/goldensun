/* OvlFunc_890_2009ca8 -- 0x02009ca8, asm/overlays/rom_78b2ac/ovl_30_c_c_a_c_b_c.s
 *
 * FIVE differing encodings of 456, at EXACT SIZE (1120 bytes), exact encoding
 * count, and RELOCATIONS IDENTICAL. Stable over three runs. Candidate and the
 * full sweep are in scratch_elev/b257/ (NOTES.md and the file header).
 *
 * Path to the floor: 442 -> 416 (eviction pins AS A SET, which fixed the push
 * mask) -> 349 (ordering pins) -> 80 (__CopyMapTiles stack slots as plain
 * literals) -> 14 -> 5.
 *
 * BLOCKER CLASS: CONSTANT COMMONING WITH NO BASIC BLOCK TO PUT cprop AT. Of
 * three literal `0`s in the duplicated actor tail, the ROM splits one into a
 * second callee-saved register IN THE FIRST COPY ONLY. cprop is cross-block and
 * the tail is straight-line, so there is no join to hang the split on -- the
 * same shape as the recorded dominance-contradiction class, but without the
 * guarded block that class relies on.
 *
 * AXES CLOSED, measured: `-fno-schedule-insns2` REGRESSES 14 -> 210, so sched2
 * is already producing the ROM's order and ALIAS IS THE WRONG AXIS. And
 * -fno-gcse, -fno-rerun-cse-after-loop, -fno-strict-aliasing and
 * -fno-strength-reduce are all BYTE-IDENTICAL to no flag, so no existing
 * Makefile flag group reaches this one either.
 *
 * *** DO NOT CHASE THE 3-OF-456 SPELLING. IT IS A MISCOMPILE. ***
 * Pinning the split zero to r7 reaches 3 differing -- the best number any
 * spelling produced -- and emits `strb r7, [r3]`, storing the POINTER instead
 * of the zero. `f` is also allocated r7, gcse commons the other zero into the
 * pinned pseudo, and the store picks up the wrong value. See the general entry
 * "A PIN-INDUCED MISCOMPILE IS A LOCAL MINIMUM THAT CANNOT REACH ZERO" in
 * docs/elevation.md. The 5-differing spelling is the real floor.
 *
 * Three smaller results, all in the scratch NOTES.md: the stack-argument rule
 * is about whether the two slots SHARE A REGISTER at one site, not about naming
 * them (named locals get the right register COUNT and the wrong MEMBERSHIP); a
 * pin at a site that ALREADY MATCHES can be the lever for a defect thirty
 * instructions downstream, worth 9 of the last 14; and a sibling's cure for a
 * same-shaped block can be actively worse -- third confirmation.
 *
 * LANDING SHAPE IF CLOSED: the .s holds this and OvlFunc_890_200a108, with no
 * data and disjoint labels, so split_s.py cuts it cleanly and nothing needs
 * `.global`. One .text line, overlays/rom_78b2ac/overlay.ld:34, which MUST KEEP
 * its asm/ path. makefile_flags() is empty. Closing 200a108 too would land the
 * file WHOLE and is worth sizing first.
 */
