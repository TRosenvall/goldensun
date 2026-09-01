/* Func_801f730 (0x0801f730) -- NON-MATCHING.
 * Blocker class: a byte test gcc-2.96 will not spell the ROM's way.
 *
 * 33 lines against the ROM's 33, EIGHT differing, and all eight are one
 * region -- how the loop's byte test is materialised, plus the `add r2, #0x40`
 * that the scheduler moves with it.
 *
 *     rom    ldrb r3, [r2] / lsl r3, #24 / add r2, #0x40 / cmp r3, #0 / beq
 *     ours   mov r3, #0 / ldrsb r3, [r2, r3] / cmp r3, #0 / beq
 *                                              ... add r2, #0x40 later
 *
 * Both are two instructions for the load, so the LENGTH is right; the ROM
 * loads UNSIGNED and normalises with a shift, we load SIGN-EXTENDING with the
 * register-offset form Thumb-1 requires for `ldrsb`.
 *
 * WHY THIS IS NOT A SPELLING PROBLEM. A direct probe of gcc-2.96 under this
 * tree's exact flags, ten shapes, none of which produces `ldrb` + `lsl #24`
 * before a `cmp #0`:
 *
 *     p[0] != 0                       unsigned char *   ldrb / cmp
 *     p[0] != 0                       signed char *     mov #0 / ldrsb / cmp
 *     (signed char)p[0] != 0                            ldrb / cmp   (cast folded)
 *     signed char c = p[0]; c != 0                      mov #0 / ldrsb / cmp
 *     signed char f : 8   bitfield                      mov #0 / ldrsb / cmp
 *     unsigned char f : 8 bitfield                      ldrb / cmp
 *     signed char f : 7   bitfield                      ldrb / mov #127 / cmp
 *     int f : 8 / unsigned int f : 8 bitfields          ldrsb / ldrb
 *     volatile unsigned char *                          ldrb / cmp
 *     (p[0] << 24) != 0                                 ldrb / cmp   (fold)
 *
 * The last line is the important one: gcc-2.96 KNOWS `p[0]` is 0..255 and
 * folds `(x << 24) != 0` to `x != 0`, so the shift cannot be asked for
 * directly. The only shape that kept a `lsl #24` was one followed by a
 * VARIABLE shift, which is not what the ROM has. Every narrowing spelling
 * collapses, and every signed spelling reaches for `ldrsb`.
 *
 * So the ROM's sequence is a narrowing this compiler declines to emit for a
 * `!= 0` test. Note also that Thumb's `lsl` sets the flags, so the ROM's
 * `cmp r3, #0` after it is redundant -- the pair reads like a value
 * computation whose result is then tested, not like a compare.
 *
 * WHAT IS RIGHT AND SHOULD BE KEPT (the rest is exact):
 *   - `r = -9` assigned AFTER the Func_80056cc call, which is what gives
 *     `bl / mov r5, #9 / neg r5, r5 / cmp r0, #0` in that order; a temp for
 *     the call result is needed to express it.
 *   - the descending `for (i = 2; i >= 0; i--)` with the pointer advanced by
 *     0x40 inside the body.
 *   - the pooled 0x1071 offset added to the dereferenced global.
 *
 * NEXT: identify what the byte at [iwram_3001f1c + 0x1071 + n*0x40] actually
 * is. If it is a member of a declared struct somewhere in this tree, its type
 * may be the thing that produces the ROM's load; nothing else measured here
 * does.
 */
extern int Func_80056cc(void);
extern int Func_8005c68(void);
extern void Func_8005cf8(void);
extern unsigned char *iwram_3001f1c;

int Func_801f730(int a)
{
    int r;
    int t;
    unsigned char *p;
    int i;
    signed char c;

    t = Func_80056cc();
    r = -9;
    if (t == 0) {
        r = Func_8005c68();
        if (a != 0) {
            p = iwram_3001f1c + 0x1071;
            for (i = 2; i >= 0; i--) {
                c = p[0];
                if (c != 0) {
                    r--;
                }
                p += 0x40;
            }
        }
    }
    Func_8005cf8();
    return r;
}
