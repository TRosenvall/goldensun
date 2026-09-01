extern unsigned char Lb41ac[] __asm__(".Lb41ac");
extern int _GetFlag(int id);
extern void _SetFlag(int id);
extern void _Func_8078ad0(int a, int b);

void Func_80b26cc(int id)
{
    short *p;
    int k;
    int off;
    int n;
    unsigned char *base;
    unsigned char *q;
    int v;

    if (_GetFlag(id + (0x80 << 3)) != 0) {
        return;
    }
    _SetFlag(id + (0x80 << 3));
    off = (id * 32 + id) * 2;
    base = Lb41ac;
    n = off + 0x30;
    v = *(short *)(base + n);
    k = 0;
    if (v == 0) {
        return;
    }
    q = base + off;
    p = (short *)(q + 0x30);
    do {
        _Func_8078ad0(v, 1);
        k++;
        if (k > 7) {
            return;
        }
        p++;
        v = *p;
    } while (v != 0);
}
