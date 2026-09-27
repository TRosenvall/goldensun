/* Field_Frost  --  0x08099160, was asm/rom_8a000/rom_97b54_a_c_c_a_c_c_c_a.s (this
 * function alone), so it converts whole. Matched from scratch. A `bp = v;` that
 * loop.c would hoist anyway, written before the counter's initialisation, puts the
 * hoisted `add r7, sp, #4` inside the counter's reload pair (2 -> 0).
 */
extern unsigned char *iwram_3001f30;
extern unsigned char gState[];
extern void Func_8097384(void);
extern void Func_809748c(void);
extern void _PlaySound(int id);
extern unsigned char *CreateParticleActor(int a, int b, int c, int d);
extern unsigned int Random(void);
extern void Func_8099070(void);
extern void Func_80990cc(void);
extern void Func_8099018(void);
extern void _Actor_SetColorswap(unsigned char *a, int n);
extern void _Actor_SetAnim(unsigned char *a, int n);
extern void vec3_translate(int a, int b, int *v);
extern void WaitFrames(int n);
extern int Func_808e4b4(int a, int b, int *out);
extern void Func_8096b28(int a, int b, int c);

void Field_Frost(void)
{
    unsigned char *m;
    unsigned char *t;
    unsigned char *p;
    unsigned char *g;
    int *bp;
    int out;
    int v[3];
    int s;
    int d;
    int i;
    int r;

    m = iwram_3001f30;
    t = *(unsigned char **)(m + 0x14);
    Func_8097384();
    _PlaySound(0x73);
    bp = v;
    i = 0xf;
    do {
        p = CreateParticleActor(0xe8, 0, 0, 0);
        if (p != 0) {
            s = (Random() >> 1) + 0x8000;
            *(int *)(p + 0x1c) = s;
            *(int *)(p + 0x18) = s;
            if (Random() & 1)
                *(void (**)(void))(p + 0x6c) = Func_8099070;
            else
                *(void (**)(void))(p + 0x6c) = Func_80990cc;
            *(unsigned short *)(p + 6) = Random();
            {
                unsigned short *h = (unsigned short *)(p + 0x64);
                int k = 0x3c;
                *h = k;
            }
            *(unsigned short *)(p + 0x66) = Random();
            _Actor_SetColorswap(p, 9);
            bp[0] = *(int *)(m + 4);
            bp[1] = *(int *)(m + 8);
            bp[2] = *(int *)(m + 0xc);
            d = Random() * 4 + (0x80 << 10);
            vec3_translate(d, Random(), bp);
            *(int *)(p + 0x38) = bp[0];
            *(int *)(p + 0x3c) = bp[1];
            *(int *)(p + 0x40) = bp[2];
        }
        WaitFrames(3);
    } while (--i >= 0);
    WaitFrames(10);
    _PlaySound(0x73);
    WaitFrames(0x32);
    if (t != 0 && *(signed char *)(m + 0x20) == 0) {
        _PlaySound(0xd4);
        i = 0xf;
        do {
            _Actor_SetColorswap(t, 7);
            WaitFrames(1);
            _Actor_SetColorswap(t, 0);
            WaitFrames(4);
        } while (--i >= 0);
        if (*(signed char *)(m + 0x34) == 0) {
            _PlaySound(0xdc);
            _Actor_SetAnim(t, 2);
        }
        *(void (**)(void))(t + 0x6c) = Func_8099018;
        r = Func_808e4b4(0x50000005, 6, &out);
        if (r != 0) {
            g = gState;
            Func_8096b28(r, *(int *)(g + (0xfa << 1)), out);
        }
        WaitFrames(0x14);
    }
    Func_809748c();
}
