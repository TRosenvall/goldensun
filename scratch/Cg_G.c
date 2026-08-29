extern unsigned char gFlags[];
unsigned char GetFlagByte(unsigned int id) {
    unsigned int a, b;
    a = id << 20;
    b = a >> 23;
    return gFlags[b];
}
