unsigned long long OvlFunc_common2_41c(unsigned long long v, int shift)
{
    unsigned int lo;
    unsigned int hi;
    unsigned int rlo;
    unsigned int rhi;
    int n;

    if (shift == 0)
        return v;
    lo = (unsigned int)v;
    hi = (unsigned int)(v >> 32);
    n = 32 - shift;
    if (n > 0) {
        rlo = (lo >> shift) | (hi << n);
        rhi = hi >> shift;
    } else {
        rlo = hi >> -n;
        rhi = 0;
    }
    return ((unsigned long long)rhi << 32) | rlo;
}
