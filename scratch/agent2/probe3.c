typedef unsigned short u16;
struct A { short q:16; };
int h1(struct A *rd, struct A *wr, int i) {
    int n = 0;
    for (; i >= 0; i--) { int t = rd->q; rd++; if (t) { wr->q = t; n++; wr++; } }
    return n;
}
struct B { unsigned short q:16; };
int h2(struct B *rd, struct B *wr, int i) {
    int n = 0;
    for (; i >= 0; i--) { int t = rd->q; rd++; if (t) { wr->q = t; n++; wr++; } }
    return n;
}
int h3(u16 *rd, u16 *wr, int i) {
    int n = 0;
    for (; i >= 0; i--) { int t = *rd; rd++; if ((short)t) { *wr = t; n++; wr++; } }
    return n;
}
int h4(short *rd, u16 *wr, int i) {
    int n = 0;
    for (; i >= 0; i--) { u16 t = *rd; rd++; if (t) { *wr = t; n++; wr++; } }
    return n;
}
