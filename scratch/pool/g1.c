extern unsigned char gFlags[512];

int GetFlag(int flagID)
{
    int bit, val, t;
    bit = 1 << (flagID & 7);
    t = flagID << 20;
    val = gFlags[(unsigned)t >> 23] & bit;
    return (unsigned)(-val | val) >> 31;
}
