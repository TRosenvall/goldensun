extern unsigned int iwram_3001ebc;
extern volatile int gKeyPress;
extern short *Func_808d394(int a);
extern void WaitFrames(int n);
extern int _Func_8017364(void);
extern int _YesNoMenu(int a, int b, int c, int d);
extern void MapActor_SetAnim(int actorID, int anim);
extern void _Func_8019e48(int a);
extern void _Func_8019a54(void);
extern void MapActor_WaitAnim(int actorID, int anim);

int Func_8091c7c(int slot, int simple)
{
    unsigned int g;
    int idx;
    unsigned int p1;
    unsigned int p2;
    int style;
    int t;
    int u;
    int r;

    g = iwram_3001ebc;
    idx = *Func_808d394(*(int *)(g + (0xfa << 1)));
    p1 = *(unsigned int *)(g + (0xfc << 1));
    p2 = *(unsigned int *)(g + (0xfe << 1));
    style = 1;
    while (gKeyPress != 0)
        WaitFrames(1);
    while (_Func_8017364() == 0)
        WaitFrames(1);
    WaitFrames(3);
    if (simple == 0) {
        t = *(unsigned short *)(p1 + 0xe) + *(unsigned short *)(p1 + 0xa);
        if (p2 != 0) {
            u = *(unsigned short *)(p2 + 0xe) + *(unsigned short *)(p2 + 0xa);
            if (t < u)
                t = u;
        }
        if (t > 0xf)
            style = 0;
    }
    r = _YesNoMenu(style, *(short *)(g + 0xcc2), *(short *)(g + 0xcc4), 0);
    if (r != 0) {
        MapActor_SetAnim(slot, 4);
        _Func_8019e48(idx);
        _Func_8019a54();
        MapActor_WaitAnim(slot, 4);
    } else {
        MapActor_SetAnim(slot, 3);
        _Func_8019e48(idx);
        _Func_8019a54();
        MapActor_WaitAnim(slot, 3);
    }
    return r;
}
