struct M {
    unsigned char pad00[1];
    unsigned char f1;
    unsigned char pad02[1];
    unsigned char f3;
};

extern struct M *_GetMoveInfo(int id);
extern void Func_80a2438(int n);

void Func_80aa460(int move)
{
    struct M *m;
    int k;

    m = _GetMoveInfo(move);
    k = m->f1 & 0xf;
    switch (k) {
    case 1:
        Func_80a2438(0x7e);
    case 0xb:
        Func_80a2438(0x7e);
        break;
    default:
        switch (m->f3) {
        case 1:
        case 2:
        case 31:
        case 32:
            break;
        case 5:
            Func_80a2438(0x52);
            break;
        case 3:
            Func_80a2438(0x54);
            break;
        default:
            Func_80a2438(0x5b);
            break;
        }
        break;
    }
}
