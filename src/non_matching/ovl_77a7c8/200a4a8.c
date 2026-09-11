/* OvlFunc_881_200a4a8 -- 0x0200a4a8,
 * asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_c_c.s
 *
 * FOURTEEN differing of 270, at EXACT size (704 bytes) and exact encoding
 * count. Candidate: scratch_elev/b258/rich/final/ovl_30_c_a_c_c_a_c_a_a_a_c_c.c.
 * Ladder: 263 -> 165 (35 ROM-order pin blocks) -> 33 (gState tail plus `m = 1`
 * placement) -> 19 -> 14.
 *
 * THE RELOCATION LINE IS ONE OURS-ONLY ENTRY AND NOTHING ELSE --
 * ('000002bc','R_ARM_ABS32','_AREA_02'), the documented phantom. Every other
 * relocation matches by offset and name. That is NOT the residue; the 14
 * encodings are.
 *
 * TWO BLOCKERS IN TENSION, and the floors are separate:
 *   floor 4  -- a cprop rematerialisation needing a value to be
 *               high-register-resident AND opaque at the same time. The
 *               obvious device is `__asm__("r8")`, which is forbidden.
 *   floor 10 -- the gState tail's two halves pull against each other: naming
 *               the value fixes store 1 and triggers `use_related_value` on
 *               store 2.
 *
 * ALIAS IS THE WRONG AXIS: `-fno-schedule-insns2` REGRESSES, 14 -> 49. Eleven
 * other optimisation flags are inert.
 *
 * A MID-FUNCTION POOL DUMP THE ROM LACKS WAS A LENGTH SYMPTOM, NOT A POOL
 * PROBLEM. 132 of the 165 differing at that stage were shifted `ldr [pc]`
 * entries, and fixing a TWELVE-BYTE SHORTFALL ELSEWHERE dissolved all of them.
 * Do not chase a spurious pool directly -- measure the length first.
 *
 * `.L679c` is defined and .global in ovl_30_c_c_c_c_c_c.s, consumed only, so
 * nothing needs exporting. asmfacts.py says WHOLE -- convert directly;
 * makefile_flags() is empty; overlay.ld:52 already names the asm/ path and MUST
 * STAY VERBATIM.
 */
