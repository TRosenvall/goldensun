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
    int z;
    int k;
    int prio;
    int o;

    p = iwram_3001ebc;
    prio = 0xc8 << 4;
    _Func_801c428();
    Func_8091660();
    if (*(short *)(p + 0xcb6) != 0)
        Func_808e118();
    z = 0;
    o = 0xcc2;
    *(short *)(p + o) = z;
    o += 2;
    *(short *)(p + o) = z;
    *(int *)(p + (0xe4 << 1)) = 0x10;
    *(int *)(p + (0xe6 << 1)) = z;
    *(short *)(p + (0xed << 1)) = 0xffff;
    *(short *)(p + (0xee << 1)) = -1;
    *(short *)(p + (0xef << 1)) = -1;
    StartTask(Task_Cutscene, prio);
    _ClearFlag(0x99 << 1);
    k = 0xfa << 1;
    *(int *)(p + k) = *(int *)(gState + k);
    k += 4;
    *(int *)(p + k) = z;
}
