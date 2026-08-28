extern unsigned char *_GetUnit(int id);
extern void **GetBattleActor(int id);
extern void _Actor_SetAnim(void *actor, int anim);
extern int _Actor_SetAnimSpeed(void *actor, int speed);

int Func_80b7aac(int id)
{
    unsigned char *u;
    int a;

    u = _GetUnit(id);
    a = 1;
    if (*(short *)(u + 0x38) != 0) {
        if (u[0x13c] != 0 || u[0x13b] != 0 || u[0x145] != 0)
            a = (u[0x12a] != 1) << 2;
    } else {
        a = 4 + (u[0x12a] == 1);
    }
    _Actor_SetAnim(*GetBattleActor(id), a);
    return _Actor_SetAnimSpeed(*GetBattleActor(id), 0xe | (id & 3));
}
