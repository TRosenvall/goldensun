/* OvlFunc_971_2008f8c -- 0x02008f8c,
 * asm/overlays/rom_7fb4a8/ovl_30_c_a_a_a_a.s
 *
 * 71 vs 75 lines, 45 differing.  Candidate at scratch/L8f8c.c.
 *
 * SOLVED, and this is a THIRD instance of the symbol-base lever: the ROM holds
 * the default message id 0x294e in r5 across three calls and every branch,
 * reaching the final `__MessageID(m + slot - 1)` with it.  Written as
 * `int m = 0x294e;` gcc sinks the load into the branches that need it and never
 * spends a callee-saved register -- 63 differing.  `extern int _MSG_294e;` with
 * `m = (int)(&_MSG_294e);` puts `ldr r5, =...` in the ROM's position: 63 -> 50.
 * (The symbol is NOT yet in message.sym; the screen ran with it unresolved.)
 *
 * Naming the two `0xbc << 2` builds separately took 50 -> 45; ours had CSE'd
 * them into one register where the ROM builds the flag id twice.
 *
 * BLOCKER: ours is FOUR instructions short and the parameter/result registers
 * are permuted -- the ROM puts the slot parameter in r6 and the second
 * OvlFunc_971_2008f30 result in r7, ours the reverse, and that propagates.
 * Being short by four suggests something structural is still missing rather
 * than a pure allocation difference, so this is not a one-lever fix.
 *
 * Note the ROM calls `__GetFlag(0xbc << 2)` and DISCARDS the result (r0 is
 * overwritten by the following `add` before the next call).  That is
 * transcribed as a bare call statement and is not the problem.
 */
