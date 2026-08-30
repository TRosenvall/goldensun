extern unsigned char *iwram_3001e8c;

void Func_8019908(int cb, int id)
{
    unsigned char *b;
    int *p;
    unsigned char *q;
    int i;
    int n;
    int c2;
    int d2;

    d2 = id;
    b = iwram_3001e8c;
    c2 = cb;
    q = (unsigned char *)0x12dc;
    n = 8;
    i = 0;
    p = (int *)(b + 0x12bc);
    do {
        if (*(unsigned short *)(q + (int)b) == 0) {
            *p = c2;
            *(unsigned short *)(q + (int)b) = d2;
            break;
        }
        i++;
        p++;
        q += 2;
    } while (i != n);
}
