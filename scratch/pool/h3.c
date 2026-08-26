extern unsigned char gFlags[512];

int GetFlag(int flagID)
{
    int bit, val;
    unsigned char *f;
    bit = 1 << (flagID & 7);
    f = gFlags;
    val = f[(unsigned)(flagID << 20) >> 23] & bit;
    return (unsigned)(-val | val) >> 31;
}
