/* Cluster OvlFunc_943_20097a0..OvlFunc_943_20097a0 extracted from
 * goldensun/asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_a_a.s.
 *
 * A placement routine: seven __MapActor_SetPos calls and four halfword writes
 * to the actors they place.  Three levers, and two of them are the same lever
 * pulled in opposite directions at different sites.
 *
 * 1. THE HALFWORD WRITES NEED A TYPED FIELD.  `*(short *)(e + 6) = 0x80 << 8`
 *    compiles to `ldr r3, =0xffff8000` -- gcc pools the constant, sign-extended
 *    to the destination's width -- where the ROM builds it `mov r3, #0x80 /
 *    lsl r3, #8`.  Through `e->f6` it builds it, in a SCRATCH register, at no
 *    cost.  37 differing -> 6.
 *
 *    Naming the value in an int local also defeats the pool, but it takes a
 *    CALLEE-SAVED register, and this function has exactly two to spend -- on
 *    `h` and `z`, which really do live across calls.  The two single-use
 *    constants must not compete for those.  That is the whole difference
 *    between the two spellings and it is why the struct is the right one here.
 *    See src/non_matching/ovl_7b2078/2008afc.c for the same pair measured.
 *
 * 2. `h` AND `z` ARE NAMED ON PURPOSE.  `h = 0xa0 << 7` is written at two of
 *    the four halfword sites and the ROM keeps it in r6 for the whole function;
 *    `z = 0` is written at one, and the ROM still spends r5 on it rather than
 *    emitting `mov r3, #0` beside the store.  This is the exception to "do not
 *    name zeros": the ROM's prologue pushes {r5, r6}, so both are asked for.
 *
 * 3. __MapActor_SetPos IS DECLARED TWICE.  Five of the seven calls want
 *    `mov r0` LAST and two want it FIRST, and one declaration cannot do both.
 *    The callee is left UNDECLARED for the five (implicit `int`, which puts r0
 *    at the end) and the last two go through SetPosD, an __asm__ alias with a
 *    real prototype.  Do not add a prototype for __MapActor_SetPos here.
 */
struct A {
    unsigned char pad00[6];
    short f6;
};

extern struct A *__MapActor_GetActor(int slot);
extern void __Func_8092b08(int a, int b);
extern void SetPosD(int a, int b, int c) __asm__("__MapActor_SetPos");

void OvlFunc_943_20097a0(void)
{
    struct A *e;
    int h;
    int z;

    __MapActor_SetPos(0x15, 0x83 << 17, 0x2c20000);
    e = __MapActor_GetActor(0x15);
    h = 0xa0 << 7;
    e->f6 = h;
    __MapActor_SetPos(0x18, 0xa4 << 16, 0xa2 << 18);
    e = __MapActor_GetActor(0x18);
    z = 0;
    e->f6 = z;
    __Func_8092b08(0x18, 1);
    __MapActor_SetPos(0x19, 0xc6 << 16, 0x2990000);
    e = __MapActor_GetActor(0x19);
    e->f6 = 0x80 << 8;
    __Func_8092b08(0x19, 1);
    __MapActor_SetPos(0x1a, 0xbc << 16, 0x2a60000);
    e = __MapActor_GetActor(0x1a);
    e->f6 = 0xb0 << 8;
    __MapActor_SetPos(0x1b, 0xba << 16, 0x27b0000);
    e = __MapActor_GetActor(0x1b);
    e->f6 = h;
    __MapActor_SetPos(0x16, 0, 0);
    SetPosD(0x17, 0, 0);
    SetPosD(0x14, 0, 0);
}
