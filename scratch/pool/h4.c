extern unsigned char gFlags[512];

int GetFlag(int flagID)
{
    int bit, val;
    bit = 1 << (flagID & 7);
    val = bit & gFlags[(unsigned)(flagID << 20) >> 23];
    return (unsigned)(-val | val) >> 31;
}
