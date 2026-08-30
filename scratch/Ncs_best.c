struct Ctx {
    unsigned char pad000[0xcb6];
    short fcb6;
    unsigned char padcb8[0xcc2 - 0xcb8];
    short fcc2;
    short fcc4;
};

extern unsigned char *iwram_3001ebc;
extern unsigned char gState[];

extern void _Func_801c428(void);
extern void Func_8091660(void);
extern void Func_808e118(void);
extern int StartTask(void *f, int pri);
extern void _ClearFlag(int id);
extern void Task_Cutscene(void);

void CutsceneStart(void)
{
    unsigned char *p;
    short *q;
    int o;
    int z;

    p = iwram_3001ebc;
    _Func_801c428();
    Func_8091660();
    if (((struct Ctx *)p)->fcb6 != 0)
        Func_808e118();
    z = 0;
    o = 0xcc2;
    q = (short *)(p + o);
    o += 2;
    *q = z;
    q = (short *)(p + o);
    *q = z;
    *(int *)(p + (0xe4 << 1)) = 0x10;
    *(int *)(p + (0xe6 << 1)) = z;
    *(unsigned short *)(p + (0xed << 1)) = 0xffff;
    *(short *)(p + (0xee << 1)) = -1;
    *(short *)(p + (0xef << 1)) = -1;
    StartTask(Task_Cutscene, 0xc8 << 4);
    _ClearFlag(0x99 << 1);
    o = 0xfa << 1;
    *(int *)(p + o) = *(int *)(gState + o);
    o += 4;
    *(int *)(p + o) = z;
}
