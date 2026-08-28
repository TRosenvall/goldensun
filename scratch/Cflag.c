extern unsigned char gFlags[];

unsigned char GetFlagByte(unsigned int id) {
    return gFlags[(id << 20) >> 23];
}

void SetFlagByte(unsigned int id, unsigned char val) {
    gFlags[(id << 20) >> 23] = val;
}
