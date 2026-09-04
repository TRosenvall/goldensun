/* Func_8010560 -- 0x08010560, asm/rom_9000/rom_10424_a.s
 *
 * Runs a map-tile animation script: a table of 10-byte records, each an
 * unsigned short id followed by four shorts, walked until the id reads 0xffff.
 * Every record calls CopyMapTiles with the id, the first field, the caller's
 * two coordinates and the next two fields, then waits the fourth field's worth
 * of frames.
 *
 * 47 of 55, and OURS IS 47 INSTRUCTIONS AGAINST THE ROM'S 55 -- the candidate
 * is not wrong so much as eight instructions SHORTER, and every spelling that
 * closes part of the gap opens another.
 *
 * BLOCKER: gcc WILL NOT ISSUE `ldrsh` + ZERO-EXTEND FOR ALL FOUR FIELDS. The
 * ROM reads each of the four shorts as
 *
 *     mov   r4, #N
 *     ldrsh rX, [r6, r4]      @ signed load; ldrsh has no immediate form
 *     ...
 *     lsl   rX, #16
 *     lsr   rX, #16           @ then narrowed back to unsigned 16 bits
 *
 * -- four instructions per field, sixteen in total, which is most of the eight
 * instructions we are short. Net, that sequence is just an unsigned halfword
 * load, and gcc knows it: it folds the pair into a single `ldrh rX, [r6, #N]`,
 * which also drops the offset `mov` because ldrh DOES have an immediate form.
 * The two-instruction saving per field is exactly the gap.
 *
 * MEASURED, and the asymmetry is the interesting part: gcc folds SOME of the
 * four and not others -- with named `short` locals it emitted ldrsh for the
 * fields at +4 and +6 and ldrh for those at +0 and +2, in the same loop, from
 * identical source. So this is not a clean "signed value used unsigned" rule
 * that a cast could steer; it is a per-load decision made after CSE, and the
 * source has no handle on it.
 *
 * TRIED -- five spellings, three of them tying at EXACTLY 47:
 *   a  struct Cmd with (unsigned short) casts at the call        35i, 53 diff
 *   b  two pointers, named `end`, casts at the call              37i, 53 diff
 *   c  struct + named `v`/`end`, rotated while                   33i, 54 diff
 *   d  struct + four named `short` locals                        42i, 53 diff
 *   e  two pointers + four named `short` locals                  47i, 47 diff
 *   f  e, plus four `unsigned short` locals for the conversion   47i, 47 diff
 *   g  two pointers + four `int` locals holding the casts        41i, 47 diff
 *   h  callee declared with `unsigned short` parameters          37i, 53 diff
 *   i  callees left undeclared entirely                          41i, 53 diff
 * e, f and g tie exactly, and they differ in how the conversion is spelled,
 * which is this notebook's own signal that the lever is not in the spelling.
 * h and i are worse, so the conversion does not live at the call boundary
 * either.
 *
 * WHAT WAS REPRODUCED, and should be kept if this is revisited:
 *   - THE SENTINEL IS A NAMED LOCAL. The ROM materialises 0xffff into r9, a
 *     callee-saved register, and holds it across both calls. Writing
 *     `end = 0xffff;` and comparing against it produces that; comparing
 *     against the literal reloads it.
 *   - TWO INDUCTION POINTERS. The ROM advances r7 (the id) and r6 (the fields,
 *     = id + 2) independently, both by 10. A single struct pointer gives one
 *     induction variable and a shorter loop; a separate `short *q = (short *)(p
 *     + 1)` advanced alongside is what produces the pair.
 *   - THE FOUR LOADS ARE EAGER. All four happen before any argument setup, so
 *     they are named locals rather than expressions at the call -- the
 *     eager-issue face of the named-local rule. This is what took 53 to 47.
 *
 * The residue also carries a three-way rotation of the callee-saved registers
 * (ROM r10=x, r8=y, r9=sentinel; ours r9=x, r10=y, r8=sentinel), which is
 * downstream of the load decision rather than independent of it -- the register
 * assignment cannot be expected to settle while the loop body is still two
 * instructions per field short.
 *
 * NOT tried, and the one thing left worth a screen: whether any declaration of
 * the record's fields as `unsigned short` with an explicit signed intermediate
 * -- rather than the reverse, which is what a..i all do -- reaches the ldrsh.
 */

extern void CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void WaitFrames(int n);

void Func_8010560(unsigned short *p, int x, int y)
{
    short *q;
    unsigned int v;
    unsigned int end;
    short a, b, c, d;

    v = *p;
    end = 0xffff;
    if (v != end) {
        q = (short *)(p + 1);
        do {
            a = q[0];
            b = q[1];
            c = q[2];
            d = q[3];
            CopyMapTiles(v, (unsigned short)a, x, y,
                         (unsigned short)b, (unsigned short)c);
            WaitFrames((unsigned short)d);
            p += 5;
            v = *p;
            q += 5;
        } while (v != end);
    }
}
