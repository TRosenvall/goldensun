extern unsigned int iwram_3001e74;
extern unsigned int ewram_2002024;
extern unsigned int ewram_2002224;
extern unsigned short iwram_3001f64;
extern int WaitFrames(int frames);

int Func_80b60a0(void)
{
    unsigned char *p;
    unsigned short *q;
    unsigned short *r;
    int n;
    int i;

    p = *(unsigned char **)&iwram_3001e74;
    n = 0;
    if (p[0x44] != 0) {
        r = (unsigned short *)((unsigned char *)&ewram_2002024 + (1 ^ p[0x50]) * 24);
        q = (unsigned short *)&ewram_2002224;
        if (p[0x52] == 0) {
            q[0] = 0x45;
            q[1] = 0x58;
            q[2] = 0x45;
            q[3] = 0x43;
            i = 0;
            do {
                if ((iwram_3001f64 & 3) != 3) {
                    n++;
                    if (n > 0x18)
                        return -1;
                } else {
                    n = 0;
                    if (q[2] == r[2] && q[3] == r[3])
                        return 0;
                }
                WaitFrames(1);
                i++;
            } while (i <= 0x1d);
        }
        return -1;
    }
    return 0;
}
