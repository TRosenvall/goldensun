extern unsigned char gFlags[512];

int GetFlag(unsigned int flagID)
{
    int bit, val;
    bit = 1 << (flagID & 7);
    val = gFlags[(flagID << 20) >> 23] & bit;
    return (unsigned)(-val | val) >> 31;
}
