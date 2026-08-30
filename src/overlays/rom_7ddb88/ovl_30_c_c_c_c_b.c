/* Cluster OvlFunc_955_20092f0..OvlFunc_955_20092f0 extracted from
 * goldensun/asm/overlays/rom_7ddb88/ovl_30_c_c_c_c.s.
 *
 * THIS FUNCTION WAS PARKED AND THE PARK WAS WRONG, which is worth more than the
 * function.  It sat at 123 of 123 lines with 15 differing, the instruction
 * multiset exact, every difference a one-slot displacement of `mov r0` at six
 * call sites.  Seven spellings of the calls left the count EXACTLY unchanged,
 * and that invariance was read as proof the residue lived below the source.  It
 * did not.  It lived in the DECLARATIONS.
 *
 * TWO LEVERS, AND THEY ARE NOT INTERCHANGEABLE:
 *
 *   1. `mov r0` INTO THE MIDDLE of another argument's mov/lsl pair -- name the
 *      constant in a block that DOMINATES the call.  sx, sy and m are assigned
 *      before the `if`, not beside their calls, and that is what splits
 *      __MapActor_SetSpeed's and the last OvlFunc_common1_15b8's setup the way
 *      the ROM has it.  18 differing -> 15.  Written inline those two sites are
 *      contiguous and wrong; hoisting the OTHER constants as well makes it worse.
 *
 *   2. `mov r0` AT THE END -- DELETE THE PROTOTYPE.  OvlFunc_common1_1078,
 *      OvlFunc_common1_15b8 and OvlFunc_common1_5e4 are called through C's
 *      implicit `int f()` and are deliberately undeclared below.  15 -> exact.
 *      DO NOT ADD PROTOTYPES FOR THEM.  Note this is not the same as an empty
 *      parameter list `extern void f();`, which was screened here and moved
 *      nothing.
 *
 * The two levers are the pair recorded on OvlFunc_954_20095e0
 * (src/overlays/rom_7db0c8/ovl_30_c_c_c_c_b.c), which is the same cutscene frame
 * and needed both at once as well.  The lesson for the next park note: when
 * several spellings of the CALL leave a count unchanged, that is evidence the
 * call site is not where the lever is -- not evidence that there is none.
 *
 * Structure: signed-halfword read of gState + 0xe1*2, early return through
 * OvlFunc_common1_2c4 when it is 2; otherwise a three-way dispatch on
 * OvlFunc_common1_4cc(param, 1) with a long arm for 0 (message 0x209e, camera
 * setup, five walk steps off a running k, an anim change and a surprise), a
 * two-line arm for 1, and a shared tail.
 */
extern unsigned char gState[];
extern void OvlFunc_common1_2c4(void);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern int OvlFunc_common1_4cc(int a, int b);
extern void __MessageID(int id);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void __ActorMessage(int a, int b);
extern void __MapActor_SetSpeed(int a, int b, int c);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int a, int b);
extern void __MapActor_Surprise(int a, int b);
extern void OvlFunc_common1_1254(int a);
extern void __SetCameraTarget(int a, int b);
extern void OvlFunc_common1_588(int a, int b);

void OvlFunc_955_20092f0(int a)
{
    unsigned char *gp;
    unsigned char *e;
    int r;
    int k;
    int sx;
    int sy;
    int m;

    sx = 0xc0 << 9;
    sy = 0xc0 << 8;
    m = 0x95 << 3;
    gp = gState;
    if (*(short *)(gp + (0xe1 << 1)) == 2) {
        OvlFunc_common1_2c4();
        return;
    }
    __CutsceneStart();
    r = OvlFunc_common1_4cc(a, 1);
    if (r == 0) {
        __MessageID(0x209e);
        __Func_80933d4(0xc0 << 10, 0xc0 << 7);
        __Func_80933f8(0x99 << 19, -1, 0xb8 << 16, 1);
        __Func_8093530();
        __ActorMessage(a, 0);
        OvlFunc_common1_1078(0, 0x9f << 3, 0xa8);
        k = 0xa1 << 3;
        __MapActor_SetSpeed(0, sx, sy);
        OvlFunc_common1_15b8(0, k, 0xb8);
        OvlFunc_common1_15b8(0, k, 0xd8);
        k -= 0x40;
        OvlFunc_common1_15b8(0, k, 0xd8);
        __ActorMessage(a, 0);
        OvlFunc_common1_15b8(0, k, 0xf8);
        OvlFunc_common1_15b8(0, m, 0xf8);
        __CutsceneWait(3);
        e = __MapActor_GetActor(0);
        *(int *)(e + 0x28) = 0x80 << 11;
        __MapActor_SetAnim(0, 0x1c);
        __MapActor_Surprise(0, 0x81 << 1);
        __CutsceneWait(0x1e);
        __ActorMessage(a, 0);
        OvlFunc_common1_1254(0);
        __SetCameraTarget(0, 0);
        OvlFunc_common1_588(a, 1);
    } else if (r == 1) {
        __MessageID(0x209d);
        __ActorMessage(a, 0);
    }
    OvlFunc_common1_5e4(r, a, 1);
    __CutsceneEnd();
}
