struct Actor { unsigned char pad00[8]; int f8; int fc; int f10; };

extern struct Actor *GetFieldActor(int a);
extern void _Actor_Stop(struct Actor *a);
extern void _Actor_SetAnim(struct Actor *a, int n);
extern void _Actor_TravelTo(struct Actor *a, int x, int y, int z);
extern void _Actor_WaitMovement(struct Actor *a);
extern void Func_8092b08(int a, int b);

void Func_8092208(int a, int b, int c)
{
    struct Actor *act;
    short m;
    int t;

    act = GetFieldActor(a);
    if (act == 0)
        return;
    m = *(short *)((char *)act + 0xa) % 16;
    *((unsigned char *)act + 0x5b) = 0;
    _Actor_Stop(act);
    _Actor_SetAnim(act, 2);
    t = 8 - m;
    _Actor_TravelTo(act, act->f8 + (t << 16), act->fc, act->f10);
    _Actor_WaitMovement(act);
    Func_8092b08(a, b);
    _Actor_TravelTo(act, act->f8, act->fc, act->f10 + (c << 16));
}
