extern unsigned char gFlags[];
unsigned char GetFlagByte(unsigned int id) {
    return gFlags[(id >> 3) & 0x1ff];
}
