/* OvlFunc_939_200849c  --  0x0200849c, was
 * asm/overlays/rom_7c460c/ovl_314_a_c_a_a_c_a_c_a.s (this function alone), so it
 * converts whole. Matched from scratch; the -1 argument pair closed on the
 * dominating-block locals lever (reports/arg-interleave.md), and a stack argument
 * shared by two calls wants one local per call.
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_808edac(int a, int b, int c);
extern void __Func_8092304(int a, int b, int c);
extern int __GetFlag(int id);
extern void __ClearFlag(int id);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __StartTask(void (*fn)(void), int n);
extern void OvlFunc_939_2008468(void);

void OvlFunc_939_200849c(void)
{
    unsigned char *a;
    int x, y;
    int e, f;
    int k, f1;
    int m1, m2;
    int px, py;

    a = __MapActor_GetActor(0);
    x = *(int *)(a + 8) / 0x100000;
    y = *(int *)(a + 0x10) / 0x100000;
    px = 0xe8 << 16;
    py = 0x90 << 15;
    if (__GetFlag(0xf27) == 0) {
        m1 = -1;
        m2 = -1;
        if (x == 7 && y == 0x10)
            __Func_8092304(0, 0, 0x10);
        __Func_808edac(0x66, m1, m2);
        e = 7;
        f = 0x10;
        __Func_8010704(0x1c, 0x1f, 1, 1, e, f);
    }
    k = 0x2e;
    f1 = 4;
    __Func_80105d4(0x2f, 4, 1, 1, k, f1);
    e = 0xd;
    f = 3;
    __Func_8010704(0x22, 0x25, 3, 3, e, f);
    __MapActor_SetPos(8, px, py);
    *(int *)(__MapActor_GetActor(8) + 0xc) = 0;
    if (__GetFlag(0x202)) {
        __Func_80105d4(0x29, 0x31, 3, 4, 1, 0xe);
        __Func_80105d4(0x2c, 0x31, 3, 4, 0x21, 0xe);
        __Func_80105d4(0x2f, 0x31, 3, 4, 1, k);
    } else {
        __MapActor_SetPos(0x13, 0xe0 << 14, 0x86 << 17);
        __Actor_SetSpriteFlags(__MapActor_GetActor(0x13), 0);
        a = __MapActor_GetActor(0x13);
        if (a != 0) {
            a[0x55] = 8;
            *(int *)(a + 0xc) = 0x80 << 13;
            a[0x23] = 2;
            *(int *)(a + 0x18) = 0x13333;
            *(int *)(a + 0x1c) = 0xc0 << 9;
        }
    }
    __StartTask(OvlFunc_939_2008468, 0xc8 << 4);
    __ClearFlag(0x201);
}
