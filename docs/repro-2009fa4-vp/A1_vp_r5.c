typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned int gKeyHeld;
extern short L2430[] __asm__(".L2430");

extern unsigned char *__GetFieldActor(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __WaitFrames(int n);
extern int __Func_8012038(int a, int b, int c);
extern int __Func_8011f54(int a, int b, int c);
extern void __vec3_translate(int len, int dir, int *v);
extern void __Actor_TravelTo(unsigned char *a, int x, int y, int z);
extern void __Actor_SetAnim(unsigned char *a, int n);
extern void __Actor_SetAnimSpeed(unsigned char *a, int n);
extern void __Actor_WaitMovement(unsigned char *a);
extern void OvlFunc_921_2009f24(void);

void OvlFunc_921_2009fa4(void)
{
    unsigned char *a;
    unsigned char *b22;
    unsigned char *f;
    int v[3];
    unsigned int base;
    unsigned int off;
    int dir;
    int first;
    int t;
    int px;
    int pz;
    register int lim __asm__("r11");
    register int *vp __asm__("r5");

    base = (unsigned int)&gState;
    off = 0xfa;
    off <<= 1;
    base += off;
    a = __GetFieldActor(*(int *)base);
    vp = v;
top:
    dir = L2430[(gKeyHeld >> 4) & 0xf];
    if (dir << 16 == (int)0xffff0000)
        return;
    __CutsceneStart();
    lim = 0x80 << 12;
    vp[0] = (*(int *)(a + 8) & 0xfff00000) + lim;
    vp[1] = *(int *)(a + 0xc);
    vp[2] = (*(int *)(a + 0x10) & 0xfff00000) + lim;
    pz = vp[2];
    px = vp[0];
    b22 = a + 0x22;
    first = __Func_8012038(*b22, px, pz);
    __vec3_translate(0x80 << 13, dir, v);
    t = __Func_8012038(*b22, vp[0], vp[2]);
    if (t == 0xff)
        goto face;
    if (__Func_8011f54(*b22, vp[0], vp[2]) - *(int *)(a + 0xc) > lim)
        goto face;
    vp[0] = px;
    vp[2] = pz;
    *(int *)(a + 0x30) = 0x80 << 10;
    *(int *)(a + 0x34) = 0x1999;
    { register int h3 __asm__("r3"); h3 = 0; *(short *)(a + 0x64) = h3; }
    __Actor_TravelTo(a, px, *(int *)(a + 0xc), pz);
    __Actor_SetAnim(a, 2);
    __Actor_SetAnimSpeed(a, 0x30);
    __Actor_WaitMovement(a);
    *(int *)(a + 0x6c) = (int)OvlFunc_921_2009f24;
    goto step;
face:
    *(short *)(a + 6) = dir;
    goto tail;
walk:
    if (__Func_8011f54(*b22, vp[0], vp[2]) - *(int *)(a + 0xc) > (0x80 << 12))
        goto settle;
    px = vp[0];
    pz = vp[2];
    *(int *)(a + 0x30) = 0x80 << 10;
    *(int *)(a + 0x34) = 0x1999;
    __Actor_TravelTo(a, vp[0], vp[1], vp[2]);
    __Actor_WaitMovement(a);
    if (t != first)
        goto stop;
step:
    { register int q0 __asm__("r0"); register int q1 __asm__("r1");
      register int q2 __asm__("r2");
      q2 = (int)v; q0 = 0x80; q1 = dir; q0 <<= 13;
      __vec3_translate(q0, q1, (int *)q2); }
    t = __Func_8012038(*b22, vp[0], vp[2]);
    if (t != 0xff)
        goto walk;
settle:
    *(int *)(a + 0x30) = 0x80 << 10;
    *(int *)(a + 0x34) = 0x80 << 9;
    __Actor_TravelTo(a, px, *(int *)(a + 0xc), pz);
    __Actor_WaitMovement(a);
    __WaitFrames(2);
    goto top;
stop:
    *(int *)(a + 0x6c) = 0;
    f = a + 0x5a;
    { register int m3 __asm__("r3"); m3 = 1; m3 |= *f; *f = m3; }
    *(int *)(a + 0x34) = 0x80 << 7;
tail:
    __WaitFrames(0xa);
    __CutsceneEnd();
}
