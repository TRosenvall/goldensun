struct Actor {
    unsigned char pad00[0x5b];
    unsigned char f5b;
    unsigned char pad5c[0x6c - 0x5c];
    void *f6c;
};

extern unsigned char gState[];
extern void Func_809ad70(void);
extern struct Actor *GetFieldActor(int id);
extern void _Actor_SetColorswap(struct Actor *a, int c);
extern void _Actor_SetAnimSpeed(struct Actor *a, int s);

void Func_809ade8(int id)
{
    struct Actor *a;
    unsigned char *g;

    a = GetFieldActor(id);
    if (a != 0) {
        if (a->f6c == (void *)Func_809ad70) {
            g = gState;
            a->f6c = *(void **)(g + 0x250);
            *(void **)(g + 0x250) = 0;
            _Actor_SetColorswap(a, *(signed char *)(g + 0x249));
        }
        a->f5b = 0;
        _Actor_SetAnimSpeed(a, 0x10);
    }
}
