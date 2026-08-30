extern unsigned char *iwram_3001ebc;
extern void PhysMove(int *out, int *buf);

void Func_80974d8(int *out)
{
    int buf[3];
    unsigned char *p;
    unsigned char *q;
    int a;
    int b;
    int v;

    p = iwram_3001ebc;
    if (*(short *)(p + (0xcf << 1)) == 3) {
        PhysMove(out, buf);
        out[0] = buf[0] << 16;
        v = buf[1] << 16;
    } else {
        q = *(unsigned char **)((char *)&iwram_3001ebc - 0x4c);
        a = *(int *)(q + 0xe4) & 0xffff0000;
        b = *(int *)(q + 0xe8) & 0xffff0000;
        out[0] -= a;
        v = out[2] - out[1] - b;
    }
    out[2] = v;
    out[1] = 0;
}
