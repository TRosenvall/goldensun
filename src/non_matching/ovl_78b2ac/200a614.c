/* OvlFunc_890_200a614 -- 0x0200a614, asm/overlays/rom_78b2ac/ovl_30_c_c_c_c_a.s
 *
 * TWENTY-ONE differing of 313, at EXACT size (784 bytes), exact encoding count,
 * and RELOCATIONS IDENTICAL -- objcmp prints no relocation line at all.
 * Candidate: scratch_elev/b258/rich/final/ovl_30_c_c_c_c_a.c. Normalised-diff
 * ladder 75 -> 72 -> 52 -> 32 -> 21.
 *
 * BLOCKER: the `mov` order at a shift-built argument site, floor 12, plus three
 * small scheduling/ordering slots.
 *
 * AND THE ORDER THERE IS INERT TO *EVERY* PERMUTATION -- all 48 measured
 * EXACTLY 21, not one encoding moved. So this is not a fill-order problem and
 * another sweep will not find it. Worse, one ordering barrier at that site COST
 * A CALLEE-SAVED REGISTER: r10 dropped and the count went 21 -> 285.
 *
 * A PUSH MASK ONE REGISTER NARROW WITH A SMALL CONSTANT MISSING IS A
 * BIRTH-POINT QUESTION. Hoisting `z = 0` above the block it conflicts with
 * makes the prologue exact, and sched2 then puts the `mov` back where the ROM
 * has it by itself. That is already in the candidate.
 *
 * CROSS-REFERENCE WORTH FOLLOWING: this function's iwram tail is the SAME BLOCK
 * that src/non_matching/ovl_7892c8/200874c.c is parked on, and this file PROVES
 * the two iwram pointers are one object -- the ROM reaches iwram_3001ed0 as
 * `[&iwram_3001ebc + 0x14]`. That is the first half of that park's "build the
 * struct" note, established here rather than assumed.
 *
 * BASENAME HAZARD: this file's basename exists in EIGHT other overlay
 * directories. Match on the full path; funcindex.py resolves it correctly.
 *
 * asmfacts.py says WHOLE -- convert directly; makefile_flags() is empty; all
 * three `.L` symbols are local; overlay.ld:37 already names the asm/ path and
 * MUST STAY VERBATIM.
 */
