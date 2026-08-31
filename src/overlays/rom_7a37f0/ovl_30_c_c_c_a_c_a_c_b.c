extern unsigned char gBuffer[];
extern unsigned char ewram_202c000[];

int OvlFunc_916_2008be4(int x, int z, int f)
{
    unsigned char *q;
    int i;

    for (i = 0; i <= 3; i++) {
        q = &gBuffer[(x + (z << 7)) << 2];
        if (q[2] == 0xff)
            return -1;
        if (*(unsigned char *)((q[3] << 2) + (int)ewram_202c000) != 0)
            return -1;
        if (f == 0)
            x++;
        else
            z++;
    }
    return 0;
}
