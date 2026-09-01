/* Func_80bf400 (0x080bf400) -- NON-MATCHING.
 * Covers the 30-instruction status-counter family: Func_80bf37c (parked
 * separately), Func_80bf400, Func_80bf484. tools/twin_families.py groups all
 * three, and they differ only in the record offset and the Func_80bf208
 * constant, so the body below is the template for all of them.
 *
 * Blocker class: SCRATCH-REGISTER SELECTION plus exit-block layout.
 *
 * 32 lines against the ROM's 32, SEVEN differing:
 *
 *     rom    ldrb r2, [r5] / mov r3, r2       loaded value copied before use
 *     ours   ldrb r3, [r5] / mov r0, #0x0     loaded direct, return-0 hoisted
 *
 * and the two exit blocks emitted in the opposite order -- the ROM lays the
 * `mov r0, #0` exit before the epilogue label, we lay it inline earlier.
 *
 * TWO READINGS LANDED and are worth keeping:
 *
 * 1. THE NARROWING SHIFT. `(unsigned char)t == 0` on an `unsigned int t`
 *    produces the ROM's `lsl r3, #24` before the compare. This is the case
 *    docs/elevation.md's CORRECTION covers: the value is `v + 0xff`, so its
 *    range exceeds a byte and gcc cannot fold the narrowing away.
 *
 * 2. BRANCH POLARITY at the final test. `if (f(...) == 0) return 0;` followed
 *    by the success block is 11 differing; writing the success arm as the `if`
 *    body -- `if (f(...) != 0) { *p = 0; return 1; } return 0;` -- is 7. Fourth
 *    function closed or improved by that reading.
 *
 * MEASURED and inert, all 32 lines and 7 differing:
 *   `v = *p; t = v;` -- the copy-then-modify spelling the ROM's `ldrb r2 /
 *     mov r3, r2` reads as. Byte-identical to not writing it, exactly as on
 *     Func_80bf2b4, where the same spelling was also inert.
 * And worse: wrapping the body in `if (t != 0) { ... }` with a single trailing
 *   `return 0` -- 31 lines, 26 differing.
 *
 * NEXT: nothing source-level. Three functions close together off this file if
 * the scratch-register class is ever cracked; see
 * src/non_matching/rom_b5000/80bf2b4.c for the 46-instruction variant of the
 * same family and the same wall.
 */
extern unsigned char *_GetUnit(int id);
extern int Func_80bf208(int a, int b, int c);

int Func_80bf400(int id)
{
    unsigned char *u;
    unsigned char *p;
    unsigned int t;

    u = _GetUnit(id);
    p = u + (0x9d << 1);
    t = *p;
    if (t == 0)
        return 0;
    t = t + 0xff;
    *p = t;
    if ((unsigned char)t == 0)
        return 1;
    if (Func_80bf208(id, *p, 0x46) != 0) {
        *p = 0;
        return 1;
    }
    return 0;
}
