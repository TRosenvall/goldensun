extern unsigned char gFlags[];

void ClearFlag(int flagID) {
    int bit;
    unsigned int t;
    int idx;

    bit = 1 << (flagID & 7);
    t = (unsigned)flagID << 20;
    idx = t >> 23;
    gFlags[idx] &= ~bit;
}
