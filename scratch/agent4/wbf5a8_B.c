extern unsigned char *_Func_8077330(int);
extern unsigned char *_GetUnit(int);
extern void _SetDjinni(int, int, int);
extern void _Func_807a3a8(int, int, int);
extern void _CalcStats(int);

int Func_80bf5a8(void)
{
    unsigned char *a;
    unsigned char *p;
    unsigned char *q;
    int i;
    int res;
    int id;
    int t;

    a = _Func_8077330(0) + 8;
    res = 0;
    i = 0;
    p = a;
    while (i < *(int *)(a + 0x100)) {
        if ((signed char)p[3] > 0) {
            if (*(short *)(_GetUnit(p[2]) + 0x38) != 0) {
                t = p[3];
                t -= 1;
                p[3] = t;
            }
        }
        i++;
        p += 4;
    }
    i = 0;
    q = a;
    while (i < *(int *)(a + 0x100)) {
        if ((signed char)q[3] == 0) {
            id = q[2];
            _SetDjinni(id, q[0], q[1]);
            _Func_807a3a8(id, q[0], q[1]);
            _CalcStats(id);
            res = 1;
        } else {
            q += 4;
            i++;
        }
    }
    return res;
}
