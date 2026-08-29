extern char *iwram_3001e70;

int Func_801219c(int *pos)
{
    int x, z;
    char *m;
    unsigned char *layer;
    int t;

    x = pos[0] / 0x10000;
    z = (pos[2] - pos[1]) / 0x10000;
    m = iwram_3001e70;
    if (m == 0)
        return 0;
    layer = *(unsigned char **)(m + (0xc8 << 1));
    t = layer[(x / 16 + (z / 16) * 128) * 4 + 2] ^ 0xff;
    return (t != 0) - 1;
}
