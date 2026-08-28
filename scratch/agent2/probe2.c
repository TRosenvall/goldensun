typedef unsigned short u16;
int g1(volatile u16 *rd, u16 *wr, int i) {
    int n = 0;
    for (; i >= 0; i--) { u16 t = *rd; rd++; if (t) { *wr = t; n++; wr++; } }
    return n;
}
int g2(u16 *rd, u16 *wr, int i) {
    int n = 0;
    for (; i >= 0; i--) { int t = *rd; rd++; if ((t & 0xffff) != 0) { *wr = t; n++; wr++; } }
    return n;
}
int g3(u16 *rd, u16 *wr, int i) {
    int n = 0;
    for (; i >= 0; i--) { u16 t; t = *rd; rd++; if (t != 0) { *wr = t; n++; wr++; } }
    return n;
}
struct S { u16 v; };
int g4(struct S *rd, struct S *wr, int i) {
    int n = 0;
    for (; i >= 0; i--) { u16 t = rd->v; rd++; if (t) { wr->v = t; n++; wr++; } }
    return n;
}
