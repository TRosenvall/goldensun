/* OvlFunc_932_200b484  --  0x0200b484, was asm/overlays/rom_7b9cb4/ovl_30_a_c_c_c_c.s
 * (this function alone), so it converts whole. Matched from scratch.
 *
 * `p = s + 0x27; one = 1; *p = one;` -- address first, then the constant -- moves the
 * constant's birth one insn later, raising its local-alloc priority
 * (floor_log2(refs)*refs/(death-birth): 1875 vs 1901, read under gdb in
 * find_free_reg), and with high-register order 8,10,9,11 all four high
 * assignments reshuffle into the ROM's (18 -> 1).
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;

extern void __PlaySound(int id);
extern void __Actor_TravelTo(unsigned char *a, int x, int y, int z);
extern void __Actor_WaitMovement(unsigned char *a);
extern void __Actor_SetAnim(unsigned char *a, int anim);
extern void __WaitFrames(int frames);
extern void __DeleteSpriteLayer(int layer);
extern unsigned char *__GetFieldActor(int id);

void OvlFunc_932_200b484(unsigned char *a)
{
    unsigned char *s;
    unsigned int base;
    unsigned int off;
    int x;
    int z;
    int z2;
    int zero;
    int one;
    int h;
    unsigned char *b;

    __PlaySound(0x120);
    __PlaySound(0xe8);
    x = (*(int *)(a + 8) & 0xfff00000) + 0x80000;
    z = *(int *)(a + 0x10) & 0xfff00000;
    z2 = z + 0x80000;
    *(int *)(a + 0x34) = 0x20000;
    __Actor_TravelTo(a, x, *(int *)(a + 0xc), z2);
    __Actor_WaitMovement(a);
    { unsigned char *p = a + 0x22; zero = 0; *p = zero; }
    *(int *)(a + 8) = x;
    *(int *)(a + 0x10) = z2;
    *(int *)(a + 0x24) = zero;
    *(int *)(a + 0x2c) = zero;
    __Actor_SetAnim(a, 2);
    __WaitFrames(0xf);
    __Actor_SetAnim(a, 1);
    __WaitFrames(0x1e);
    s = *(unsigned char **)(a + 0x50);
    { unsigned char *p = s + 0x27; one = 1; *p = one; }
    __DeleteSpriteLayer(*(int *)(s + 0x2c));
    *(int *)(s + 0x2c) = zero;
    s[0x25] = one;
    base = (unsigned int)&gState;
    off = 0xfa;
    off <<= 1;
    base += off;
    b = __GetFieldActor(*(int *)base);
    __PlaySound(0x98);
    *(int *)(b + 8) = x;
    *(int *)(b + 0x28) = 0x60000;
    *(int *)(b + 0x48) = 0x10000;
    *(int *)(b + 0x10) = z2;
    (*(signed char **)(b + 0x50))[9] &= ~0xc;
    __Actor_SetAnim(b, 7);
    z += 0x180000;
    __Actor_TravelTo(b, x, *(int *)(b + 0xc), z);
    h = 0x4000;
    *(unsigned short *)(b + 6) = h;
    __WaitFrames(0x14);
    { signed char *p = *(signed char **)(b + 0x50); p[9] = (p[9] & ~0xc) | 8; }
    __PlaySound(0x9f);
}
