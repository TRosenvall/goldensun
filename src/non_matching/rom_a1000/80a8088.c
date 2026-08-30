/* Func_80a8088 -- 0x080a8088, asm/rom_a1000/rom_a7380.s
 *
 * 65 of 65 lines, THREE differing, and they are one instruction moved:
 *
 *      rom   add r2, #0xa4 / mov r3, #0xf0 / strb r3, [r0, #0xf]
 *      ours  mov r3, #0xf0 / strb r3, [r0, #0xf] / add r2, #0xa4
 *
 * The ROM derives the second field offset (0x17c + 0xa4 = 0x220) BEFORE the
 * byte store; we derive it after.  Everything else, including the derivation
 * itself, is identical.  Candidate at scratch/Na088_best.c.
 *
 * SOLVED, and it sharpens the batch-152 pointer-local rule rather than just
 * applying it.  The store `*(void **)(p + o) = r` with a NAMED OFFSET gives the
 * register-offset form `str r0, [r6, r3]` where the ROM computes the address --
 * 33 differing.  On CutsceneStart a pointer temporary fixed exactly this.  Here
 * it does NOT: gcc folds `w = (void **)(p + o); *w = r;` straight back into the
 * register-offset store, because nothing forces the address to exist as a value.
 *
 * What worked instead was DROPPING THE SHARED OFFSET VARIABLE and writing both
 * offsets as plain constants.  gcc then computes the first address with an
 * `add`, and still DERIVES the second from it (`add r2, #0xa4`) of its own
 * accord.  33 differing -> 3, line count exact.
 *
 * So the rule is narrower than CutsceneStart suggested: a pointer local forces
 * the `add` only when something else keeps the offset alive -- there, the offset
 * was mutated between the address and the store.  With nothing to keep it, drop
 * the offset variable entirely and let the constants speak.
 *
 * SCREENED, none better than 3: the pointer temporaries with the shared offset
 * (33); the offset mutated between the address and the store (64 lines, 29);
 * the second address computed BEFORE the byte store, which is where the ROM
 * derives it (66 lines, 30 -- it hoists more than the derivation); and the
 * second address written as `(unsigned char *)w + 0xa4`, expressing the ROM's
 * derivation directly (3, byte-identical to the constants form).
 *
 * The last of those is the useful negative: gcc reaches the same code from
 * "derive from the first pointer" and "write the second constant", so that
 * choice is not the handle on the ordering.
 */
