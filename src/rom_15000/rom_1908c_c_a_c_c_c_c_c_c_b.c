struct Box {
    unsigned char pad0[0x14];
    unsigned short f14;
    unsigned short f16;
    int f18;
};

extern unsigned char *iwram_3001e8c;
extern void CloseUIBox(struct Box *b, unsigned short f);
extern void WaitFrames(int n);

void Func_8019a54(void)
{
    unsigned char *q;
    struct Box *b;
    int i;

    q = iwram_3001e8c + (0xc4 << 3);
    i = 0;
    do {
        b = *(struct Box **)q;
        if (b != 0 && b->f18 == 0) {
            if (b->f16 != 0 && b->f14 != 0) {
                CloseUIBox(b, b->f16 & 2);
            }
        }
        q += 0x28;
        i++;
    } while (i != 3);
    WaitFrames(0xa);
}
