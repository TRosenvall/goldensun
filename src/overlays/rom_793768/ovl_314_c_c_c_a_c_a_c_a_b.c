/* OvlFunc_898_2008fb4  --  0x02008fb4
 * OvlFunc_898_2009010  --  0x02009010
 *
 * Cut from goldensun/asm/overlays/rom_793768/ovl_314_c_c_c_a_c_a_c_a.s.
 *
 * Two more of the door-opening family whose third member is
 * src/overlays/rom_793768/ovl_314_c_c_c_a_c_a_c_c.c: play the sound, run a tile
 * animation from a table in this overlay, repaint an attribute block, clear one
 * flag bit on the actor and set two on the model it points at.
 *
 * Both matched on the first screen once the family's shape was known, which is
 * the point of writing the three up together. The two bit operations want
 * DIFFERENT spellings in the same function --
 *
 *     rom   ldrb r2, [r5] / mov r3, #0xfe / and r3, r2      CONSTANT is rd
 *     rom   ldrb r3, [r6, #9] / mov r2, #0xc / orr r3, r2   VALUE is rd
 *
 * -- and the plain `*p &= 0xfe` and `m[9] |= 0xc` give exactly those, with
 * nothing named. The two stack arguments ARE named, because the ROM builds both
 * before storing either.
 *
 * Each animation table is reached with gcc's asm-label extension, so nothing
 * outside this file changes.
 */
struct A { unsigned char pad00[0x50]; unsigned char *f50; };

extern void *__MapActor_GetActor(int slot);
extern void __PlaySound(int id);
extern unsigned char L286a[] __asm__(".L286a");
extern void __Func_8010560(void *t, int a, int b);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_898_2008ef4(int a, int b, int c);

void OvlFunc_898_2008fb4(void)
{
    struct A *a;
    unsigned char *m;
    unsigned char *p;
    int e5, e6;

    a = __MapActor_GetActor(0);
    m = a->f50;
    __PlaySound(0x9e);
    __Func_8010560(L286a, 0x36, 0xd);
    p = (unsigned char *)a + 0x23;
    e5 = 0x17;
    e6 = 0xc;
    __Func_8010704(0x21, 0x14, 1, 3, e5, e6);
    *p &= 0xfe;
    m[9] |= 0xc;
    OvlFunc_898_2008ef4(0xbc << 1, 0xe0, 8);
}


extern unsigned char L2880[] __asm__(".L2880");

void OvlFunc_898_2009010(void)
{
    struct A *a;
    unsigned char *m;
    unsigned char *p;
    int e5, e6;

    a = __MapActor_GetActor(0);
    m = a->f50;
    __PlaySound(0x9e);
    __Func_8010560(L2880, 0x31, 0xa);
    p = (unsigned char *)a + 0x23;
    e5 = 0x12;
    e6 = 0xa;
    __Func_8010704(0x21, 0x14, 1, 3, e5, e6);
    *p &= 0xfe;
    m[9] |= 0xc;
    OvlFunc_898_2008ef4(0x94 << 1, 0xb0, 9);
}
