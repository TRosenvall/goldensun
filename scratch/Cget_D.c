extern unsigned char gFlags[];
unsigned char GetFlagByte(unsigned int id) {
    return gFlags[(id & 0xff8) >> 3];
}
