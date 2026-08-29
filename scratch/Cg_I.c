extern unsigned char gFlags[];
unsigned char GetFlagByte(unsigned int id) {
    unsigned int i;
    i = id << 20;
    i >>= 23;
    return gFlags[i];
}
