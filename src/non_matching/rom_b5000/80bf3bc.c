/* Func_80bf3bc  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/rom_b5000/rom_bbb0c_a_c_c_a.s
 * Best screen: 2 instructions in disagreeing regions, of 31 (rom 31, ours 30).
 *
 * BLOCKER CLASS: an elided copy, and nothing else.
 *
 *      rom   ldrb r2, [r5, #0x0] / mov r3, r2 / cmp r3, #0x0
 *      ours  ldrb r3, [r5, #0x0] /             cmp r3, #0x0
 *
 * THIS FUNCTION IS THE DISPROOF OF A CLAIM THIS PROJECT MADE, which is why it
 * is worth its own park rather than a line in a sibling's note.
 *
 * src/non_matching/rom_b5000/80bf54c.c used to explain the same missing copy as
 * a register-pressure effect: its sibling Func_80bf574 supposedly DID emit the
 * copy because a second store kept more values live. That was asserted without
 * screening it. Screening Func_80bf574 shows it emits `ldrb r3` with no copy,
 * exactly like the other two.
 *
 * Func_80bf3bc settles it from the other direction. It has strictly MORE
 * pressure than either -- a parameter held in r6 across a three-argument call,
 * plus a callee-saved pointer in r5 -- and it elides the copy just the same. All
 * three siblings elide it regardless of pressure, so for this shape the copy is
 * a plain codegen difference. HANDOFF.md has been corrected.
 *
 * WHAT DID WORK, and took this from 8 of 31 to 2:
 *
 *   PLAIN `return 1;` ON EACH PATH, not a result variable. Written as
 *   `r = 1; ... goto out; ... out: return r;`, gcc sees one value live across
 *   the call and parks it in a callee-saved register, `mov r7, #0x1`. The ROM
 *   re-materialises `mov r0, #0x1` separately on each path and lets the two
 *   flow into a shared epilogue. Two `return 1;` statements produce exactly
 *   that -- gcc merges the epilogues on its own, which is the part the result
 *   variable was trying to do by hand and doing worse.
 *
 * The rest of the shape follows the siblings: `t = t + 0xff` rather than
 * `t - 1`, and `t = t << 24` as its own statement with no following `lsr`.
 */
extern unsigned char *_GetUnit(void);
extern int Func_80bf208(int a, int b, int c);

int Func_80bf3bc(int who)
{
    unsigned char *p;
    unsigned char *q;
    unsigned int off;
    int w;
    int v;
    int t;
    int n;
    int z;

    w = who;
    p = _GetUnit();
    off = 0x139;
    q = p + off;
    v = *q;
    t = v;
    if (t == 0)
        goto zero;
    t = t + 0xff;
    *q = t;
    t = t << 24;
    if (t == 0)
        return 1;
    n = *q;
    if (Func_80bf208(w, n, 0x3c) == 0)
        goto zero;
    z = 0;
    *q = z;
    return 1;
zero:
    return 0;
}
