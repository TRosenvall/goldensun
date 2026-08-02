/* Func_912a8 -- ClampFadeDuration
 *
 * r0=duration. Clamps to at most 0x7C00, the longest fade the step arithmetic
 * can represent without overflowing.
 *
 * STATUS: MATCHING.  The register pins are matching aids -- agbcc otherwise
 * picks different registers for the same instruction sequence.
 */
int Func_912a8(int v)
{
    int hi;
    hi = 0xf8 << 7;
    if (v > hi)
        return hi;
    return v;
}
