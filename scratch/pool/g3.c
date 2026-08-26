extern unsigned char gFlags[512];

int GetFlag(int flagID)
{
    int bit, val, t, idx;
    bit = 1 << (flagID & 7);
    t = flagID << 20;
    idx = (unsigned)t >> 23;
    val = gFlags[idx] & bit;
    return (unsigned)(-val | val) >> 31;
}
