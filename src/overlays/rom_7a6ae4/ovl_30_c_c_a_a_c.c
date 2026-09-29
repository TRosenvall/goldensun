extern unsigned char *iwram_3001ebc;

extern int __GetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Func_8092b08(int a, int b);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_920_2008904(int slot);

void OvlFunc_920_2008538(void)
{
    *(int *)(iwram_3001ebc + 0x1c0) = 0x204;
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x12), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x13), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x14), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x15), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x16), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x17), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x18), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x19), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x1a), 0);
    __MapActor_SetAnim(0x12, 5);
    __MapActor_SetAnim(0x13, 5);
    __MapActor_SetAnim(0x14, 5);
    __MapActor_SetAnim(0x15, 5);
    __MapActor_SetAnim(0x16, 5);
    __MapActor_SetAnim(0x17, 3);
    __MapActor_SetAnim(0x18, 3);
    __MapActor_SetAnim(0x19, 3);
    __MapActor_SetAnim(0x1a, 3);
    __MapActor_SetAnim(9, 2);
    __MapActor_SetAnim(0xa, 2);
    __MapActor_SetAnim(0xb, 2);
    __MapActor_SetAnim(0xc, 2);
    __MapActor_SetAnim(0xd, 2);
    __MapActor_SetAnim(0xe, 2);
    OvlFunc_920_2008904(0x12);
    OvlFunc_920_2008904(0x13);
    OvlFunc_920_2008904(0x14);
    OvlFunc_920_2008904(0x15);
    OvlFunc_920_2008904(0x16);
    OvlFunc_920_2008904(0x17);
    OvlFunc_920_2008904(0x18);
    OvlFunc_920_2008904(0x19);
    OvlFunc_920_2008904(0x1a);
    OvlFunc_920_2008904(9);
    OvlFunc_920_2008904(0xa);
    OvlFunc_920_2008904(0xb);
    OvlFunc_920_2008904(0xc);
    OvlFunc_920_2008904(0xd);
    OvlFunc_920_2008904(0xe);
    if (__GetFlag(0x883) != 0) {
        __MapActor_SetPos(8, 0, 0);
        __MapActor_SetAnim(0xf, 5);
        __MapActor_GetActor(0xf)[0x55] = 0;
        *(int *)(__MapActor_GetActor(0xf) + 0xc) = 0xfffc0000;
        { unsigned char *q = __MapActor_GetActor(0xf) + 0x23;
          unsigned char t = 2;
          *q = t | *q; }
        __Func_8092b08(0xf, 2);
        { int e5 = 0x12; int e6 = 0xe; __Func_8010704(0, 0, 1, 1, e5, e6); }
    } else {
        __MapActor_SetAnim(8, 2);
        __Actor_SetSpriteFlags(__MapActor_GetActor(8), 0);
        __MapActor_SetAnim(0xf, 1);
    }
    __MapActor_SetAnim(0x10, 1);
    if (__GetFlag(0x302) != 0) {
        __MapActor_SetAnim(0x11, 1);
        __Func_8010704(0, 1, 1, 1, 0x24, 0x16);
        __Func_8010704(0, 2, 1, 1, 0x24, 0x18);
    } else {
        __MapActor_SetAnim(0x11, 5);
        __Func_8010704(1, 1, 1, 1, 0x24, 0x16);
        __Func_8010704(1, 2, 1, 1, 0x24, 0x18);
    }
    if (__GetFlag(0x303) != 0)
        { register int q0 __asm__("r0"); register int q1 __asm__("r1");
          q0 = 0xb; q1 = 0x23a0000; __MapActor_SetPos(q0, q1, 0xbc << 17); }
    if (__GetFlag(0xc1 << 2) != 0)
        { register int q0 __asm__("r0"); register int q1 __asm__("r1");
          q0 = 0xc; q1 = 0x23a0000; __MapActor_SetPos(q0, q1, 0xbc << 17); }
}
