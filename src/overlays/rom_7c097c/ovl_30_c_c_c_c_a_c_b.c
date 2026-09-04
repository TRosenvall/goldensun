// fakematch
/* OvlFunc_936_200b768  --  0x0200b768
 *
 * Cut out of goldensun/asm/overlays/rom_7c097c/ovl_30_c_c_c_c_a_c.s.
 *
 * WAS PARKED at 66 of 99, ours 101 lines against 99, on "it takes one more
 * callee-saved register than the ROM". The park recorded that scoping the
 * short-lived locals into their block was BYTE-IDENTICAL to leaving them at
 * function scope, and concluded the extra register was not held by anything the
 * source's scope controls. That was right, and it was also the wrong place to
 * look.
 *
 * THE EXTRA REGISTERS WERE THE TWO FLAG IDS. One `xgcc -S` and a grep for r5
 * and r6 shows
 *
 *     ldr r6, .L9+16 / mov r0, r6 / ... / mov r0, r6
 *     ldr r5, .L9+28 / mov r0, r5 / ... / mov r0, r5
 *
 * -- `0x202` and `0x203` are each tested and then set, and gcc cached both in
 * callee-saved registers where the ROM reloads `ldr r0, =0x202` at each use.
 * That is the ordinary rematerialisation lever, wearing a register-pressure
 * disguise. An r0 pin before each of the four calls takes 66 of 101 to 10 of
 * 99, with the length becoming exact.
 *
 * SECOND FUNCTION IN TWO ROUNDS where an unexplained callee-saved register was
 * a cached constant rather than anything structural, and the second where
 * reading the generated .s answered in minutes what reasoning about liveness
 * had got wrong. The other is
 * src/non_matching/ovl_7aa430/2009df8.c, where the cached value was a commoned
 * zero. READ THE .s BEFORE THEORISING ABOUT PRESSURE.
 *
 * THE LAST TEN INSTRUCTIONS were the sprite-flag merge, which wants all four of
 * its registers named. The ROM is
 *
 *     ldr r3, [r0, #0x50] / ldr r4, [r5, #0x50] / ldrb r3, [r3, #9] /
 *     mov r2, #0xc / and r2, r3 / ldrb r1, [r4, #9] / mov r3, #0xd /
 *     neg r3, r3 / and r3, r1 / orr r3, r2 / strb r3, [r4, #9]
 *
 * -- the player's sprite pointer in r3, the actor's in r4 (free because of
 * `-fcall-used-r4`), and r3 REUSED twice more: first for the byte loaded
 * through it, then for the 0xd constant. Three roles, one register, and the
 * only way to spell that is three `register` declarations naming r3 in nested
 * scopes. Pinning the two pointers and the mask leaves 2 differing -- the
 * loaded byte and the constant in each other's registers -- and naming BOTH
 * operands of that mask (`tb = t[9]; m = 0xc; m &= tb;`) is exact.
 *
 * The `-0xd` still needs the batch-208 cure of naming the constant, since
 * writing the mask as a literal lets gcc narrow it to a byte.
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
    register int p0 __asm__("r0");

    a = __MapActor_GetActor(8);
    b = __MapActor_GetActor(9);
    if ((unsigned)(*(short *)(a + 0xa) - 0x17d) <= 0xc
        && *(short *)(a + 0x12) > 0x309) {
        register unsigned char *t __asm__("r3");
        register unsigned char *u __asm__("r4");
        register int m __asm__("r2");
        t = *(unsigned char **)(__MapActor_GetActor(0) + 0x50);
        u = *(unsigned char **)(a + 0x50);
        {
            register int tb __asm__("r3");
            tb = t[9];
            m = 0xc;
            m &= tb;
        }
        {
            register int k __asm__("r3");
            k = 0xd;
            u[9] = (u[9] & -k) | m;
        }
    } else if (__GetFlag(0x302) == 0
               && *(short *)(a + 0xa) <= 0xf5
               && (iwram_3001e40 & 1) == 0) {
        p0 = 0x202;
        if (__GetFlag(p0) == 0) {
            __Func_8091ff0(-1);
            __PlaySound(0xe6);
            p0 = 0x202;
            __SetFlag(p0);
        }
        OvlFunc_936_200b864(*(int *)(a + 8), *(int *)(a + 0xc),
                            *(int *)(a + 0x10));
    }
    if (__GetFlag(0x303) == 0
        && *(short *)(b + 0xa) <= 0x2c5
        && (iwram_3001e40 & 1) == 0) {
        p0 = 0x203;
        if (__GetFlag(p0) == 0) {
            __Func_8091ff0(-1);
            __PlaySound(0xe6);
            p0 = 0x203;
            __SetFlag(p0);
        }
        OvlFunc_936_200b864(*(int *)(b + 8), *(int *)(b + 0xc),
                            *(int *)(b + 0x10));
    }
}
