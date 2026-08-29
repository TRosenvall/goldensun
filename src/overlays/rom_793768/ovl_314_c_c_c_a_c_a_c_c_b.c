/* OvlFunc_898_200913c  --  0x0200913c, cut from
 * goldensun/asm/overlays/rom_793768/ovl_314_c_c_c_a_c_a_c_c.s.
 *
 * Opens a door: play the sound, run the tile animation, repaint the attribute
 * block, and clear one flag bit on the actor while setting two on the model it
 * points at.
 *
 * THE TWO BIT OPERATIONS WANT DIFFERENT SPELLINGS, in one function, which is
 * the clearest evidence yet that batch 83's lever is a spelling to try:
 *
 *     rom   ldrb r2, [r5] / mov r3, #0xfe / and r3, r2   <- CONSTANT is rd
 *     rom   ldrb r3, [r6, #9] / mov r2, #0xc / orr r3, r2 <- VALUE is rd
 *
 * and the plain `*p &= 0xfe` and `m[9] |= 0xc` give exactly those. Nothing has
 * to be named; naming either constant would move the wrong one.
 *
 * The two stack arguments ARE named, because the ROM builds both before storing
 * either (`mov r3, #4 / mov r2, #0xa / str / str`) where literals make gcc
 * compute and store each in turn. That is batch 83's other half and it does
 * apply here.
 *
 * The animation table is reached with gcc's asm-label extension rather than by
 * renaming it, per docs/elevation.md.
 */
struct A { unsigned char pad00[0x50]; unsigned char *f50; };

extern void *__MapActor_GetActor(int slot);
extern void __PlaySound(int id);
extern unsigned char L28ac[] __asm__(".L28ac");
extern void __Func_8010560(void *t, int a, int b);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_898_2008ef4(int a, int b, int c);

void OvlFunc_898_200913c(void)
{
    struct A *a;
    unsigned char *m;
    unsigned char *p;
    int e5, e6;

    a = __MapActor_GetActor(0);
    m = a->f50;
    __PlaySound(0x9e);
    __Func_8010560(L28ac, 0x23, 9);
    p = (unsigned char *)a + 0x23;
    e5 = 4;
    e6 = 0xa;
    __Func_8010704(0x21, 0x14, 1, 3, e5, e6);
    *p &= 0xfe;
    m[9] |= 0xc;
    OvlFunc_898_2008ef4(0x48, 0xa0, 0xc);
}
