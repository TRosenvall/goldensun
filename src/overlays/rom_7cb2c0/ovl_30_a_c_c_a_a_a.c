/* Cluster OvlFunc_945_20080fc..OvlFunc_945_20080fc extracted from goldensun/asm/overlays/rom_7cb2c0/ovl_30_a_c_c_a_a_a.s.
 *
 * MATCHES.  0 differing encodings of 24; whole-file clean, 48 bytes, and no
 * relocations.  PINS: 0 (no inline asm, no shim, no per-file flag).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/overlays/rom_7cb2c0/ovl_30_a_c_c_a_a_a.c asm/overlays/rom_7cb2c0/ovl_30_a_c_c_a_a_a.s --whole
 *
 * SPLIT SHAPE: none.  datacheck.py prints nothing for the reference and the .s
 * holds exactly one .thumb_func_start, so the file is already one TU.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7cb2c0/ovl_30_a_c_b.o and
 * asm/overlays/rom_7cb2c0/ovl_30_a_c_c_a_a_b.o in
 * goldensun/overlays/rom_7cb2c0/overlay.ld.
 *
 * WHAT IT DOES.  A countdown at +0x4c gates three checks.  While the counter is
 * still running it is decremented and the function reports ready only when all
 * three of the move targets at +0x38, +0x3c and +0x40 are still the idle
 * sentinel; when the counter has reached zero it reports ready unconditionally.
 * The three comparisons are chained -- each tests against the value the
 * PREVIOUS one loaded -- which is what produces `cmp r2, r3` and `cmp r3, r2`
 * rather than three compares against the constant.  That part of the park was
 * right and is kept.
 *
 * ============ THE PARK'S FIGURE WAS A MISALIGNMENT, AND IT TOOK TWO ============
 *
 * The superseded claim was nineteen of twenty-four, and objcmp now says in its
 * own output why that was not a distance: ref twenty-four instructions against
 * ours twenty-three, with the sizes equal because our shorter body picks up a
 * two-byte pad.  Read through tools/aligncmp.py the real residue was fifteen
 * insert/delete/replace in seven hunks, aligned-equal fifteen of twenty-four.
 *
 * The park had tried, and recorded as inert, exactly two things: a plain early
 * `return 1;`, and the same early return spelled with gotos.  It then recorded
 * a CORRECTION saying that inverting the guard -- body inside the `if`, the
 * short return after -- "does reach it" in general but "does not rescue THIS
 * function ... still 19 of 28".  Both halves of that are now resolved:
 *
 *   * the guard inversion IS half the answer, but as an `if`/`else` whose ELSE
 *     holds `return 1;`, which is what puts the one-instruction return block
 *     BETWEEN the decrement and the comparison chain with a `b` over it --
 *     the ROM's `beq .L10a / sub / str / b .L10e / .L10a: mov r0,#1 / b .L128`;
 *   * on its own it is worth NOTHING.  Measured: if/else alone still reads the
 *     same nineteen-of-twenty-four misalignment, and the tail change alone
 *     reads twenty-one.  Together: zero.
 *
 * The tail change is the half nobody had varied.  The park's body ended
 *     if (y != x) return 0;
 *     return 1;
 * and gcc then cross-jumped the two `mov r0, #1` blocks -- the counter guard's
 * and the final one -- into a single tail, which is why its `beq` at index 3
 * jumped all the way to the end of the function.  Cross-jumping needs TWO
 * matching instructions before a shared jump (jump.c:675, :1602, :1607) and is
 * unconditional at -O1 and above (toplev.c:3515), so no flag reaches it; the
 * way to stop it is to make the two blocks not end at the same place.  Writing
 * the chain as nested POSITIVE tests with one trailing `return 0`
 *     if (w == k) { if (x == w) { if (y == x) return 1; } }
 *     return 0;
 * does that: the final `mov r0, #1` is now followed by the compare rather than
 * by a jump to the epilogue, so there is nothing to merge, and the two earlier
 * failures share the single `mov r0, #0` block at the end instead.  That is the
 * ROM's `ldr r3,[r0,#0x40] / mov r0,#1 / cmp r3,r2 / beq .L128 / .L126: mov
 * r0,#0`.
 *
 * MEASURED WORSE, both longer than the reference, so both say the result is NOT
 * computed into a variable: a `r = 1; if (y != x) r = 0; return r;` tail is 26
 * instructions of 24, and `return y == x;` is 28 of 24.  The ROM's "set one,
 * then conditionally clear" shape is a CONSEQUENCE of the nested-if spelling,
 * not something to write out.
 *
 * The epilogue was already right in the park and is worth restating: `pop {r1}
 * / bx r1` rather than `pop {r0} / bx r0` is thumb_exit (arm.c:8290-8320)
 * choosing from ARG_REGISTER(2)|ARG_REGISTER(3) because the return mode's size
 * is four, not from the VOIDmode set that also offers r0.
 */
int OvlFunc_945_20080fc(unsigned char *a)
{
    int v;
    int k;
    int w;
    int x;
    int y;

    v = *(int *)(a + 0x4c);
    if (v != 0) {
        v = v - 1;
        *(int *)(a + 0x4c) = v;
    } else {
        return 1;
    }
    k = 0x80 << 24;
    w = *(int *)(a + 0x38);
    if (w == k) {
        x = *(int *)(a + 0x3c);
        if (x == w) {
            y = *(int *)(a + 0x40);
            if (y == x)
                return 1;
        }
    }
    return 0;
}
