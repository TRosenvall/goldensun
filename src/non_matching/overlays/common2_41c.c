/* OvlFunc_common2_41c -- asm/overlays/common/common2_c_c_c_c_c_c_c_a.s
 *
 * BLOCKER: UNKNOWN -- the C shape is probably wrong, not the flags
 *
 * 22 of 28 differing against a 27-line ROM, so ours is a line long too.
 *
 * The function is a 64-bit logical right shift: r0 = lo, r1 = hi, r2 = count,
 * returning r0/r1.  `if (count == 0) return v;` early out, then the usual
 * two-arm split on `32 - count`.  gcc will not inline `v >> n` for a variable
 * n (it calls __lshrdi3), so the source must do the word arithmetic by hand;
 * my union-of-two-u32 spelling is in scratch/m41c.c and is only a first guess.
 *
 * MEASURED:
 *   union { unsigned long long q; struct { u32 lo, hi; } w; }   22
 *   ... the same with --cflags "-fcall-saved-r4"                22
 *
 * The flag result is worth recording precisely because it is a NEGATIVE for
 * the batch-116 `push {r4` hypothesis -- this ref pushes {r4, r5, r6, lr},
 * which is the signature, and the flag changed nothing.  That is NOT yet
 * evidence against the hypothesis, because with the C shape this far off the
 * flag has nothing to bite on; it means this function cannot serve as a test
 * of it.  Test the hypothesis on OvlFunc_common2_380, which is 12 of 52 with
 * every differing line an r4/r5 rename, not here.
 */
