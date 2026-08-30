extern unsigned char *GetUnit(int id);
extern int Func_807a1f8(int id, int entry, int bit);
extern void Func_8079ae8(int id);

int SetDjinni(int id, int entry, int bit)
{
    unsigned char *u;
    int r;
    int m;
    int k;
    int i;

    u = GetUnit(id);
    r = Func_807a1f8(id, entry, bit);
    if (r != 0) {
        k = entry * 4;
        i = k + 0xf8;
        m = 1 << bit;
        if ((*(int *)(u + i) & m) != 0) {
            k = k + (0x84 << 1);
            *(int *)(u + k) |= m;
            k = entry + (0x8e << 1);
            (*(unsigned char *)(u + k))++;
            Func_8079ae8(id);
        } else {
            return 0;
        }
    }
    return r;
}
