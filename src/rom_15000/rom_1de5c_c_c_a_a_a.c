/* DrawSmallText  --  0x0801e74c
 *
 * The whole of goldensun/asm/rom_15000/rom_1de5c_c_c_a_a_a.s: one function, no
 * data outside its own inline pool, no split. The .s comment block calls it
 * DrawStringAt; the exported name is DrawSmallText.
 *
 * Formats a string id into the shared text buffer and hands the buffer to the
 * glyph renderer.
 *
 * MATCHED ON THE FIRST CANDIDATE, and the whole of it came from the sibling
 * split out of the same parent: Func_801e7c0 is the SAME ROUTINE up to its last
 * statement. Nine lines of already-byte-matched C transplanted unchanged,
 * carrying three levers that would each otherwise have been a sweep -- the named
 * zero that survives the call in a callee-saved register, the accumulator-first
 * offset build that keeps the base constant in one register for reuse, and the
 * named pointer read three times because the ROM reloads through it after the
 * call.
 *
 * THE STRONGEST NEIGHBOUR SIGNAL IS A SIBLING THAT SHARES A PREFIX OF ITS BODY,
 * AND THE SCORE CANNOT SEE IT. tools/neighbour.py returned that file at 2 of 3,
 * in a FOUR-WAY TIE, and not ranked first. Nothing in the ranking flagged that
 * one of the four was a body-prefix twin. So when several files tie, read all of
 * them: the tie-break is body shape, which a shared-symbol count cannot measure.
 *
 * GREPPING THE TARGET'S OWN CALLERS SETTLED THE SIGNATURE OUTRIGHT. Four solved
 * files already declare this function, all agreeing on the argument order and
 * all ignoring the result, so the `void` return needed no oracle and no
 * return-type sweep. That is the second consecutive round where a caller-grep
 * eliminated the declaration lever entirely; it is worth doing FIRST whenever
 * the target is exported.
 *
 * TWO SCREEN ARTEFACTS, BOTH BENIGN, BOTH VERIFIED AGAINST THE ROM BYTES. The
 * branch-over-pool came out spontaneously at the ROM's exact address with no
 * source construct, driven by the narrow first pool entry's short range. And the
 * halfword pool load against the ROM's word load is the known Thumb-1 false
 * positive -- both assemble to the same two bytes. Confirmed in the byte dump,
 * including that the three pool words come out in the ROM's order.
 */
extern unsigned char *iwram_3001e8c;
extern void BufferString(int id, int n);
extern void Func_8017aa4(void *dst, void *w, int x, int y);

void DrawSmallText(int id, void *w, int x, int y)
{
    unsigned char *g;
    unsigned short *c;
    int z;
    int o;

    g = iwram_3001e8c;
    c = (unsigned short *)(g + 0x12b2);
    z = 0;
    *c = z;
    BufferString(id, 1);
    o = *c << 1;
    o += 0xeb0;
    *(unsigned short *)(g + o) = z;
    *c = (*c + 1) & 0x1ff;
    Func_8017aa4(g + 0xeb0, w, x, y);
}
