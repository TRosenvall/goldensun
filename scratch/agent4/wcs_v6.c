struct D {
    unsigned char pad000[0x1c8];
    int f1c8;
    int f1cc;
    unsigned char pad1d0[0x1da - 0x1d0];
    unsigned short f1da;
    short f1dc;
    short f1de;
    unsigned char pad1e0[0x1f4 - 0x1e0];
    int f1f4;
    int f1f8;
    unsigned char pad1fc[0xcb6 - 0x1fc];
    short fcb6;
    unsigned char padcb8[0xcc2 - 0xcb8];
    short fcc2;
    short fcc4;
};

extern struct D *iwram_3001ebc;
extern unsigned char gState[];

extern void _Func_801c428(void);
extern void Func_8091660(void);
extern void Func_808e118(void);
extern void StartTask(void (*fn)(void), int prio);
extern void Task_Cutscene(void);
extern void _ClearFlag(int id);

void CutsceneStart(void)
{
    struct D *p;
    unsigned char *gs;
    int zero;
    int none;
    int m1;
    int m2;

    p = iwram_3001ebc;
    _Func_801c428();
    Func_8091660();
    if (p->fcb6 != 0) {
        Func_808e118();
    }
    zero = 0;
    none = 0xffff;
    m1 = -1;
    m2 = -1;
    p->fcc2 = zero;
    p->fcc4 = zero;
    p->f1c8 = 0x10;
    p->f1cc = zero;
    p->f1da = none;
    p->f1dc = m1;
    p->f1de = m2;
    StartTask(Task_Cutscene, 0xc8 << 4);
    _ClearFlag(0x99 << 1);
    gs = gState;
    *(int *)((unsigned char *)p + 0x1f4) = *(int *)(gs + 0x1f4);
    *(int *)((unsigned char *)p + 0x1f8) = zero;
}
