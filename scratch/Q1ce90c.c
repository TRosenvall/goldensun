extern unsigned char gState[];

void Func_801ce90(char *rec)
{
    unsigned short *ph;
    unsigned char *g;
    unsigned char *p;
    int v;
    int t;

    rec += 0x574;
    ph = (unsigned short *)rec;
    switch (*ph) {
    case 0:
        g = gState;
        p = g + (0x83 << 2);
        break;
    case 1:
        g = gState;
        p = g + 0x205;
        break;
    case 2:
        g = gState;
        p = g + 0x206;
        break;
    default:
        return;
    }
    v = *p;
    t = v;
    if (t == 0)
        return;
    t += 0xff;
    *p = t;
}
