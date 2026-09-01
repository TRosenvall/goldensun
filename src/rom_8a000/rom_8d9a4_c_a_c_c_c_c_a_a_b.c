extern unsigned char *iwram_3001ebc;
extern int GetMapActorIndex(int a);
extern int _Func_8011f54(int a, int b, int c);

void Func_808edac(int a, int b, int c)
{
    unsigned char *base;
    unsigned char *p;
    unsigned char *ent;
    int v;
    int idx;

    idx = GetMapActorIndex(a);
    if (idx == -1)
        return;
    base = iwram_3001ebc;
    p = base + idx * 8 + (0x8e << 1);
    ent = *(unsigned char **)p;
    if (ent == 0)
        return;
    if (b == -1)
        b = (p[6] << 20) + (0x80 << 12);
    if (c == -1)
        c = (p[7] << 20) + (0x80 << 12);
    *(int *)(ent + 8) = b;
    *(int *)(ent + 0x10) = c;
    v = _Func_8011f54(0, b, c);
    *(int *)(ent + 0x14) = v;
    *(int *)(ent + 0xc) = v;
}
