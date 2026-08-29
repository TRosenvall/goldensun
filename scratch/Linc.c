extern unsigned char gFlags[];

int IncFlagByte(int idx)
{
    unsigned int i;
    unsigned int t;
    int v;

    t = idx << 20;
    i = t >> 23;
    v = gFlags[i];
    if (v <= 0xfe)
        gFlags[i] = v + 1;
    return gFlags[i];
}
