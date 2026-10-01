/* OvlFunc_882_200b1ac -- 0x0200b1ac -- REFERENCE ANALYSIS ONLY, NOT A PARK.
 *
 * NO CANDIDATE WAS COMPILED.  No `N of M` line and no tool figure, deliberately.
 *
 * TAKE THIS ONE LAST.  It is the hardest of the five and the reason is not its
 * size.
 *
 * REFERENCE FACTS.  1026 instructions, FOUR branches to a label, 6 labels, 245
 * calls, `sub sp, #4`, FIFTY-THREE mentions of r8-r11 behind the full high-save
 * prologue.  Reference file
 * asm/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_a_a_c_c_c_c_a_c.s.
 *
 * IT HAS A LOOP, WHICH NONE OF THE OTHER FOUR DOES.  `.L3662` is defined at ref
 * line 470 and `bne .L3662` at 476 branches BACKWARD to it -- a 6-instruction
 * do/while.  Every claim in docs/band-800plus.md about this band was measured on
 * loop-free code, and a loop puts `08.loop` and `07.gcse` in play, so
 * `-fno-rerun-cse-after-loop` is the one flag whose inertness here is NOT
 * already settled.  Screen it with tools/flagcmp.py if a candidate stalls --
 * and remember flagcmp's numbers must never reach a claim line, and that no
 * Makefile row should be written on the shape alone.
 * Of the other three branches, `b .L33c4` (193), `b .L382c` (629) and
 * `b .L3adc` (902) each precede a pool directive and are SKIPS, and `.L3394`,
 * `.L3abc` are the pools' own labels.  So: ONE loop, ZERO if/else.
 *
 * LANDING IS A WHOLE-FILE CONVERSION.  One function, datacheck.py silent, no
 * split, ONE linker row: overlays/rom_77dd1c/overlay.ld:61.
 * It calls `_umodsi3_RAM`, which is the alias at overlays/rom_77dd1c/overlay.ld
 * :103 -- a candidate spelling `%` gets `__umodsi3` and objcmp will report one
 * relocation difference that is NOT a defect.  That exact non-residue is
 * already recorded in src/non_matching/ovl_77dd1c/2008434.c; expect it, and
 * report it rather than filtering it.
 *
 * WHY THE 884/969 PRESCRIPTION DOES *NOT* TRANSFER.  Thirteen high-register
 * live ranges, and only THREE of them hold constants:
 *     constants: r8 @840 `mov r2,#0xe0 / lsl r2,#8` (5 reads);
 *                r10 @660 (4 reads);  r9 @697 (2 reads)
 *     POINTERS and DERIVED ADDRESSES, the other ten:
 *                r11 @18  `ldr r1,[r6,#0x50]`   -- a loaded field, 4 reads
 *                r10 @19  `ldr r0,[r7,#0x50]`   -- a loaded field, 1 read
 *                r11 @656 and @684 `mov r1,r6 / add r1,#0x23` -- a +0x23
 *                         address, re-derived into the SAME register twice
 *                r11 @711 and @736 `mov r6,r0 / ldr r1,[r6,#0x50]`
 *                r9 @174, r8 @175, r8 @489, r10 @531, r10 @961
 * Function-wide it still reads 137 builds against 35 reuses (20.3%), i.e. the
 * same reuse fraction as 884 and 969 -- but the REUSED QUANTITIES ARE MOSTLY
 * ADDRESSES, so naming constants will not create them.  What creates them is
 * the OTHER lever: the derived address as a named local in its own statement,
 * whose precondition (more than one use) is satisfied at `+0x23`, at `[+0x50]`
 * and at the `base + 0x40c`-style sites.  That lever is what carried this
 * batch's 897 from +28/+9 to size-and-count exact, and it has to be measured in
 * BOTH directions per site -- an int temp can also stop a pool.
 *
 * ITS MEMORY TRAFFIC IS THE REAL WORK, NOT ITS CALLS.  48 `str`, 21 `strh`, 16
 * `strb`, 13 `ldrb`, 33 `add`, 13 `and`, 2 `orr` -- against 889's 4 `str` and
 * 2 `strh`.  Roughly a quarter of this function is structure writes, so expect
 * the `&= mask` / `|= bit` pair hazard (the two sites pull opposite ways; one
 * wants a narrow local, the other wants NO local) and the pooled-zero defect at
 * the 21 `strh` sites.  Budget most of the time there and transcribe the calls
 * last.
 *
 * CALLEES: 23 __CutsceneWait, 20 each __MapActor_SetPos and __MapActor_GetActor,
 * 17 __MapActor_SetBehavior, 15 __MapActor_SetAnim, 12 each __Func_8093554 and
 * __Func_8012330, 11 __WaitFrames, 10 __Func_8091200, 9 each _umodsi3_RAM,
 * __Random and __Func_80933f8, plus 29 more.  `__Random` and `_umodsi3_RAM`
 * together mean a `% N` expression; its result sequencing was MEASURED MUCH
 * WORSE through a temp on the sibling 2008434, so write it inline.
 */
