/* IncFlagByte  [rom_77000]
 * Source asm: goldensun/asm/rom_77000/rom_79338_c_a.s
 * (path updated: the .s was split or renamed after this was parked)
 *
 * Parked: logic faithful, does NOT byte-match (endgame permuter seed).
 * Candidate: tools/runs/run_20260607T010203Z/IncFlagByte-iter-7.c
 * TODO(residual): rom_79xxx flag-array family; ((unsigned)x<<20)>>23 logical shift correct, reg-alloc/scheduling diverges (siblings 8079358/74/418 same wall). Permuter.

 * TRIED AND FAILED (batch 29): splitting the shift into two statements, so the
 * intermediate has its own name --
 *
 *     t = (unsigned int)flagID << 20;  idx = t >> 23;
 *
 * -- to reproduce the ROM's non-destructive `lsl r3, r0, #0x14`. gcc still
 * writes into r0 and folds to the two-operand form, because flagID is dead
 * after the shift and there is nothing to keep it alive. Byte-identical to the
 * single-expression form below. The family wall stands.
 */