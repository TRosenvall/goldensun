extern char *iwram_3001ebc;
extern unsigned char gScript_882__0200cd6c[];

struct Actor { unsigned char pad00[8]; int f8; int fc; int f10; };

extern struct Actor *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __CutsceneWait(int n);
extern void __Func_808f1c0(int a, int b);
extern void __Func_8091a58(int a, int b);
extern unsigned char *__Func_8093554(void);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void OvlFunc_932_20086dc(void);
extern void __SetMapEvents(unsigned char *s);

void OvlFunc_932_200ad58(void)
{
    struct Actor *a;
    char **pp;
    int k;
    int n;

    a = __MapActor_GetActor(0);
    __CutsceneStart();
    __MapActor_SetPos(0xa, 0, 0);
    __MapActor_SetPos(8, 0, 0);
    pp = &iwram_3001ebc;
    k = 0xe0 << 1;
    *(int *)(*pp + k) = 0x201;
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x14);
    __Func_808f1c0(0xca, 3);
    __Func_8091a58(0xca, 0);
    __Func_8093554()[0x55] = 0;
    __Func_80933d4(0x19999, 0x3333);
    n = 0;
    __Func_80933f8(0xc8 << 15, n, 0xf9 << 16, 1);
    __Func_8093530();
    __CutsceneWait(0x14);
    OvlFunc_932_20086dc();
    __Func_80933f8(a->f8, a->fc, a->f10, 1);
    __Func_8093530();
    __SetMapEvents(gScript_882__0200cd6c);
    *(int *)(*pp + k) = 0x81 << 2;
    __CutsceneEnd();
}
