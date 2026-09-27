/* Func_809641c  --  0x0809641c, split out of asm/rom_8a000/rom_944ec_a_c_a_c_c_a_c.s;
 * GetVenusDjinni (parked) stays in _a.s, alone. Matched from scratch.
 *
 * A QImode range test comes from RE-READING the byte: `(unsigned char)(*p - 1) <= 1`
 * beside an `int k = *p` reproduces the ROM's extra `ldrb` and the
 * `lsl #24 / cmp #1<<24 / bhi` test; `k == 1 || k == 2` and a signed char k do not.
 */
extern unsigned char gState[];
extern unsigned char *MapActor_GetActor(int slot);
extern unsigned int Random(void);
extern void vec3_translate(int a, int b, int *v);
extern void Func_80974d8(int *v);
extern int Func_809ba34(unsigned char *e);
extern void Func_809bb34(unsigned char *e);
extern void _PlaySound(int id);

void Func_809641c(unsigned char *e)
{
    unsigned char *g;
    unsigned char *a;
    signed char *p;
    int *bp;
    int *bq;
    int v[3];
    int k;
    int d;
    int r;
    int t;
    int x, z;

    g = gState;
    g += 0xfa << 1;
    a = MapActor_GetActor(*(int *)g);
    p = (signed char *)(e + 0x40);
    k = *p;
    if (k == 0) {
        bp = v;
        bp[0] = *(int *)(a + 8);
        bp[1] = *(int *)(a + 0xc) + Random() * 5 + (0xf0 << 12);
        bp[2] = *(int *)(a + 0x10);
        Func_80974d8(bp);
        d = Random() * 6 + (0x80 << 10);
        vec3_translate(d, Random(), bp);
        x = bp[0];
        *(int *)(e + 0xc) = x;
        z = bp[2];
        *(int *)(e + 0x10) = z;
        *(int *)(e + 4) = x;
        *(int *)(e + 8) = z - 0x640000;
        t = 0xc0 << 10;
        *(int *)(e + 0x24) = t;
        *(int *)(e + 0x20) = Random() * 3 + t;
        *(int *)(e + 0x28) = 0x80 << 9;
        *(int *)(e + 0x2c) = 0x80 << 9;
        *(char *)(e + 0x42) = k;
        *(char *)(e + 0x41) = 1;
        *p = *p + 1;
    } else if ((unsigned char)(*p - 1) <= 1) {
        r = Func_809ba34(e);
        if (r == 0) {
            bq = v;
            bq[0] = *(int *)(e + 4);
            bq[2] = *(int *)(e + 8);
            vec3_translate(0xc0 << 12, Random(), bq);
            *(int *)(e + 0xc) = bq[0];
            *(int *)(e + 0x10) = bq[2];
            *(char *)(e + 0x41) = r;
            *(int *)(e + 0x1c) = 0x80 << 9;
            *(int *)(e + 0x24) = r;
            *(int *)(e + 0x20) = Random() + 0x23333;
            *(int *)(e + 0x28) = 0x80 << 8;
            *(int *)(e + 0x2c) = 0x80 << 8;
            _PlaySound(0x8f);
            if (*p == 1)
                *p = *p - 1;
            else
                *p = *p + 1;
            {
                int s6 = 6;
                *(short *)(e + 0x3a) = s6;
            }
        }
    } else if (k == 3) {
        Func_809bb34(e);
    }
}
