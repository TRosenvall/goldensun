extern unsigned char gFlags[512];

int GetFlag(int flagID)
{
    int val;
    val = gFlags[(unsigned)(flagID << 20) >> 23] & (1 << (flagID & 7));
    return (unsigned)(-val | val) >> 31;
}
