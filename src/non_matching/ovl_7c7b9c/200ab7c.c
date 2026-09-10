/* OvlFunc_943_200ab7c -- 0x0200ab7c,
 * asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_c_c.s
 *
 * THREE differing of 97, and ONE OF THE THREE IS A BENIGN LINKER ALIAS rather
 * than a defect -- the same `__divsi3` / `_divsi3_RAM` class objcmp resolves as
 * one symbol elsewhere. So the real residue is two. Candidate in
 * scratch_elev/b257/a4/final/ (second function of that file).
 *
 * Attempted as a file-mate of OvlFunc_943_200ac84, which is parked at 57 of 530
 * in the same .s. CLOSING BOTH LANDS THE FILE WHOLE, and this one is much the
 * closer of the two, so it is the better place to restart.
 *
 * NOT FULLY CHARACTERISED -- this was a bonus attempt, not a target, and the
 * round ended with its two remaining encodings undiagnosed. Recorded so the
 * floor is not lost rather than because the analysis is finished. The next
 * attempt should start by running `-fno-schedule-insns2` and reading the SIGN,
 * since that has closed or opened the alias axis on every function this week,
 * and then check the relocation line: 2-3 differing with relocations silent is
 * an ordering problem, 14+ with them differing is a CSE problem.
 *
 * The overlay.ld line MUST KEEP its asm/ path. makefile_flags() is empty.
 */
