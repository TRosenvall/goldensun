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

    p = iwram_3001ebc;
    _Func_801c428();
    Func_8091660();
    if (p[0x65b] != 0)
        Func_808e118();
    p[0x661] = 0;
    p[0x662] = 0;
    *(int *)(p + 0xe4) = 0x10;
    *(int *)(p + 0xe6) = 0;
    *(unsigned short *)(p + 0xed) = 0xffff;
    p[0xee] = -1;
    p[0xef] = -1;
    StartTask(Task_Cutscene, 0xc8 << 4);
    _ClearFlag(0x99 << 1);
    *(int *)(p + 0xfa) = *(int *)((short *)&gState + 0xfa);
    *(int *)(p + 0xfc) = 0;
}
