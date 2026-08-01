/* Func_91294 -- ClampFadeLevel
 *
 * r0=level. Clamps to the 0..0x1F range the hardware blend registers accept.
 *
 * STATUS: MATCHING.
 */
int Func_91294(int v)
{
    if (v > 31)
        return 31;
    if (v < 0)
        return 0;
    return v;
}
