union U {
    unsigned long long q;
    struct { unsigned int lo; unsigned int hi; } h;
};

unsigned long long OvlFunc_common2_41c(unsigned long long v, int shift)
{
    union U u;
    unsigned int rlo;
    unsigned int rhi;
    int n;

    if (shift == 0)
        return v;
    u.q = v;
    n = 32 - shift;
    if (n > 0)
        goto wide;
    rlo = u.h.hi >> -n;
    rhi = 0;
    goto join;
wide:
    rlo = (u.h.lo >> shift) | (u.h.hi << n);
    rhi = u.h.hi >> shift;
join:
    u.h.lo = rlo;
    u.h.hi = rhi;
    return u.q;
}
