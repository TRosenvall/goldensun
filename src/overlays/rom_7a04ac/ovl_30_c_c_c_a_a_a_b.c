/* Cluster OvlFunc_913_2008c68..OvlFunc_913_2008c68 extracted from
 * goldensun/asm/overlays/rom_7a04ac/ovl_30_c_c_c_a_a_a.s.
 *
 * Total .text for this TU = 212 bytes.
 *
 * OvlFunc_913_2008244 IS DELIBERATELY LEFT UNDECLARED. The first transcription
 * was 4 of 85, all at that one call: the ROM wants `mov r0, #0x2` LAST, after
 * every other argument, and withholding the prototype is what moves it there.
 * Do not add a declaration for it.
 *
 * The `int` return type is read off the epilogue, which pops into r1.
 *
 * NOTE FOR ANY FUTURE EDIT: tryc warns that this reference keeps its literal
 * pool INSIDE the function, so the screen cannot see PC-relative offsets.
 * Verify changes with `make compare`, not with the screen alone.
 */
struct A {
    unsigned char pad00[0x23];
    unsigned char f23;
};

extern unsigned char gState[];
extern void OvlFunc_913_20088c0(int a);
extern int __GetFlag(int id);
extern struct A *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Actor_SetSpriteFlags(struct A *a, int n);
extern void OvlFunc_913_2008d3c(void);
extern void __MapActor_SetPos(int slot, int x, int y);

int OvlFunc_913_2008c68(void)
{
    unsigned char *g;
    int area;
    int e1, f1;
    int e2, f2;

    OvlFunc_913_20088c0(0xa);
    if (__GetFlag(0x80 << 2) != 0) {
        __MapActor_GetActor(0xa)->f23 = 2;
        e1 = 0x13;
        f1 = 0x11;
        __Func_8010704(0, 0x11, 2, 4, e1, f1);
        e2 = 4;
        f2 = 0;
        OvlFunc_913_2008244(2, 0x14, 0x11, 1, e2, f2);
        __Actor_SetSpriteFlags(__MapActor_GetActor(0xa), 0);
    }
    OvlFunc_913_20088c0(8);
    OvlFunc_913_20088c0(9);
    g = gState;
    area = *(short *)(g + 0x1c2);
    if (area == 4 && __GetFlag(0x843) == 0)
        OvlFunc_913_2008d3c();
    if (__GetFlag(0x845) != 0) {
        __MapActor_SetPos(0x11, 0, 0);
        __MapActor_SetPos(0x12, 0, 0);
        __MapActor_SetPos(0x13, 0, 0);
        __MapActor_SetPos(0x14, 0, 0);
        __MapActor_SetPos(0x15, 0, 0);
    }
    return 0;
}
