extern unsigned char *iwram_3001e8c;

void Func_8019908(int cb, int id)
{
    unsigned char *b;
    int *p;
    unsigned char *q;
    int i;
    int n;

    b = iwram_3001e8c;
    q = (unsigned char *)0x12dc;
    n = 8;
    i = 0;
    p = (int *)(b + 0x12bc);
    do {
        if (*(unsigned short *)(q + (int)b) == 0) {
            *p = cb;
            *(unsigned short *)(q + (int)b) = id;
            break;
        }
        i++;
        p++;
        q += 2;
    } while (i != n);
}
