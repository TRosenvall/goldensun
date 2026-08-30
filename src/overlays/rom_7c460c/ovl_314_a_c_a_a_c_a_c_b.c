/* Cluster OvlFunc_939_20085f0..OvlFunc_939_20085f0 extracted from
 * goldensun/asm/overlays/rom_7c460c/ovl_314_a_c_a_a_c_a_c.s.
 *
 * Total .text for this TU = 204 bytes.
 *
 * ALL TEN STACK ARGUMENTS ARE NAMED, and it is all-or-nothing. The first
 * transcription was 20 of 81 and the whole diff was one shape: the ROM builds
 * BOTH stack arguments into separate registers before storing either
 * (`mov r3, #0x2e / mov r2, #4 / str r3, [sp] / str r2, [sp, #4]`), while gcc
 * reuses r3 for both. Naming every stack argument as its own local was exact
 * on the next screen.
 */
struct A {
    unsigned char pad00[0x6c];
    int f6c;
};

extern struct A *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_808edac(int a, int b, int c);
extern void __ClearFlag(int id);
extern void __MapActor_SetAnim(int slot, int n);
extern void __Actor_SetColorswap(struct A *a, int n);
extern void __StopTask(void (*f)(void));
extern void __SetFlag(int id);
extern void OvlFunc_939_2008468(void);

void OvlFunc_939_20085f0(void)
{
    int e1, f1;
    int e2, f2;
    int e3, f3;
    int e4, f4;
    int e5, f5;

    __MapActor_GetActor(0);
    __MapActor_SetPos(0x13, 0, 0);
    __MapActor_SetPos(8, 0, 0);
    e1 = 0x2e;
    f1 = 4;
    __Func_80105d4(0x26, 0x26, 1, 1, e1, f1);
    e2 = 0xd;
    f2 = 3;
    __Func_8010704(0x25, 0x25, 3, 3, e2, f2);
    e3 = 0xe;
    f3 = 2;
    __Func_8010704(0x25, 0x25, 1, 1, e3, f3);
    e4 = 7;
    f4 = 0x10;
    __Func_8010704(8, 0x10, 1, 1, e4, f4);
    __Func_808edac(0x66, 0, 0);
    e5 = 1;
    f5 = 0xf;
    __Func_80105d4(0x20, 0x2a, 3, 2, e5, f5);
    __ClearFlag(0x80 << 2);
    __MapActor_SetAnim(8, 1);
    __MapActor_GetActor(8)->f6c = 0;
    __Actor_SetColorswap(__MapActor_GetActor(8), 0);
    __StopTask(OvlFunc_939_2008468);
    __SetFlag(0x201);
}
