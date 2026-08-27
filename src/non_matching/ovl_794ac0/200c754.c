/* OvlFunc_899_200c754 -- asm/overlays/rom_794ac0/ovl_30_c_c_c_c_c_c_a_b_a.s
 *
 * BLOCKER: CSE OF TWO DIFFERENTLY-SIGNED READS OF ONE HALFWORD
 *
 * 37 of 53 differing.
 *
 * 53/53 lines, structurally right, two residues:
 *   * the ROM emits a separate `ldrh r2,[r6]` for the unsigned read and
 *     `ldrsh r1,[r6,r3]` for the signed read of the SAME halfword; gcc CSEs them
 *     and derives one from the other with lsl/lsr.
 *   * the ROM computes the asr extract only INSIDE the if; we compute it eagerly.
 * * MEASURED: 5 spellings of the (short)/(unsigned short) extract pair; a separate
 * unsigned short* alias for the base read; unsigned short vs unsigned int for the
 * base; -fno-schedule-insns2, -O1, -fno-rerun-cse-after-loop, -fno-strict-aliasing,
 * -fno-gcse, -fno-cse-follow-jumps.
 */
