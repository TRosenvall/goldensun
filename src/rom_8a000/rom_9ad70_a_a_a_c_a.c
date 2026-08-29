extern unsigned int gState;
extern unsigned char *GetFieldActor(int slot);
extern void _Actor_SetAnimSpeed(unsigned char *a, int n);
extern void Func_809ad70(void);

void Func_809ad90(int slot)
{
    unsigned char *a;
    unsigned char *p;
    unsigned char *q;
    unsigned char *s;
    unsigned int g;
    unsigned int off;
    int *d;

    a = GetFieldActor(slot);
    if (a != 0) {
        g = (unsigned int)&gState;
        off = 0x94;
        off <<= 2;
        d = (int *)(g + off);
        *d = *(int *)(a + 0x6c);
        g += 0x249;
        *(unsigned char *)g = 0;
        p = a + 0x54;
        if (*p == 1) {
            s = *(unsigned char **)(*(unsigned char **)(a + 0x50) + 0x28);
            if (s != 0)
                *(unsigned char *)g = s[5];
        }
        *(void **)(a + 0x6c) = (void *)Func_809ad70;
        q = a + 0x5b;
        *q = 1;
        _Actor_SetAnimSpeed(a, 0);
    }
}
