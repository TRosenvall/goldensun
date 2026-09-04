/* OvlFunc_936_200b768  --  0x0200b768  [asm/overlays/rom_7c097c/ovl_30_c_c_c_c_a_c.s]
 *
 * NOT MATCHING. Best 66 of 99, and ours is 101 lines against 99. Like
 * src/non_matching/ovl_7aa430/2009df8.c parked the same round, the excess is a
 * REGISTER the ROM does not spend: the ROM pushes r5 and r6, ours pushes r5, r6
 * and r7, so the count past the prologue is mostly displacement.
 *
 * A proximity check on two actors: if the first is inside a coordinate box,
 * copy two sprite flag bits from the player onto it; otherwise, and then again
 * for the second actor, ring a bell once and hand the position to a helper.
 *
 * WHAT IS BELIEVED CORRECT AND IS NOT THE PROBLEM: the box test is the unsigned
 * -offset idiom (`(unsigned)(x - 0x17d) <= 0xc`, from the ROM's
 * `ldr r2, =0xfffffe83 / add / cmp #0xc / bhi`); the flag merge is
 * `u[9] = (u[9] & -k) | (t[9] & 0xc)` with `int k = 0xd`, which is the
 * name-the-constant cure for the narrowing recorded in
 * src/non_matching/ovl_77dd1c/2008d5c.c; and `iwram_3001e40` is a global int
 * tested with `& 1`. The two halves of the function are written out rather than
 * looped, since the ROM emits both copies.
 *
 * MEASURED, two forms, identical to each other:
 *
 *     t, u and k at function scope    66 differ, 101 lines
 *     t, u and k scoped into the arm  66 differ, 101 lines  (byte-identical)
 *
 * SCOPING IS INERT, which is the same negative result the other parked function
 * in this round produced, and together they say the extra register is not held
 * by anything whose scope the source controls.
 *
 * NEXT: as with 2009df8, read the generated .s and find what occupies the third
 * callee-saved register. The two functions are stuck on the same question and
 * should be worked together; a lever for one is likely a lever for the other.
 * Neither was carried further this round.
 */
extern int iwram_3001e40;
extern void OvlFunc_936_200b864(int a, int b, int c);

extern unsigned char *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __PlaySound(int id);
extern void __Func_8091ff0(int a);

void OvlFunc_936_200b768(void)
{
    unsigned char *a;
    unsigned char *b;

    a = __MapActor_GetActor(8);
    b = __MapActor_GetActor(9);
    if ((unsigned)(*(short *)(a + 0xa) - 0x17d) <= 0xc
        && *(short *)(a + 0x12) > 0x309) {
        unsigned char *t = *(unsigned char **)(__MapActor_GetActor(0) + 0x50);
        unsigned char *u = *(unsigned char **)(a + 0x50);
        int k = 0xd;
        u[9] = (u[9] & -k) | (t[9] & 0xc);
    } else if (__GetFlag(0x302) == 0
               && *(short *)(a + 0xa) <= 0xf5
               && (iwram_3001e40 & 1) == 0) {
        if (__GetFlag(0x202) == 0) {
            __Func_8091ff0(-1);
            __PlaySound(0xe6);
            __SetFlag(0x202);
        }
        OvlFunc_936_200b864(*(int *)(a + 8), *(int *)(a + 0xc),
                            *(int *)(a + 0x10));
    }
    if (__GetFlag(0x303) == 0
        && *(short *)(b + 0xa) <= 0x2c5
        && (iwram_3001e40 & 1) == 0) {
        if (__GetFlag(0x203) == 0) {
            __Func_8091ff0(-1);
            __PlaySound(0xe6);
            __SetFlag(0x203);
        }
        OvlFunc_936_200b864(*(int *)(b + 8), *(int *)(b + 0xc),
                            *(int *)(b + 0x10));
    }
}
