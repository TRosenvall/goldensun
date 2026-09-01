/* Func_80bf2b4 (0x080bf2b4) -- NON-MATCHING.
 * Blocker class: SCRATCH-REGISTER SELECTION (see docs/elevation.md).
 * 50 lines against the ROM's 49, 29 differing.
 *
 * BUT THE NARROWING SHIFT LANDED, and that is the point of this file.
 *
 * Batch 172 recorded, from Func_801f730 and Func_80788c4, that "any narrowing
 * shift placed before a zero test is unreachable, because constant-range
 * folding removes it". That generalisation is TOO BROAD and this function is
 * the counter-example. Its ROM has
 *
 *     add r3, #0xff / strb r3, [r5] / lsl r3, #0x18 / cmp r3, #0
 *
 * and `(unsigned char)t == 0` on an `unsigned int t` produces the `lsl #0x18`
 * exactly, on the first screen.
 *
 * The difference is where the value comes from. In 801f730 and 80788c4 the
 * tested value is a freshly LOADED byte or halfword, so gcc knows its range is
 * 0..255 (or 0..65535) and folds the shift away. Here it is `v + 0xff` with v a
 * loaded byte, so the range is 0xff..0x1fe -- wider than a byte -- and the
 * narrowing is REAL: `(unsigned char)t == 0` is genuinely not `t == 0`, so gcc
 * must emit it.
 *
 * The corrected rule is in docs/elevation.md. Recognising which case you have
 * costs nothing: look at what produced the value, not at the shift.
 *
 * WHAT IS LEFT is the recorded scratch-register wall -- r1/r2 carrying the base
 * and the offset the other way round, and the shared `mov r0, #0` hoisted to
 * the top where the ROM emits it at the joined exit. The copy-then-modify
 * spelling for the load (`v = *p; t = v;`, which the ROM's `ldrb r2 / mov r3,
 * r2` reads as) is byte-identical to not writing it: 50 lines, 29 differing
 * either way, gcc coalescing the copy.
 *
 * WORTH KNOWING: this is one of a THREE-MEMBER family (Func_80bf250,
 * Func_80bf2b4, Func_80bf318 -- 46 instructions, identical opcode stream) found
 * by tools/twin_families.py, and Func_80bf250 is already parked separately. If
 * the scratch-register class is ever cracked, all three come at once, and the
 * body below is the template.
 */
extern unsigned char *_GetUnit(int id);
extern int Func_80bf208(int a, int b, int c);

int Func_80bf2b4(int id)
{
    unsigned char *u;
    unsigned char *p;
    signed char *q;
    unsigned int t;
    int z;

    u = _GetUnit(id);
    p = u + (0x9a << 1);
    t = *p;
    if (t == 0)
        return 0;
    t = t + 0xff;
    *p = t;
    z = 0;
    if ((unsigned char)t == 0) {
        u[0x135] = z;
        return 1;
    }
    q = (signed char *)(u + 0x135);
    if (*q >= 0)
        return 0;
    if (Func_80bf208(id, *p, 0x14) == 0)
        return 0;
    *q = z;
    *p = z;
    return 1;
}
