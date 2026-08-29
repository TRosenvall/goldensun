typedef unsigned char u8;
typedef unsigned short u16;

extern unsigned int iwram_3001e74;
extern u8 gState[];
extern int _GetPartySize(void);
extern u8 *_GetUnit(int id);

int Func_80b6a60(u16 *dst)
{
    int cap;
    int n;
    int i;
    u8 *p;
    u8 *u;
    int id;
    int off;
    int k;

    off = 0xfc * 2;
    cap = 4;
    if (*(u8 *)(iwram_3001e74 + 0x44) != 0)
        cap = 3;
    n = _GetPartySize();
    if (n > cap)
        n = cap;
    if (n > 0) {
        p = gState + off;
        k = 2;
        i = n;
        while (1) {
            id = *p;
            p++;
            if (dst != 0) {
                *dst = id;
                dst++;
            }
            u = _GetUnit(id);
            *(u8 *)(u + 0x95 * 2) = k;
            i--;
            if (i == 0)
                break;
        }
    }
    if (dst != 0)
        *dst = 0xff;
    return n;
}
