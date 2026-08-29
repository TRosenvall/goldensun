struct Actor {
    unsigned char pad00[6];
    unsigned short f6;
    unsigned char pad08[0x12 - 8];
    short f12;
};

extern struct Actor *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void OvlFunc_926_2008e94(void);
extern void OvlFunc_926_2008bf4(void);
extern void OvlFunc_926_2008db4(void);
extern void OvlFunc_926_2008cd4(void);
extern void __Func_80933d4(int a, int b);
extern void __Func_8093500(int a, int b);
extern void __Func_8093530(void);
extern int __GetFlag(int id);
extern void OvlFunc_926_200902c(int a);
extern void OvlFunc_926_2009160(void);
extern void OvlFunc_926_2009494(void);
extern void OvlFunc_926_2009dbc(void);

void OvlFunc_926_20093b8(void)
{
    struct Actor *a;
    unsigned short v;

    a = __MapActor_GetActor(0);
    __CutsceneStart();
    v = a->f6;
    if ((unsigned short)(v - 0x2000) <= 0x3fff) {
        OvlFunc_926_2008e94();
    } else if ((unsigned short)(v - 0x6000) <= 0x3fff) {
        OvlFunc_926_2008bf4();
    } else if ((unsigned short)(v + 0x6000) <= 0x3fff) {
        OvlFunc_926_2008db4();
    } else {
        OvlFunc_926_2008cd4();
    }
    __Func_80933d4(0x80 << 9, 0x80 << 6);
    __Func_8093500(0x14, 1);
    __Func_8093530();
    if (a->f12 <= 0xd1) {
        if (__GetFlag(0x89a) == 0 || __GetFlag(0x89b) != 0) {
            OvlFunc_926_200902c(0);
        } else {
            OvlFunc_926_2009160();
        }
        __CutsceneEnd();
    } else {
        if (__GetFlag(0x89b) != 0) {
            OvlFunc_926_200902c(2);
        } else if (__GetFlag(0x89a) == 0) {
            OvlFunc_926_2009494();
        } else {
            OvlFunc_926_2009dbc();
        }
        __CutsceneEnd();
    }
}
