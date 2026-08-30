union U {
    unsigned long long q;
    struct { unsigned int lo; unsigned int hi; } h;
};
unsigned long long OvlFunc_common2_41c(unsigned long long v, int shift)
{
    union U u;
    int n;
    if (shift == 0)
        return v;
    u.q = v;
    n = 32 - shift;
    if (n > 0)
        goto wide;
    u.h.lo = u.h.hi >> -n;
    u.h.hi = 0;
    goto join;
wide:
    u.h.lo = (u.h.lo >> shift) | (u.h.hi << n);
    u.h.hi = u.h.hi >> shift;
join:
    return u.q;
}
