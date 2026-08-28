extern unsigned char gFlags[];
unsigned char GetFlagByte(unsigned int id) {
    unsigned int i;
    i = id;
    i = (i << 20) >> 23;
    return gFlags[i];
}
