extern void *_Func_8077330(int);
extern unsigned char *_GetUnit(int);
extern void _SetDjinni(int, int, int);
extern void _Func_807a3a8(int, int, int);
extern void _CalcStats(int);

typedef struct {
    unsigned char pad[8];
    unsigned char e[0x100];
    int n;
} Rec;

int Func_80bf5a8(void)
{
    Rec *s;
    unsigned char *p;
    int i;
    int res;
    int id;

    s = (Rec *)_Func_8077330(0);
    res = 0;
    i = 0;
    p = s->e;
    while (i < s->n) {
        if ((signed char)p[3] > 0) {
            if (*(short *)(_GetUnit(p[2]) + 0x38) != 0) {
                p[3] = p[3] - 1;
            }
        }
        i++;
        p += 4;
    }
    i = 0;
    p = s->e;
    while (i < s->n) {
        if ((signed char)p[3] == 0) {
            id = p[2];
            _SetDjinni(id, p[0], p[1]);
            _Func_807a3a8(id, p[0], p[1]);
            _CalcStats(id);
            res = 1;
        } else {
            p += 4;
            i++;
        }
    }
    return res;
}
