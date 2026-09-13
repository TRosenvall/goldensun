/* OvlFunc_942_20087dc -- asm/overlays/rom_7c6bac/ovl_30_c_c_a_c_c_c_c_a_c.s
 *
 * A cutscene: set the player actor's walk speed, hand a script to the map
 * interpreter, then pick the camera target and script from the AREA id at
 * gState+0x1c0.  The two area ids are pooled rather than compared as `cmp
 * Rn, #imm8`, which is the `area.sym` tell -- both fit an eight-bit immediate,
 * so gcc had no reason to pool a literal.
 *
 * FAKEMATCH: two `register ... __asm__("rN")` pins, on the r0 and r1 arguments
 * of the two calls whose argument fill the ROM interleaves.
 *
 * The blocker was an r0-in-the-middle argument interleave at
 * __MapActor_SetSpeed(0, 0x8000, 0x4000):
 *
 *     rom   mov r1,#0x80 / mov r2,#0x80 / lsl r2,#7 / mov r0,#0 / lsl r1,#8
 *     plain mov r1,#0x80 / mov r2,#0x80 / lsl r2,#7 / lsl r1,#8 / mov r0,#0
 *
 * The zero is the CHEAP argument, so precompute_register_parameters emits it
 * last and it carries the highest INSN_LUID of the group; sched2 breaks the
 * tie against `lsl r1,#8` on LUID and the zero loses.  A bare hard-register
 * declaration on r0 AND on r1 -- declared in argument order, r0 first --
 * gives the zero the lower LUID and reproduces the ROM.  Pinning r0 alone is
 * not enough (8 differing); the expensive shifted argument has to be pinned
 * with it.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_6b;
extern int _AREA_70;
extern unsigned char L16ce[] __asm__(".L16ce");
extern unsigned char gScript_930__020096b8[];
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __MapActor_SetSpeed(int slot, int x, int z);
extern void __Func_8092b08(int slot, int a);
extern void __Func_809218c(int slot, int x, int y);
extern void __Func_8010560(void *s, int a, int b);
extern void __Func_8091e9c(int n);

void OvlFunc_942_20087dc(void)
{
    unsigned char *g;
    unsigned int k;
    int v;

    __CutsceneStart();
    __PlaySound(0x9e);
    {
        register int p0 __asm__("r0") = 0;
        register int p1 __asm__("r1") = 0x80 << 8;

        __MapActor_SetSpeed(p0, p1, 0x80 << 7);
    }
    __Func_8092b08(0, 3);
    k = 0xe0 << 1;
    g = (unsigned char *)&gState + k;
    v = *(short *)(g + (unsigned int)0);
    if (v == (int)(&_AREA_6b)) {
        {
            register int q0 __asm__("r0") = 0;
            register int q1 __asm__("r1") = 0x98 << 1;

            __Func_809218c(q0, q1, 0xae << 3);
        }
        __Func_8010560(gScript_930__020096b8, 0x4e, 0x56);
    } else if (v == (int)(&_AREA_70)) {
        __Func_809218c(0, 0xf8, 0xc0);
        __Func_8010560(L16ce, 0x4a, 9);
    }
    __CutsceneWait(0x10);
    __Func_8091e9c(3);
    __CutsceneEnd();
}
