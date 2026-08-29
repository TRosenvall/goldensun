/* Func_f3898 -- ClampComponent
 *
 * r0 = a colour component. Clamps to 0..0x1F -- the five bits the GBA gives each
 * channel.
 *
 * STATUS: MATCHING.
 */
int Func_f3898(int v)
{
    if (v > 31)
        return 31;
    if (v < 0)
        return 0;
    return v;
}
