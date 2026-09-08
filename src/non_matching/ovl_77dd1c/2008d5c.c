/* OvlFunc_882_2008d5c -- 0x02008d5c,
 * asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_a_c_c_c_c.s
 *
 * FOUR differing encodings, AT THE ROM'S EXACT 143 encodings and 360 bytes,
 * WITH RELOCATIONS SILENT. Best candidate in scratch_elev/b255/a3/ (NOTES.md
 * carries the full sweep). Attempted as a bonus while landing its file-mate
 * OvlFunc_882_2008a10, not as an assigned target.
 *
 * BLOCKER CLASS: ORDERING. Two adjacent-instruction reorders inside a
 * byte-field region. The recorded relocation partition places it exactly --
 * 2-3 differing with relocations silent is an ordering problem, 14+ with them
 * differing is a CSE problem, and nothing lands in between. This is four,
 * silent, at exact size.
 *
 * ALIAS IS THE WRONG AXIS, AND THE DIAGNOSTIC SAYS SO. `-fno-schedule-insns2`
 * REGRESSES it, 4 -> 32. By the recorded sign rule that means sched2 is already
 * producing the ROM's order and there is nothing for an alias lever to buy.
 * DO NOT SPEND A ROUND ON UNION SPELLINGS HERE. (The contrast is the
 * eighteen-copy block-push park, where the same flag IMPROVED the window 7 -> 4
 * and alias set 0 closed it. Same diagnostic, opposite sign, opposite verdict.)
 *
 * MEASURED WORSE OR EQUAL, so do not re-spend them: fifteen mask spellings,
 * four byte-field spellings, four barrier placements.
 *
 * ONE SETTLED SUB-RESULT, already load-bearing in the candidate: the mask must
 * be a named `int msk = ~0xc`. Written inline it narrows to `movs r3, #243` and
 * the function comes out TWELVE BYTES LONG.
 *
 * FILE CONTEXT. Its .s now holds this function and OvlFunc_882_2008434 (~280
 * insns, never attempted) after the three-way split that landed 2008a10.
 * Closing BOTH would let the two halves collapse back toward one TU; closing
 * only this one buys a further split rather than a whole file, so it is worth
 * sizing 2008434 first.
 *
 * The overlay.ld lines MUST KEEP their asm/ paths.
 */
