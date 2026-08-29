/* OvlFunc_957_2008a54 -- asm/overlays/rom_7e3e08/ovl_30_c_c_a_a_a_c.s
 *
 * BLOCKER: INSTRUCTION SCHEDULING (store vs sign-extension)
 *
 * 3 of 50 differing.
 *
 * The ROM issues the REG_BLDCNT store BETWEEN the ldrb of ewram_2001004[0] and its
 * lsl #24 / asr #24 sign-extension; gcc schedules both shifts first.  The other 47
 * lines, including the add r3,#2 CSE of BLDCNT->BLDALPHA in the fall-through arm,
 * are exact.
 * * MEASURED (all 3 or worse): signed char local; int + explicit <<24 >>24;
 * volatile on the ewram array; non-volatile register macros; a named volatile u16*
 * for BLDCNT; store-before-load and load-before-store source order; initialiser vs
 * assignment order; -fno-rerun-cse-after-loop, -fno-schedule-insns, -fno-peephole,
 * -fno-cse-follow-jumps.  -fno-schedule-insns2 is 38.
 */
