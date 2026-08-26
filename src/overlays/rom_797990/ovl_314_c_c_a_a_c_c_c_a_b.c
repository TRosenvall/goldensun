/* OvlFunc_901_2008b40  --  0x02008b40
 * OvlFunc_901_2008b9c  --  0x02008b9c
 *
 * Cut from goldensun/asm/overlays/rom_797990/ovl_314_c_c_a_a_c_c_c_a.s.
 *
 * More of the door-opening family whose first three members are in
 * src/overlays/rom_793768/. Play the sound, run a tile animation from a table
 * in this overlay, repaint an attribute block, clear one flag bit on the actor
 * and set two on the model it points at.
 *
 * FOUND MECHANICALLY. Once one member was solved, the remaining ones were
 * pulled out of the assembly by matching the exact instruction shape and
 * reading each function's constants out of the match -- table label, the two
 * __Func_8010560 arguments, the two stack arguments and the three callback
 * arguments. Every member then screened clean on the first attempt. That is
 * worth doing whenever a solve looks like a template: it is cheaper than
 * find_twins.py's byte-identical test, which these are NOT.
 *
 * The two bit operations want DIFFERENT spellings and both are plain:
 *
 *     rom   ldrb r2, [r5] / mov r3, #0xfe / and r3, r2      CONSTANT is rd
 *     rom   ldrb r3, [r6, #9] / mov r2, #0xc / orr r3, r2   VALUE is rd
 *
 * The two stack arguments ARE named, because the ROM builds both before storing
 * either.
 */
struct A { unsigned char pad00[0x50]; unsigned char *f50; };

extern void *__MapActor_GetActor(int slot);
extern void __PlaySound(int id);
extern unsigned char L1782[] __asm__(".L1782");
extern void __Func_8010560(void *t, int a, int b);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_901_2008a80(int a, int b, int c);

void OvlFunc_901_2008b40(void)
{
    struct A *a;
    unsigned char *m;
    unsigned char *p;
    int e5, e6;

    a = __MapActor_GetActor(0);
    m = a->f50;
    __PlaySound(0x9e);
    __Func_8010560(L1782, 0x36, 0xd);
    p = (unsigned char *)a + 0x23;
    e5 = 0x17;
    e6 = 0xc;
    __Func_8010704(0x21, 0x14, 1, 3, e5, e6);
    *p &= 0xfe;
    m[9] |= 0xc;
    OvlFunc_901_2008a80(0xbc << 1, 0xe0, 8);
}


extern unsigned char L1798[] __asm__(".L1798");

void OvlFunc_901_2008b9c(void)
{
    struct A *a;
    unsigned char *m;
    unsigned char *p;
    int e5, e6;

    a = __MapActor_GetActor(0);
    m = a->f50;
    __PlaySound(0x9e);
    __Func_8010560(L1798, 0x31, 0xa);
    p = (unsigned char *)a + 0x23;
    e5 = 0x12;
    e6 = 0xa;
    __Func_8010704(0x21, 0x14, 1, 3, e5, e6);
    *p &= 0xfe;
    m[9] |= 0xc;
    OvlFunc_901_2008a80(0x94 << 1, 0xb0, 9);
}
