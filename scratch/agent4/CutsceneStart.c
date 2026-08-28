extern short *iwram_3001ebc;
extern unsigned int gState;
extern void Task_Cutscene(void);
extern void _Func_801c428(void);
extern void Func_8091660(void);
extern void Func_808e118(void);
extern void StartTask(void (*task)(void), int mode);
extern void _ClearFlag(int id);

void CutsceneStart(void)
{
    short *p;
    short *q;
    int z;
    int o;
    int n;

    p = iwram_3001ebc;
    _Func_801c428();
    Func_8091660();
    if (p[0x65b] != 0)
        Func_808e118();
    z = 0;
    o = 0xcc2;
    q = (short *)((char *)p + o);
    *q = z;
    o += 2;
    q = (short *)((char *)p + o);
    *q = z;
    *(int *)(p + 0xe4) = 0x10;
    *(int *)(p + 0xe6) = z;
    *(unsigned short *)(p + 0xed) = 0xffff;
    n = -1;
    p[0xee] = n;
    p[0xef] = n;
    StartTask(Task_Cutscene, 0xc8 << 4);
    _ClearFlag(0x99 << 1);
    o = 0xfa;
    *(int *)(p + o) = *(int *)((short *)&gState + o);
    *(int *)(p + o + 2) = z;
}
