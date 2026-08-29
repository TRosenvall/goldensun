extern unsigned char gState[];
extern unsigned char L9e270[] __asm__(".L9e270");

void Func_808b25c(void)
{
    unsigned char *g;
    int *p;
    int e;
    int v;
    short id;

    g = gState;
    id = *(short *)(g + (0xe0 << 1));
    p = (int *)L9e270;
    while ((e = *p++) != 0 && e != id) {
        if (e & (0x80 << 24))
            v = e & 0xffff;
    }
    *(short *)(g + (0xeb << 1)) = v;
}
