extern int Func_80796c4(unsigned short *buf);
extern unsigned char *GetUnit(int id);

int GetNumDjinn(int which)
{
    unsigned short buf[16];
    unsigned char *u;
    int n;
    int total;
    int i;
    int k;

    total = 0;
    n = Func_80796c4(buf);
    for (i = 0; i < n; i++) {
        u = GetUnit(buf[i]);
        if (which == -1) {
            total += u[0x8c << 1];
            total += u[(0x8c << 1) + 1];
            total += u[(0x8c << 1) + 2];
            total += u[(0x8c << 1) + 3];
        } else {
            k = which + (0x8c << 1);
            total += u[k];
        }
    }
    return total;
}
