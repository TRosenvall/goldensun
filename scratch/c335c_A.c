struct A {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
};

struct C {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
};

extern void **iwram_3001e70;
extern struct A *GetFieldActor(int id);
extern unsigned char *galloc_ewram(int slot, int size);
extern void _Camera_SetTarget(struct C *c, struct A *a);
extern void WaitFrames(int n);
extern void _Func_800fe9c(void);

void SetCameraTarget(int id, int flag)
{
    struct A *a;
    struct C *c;
    unsigned char *m;
    int *t;
    void **e;

    a = GetFieldActor(id);
    m = galloc_ewram(0x1b, 0xccc);
    c = *(struct C **)(m + (0xf0 << 1));
    e = iwram_3001e70;
    if (a != 0) {
        t = &c->f8;
        *e = t;
        _Camera_SetTarget(c, a);
        if (flag == 0) {
            *t = a->f8;
            c->fc = a->fc;
            c->f10 = a->f10;
            WaitFrames(1);
            if (*(short *)(m + (0xcf << 1)) != 3)
                _Func_800fe9c();
        }
    }
}
