extern void *GetUnit(int id);
extern void *GetEnemyInfo(int id);
extern void Func_80797fc(int a, unsigned char *b, int *out);

struct EnemyRow { int p; unsigned char pad[4]; int q[4]; };
extern struct EnemyRow L88e38[] __asm__(".L88e38");
extern unsigned char L88df8[] __asm__(".L88df8");

void Func_80798e0(int id, void *out)
{
    unsigned char *u;
    void *e;
    unsigned int k;
    int i;
    int buf[4];
    int *s;
    int *d;
    unsigned char *tbl;
    short *o;
    int v, rem, q, off;

    u = (unsigned char *)GetUnit(id);
    if (u[0x129] == 0) {
        e = GetEnemyInfo(u[0x94 << 1]);
        k = *((unsigned char *)e + 0x34);
        if (k > 0x2b)
            k = 0;
        s = L88e38[k].q;
        d = (int *)out;
        for (i = 0; i <= 3; i++)
            *d++ = *s++;
    } else {
        Func_80797fc(u[0x94 << 1], u + 0xf8, buf);
        tbl = L88df8;
        for (i = 0; i <= 3; i++) {
            v = buf[i];
            rem = v % 10;
            q = v / 10;
            if (q > 0xf)
                q = 0xf;
            if (q < 0)
                q = 0;
            off = q << 2;
            o = (short *)((char *)out + i * 4);
            o[0] = *(unsigned short *)(tbl + off) + rem;
            o[1] = *(unsigned short *)(tbl + off + 2) + rem;
        }
    }
}
