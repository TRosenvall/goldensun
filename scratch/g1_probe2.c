extern int g(int);
/* p1: early return + goto loop early return */
int p1(unsigned int a, int *t) {
    int i;
    if (a > 0x5f) return -1;
    i = 0;
loop:
    if (i >= 0x200) return -1;
    if (t[i] == 0xff) return i << 6;
    i += g(i);
    goto loop;
}
/* p2: else form for the first */
int p2(unsigned int a, int *t) {
    int i;
    if (a <= 0x5f) {
        i = 0;
    loop:
        if (i >= 0x200) return -1;
        if (t[i] == 0xff) return i << 6;
        i += g(i);
        goto loop;
    } else {
        return -1;
    }
}
/* p3: first exit returns -1 via a distinct path with a volatile barrier */
int p3(unsigned int a, int *t) {
    int i;
    int r = -1;
    if (a > 0x5f) return r;
    i = 0;
loop:
    if (i >= 0x200) return -1;
    if (t[i] == 0xff) return i << 6;
    i += g(i);
    goto loop;
}
