struct Q {
    unsigned char pad00[5];
    unsigned char f5;
    unsigned char pad06[0x16 - 6];
    unsigned char f16;
};

struct Spr {
    unsigned char pad00[0x28];
    struct Q *f28;
};

extern unsigned char *_GetUnit(int id);
extern void **GetBattleActor(int id);
extern struct Spr *Func_80b7f70(void *a, int i);
extern void _Sprite_SetAnim(struct Spr *s, int n);
extern void WaitFrames(int n);
extern void Func_80bac6c(int id);
extern void _Func_800bf34(void *arr, int n);
extern void Func_80b7e60(int id);

void Func_80bace8(int id)
{
    unsigned char *u;
    struct Spr *s;
    struct Q *q;
    struct Spr *arr[4];
    int i;

    u = _GetUnit(id);
    i = 0;
    while ((s = Func_80b7f70(*GetBattleActor(id), i)) != 0) {
        if (u[0x12a] != 1) {
            _Sprite_SetAnim(s, 4);
        } else {
            _Sprite_SetAnim(s, 5);
        }
        i++;
    }
    if (u[0x12a] == 1) {
        i = 0;
        while ((s = Func_80b7f70(*GetBattleActor(id), i)) != 0) {
            arr[i] = s;
            q = s->f28;
            q->f5 = 6;
            q->f16 = 0xff;
            i++;
        }
        WaitFrames(4);
        Func_80bac6c(id);
        _Func_800bf34(arr, i);
        Func_80b7e60(id);
    }
}
