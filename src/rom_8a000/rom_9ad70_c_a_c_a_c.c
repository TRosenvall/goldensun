/* Field_Retreat  --  0x0809b208, was asm/rom_8a000/rom_9ad70_c_a_c_a_c.s (this
 * function and its pools), so it converts whole. Matched from scratch.
 *
 * A function-pointer store written through a char union (alias set 0) adds the
 * one dependent that wins its `str`/`add` sched2 tie -- batch 286's alias-set
 * lever on a pointer store.
 */
extern char *iwram_3001f30;
extern void CutsceneStart(void);
extern void Func_80933f8(int a, int b, int c, int d);
extern void Func_8097384(void);
extern void WaitFrames(int n);
extern void Func_8092adc(int a, int b, int c);
extern void Func_8096b88(void);
extern void Func_809b0b0(void);
extern void Func_809b0dc(void);
extern void Func_809b11c(void);
extern void _PlaySound(int id);
extern void _Actor_SetAnim(char *a, int n);
extern void _Actor_SetColorswap(char *a, int n);
extern void _Actor_SetSpriteFlags(char *a, int n);
extern void Func_80974d8(int *v);
extern void Func_809ba90(char *p, int a, int b, int c);
extern void Func_809ba7c(char *p, void (*f)(void));
extern void Func_809ba70(char *p, int n);
extern unsigned int Random(void);
extern void _Sprite_SetColorswap(int a, int b);
extern void Func_809748c(void);

struct HalfWord { unsigned short v; };

void Field_Retreat(void)
{
    char *g;
    char *a;
    char *p;
    char *q;
    int h;
    int v[3];
    unsigned int i;
    unsigned int r;
    struct HalfWord z;

    g = iwram_3001f30;
    a = *(char **)(g + 0x10);
    CutsceneStart();
    Func_80933f8(-1, -1, -1, 0);
    Func_8097384();
    WaitFrames(10);
    Func_8092adc(*(short *)(g + 0x18), 0x80 << 7, 0);
    WaitFrames(0x1e);
    *(void (**)(void))(a + 0x6c) = Func_8096b88;
    _PlaySound(0x83);
    _Actor_SetAnim(a, 0x1c);
    WaitFrames(0x28);
    _PlaySound(0xdc);
    _Actor_SetColorswap(a, 0);
    _Actor_SetAnim(a, 3);
    {
        short *hp;
        ((union { void (*f)(void); char c; } *)(a + 0x6c))->f = Func_809b0b0;
        hp = (short *)(a + 0x64);
        h = 0;
        *hp = h;
    }
    WaitFrames(0x46);
    _Actor_SetSpriteFlags(a, 0);
    z.v = 0;
    *(char *)(a + 0x55) = z.v;
    *(void (**)(void))(a + 0x6c) = Func_809b0dc;
    *(int *)(a + 0x38) = 0x80 << 24;
    v[0] = *(int *)(a + 8);
    v[1] = *(int *)(a + 0xc);
    v[2] = *(int *)(a + 0x10);
    Func_80974d8(v);
    i = 0;
    p = g + 0x58;
    for (; i <= 0x17; i++) {
        Func_809ba90(p, 0x8e << 1, v[0], v[2]);
        Func_809ba7c(p, Func_809b11c);
        Func_809ba70(p, 7);
        _Sprite_SetColorswap(*(int *)p, (Random() * 7) >> 16);
        r = (Random() >> 1) + 0x13333;
        *(int *)(p + 0x2c) = r;
        *(int *)(p + 0x28) = r;
        WaitFrames(1);
        p += 0x48;
    }
    WaitFrames(0x46);
    i = 0;
    {
        int two = 2;
        q = g + 0x98;
        for (; i <= 0x17; i++) {
            if (*(signed char *)(q + 5) != 0)
                *q = two;
            q += 0x48;
        }
    }
    WaitFrames(0x28);
    Func_809748c();
    WaitFrames(10);
}
