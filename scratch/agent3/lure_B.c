extern unsigned char gState[];
extern void ClearFlag(int id);
extern void SetFlag(int id);
extern int GetPartySize(void);
extern unsigned char *GetUnit(int id);
extern unsigned char *GetItemInfo(int item);

void CheckLure(void)
{
    unsigned char *g;
    unsigned short *u;
    unsigned char *info;
    int i, j, k, n;

    ClearFlag(0x167);
    n = GetPartySize();
    g = gState;
    for (i = 0; i < n; i++) {
        u = (unsigned short *)GetUnit(g[0x1f8 + i]);
        for (j = 0; j < 15; j++) {
            if ((u[0x6c + j] & 0x200) != 0) {
                info = GetItemInfo(u[0x6c + j]);
                for (k = 0; k < 4; k++) {
                    if (info[0x18 + k * 4] == 0x1b)
                        SetFlag(0x167);
                }
            }
        }
    }
}
