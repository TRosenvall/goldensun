/* OvlFunc_897_200ac1c -- NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_791794/ovl_30_c_c_a_c_c_c.s
 * Best screen: 14 differing of 52, streams the same length.
 *
 * BLOCKER CLASS: which of two parameters lands in which callee-saved register.
 *
 *     rom    mov r8, r1 / mov r6, r0     (y in r8, x in r6)
 *     ours   mov r8, r0 / mov r6, r1     (x in r8, y in r6)
 *
 * Everything else is right -- the two `<< 16` conversions, the -1 built as
 * `mov` + `neg`, the six field writes in the ROM's order with the +8 store
 * last, and both __Func_8091200 calls. The exchange propagates through every
 * later instruction that names either register.
 *
 * TRIED AND IDENTICAL AT 14: swapping the declaration order of the two locals;
 * swapping the order of the two shift assignments.
 *
 * Same class as src/non_matching/rom_15000/8028df4.c (YesNoMenu), where four
 * parameters land in the wrong four high registers. The parameters arrive where
 * the ABI puts them and nothing in the body reorders which one gets saved
 * first.
 */
struct A {
    unsigned char pad00[8];
    int f8;
    unsigned char pad0c[4];
    int f10;
    unsigned char pad14[0x24 - 0x14];
    int f24;
    unsigned char pad28[4];
    int f2c;
    unsigned char pad30[8];
    int f38;
    unsigned char pad3c[4];
    int f40;
};

extern struct A *__Func_8093554(void);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8091200(int a, int b);
extern void __Func_8091254(int a);
extern void __WaitFrames(int n);
extern void __Func_800fe9c(void);

void OvlFunc_897_200ac1c(int x, int y)
{
    struct A *a;
    int fx;
    int fy;

    a = __Func_8093554();
    fy = y << 16;
    fx = x << 16;
    __Func_80933f8(fx, -1, fy, 1);
    __Func_8091200(0, 0);
    __Func_8091254(0x14);
    __WaitFrames(0x28);
    a->f10 = fy;
    a->f38 = 0x80 << 24;
    a->f40 = 0x80 << 24;
    a->f24 = 0;
    a->f2c = 0;
    a->f8 = fx;
    __WaitFrames(5);
    __Func_800fe9c();
    __WaitFrames(5);
    __Func_8091200(0x80 << 9, 0);
    __Func_8091254(0x14);
    __WaitFrames(0x1e);
}
