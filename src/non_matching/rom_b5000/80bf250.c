/* Func_80bf250 -- asm/rom_b5000/rom_bbb0c_a_c_c_a.s
 *
 * BLOCKER: the QImode ZERO TEST is one instruction wide and neither spelling
 * hits it.  Best 40 differing of 49, ours 50 lines.
 *
 * THREE-MEMBER FAMILY, all in the same .s and all identical in shape:
 *   Func_80bf250, Func_80bf2b4, Func_80bf318
 * Solving one solves three; the C is a search-and-replace on the offsets.
 * (tools/twin_families.py finds them.)
 *
 * WHAT THE FUNCTION DOES: decrement a byte counter at unit+0x132; if it hits
 * zero clear the byte at unit+0x133 and return 1; otherwise, if that byte is
 * negative and a helper accepts, clear both and return 1; else return 0.
 *
 * THREE READINGS CONFIRMED, each worth several instructions:
 *   * `mov r3, r2` after the `ldrb` is the GUARD COPY -- read the field twice,
 *     `if (*p == 0) ...` for the guard and `*p + 0xff` for the body.
 *   * `add r3, #0xff` is NOT `- 1`.  Writing `*p - 1` emits `sub r3, #1`;
 *     writing `*p + 0xff` emits the ROM's `add`.
 *   * The three failure paths share one `mov r0, #0` at a join label.  A plain
 *     early `return 0;` makes gcc hoist the zero above the first branch;
 *     `goto fail;` with `fail: return 0;` at the end keeps it at the join and
 *     is worth one instruction.
 *
 * WHAT IS LEFT: the ROM tests the decremented byte with a SINGLE
 * `lsl r3, #0x18` followed by `cmp r3, #0` -- no second shift, no mask.
 * Nothing reaches exactly that:
 *
 *   signed char n; ... if (n == 0)        40 of 49, ours 50  (lsl + LSR)
 *   unsigned char n; ... if (n == 0)      42 of 49, ours 51  (lsl + lsr)
 *   int n; if ((n & 0xff) == 0)           42 of 49, ours 51
 *   int n; if ((n << 24) == 0)            44 of 49, ours 48  (NO shift at all)
 *   int n; if ((signed char)n == 0)       44 of 49, ours 48
 *   int n; if ((unsigned char)n == 0)     44 of 49, ours 48
 *
 * A narrow VARIABLE is normalised on entry to a register (lsl + lsr, one too
 * many); a narrow CAST used only in a comparison is folded away entirely (one
 * too few).  The ROM sits between the two and no spelling here produces it.
 *
 * Best C: scratch/Fbf250.c.
 */
