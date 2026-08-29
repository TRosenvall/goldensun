/* OvlFunc_942_20087dc -- asm/overlays/rom_7c6bac/ovl_30_c_c_a_c_c_c_c_a.s
 *
 * BLOCKER: ARG INTERLEAVE (straight-line -- basic-block lever unreachable)
 *
 * 2 of 53 differing.
 *
 * __MapActor_SetSpeed(0, 0x8000, 0x4000) in the ENTRY block.
 *   rom  mov r1,#0x80 / mov r2,#0x80 / lsl r2,#7 / mov r0,#0 / lsl r1,#8
 *   ours ... mov r0,#0 emitted after lsl r1,#8
 * The call has no dominating block that is not its own, so the basic-block lever
 * has nothing to bite on.  Same shape as OvlFunc_948_2009df8.
 * * MEASURED: bare literals 2; s1 local at top 54; s2 local at top 53; s1/s2
 * assigned adjacent to the call 2; return-type lever int 2; no prototype 2;
 * -fno-rerun-cse-after-loop 2; -fno-schedule-insns 2; -fno-peephole2 2;
 * -fno-schedule-insns2 10.  Both _AREA_6b/_AREA_70 and the rest are exact.
 */
