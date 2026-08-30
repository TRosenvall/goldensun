extern unsigned int iwram_3001e40;
extern void __Actor_SetAnim(unsigned char *a, int n);
extern unsigned int __Random(void);
extern void OvlFunc_964_2008ae8(int a, int b, int c, int d, int e, int f, int g, int *h);

int OvlFunc_964_2008f4c(unsigned char *e)
{
    int v[10];
    int z;
    int x;
    int y;
    int w;

    if (iwram_3001e40 & 2)
        __Actor_SetAnim(e, 1);
    else
        __Actor_SetAnim(e, 2);
    z = iwram_3001e40 & 3;
    if (z != 0)
        return 0;
    v[2] = 0x4ccc;
    v[3] = 0x4ccc;
    v[1] = 5;
    x = *(int *)(e + 8) + (((__Random() * 7 >> 16) - 3) << 16);
    y = *(int *)(e + 0x10) + (((__Random() * 7 >> 16) - 3) << 16);
    w = *(int *)(e + 0xc) + (0x80 << 13);
    OvlFunc_964_2008ae8(x, w, y, 0, z, z, 0x90001, v);
    return 0;
}
