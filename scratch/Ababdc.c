extern unsigned char *iwram_3001e74;

extern void _GetUnit(int id);
extern void **GetBattleActor(int id);
extern void _Actor_SetAnim(void *a, int n);
extern void _Func_802281c(short *p);
extern void Func_80ba918(void *a, int n);
extern void WaitFrames(int n);
extern int Func_80b6cd0(int id);
extern void _Func_801f200(int n);

void Func_80babdc(int id)
{
    short buf[2];
    void **a;
    int i;

    _GetUnit(id);
    _Actor_SetAnim(*GetBattleActor(id), 5);
    i = 1;
    do {
        buf[1] = 0xff;
        buf[0] = id;
        _Func_802281c(buf);
        Func_80ba918(*GetBattleActor(id), 7);
        WaitFrames(2);
        buf[0] = id;
        _Func_802281c(buf);
        a = GetBattleActor(id);
        Func_80ba918(*a, Func_80b6cd0(id));
        WaitFrames(2);
        i--;
    } while (i >= 0);
    _Func_801f200(iwram_3001e74[0x41]);
}
