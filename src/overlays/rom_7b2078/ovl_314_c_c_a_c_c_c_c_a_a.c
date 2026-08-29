/* Cluster OvlFunc_926_200a484..OvlFunc_926_200a484 extracted from goldensun/asm/overlays/rom_7b2078/ovl_314_c_c_a_c_c_c_c_a_a.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * THREE SITES IN ONE FUNCTION, which is what makes this the best evidence so
 * far that the basic-block lever is a rule and not a coincidence. `a`, `b` and
 * `c` are all assigned before the first `__GetFlag`, and all three are used in
 * the `else` arm -- a different basic block -- so gcc rematerialises each at
 * its call and splits each pair around `mov r0`:
 *
 *     mov r1,#0x80 / mov r2,#0x80 / mov r0,#0 / lsl r1,#8 / lsl r2,#7
 *     mov r1,#0x99 / mov r0,#0 / lsl r1,#1
 *
 * NOTE THE FOURTH CONSTANT IS A LITERAL. `0x98 << 1` in the __Func_809218c call
 * is NOT interleaved in the ROM -- `mov r1,#0x98 / lsl r1,#1 / mov r2,#0xd8 /
 * mov r0,#0` is contiguous -- so it must stay written at the call site. Applying
 * the lever everywhere would break it. Read each site off the ROM.
 */
extern unsigned char L477a[] __asm__(".L477a");

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern int __GetFlag(int id);
extern void __Func_801776c(int id, int b);
extern void __PlaySound(int id);
extern void __Func_8010560(void *p, int a, int b);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __Func_80921c4(int slot, int a, int b);
extern void __Func_809218c(int slot, int a, int b);
extern void __CutsceneWait(int n);
extern void __Func_8091e9c(int a);

void OvlFunc_926_200a484(void)
{
    int a;
    int b;
    int c;

    a = 0x80 << 8;
    b = 0x80 << 7;
    c = 0x99 << 1;
    __CutsceneStart();
    if (!__GetFlag(0x89a) && !__GetFlag(0x895)) {
        __Func_801776c(0x18ad, 1);
        __CutsceneEnd();
    } else {
        __PlaySound(0x9e);
        __Func_8010560(L477a, 0x4e, 0xd);
        __MapActor_SetSpeed(0, a, b);
        __Func_80921c4(0, c, 0xf8);
        __Func_809218c(0, 0x98 << 1, 0xd8);
        __CutsceneWait(0x14);
        __Func_8091e9c(4);
        __CutsceneEnd();
    }
}
