/* Cluster OvlFunc_926_200c1ec..OvlFunc_926_200c1ec split out of goldensun/asm/overlays/rom_7b2078/ovl_314_c_c_c_c_c_c_c_c.s.
 *
 * Code to this file, the trailing .section .data to its _c sibling.
 *
 * Two levers took this from 36 differing to exact.
 *
 * SEPARATE LOCALS FOR SHORT-LIVED RESULTS vs THE HELD ONE. Using one shared
 * variable for every __MapActor_GetActor result put the actor in r7 and pushed
 * the loop counter into r8, adding a `mov r7, r0` at every early store.
 * Splitting the used-once `e` from the loop-held `a` fixed the r7/r8 roles and
 * thirty instructions at once.
 *
 * THE STORED ZERO MUST BE BORN AFTER THE LOOP. Assigning `zero = 0;` inside
 * the loop and passing it as the sixth argument is 6 differing -- gcc hoists
 * the pair seven instructions early. Passing a bare literal 0 in the call and
 * assigning `zero = 0;` AFTER the loop, for the two later stores, is exact;
 * gcc still emits the ROM's in-loop `mov r3, #0 / str / mov r9, r3`.
 */
struct St {
    int f0;
    int f4;
    unsigned char pad8[0x10];
    unsigned short f18;
    unsigned char pad1a[0xe];
};

struct Actor {
    unsigned char pad0[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x58];
    void *f6c;
};

extern void __PlaySound(int id);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __CutsceneWait(int frames);
extern void __Func_8091220(int a, int b);
extern void __Func_8091200(int a, int b);
extern void __Func_8091254(int a);
extern unsigned int __Random(void);
extern void OvlFunc_common0_10c(int a, int b, int c, int d, int e, int f, int g, struct St *h);
extern void __WaitFrames(int n);
extern void __Func_8092950(int slot, int a);
extern void OvlFunc_926_200c1c4(void);

void OvlFunc_926_200c1ec(void)
{
    struct Actor *a;
    struct Actor *e;
    struct St *p;
    void *f;
    unsigned int i;
    int x, y, z;
    int zero;
    struct St st;

    __PlaySound(0x83);
    e = __MapActor_GetActor(8);
    f = OvlFunc_926_200c1c4;
    e->f6c = f;
    __CutsceneWait(0x28);
    __Func_8091220(0x80 << 9, 0);
    __Func_8091200(0x205c54, 1);
    __Func_8091254(0x3c);
    __CutsceneWait(0x28);
    __PlaySound(0x83);
    e = __MapActor_GetActor(2);
    e->f6c = f;
    __CutsceneWait(0x78);
    a = __MapActor_GetActor(8);
    p = &st;
    p->f0 = 1;
    p->f4 = 2;
    p->f18 = 0x11d;
    for (i = 0; i <= 0x3f; i++) {
        if ((i & 3) == 0)
            __PlaySound(0xf6);
        x = a->f8 + ((__Random() * 48 >> 16) << 16) - (0xc << 16);
        y = a->fc + ((__Random() * 32 >> 16) << 16) - (0x10 << 16);
        z = ((__Random() * 4 >> 16) << 15) + (0x80 << 8);
        OvlFunc_common0_10c(x, y, a->f10, 0, z, 0, 0x98 << 13, p);
        __WaitFrames(2);
    }
    zero = 0;
    __PlaySound(0xdc);
    __CutsceneWait(0x1e);
    __Func_8091200(0x80 << 9, 1);
    __Func_8091254(0x3c);
    __CutsceneWait(0x28);
    e = __MapActor_GetActor(8);
    e->f6c = (void *)zero;
    e = __MapActor_GetActor(2);
    e->f6c = (void *)zero;
    __Func_8092950(8, 0);
    __Func_8092950(2, 0);
}
