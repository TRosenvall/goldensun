struct X { unsigned char pad00[0xe]; unsigned char fe; };

extern char *iwram_3001f2c;
extern void _Func_801bcd4(int a, int b, int c, int d);

int Func_80a9dc4(unsigned char *p)
{
    char *base;
    int i;
    int v;

    base = iwram_3001f2c;
    i = 0;
    do {
        if (p[i] != 0) {
            switch (i) {
            case 0:
                v = 0x10;
                break;
            case 1:
                v = 1;
                break;
            case 2:
                v = 2;
                break;
            case 3:
                v = 0xf;
                break;
            case 4:
                v = 7;
                break;
            default:
                v = 0;
                break;
            }
            _Func_801bcd4(8, v,
                          (*(struct X **)(base + (i * 4 + 0xc8)))->fe, 0);
        }
        i++;
    } while (i <= 4);
    return 1;
}
