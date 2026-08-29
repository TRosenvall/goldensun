typedef struct {
    unsigned char pad00[0x1c2];
    unsigned short f1c2;
    unsigned char pad1c4[0x2c0 - 0x1c4];
} GlobalState;

struct A { unsigned char pad00[8]; int f8; int fc; int f10; };

extern unsigned char iwram_3001ebc[];
extern GlobalState gState;
extern struct A *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __WaitFrames(int n);
extern int __GetFlag(int id);
extern void OvlFunc_907_2008fa0(void);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_800fe9c(void);

void OvlFunc_907_2008d10(void)
{
    struct A *a;

    *(int *)(*(char **)iwram_3001ebc + (0xe0 << 1)) = 0x204;
    OvlFunc_907_2008fa0();
    if ((unsigned short)(gState.f1c2 - 3) <= 1) {
        if (__GetFlag(0x109) == 0) {
            a = __MapActor_GetActor(0);
            __CutsceneStart();
            a->fc = 0x80 << 13;
            __Func_80933f8(a->f8, 0x80 << 13, a->f10, 0);
            __Func_800fe9c();
            __CutsceneEnd();
            __WaitFrames(1);
        }
    }
}
