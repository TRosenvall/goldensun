/* Cluster OvlFunc_common0_18..OvlFunc_927_20089f4 extracted from goldensun/asm/overlays/rom_7b4558/ovl_30_a_a_c_c_c_c.s.
 *
 * Total .text for this TU = 82 bytes (= 0x52).
 * Preserves the original ROM layout when slotted between the _a and _c pieces
 * of the same .s in goldensun/overlays/rom_7b4558/overlay.ld.
 *
 * ONE OF FOUR OPERAND-IDENTICAL COPIES, elevated together from a single source
 * with only the function name changed:
 *
 *      OvlFunc_common0_18   ovl_7b4558      OvlFunc_964_20089f4   ovl_7ed0a0
 *      OvlFunc_946_20089f4   ovl_7ced6c      OvlFunc_965_20089f4   ovl_7ef4f4
 *
 * A __CreateActor wrapper. Two things make it match, and the first was parked
 * as unreachable for a round before it fell:
 *
 *   THE BODY GOES INSIDE `if (p != 0) { ... return p; }` WITH `return 0;`
 *   AFTER IT, not `if (p == 0) return 0;` followed by the body. Both are the
 *   same control flow, but only the first puts the null return's `mov r0, #0`
 *   at the END, where the ROM has it. The early-return form emits it at the
 *   guard and every label after shifts.
 *
 *   This corrects a claim made in src/non_matching/ovl_7cb2c0/20080fc.c and
 *   repeated in two later parks: that basic-block placement is decided after
 *   the source has had its say and cannot be reached from C. What was actually
 *   tested there was a goto spelling of the SAME early-return shape, which
 *   changes nothing. Inverting the guard so the body is the taken branch does
 *   reach it. A result variable with a single exit does NOT -- that lands at 11
 *   of 41.
 *
 *   The four-register argument shuffle: the wrapper takes (a, b, c, d) and
 *   calls __CreateActor(d, a, b, c), which produces the rotation through
 *   r4/r5/r6 at the top.
 *
 * The constant-as-destination spellings are load-bearing: `n = 0xd; n = -n;`
 * gives `mov r3, #0xd / neg r3, r3`, and the read-modify-write on byte +9 of
 * the actor keeps the mask as the destination throughout.
 */
extern unsigned char *__CreateActor(int a, int b, int c, int d);
extern void __Actor_SetSpriteFlags(void *a, int n);
extern void __Func_80929d8(void *a, int n);
extern void __Func_800c548(void *a, int n);

void *OvlFunc_common0_18(int a, int b, int c, int d)
{
    unsigned char *p;
    unsigned char *q;
    unsigned char *r;
    int n;
    int v;
    int z;
    int w;

    p = __CreateActor(d, a, b, c);
    if (p != 0) {
        q = *(unsigned char **)(p + 0x50);
        n = 0xd;
        v = q[9];
        n = -n;
        n &= v;
        r = p;
        q[9] = n;
        r += 0x55;
        z = 0;
        *r = z;
        r += 4;
        w = 8;
        *r = w;
        __Actor_SetSpriteFlags(p, 0);
        __Func_80929d8(p, 0xe);
        __Func_800c548(p, 1);
        return p;
    }
    return 0;
}
