extern int _Func_80122c8(void *pos, int *out);
extern int _GetFlag(int);
extern void Func_808b2b0(int);
extern short tbl[] __asm__(".L9d7a8");

int Func_808adf0(void *pos)
{
    short *e;
    int out;
    int v;
    int res;

    e = tbl;
    v = _Func_80122c8(pos, &out);
    res = 0;
    while (e[0] != -1) {
        if (e[0] == out && (e[1] == -1 || e[1] == v)
            && (e[2] == -1 || _GetFlag(e[2]) == 0)) {
            res = e[3];
            break;
        }
        e += 4;
    }
    Func_808b2b0(v);
    return res;
}
