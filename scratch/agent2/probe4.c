typedef unsigned short u16;
int k1(short *rd, u16 *wr, int i) {
    int n = 0;
    for (; i >= 0; i--) { int t = *rd; rd++; if (t) { *wr = t; n++; wr++; } }
    return n;
}
int k2(short *rd, short *wr, int i) {
    int n = 0;
    for (; i >= 0; i--) { short t = *rd; rd++; if (t) { *wr = t; n++; wr++; } }
    return n;
}
int k3(u16 *rd, u16 *wr, int i) {
    int n = 0;
    for (; i >= 0; i--) { int t = *rd; rd++; if ((short)t != 0) { *wr = t; n++; wr++; } }
    return n;
}
int k4(short *rd, u16 *wr, int i) {
    int n = 0;
    for (; i >= 0; i--) { int t; t = *rd; if (t != 0) { *wr = t; n++; wr++; } rd++; }
    return n;
}
