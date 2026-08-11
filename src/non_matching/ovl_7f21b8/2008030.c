/* OvlFunc_967_2008030  [ovl_7f21b8]
 * Source asm: goldensun/asm/overlays/rom_7f21b8/ovl_30_a.s
 *
 * Parked: logic faithful, does NOT byte-match (endgame permuter seed).
 * Candidate: tools/runs/run_20260607T010203Z/OvlFunc_967_2008030-iter-2.c
 * TODO(residual): reg-alloc/scheduling divergence (register swap / op-order); logic correct. Permuter seed.

 * CONFIRMED ARG-INTERLEAVE (batch 29). The ROM puts `mov r0, #0xe` between
 * `mov r1, #0x81` and its `lsl r1, #1`. Both declaration directions were tried
 * -- __MapActor_Surprise declared and left implicit -- and both give the same
 * two differing positions. The park's "reg-alloc/scheduling divergence" is
 * right in substance; the class has a name now and is
 * src/non_matching/ovl_794ac0/2008428.c.
 */