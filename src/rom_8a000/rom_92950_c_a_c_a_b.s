extern unsigned char *iwram_3001ebc;

int Func_8092be0(int id)
{
    unsigned char *base;
    unsigned char *a;
    unsigned char *t;
    int i;
    int r;
    int off;

    base = iwram_3001ebc;
    a = *(unsigned char **)(base + 0x34);
    r = -1;
    i = 8;
    if (a == 0)
        goto next;
    if (*(a + 0x54) != 1)
        goto next;
    t = *(unsigned char **)(*(unsigned char **)(a + 0x50) + 0x28);
    if (*(short *)(t + (unsigned int)0) != id)
        goto next;
    r = 8;
    goto out;
next:
    i++;
    if (i > 0x41)
        goto out;
    off = (i << 2) + 0x14;
    a = *(unsigned char **)(base + off);
    if (a == 0)
        goto next;
    if (*(a + 0x54) != 1)
        goto next;
    t = *(unsigned char **)(*(unsigned char **)(a + 0x50) + 0x28);
    if (*(short *)(t + (unsigned int)0) != id)
        goto next;
    r = i;
out:
    return r;
}
