extern unsigned char *_Func_8077330(int);
extern unsigned char *_GetUnit(int);
extern void _SetDjinni(int, int, int);
extern void _Func_807a3a8(int, int, int);
extern void _CalcStats(int);

typedef struct {
    unsigned char a;
    unsigned char b;
    unsigned char c;
    signed char d;
} E;

int Func_80bf5a8(void)
{
    unsigned char *a;
    E *p;
    E *q;
    int i;
    int res;
    int id;
    int t;

    res = 0;
    a = _Func_8077330(0) + 8;
    for (i = 0, p = (E *)a; i < *(int *)(a + 0x100); ) {
        if (p->d > 0) {
            if (*(short *)(_GetUnit(p->c) + 0x38) != 0) {
                t = *(unsigned char *)&p->d;
                t -= 1;
                *(unsigned char *)&p->d = t;
            }
        }
        i++;
        p++;
    }
    for (i = 0, q = (E *)a; i < *(int *)(a + 0x100); ) {
        if (q->d == 0) {
            id = q->c;
            _SetDjinni(id, q->a, q->b);
            _Func_807a3a8(id, q->a, q->b);
            _CalcStats(id);
            res = 1;
        } else {
            q++;
            i++;
        }
    }
    return res;
}
