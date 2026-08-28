extern unsigned char gState[];

void Func_801ce90(char *rec)
{
    unsigned short *ph;
    unsigned char *p;
    int off;
    int v;
    int t;

    rec += 0x574;
    ph = (unsigned short *)rec;
    switch (*ph) {
    case 0:
        off = 0x83 << 2;
        break;
    case 1:
        off = 0x205;
        break;
    case 2:
        off = 0x206;
        break;
    default:
        return;
    }
    p = gState + off;
    v = *p;
    t = v;
    if (t == 0)
        return;
    t += 0xff;
    *p = t;
}
