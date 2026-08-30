union U {
    unsigned long long q;
    struct { unsigned int lo; unsigned int hi; } h;
};

unsigned long long OvlFunc_common2_41c(unsigned int lo, unsigned int hi, int shift)
{
    union U u;
    int n;

    if (shift == 0)
        return ((unsigned long long)hi << 32) | lo;
    u.h.lo = lo; u.h.hi = hi;
    n = 32 - shift;
    if (n > 0) {
        u.h.lo = (u.h.lo >> shift) | (u.h.hi << n);
        u.h.hi = u.h.hi >> shift;
    } else {
        u.h.lo = u.h.hi >> -n;
        u.h.hi = 0;
    }
    return u.q;
}
