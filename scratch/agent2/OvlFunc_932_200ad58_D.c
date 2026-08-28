typedef unsigned char u8;

struct Ent {
    u8 pad0[8];
    int f8;
    int fc;
    int f10;
};

extern u8 *iwram_3001ebc;
extern u8 gScript_882__0200cd6c[];

extern struct Ent *__MapActor_GetActor(int i);
extern void __CutsceneStart(void);
extern void __MapActor_SetPos(int a, int b, int c);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __CutsceneWait(int n);
extern void __Func_808f1c0(int a, int b);
extern void __Func_8091a58(int a, int b);
extern u8 *__Func_8093554(void);
extern int __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void OvlFunc_932_20086dc(void);
extern void __SetMapEvents(u8 *s);
extern void __CutsceneEnd(void);

void OvlFunc_932_200ad58(void)
{
    struct Ent *e;
    u8 *p;
    int n;
    int y;

    e = __MapActor_GetActor(0);
    __CutsceneStart();
    __MapActor_SetPos(0xa, 0, 0);
    __MapActor_SetPos(8, 0, 0);
    n = 0xe0 * 2;
    *(int *)(iwram_3001ebc + n) = 0x201;
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x14);
    __Func_808f1c0(0xca, 3);
    __Func_8091a58(0xca, 0);
    p = __Func_8093554();
    p[0x55] = 0;
    __Func_80933d4(0x19999, 0x3333);
    y = 0xf9;
    y <<= 16;
    __Func_80933f8(0x640000, 0, y, 1);
    __Func_8093530();
    __CutsceneWait(0x14);
    OvlFunc_932_20086dc();
    __Func_80933f8(e->f8, e->fc, e->f10, 1);
    __Func_8093530();
    __SetMapEvents(gScript_882__0200cd6c);
    *(int *)(iwram_3001ebc + n) = 0x204;
    __CutsceneEnd();
}
