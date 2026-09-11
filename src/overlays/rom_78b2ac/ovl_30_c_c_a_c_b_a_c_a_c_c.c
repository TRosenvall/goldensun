extern unsigned char *iwram_3001ebc;
extern unsigned short L2ddc __asm__(".L2ddc");
extern unsigned short L2de0 __asm__(".L2de0");
extern unsigned short L2de4 __asm__(".L2de4");
extern int L2de8 __asm__(".L2de8");
extern unsigned short L2dec __asm__(".L2dec");

extern void OvlFunc_890_2008238(void);
extern void OvlFunc_890_20082cc(void);
extern void OvlFunc_890_2008360(void);
extern void OvlFunc_890_20083f4(void);
extern void OvlFunc_890_2008d9c(void);
extern void OvlFunc_890_2008ef8(void);
extern void OvlFunc_890_200901c(void);
extern void OvlFunc_890_2009140(void);
extern void OvlFunc_890_2009264(void);
extern void OvlFunc_890_200a5fc(int a, int b);

extern void __CutsceneStart(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __PlaySound(int id);
extern unsigned int __Random(void);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __StartTask(void (*f)(void), int n);
extern void __StopTask(void (*f)(void));
extern void __Func_8091e9c(int a);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

#define BEAT(up, down, lim, w) i = 0; \
    do { \
        __PlaySound(0xf6); \
        up(); \
        __CutsceneWait(w); \
        __PlaySound(0xf6); \
        i++; \
        down(); \
        __CutsceneWait(w); \
    } while (i != lim)

#define RND(v) v = (__Random() * 60 >> 16) + 0x14

#define TILES(fa, fb, c0, c1, c2, c3) \
    if (({ PIN1; q0 = fa; __GetFlag(q0); }) && ({ PIN1; q0 = fb; __GetFlag(q0); })) { \
        { PIN1; q0 = fb; __ClearFlag(q0); } \
        { int e = 2, f = 1; __CopyMapTiles(c0, c1, c2, c3, e, f); } \
    } else if (({ PIN1; q0 = fa; __GetFlag(q0); }) && !({ PIN1; q0 = fb; __GetFlag(q0); })) { \
        { PIN1; q0 = fb; __SetFlag(q0); } \
    }

void OvlFunc_890_2009510(void)
{
    register int i __asm__("r5");

    __CutsceneStart();
    OvlFunc_890_2009264();
    L2de4 = 0;
    L2ddc = 0;
    L2de0 = 0;
    L2dec = 0;
    __MessageID(0x1001);
    { PIN3; q1 = 0x80; q0 = 0x10; q1 <<= 7; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 0x10; q1 <<= 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __MapActor_Jump(0x10, 6, 0x1e);
    { PIN4; q1 = 1; q2 = 0xae; q1 = -q1; q2 <<= 16; q3 = 1; q0 = 0x23e0000;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __CutsceneWait(0x1e);
    { PIN2; q0 = 0x8010; q1 = 0x14; OvlFunc_890_200a5fc(q0, q1); }
    BEAT(OvlFunc_890_2008238, OvlFunc_890_2008360, 4, 0xc);
    { PIN2; q1 = 6; q0 = 0x8010; OvlFunc_890_200a5fc(q0, q1); }
    RND(L2de4);
    RND(L2ddc);
    RND(L2de0);
    RND(L2dec);
    L2de8 = 0;
    { PIN2; q1 = 0xc8; q1 <<= 4; q0 = (int)OvlFunc_890_2008d9c;
      __StartTask((void (*)(void))q0, q1); }
    { PIN2; q1 = 0xc8; q1 <<= 4; q0 = (int)OvlFunc_890_2008ef8;
      __StartTask((void (*)(void))q0, q1); }
    { PIN2; q1 = 0xc8; q1 <<= 4; q0 = (int)OvlFunc_890_200901c;
      __StartTask((void (*)(void))q0, q1); }
    { PIN2; q1 = 0xc8; q0 = (int)OvlFunc_890_2009140; q1 <<= 4;
      __StartTask((void (*)(void))q0, q1); }
    BEAT(OvlFunc_890_2008238, OvlFunc_890_2008360, 6, 5);
    BEAT(OvlFunc_890_2008238, OvlFunc_890_2008360, 8, 4);
    BEAT(OvlFunc_890_2008238, OvlFunc_890_2008360, 0xa, 3);
    BEAT(OvlFunc_890_2008238, OvlFunc_890_2008360, 0xc, 2);
    { int e = 4, f = 2; __CopyMapTiles(0x2d, 0x1e, 0x22, 0xa, e, f); }
    { PIN3; q2 = 0x28; q0 = 0x10; q1 = 6; __MapActor_Jump(q0, q1, q2); }
    { PIN2; q0 = 0x8010; q1 = 6; OvlFunc_890_200a5fc(q0, q1); }
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0x10; q1 <<= 10; q2 <<= 9;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0x90; q2 = 0x8c; q1 <<= 2; q2 <<= 1; q0 = 0x10;
      __Func_80921c4(q0, q1, q2); }
    __StopTask(OvlFunc_890_2008d9c);
    __StopTask(OvlFunc_890_2008ef8);
    __StopTask(OvlFunc_890_200901c);
    __StopTask(OvlFunc_890_2009140);
    {
        register unsigned char *b __asm__("r1");
        register unsigned char *r __asm__("r2");
        register int v __asm__("r3");
        b = iwram_3001ebc;
        v = 0xe0; v <<= 1;
        r = b + v;
        v -= 0xc0;
        *(int *)r = v;
        v += 0xc8;
        r = b + v;
        v = 0x20;
        *(int *)r = v;
    }
    __MapTransitionOut();
    __WaitMapTransition();
    __Func_8091e9c(4);
}

void OvlFunc_890_2009790(void)
{
    register int i __asm__("r5");
    int m;

    __CutsceneStart();
    TILES(0x80b, 0x826, 0x2d, 0x1c, 0x22, 0xa);
    TILES(0x80c, 0x827, 0x2f, 0x1c, 0x24, 0xa);
    TILES(0x80d, 0x828, 0x2d, 0x1d, 0x22, 0xb);
    TILES(0x80e, 0x829, 0x2f, 0x1d, 0x24, 0xb);
    OvlFunc_890_2009264();
    { PIN3; q1 = 0x80; q0 = 0x10; q1 <<= 7; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    __MapActor_Jump(0x10, 6, 0x1e);
    { PIN4; q1 = 1; q2 = 0xae; q0 = 0x23e0000; q1 = -q1; q2 <<= 16; q3 = 1;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __CutsceneWait(0x1e);
    BEAT(OvlFunc_890_20082cc, OvlFunc_890_20083f4, 4, 0xc);
    BEAT(OvlFunc_890_20082cc, OvlFunc_890_20083f4, 6, 8);
    BEAT(OvlFunc_890_20082cc, OvlFunc_890_20083f4, 8, 6);
    BEAT(OvlFunc_890_20082cc, OvlFunc_890_20083f4, 0xa, 4);
    BEAT(OvlFunc_890_20082cc, OvlFunc_890_20083f4, 0xc, 2);
    __PlaySound(0xf6);
    OvlFunc_890_20082cc();
    __CutsceneWait(6);
    if (!({ PIN1; q0 = 0x822; __GetFlag(q0); })) {
        m = 0x8010;
        __MessageID(0x1025);
        OvlFunc_890_200a5fc(m, 6);
        __MapActor_DoAnim(0x10, 3);
        OvlFunc_890_200a5fc(m, 6);
    }
    {
        register unsigned char *b __asm__("r1");
        register unsigned char *r __asm__("r2");
        register int v __asm__("r3");
        b = iwram_3001ebc;
        v = 0xe0; v <<= 1;
        r = b + v;
        v -= 0xc0;
        *(int *)r = v;
        v += 0xc8;
        r = b + v;
        v = 0x20;
        *(int *)r = v;
    }
    __MapTransitionOut();
    __WaitMapTransition();
    __Func_8091e9c(5);
}
