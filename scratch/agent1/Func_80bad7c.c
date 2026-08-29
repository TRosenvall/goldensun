extern unsigned char *iwram_3001e74;
extern unsigned char *_GetUnit(int id);
extern int Random(void);

int Func_80bad7c(int side)
{
    unsigned short list[6];
    int n;
    int i;
    unsigned char *base;
    int v;

    n = 0;
    base = iwram_3001e74;
    if (side != 0) {
        i = 0;
        while (*(short *)(base + 0x58 + i * 2) != 0xff) {
            v = *(short *)(base + 0x58 + i * 2);
            if (v != 0xfe && *(short *)(_GetUnit(v) + 0x38) != 0) {
                list[n] = i | 0x100;
                n++;
            }
            i++;
        }
    } else {
        i = 0;
        while (*(short *)(base + 0x66 + i * 2) != 0xff) {
            v = *(unsigned short *)(base + 0x66 + i * 2);
            v <<= 16;
            if (v != (0xfe << 16)) {
                list[n] = i | 0x180;
                n++;
            }
            i++;
        }
    }
    if (n == 0)
        return 0;
    return list[(unsigned int)(Random() * n) >> 16];
}
