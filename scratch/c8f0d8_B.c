struct Actor {
    unsigned char pad00[6];
    unsigned short f6;
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x30 - 0x14];
    int f30;
    int f34;
    unsigned char pad38[0x55 - 0x38];
    unsigned char f55;
};

extern unsigned char gState[];
extern unsigned char L9e75c[] __asm__(".L9e75c");
extern struct Actor *GetFieldActor(int id);
extern void _Actor_TravelTo(struct Actor *a, int x, int y, int z);
extern void WaitFrames(int n);
extern void _Actor_SetAnim(struct Actor *a, int anim);
extern void _Actor_SetScript(struct Actor *a, unsigned char *s);

void Func_808f0d8(struct Actor *a)
{
    struct Actor *b;
    unsigned char *g;

    if (a != 0) {
        g = gState;
        b = GetFieldActor(*(int *)(g + 0x1f4));
        a->f34 = 0x80 << 9;
        a->f30 = 0x80 << 10;
        a->f55 = 0;
        _Actor_TravelTo(a, b->f8, b->fc + (0x90 << 14), b->f10);
        WaitFrames(3);
        _Actor_SetAnim(b, 0x1c);
        _Actor_SetScript(a, L9e75c);
        b->f6 = 0x80 << 7;
    }
}
