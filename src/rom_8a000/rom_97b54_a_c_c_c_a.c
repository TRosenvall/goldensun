/* Field_Ply  --  0x080994d0, was asm/rom_8a000/rom_97b54_a_c_c_c_a.s (this function alone), so
 * it converts whole. Matched from scratch.
 *
 * FAKEMATCH -- one pin, `register unsigned char *p __asm__("r5")`, booked in
 * fakematch.txt. The ROM keeps the second particle actor in two registers
 * (`mov r6,r0 / mov r5,r6`), r5 being the first loop's `p`; gcse's cprop_insn
 * rewrites every later use of p into t and the copy vanishes. WITHOUT the pin the
 * identical source is EXACT under -fno-gcse, so the alternative is a per-file
 * GCSE_CFLAGS row (the ColorCycleVFXPalette / OvlFunc_959_2009528 decision).
 */
extern unsigned char *iwram_3001f30;
extern void Func_8097384(void);
extern void Func_809748c(void);
extern void _PlaySound(int id);
extern unsigned char *CreateParticleActor(int a, int b, int c, int d);
extern unsigned char *Func_8096c48(void *a, unsigned char *b);
extern void _Actor_SetColorswap(unsigned char *a, int n);
extern void WaitFrames(int n);
extern void Func_8096bec(unsigned char *a, int b, int c);
extern void _Actor_WaitMovement(unsigned char *a);
extern void _DeleteActor(unsigned char *a);
extern void Func_8003f3c(int id);
extern void Func_8099340(void);
extern void Func_80993b0(void);

void Field_Ply(void)
{
    unsigned char *m;
    unsigned char *t;
    register unsigned char *p __asm__("r5");
    unsigned char *q;
    unsigned char *prev;
    void (*fn)(void);
    int snd;
    int i;
    int h;

    m = iwram_3001f30;
    t = *(unsigned char **)(m + 0x10);
    Func_8097384();
    prev = 0;
    for (i = 0; i <= 7; i++) {
        p = CreateParticleActor(0xe9, *(int *)(t + 8), *(int *)(t + 0xc) + 0x200000,
                                *(int *)(t + 0x10));
        if (p != 0) {
            *(int *)(p + 0x1c) = 0xb333;
            *(int *)(p + 0x18) = 0xb333;
            *(void (**)(void))(p + 0x6c) = Func_8099340;
            q = p + 0x64;
            {
                int h = 0x78;
                *(short *)q = h;
            }
            h = i << 13;
            q += 2;
            *(short *)q = h;
            q -= 0x11;
            *q = 4;
            prev = Func_8096c48(*(void **)(p + 0x50), prev);
        }
        WaitFrames(1);
    }
    snd = prev[0x1c];
    _PlaySound(0x82);
    WaitFrames(0x6e);
    t = CreateParticleActor(0xe9, 0, 0, 0);
    p = t;
    if (t != 0) {
        *(int *)(t + 0x1c) = 0xb333;
        *(int *)(t + 0x18) = 0xb333;
        *(int *)(t + 8) = *(int *)(m + 4);
        *(int *)(t + 0xc) = *(int *)(m + 8) + 0x100000;
        *(int *)(t + 0x10) = *(int *)(m + 0xc);
        *(t + 0x55) = 4;
        _Actor_SetColorswap(t, 7);
    }
    _PlaySound(0x83);
    WaitFrames(0xc);
    if (t != 0) {
        for (i = 0; i <= 0x1d; i++) {
            if (i & 3)
                _Actor_SetColorswap(p, 9);
            else
                _Actor_SetColorswap(p, 10);
            WaitFrames(2);
        }
    }
    _Actor_SetColorswap(p, 0);
    _PlaySound(0x54);
    if (p != 0) {
        *(void (**)(void))(t + 0x6c) = Func_80993b0;
        q = t + 0x64;
        {
            int h = 0;
            *(short *)q = h;
        }
        if (*(signed char *)(m + 0x20) != 0)
            WaitFrames(0x80);
        else
            WaitFrames(0xc0);
    }
    if (t != 0) {
        q = t + 0x64;
        {
            int h = 0xffff;
            *(short *)q = h;
        }
        *(int *)(t + 0x30) = 0x50000;
        *(int *)(t + 0x34) = 0x6666;
        q -= 0xa;
        *q = 0;
        Func_8096bec(t, 0xc00000, 0xe800);
        _Actor_WaitMovement(t);
        _DeleteActor(t);
    }
    if (snd != 0x60)
        Func_8003f3c(snd);
    fn = *(void (**)(void))(m + 0x24);
    if (fn != 0)
        fn();
    Func_809748c();
}
