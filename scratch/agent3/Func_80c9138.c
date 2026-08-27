typedef struct {
    unsigned char pad[0x7790];
    int f7790;
    int f7794;
    int f7798;
    int f779c;
    unsigned char pad2[0x30];
    int f77d0;
    int f77d4;
} Blk;

extern Blk *iwram_3001eec;

void Func_80c9138(void)
{
    Blk *b;
    int c;
    volatile unsigned int *reg;

    b = iwram_3001eec;
    c = b->f7790 + 1;
    b->f7790 = c;
    if (c == b->f7794) {
        reg = (volatile unsigned int *)0x04000028;
        *reg = b->f77d0;
        reg = (volatile unsigned int *)((char *)reg + 4);
        *reg = b->f77d4;
        b->f77d0 += b->f7798;
        b->f77d4 += b->f779c;
        b->f7790 = 0;
    }
}
