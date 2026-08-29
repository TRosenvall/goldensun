extern unsigned char gFlags[];

void ClearFlag(int flagID) {
    unsigned char *p;
    int bit;

    bit = 1 << (flagID & 7);
    p = &gFlags[((unsigned)flagID << 20) >> 23];
    *p &= ~bit;
}
