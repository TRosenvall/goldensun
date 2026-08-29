/* Func_f38ac -- ClampPackedColour
 *
 * r0 = a packed colour. Clamps at 0x7C00, which is red 0x1F with green and blue
 * zero -- the largest value the packing can produce before it would carry into
 * the unused bit 15.
 *
 * STATUS: MATCHING.  The register pins are matching aids -- agbcc otherwise
 * picks different registers for the same instruction sequence.
 */
int Func_f38ac(int v)
{
    int hi;
    hi = 0xf8 << 7;
    if (v > hi)
        return hi;
    return v;
}
