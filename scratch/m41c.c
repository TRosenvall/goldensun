typedef union {
    unsigned long long q;
    struct { unsigned int lo; unsigned int hi; } w;
} U64;

unsigned long long OvlFunc_common2_41c(unsigned long long v, int n)
{
    U64 u;
    U64 r;
    int s;

    if (n == 0)
        return v;
    u.q = v;
    s = 32 - n;
    if (s > 0) {
        r.w.lo = (u.w.lo >> n) | (u.w.hi << s);
        r.w.hi = u.w.hi >> n;
    } else {
        r.w.lo = u.w.hi >> -s;
        r.w.hi = 0;
    }
    return r.q;
}
