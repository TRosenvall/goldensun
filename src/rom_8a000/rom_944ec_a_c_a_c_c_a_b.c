/* Func_8096048  --  0x08096048, split out of asm/rom_8a000/rom_944ec_a_c_a_c_c_a.s;
 * Func_8095fcc stays in _a.s, GetVenusDjinni (parked) and Func_809641c in _c.s.
 * Matched from scratch. It takes the actor in r0 (the old .s header said no
 * arguments); the state byte is a `signed char *` so `*p = *p - 1` is `sub #1`,
 * not `add #0xff`.
 */
extern unsigned char gState[];
extern int iwram_3001800;
extern unsigned char *MapActor_GetActor(int slot);
extern unsigned int Random(void);
extern void vec3_translate(int a, int b, int *v);
extern void Func_80974d8(int *v);
extern int Func_809ba34(unsigned char *e);
extern void Func_809bb34(unsigned char *e);
extern void _PlaySound(int id);

void Func_8096048(unsigned char *e)
{
    unsigned char *g;
    unsigned char *a;
    signed char *p;
    int *bp;
    int v[3];
    int k;
    int d;
    int x, z;

    g = gState;
    g += 0xfa << 1;
    a = MapActor_GetActor(*(int *)g);
    p = (signed char *)(e + 0x40);
    k = *p;
    if (k == 0) {
        bp = v;
        bp[0] = *(int *)(a + 8);
        bp[1] = *(int *)(a + 0xc);
        bp[2] = *(int *)(a + 0x10);
        d = Random() * 10 + (0xa0 << 12);
        vec3_translate(d, Random(), bp);
        Func_80974d8(bp);
        x = bp[0];
        *(int *)(e + 0x14) = x;
        z = bp[2];
        *(int *)(e + 0x18) = z;
        *(int *)(e + 4) = x;
        *(int *)(e + 8) = z;
        bp[0] = x;
        bp[2] = z;
        vec3_translate(0xf0 << 15, 0xc0 << 8, bp);
        *(int *)(e + 0xc) = bp[0];
        *(int *)(e + 0x10) = bp[2];
        *(int *)(e + 0x24) = 0x80 << 9;
        *(int *)(e + 0x20) = 0xa0 << 11;
        *(char *)(e + 0x42) = k;
        *p = *p + 1;
        if (iwram_3001800 & 1)
            _PlaySound(0x90);
    } else if (k == 1) {
        if (Func_809ba34(e) == 0)
            *p = *p - 1;
    } else if (k == 2) {
        if (Func_809ba34(e) == 0)
            Func_809bb34(e);
    }
}
