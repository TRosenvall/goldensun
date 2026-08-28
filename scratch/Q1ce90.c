extern unsigned char gState[];

void Func_801ce90(char *rec)
{
    unsigned short *ph;
    unsigned char *p;
    int v;

    rec += 0x574;
    ph = (unsigned short *)rec;
    switch (*ph) {
    case 0:
        p = gState + (0x83 << 2);
        break;
    case 1:
        p = gState + 0x205;
        break;
    case 2:
        p = gState + 0x206;
        break;
    default:
        return;
    }
    v = *p;
    if (v != 0)
        *p = v + 0xff;
}
