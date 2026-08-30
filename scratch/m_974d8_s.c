struct V {
    int x;
    int y;
    int z;
};

extern unsigned char *iwram_3001ebc;
extern void PhysMove(struct V *out, struct V *buf);

void Func_80974d8(struct V *out)
{
    struct V buf;
    unsigned char *p;
    unsigned char *q;
    int a;
    int b;

    p = iwram_3001ebc;
    if (*(short *)(p + (0xcf << 1)) == 3) {
        PhysMove(out, &buf);
        out->x = buf.x << 16;
        out->z = buf.y << 16;
    } else {
        q = *(unsigned char **)((char *)&iwram_3001ebc - 0x4c);
        a = *(int *)(q + 0xe4) & 0xffff0000;
        b = *(int *)(q + 0xe8) & 0xffff0000;
        out->x = out->x - a;
        out->z = out->z - out->y - b;
    }
    out->y = 0;
}
