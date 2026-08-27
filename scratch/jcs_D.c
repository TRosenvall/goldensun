extern char *iwram_3001ebc;
extern unsigned char gState[];
extern void _Func_801c428(void);
extern void Func_8091660(void);
extern void Func_808e118(void);
extern void Task_Cutscene(void);
extern void StartTask(void *fn, int prio);
extern void _ClearFlag(int id);

void CutsceneStart(void)
{
    char *p;
    short *s;
    unsigned short *u;
    unsigned char *g;
    int *w;
    int z;
    int o;
    int k;
    int prio;
    int m1;
    int m2;

    p = iwram_3001ebc;
    prio = 0xc8 << 4;
    m1 = -1;
    m2 = -1;
    _Func_801c428();
    Func_8091660();
    s = (short *)(p + 0xcb6);
    if (*s != 0)
        Func_808e118();
    z = 0;
    o = 0xcc2;
    s = (short *)(p + o);
    *s = z;
    o += 2;
    s = (short *)(p + o);
    *s = z;
    w = (int *)(p + (0xe4 << 1));
    *w = 0x10;
    w = (int *)(p + (0xe6 << 1));
    *w = z;
    u = (unsigned short *)(p + (0xed << 1));
    *u = 0xffff;
    s = (short *)(p + (0xee << 1));
    *s = m1;
    s = (short *)(p + (0xef << 1));
    *s = m2;
    StartTask(Task_Cutscene, prio);
    _ClearFlag(0x99 << 1);
    g = gState;
    k = 0xfa << 1;
    *(int *)(p + k) = *(int *)(g + k);
    k += 4;
    w = (int *)(p + k);
    *w = z;
}
