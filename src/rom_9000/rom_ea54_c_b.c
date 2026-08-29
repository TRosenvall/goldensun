extern int _GetFlag(int);
extern unsigned char *iwram_3001ebc;
extern volatile unsigned int gKeyPress;

int Func_800ea60(unsigned int arg)
{
    short *p;
    unsigned int slot;
    int value;
    int fa;
    int rv;
    int lv;

    fa = 0xfa;
    rv = 0xfc88;
    lv = 0xfc87;
    value = 0x3fff;
    p = (short *)iwram_3001ebc;
    slot = arg >> 14;
    value &= arg;
    if (_GetFlag(0x107)) {
        p[0xc1] = fa;
    } else if (p[0xcf] == 3) {
        if (gKeyPress & 0x100) {
            p[0xc1] = rv;
        } else if (gKeyPress & 0x200) {
            p[0xc1] = lv;
        }
    } else {
        switch (slot) {
        case 0: p[0xbf] = value; break;
        case 1: p[0xc0] = value; break;
        }
    }
    return value;
}
