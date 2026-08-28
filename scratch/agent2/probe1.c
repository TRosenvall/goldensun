typedef unsigned short u16;
int f1(u16 *rd, u16 *wr, int i) {
    int n = 0;
    for (; i >= 0; i--) { u16 t = *rd; rd++; if (t) { *wr = t; n++; wr++; } }
    return n;
}
int f2(u16 *rd, u16 *wr, int i) {
    int n = 0;
    for (; i >= 0; i--) { if (*rd != 0) { *wr = *rd; n++; wr++; } rd++; }
    return n;
}
int f3(u16 *rd, u16 *wr, int i) {
    int n = 0;
    for (; i >= 0; i--) { unsigned t = *rd; rd++; if ((t & 0xffff) != 0) { *wr = t; n++; wr++; } }
    return n;
}
int f4(u16 *rd, u16 *wr, int i) {
    int n = 0;
    for (; i >= 0; i--) { int t = *rd; rd++; if ((int)(short)t != 0) { *wr = t; n++; wr++; } }
    return n;
}
