extern short gState[];

extern int _GetFlag(int id);
extern void _ClearFlag(int id);
extern void _PlaySound(int id);
extern void _Actor_SetAnim(char *actor, int anim);
extern void Player_ExitStairs(int n);
extern char *GetFieldActor(int n);
extern void CutsceneWait(int n);
extern void WaitFrames(int n);
extern void SetCameraTarget(int a, int b);
extern void Func_80933f8(int a, int b, int c, int d);

int Func_8094428(void)
{
    int id;
    int r;
    int who;
    char *p;

    id = 0x90 << 1;
    r = 0;
    if (_GetFlag(id)) {
        Player_ExitStairs(0x18);
        _ClearFlag(id);
        r = 1;
    } else {
        id = 0x121;
        if (_GetFlag(id)) {
            Player_ExitStairs(0x17);
            _ClearFlag(id);
            r = 2;
        } else {
            id = 0x91 << 1;
            if (_GetFlag(id)) {
                _ClearFlag(id);
                who = *(int *)&gState[0xfa];
                p = GetFieldActor(who);
                *(int *)(p + 0xc) = *(int *)(p + 0xc) + (0xa0 << 16);
                Func_80933f8(-1, -1, -1, 0);
                while (*(int *)(p + 0xc) + *(int *)(p + 0x28) > *(int *)(p + 0x14))
                    WaitFrames(1);
                _PlaySound(0x9f);
                *(int *)(p + 0xc) = *(int *)(p + 0x14);
                _Actor_SetAnim(p, 0x16);
                CutsceneWait(0xf);
                SetCameraTarget(who, 1);
                r = 3;
            }
        }
    }
    return r;
}
