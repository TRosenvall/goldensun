extern unsigned char gState[];
extern void ClearFlag(int id);
extern void SetFlag(int id);
extern int GetPartySize(void);
extern unsigned char *GetUnit(int id);
extern unsigned char *GetItemInfo(int item);

void CheckLure(void)
{
    unsigned char *g;
    unsigned char *u;
    unsigned char *info;
    int i, j, k, n, off;

    ClearFlag(0x167);
    n = GetPartySize();
    i = 0;
    if (i < n) {
        g = gState;
        do {
            u = GetUnit(g[0x1f8 + i]);
            off = 0xd8;
            j = 14;
            do {
                if ((*(unsigned short *)(off + (int)u) & 0x200) != 0) {
                    info = GetItemInfo(*(unsigned short *)(off + (int)u));
                    for (k = 0; k < 4; k++) {
                        if (info[0x18 + k * 4] == 0x1b)
                            SetFlag(0x167);
                    }
                }
                j--;
                off += 2;
            } while (j >= 0);
            i++;
        } while (i < n);
    }
}
