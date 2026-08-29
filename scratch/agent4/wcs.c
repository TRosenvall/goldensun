extern unsigned char *iwram_3001ebc;
extern unsigned char gState[];

extern void _Func_801c428(void);
extern void Func_8091660(void);
extern void Func_808e118(void);
extern void StartTask(void (*fn)(void), int prio);
extern void Task_Cutscene(void);
extern void _ClearFlag(int id);

void CutsceneStart(void)
{
    unsigned char *p;
    unsigned char *gs;
    int zero;
    int none;
    int m1;
    int m2;

    p = iwram_3001ebc;
    _Func_801c428();
    Func_8091660();
    if (*(short *)(p + 0xcb6) != 0) {
        Func_808e118();
    }
    zero = 0;
    none = 0xffff;
    m1 = -1;
    m2 = -1;
    *(short *)(p + 0xcc2) = zero;
    *(short *)(p + 0xcc4) = zero;
    *(int *)(p + 0x1c8) = 0x10;
    *(int *)(p + 0x1cc) = zero;
    *(short *)(p + 0x1da) = none;
    *(short *)(p + 0x1dc) = m1;
    *(short *)(p + 0x1de) = m2;
    StartTask(Task_Cutscene, 0xc8 << 4);
    _ClearFlag(0x99 << 1);
    gs = gState;
    *(int *)(p + 0x1f4) = *(int *)(gs + 0x1f4);
    *(int *)(p + 0x1f8) = zero;
}
