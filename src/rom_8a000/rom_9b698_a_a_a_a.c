/* Field_Avoid  --  0x0809b698, was asm/rom_8a000/rom_9b698_a_a_a_a.s (this
 * function alone), so it converts whole. Matched from scratch.
 *
 * - A count-down `mov #N-1 / sub #1 / bge` loop whose counter the body never
 *   uses is written COUNTING UP, `for (i = 0; i < N; i++)`: loop.c reverses the
 *   biv. The literal count-down left the counter in a high register (128 -> 47).
 * - One `do { } while (0)` before WaitFrames(3) in the loop (4 off without; all
 *   six orders of the three stores stay at 4+).
 */
extern unsigned char *iwram_3001f30;
extern unsigned char gState[];
extern unsigned char L9c510[] __asm__(".L9c510");
extern int AllocSpriteSlot(void);
extern int UploadSpriteGFX(int slot, int n, void *src);
extern int _GetFlag(int id);
extern void _Actor_SetColorswap(unsigned char *a, int n);
extern void _Actor_SetSpriteFlags(unsigned char *a, int n);
extern void _PlaySound(int id);
extern void WaitFrames(int n);
extern void StartTask(void (*f)(void), int pri);
extern void StopTask(void (*f)(void));
extern void Func_8003f3c(int n);
extern void _Func_801776c(int a, int b);
extern void Func_809b5dc(void);
extern void Func_809b588(void);

void Field_Avoid(void)
{
    unsigned char *g;
    unsigned char *a;
    unsigned char *s;
    unsigned char *t;
    unsigned short *h;
    unsigned char *gs;
    unsigned char *gs2;
    int slot;
    int saved;
    int zero;
    int one;
    int i;

    g = iwram_3001f30;
    a = *(unsigned char **)(g + 0x10);
    s = *(unsigned char **)(a + 0x50);
    t = *(unsigned char **)(s + 0x28);
    saved = *(unsigned short *)(a + 6);
    slot = AllocSpriteSlot();
    {
        short *p = (short *)(g + 0x71a);
        zero = 0;
        *p = slot;
    }
    UploadSpriteGFX(*(short *)(g + 0x71a), 0x80 << 1, L9c510);
    gs = gState;
    *(int *)(gs + (0x91 << 2)) = 0x96 << 20;
    gs[0x92 << 2] = _GetFlag(0x145);
    _Actor_SetColorswap(a, 0);
    *(void (**)(void))(a + 0x6c) = Func_809b5dc;
    h = (unsigned short *)(a + 0x64);
    *h = zero;
    *(unsigned short *)(a + 0x66) = zero;
    _PlaySound(0x8c);
    WaitFrames(0xf);
    one = 1;
    *h = one;
    WaitFrames(10);
    for (i = 0; i < 20; i++) {
        t[5] = 7;
        s[0x25] = 1;
        WaitFrames(2);
        s[0x25] = 1;
        t[5] = 0;
        s[0x26] = 1;
        do { } while (0);
        WaitFrames(3);
    }
    *(int *)(a + 0x6c) = 0;
    *(unsigned short *)(a + 6) = saved;
    StartTask(Func_809b588, 0xc8 << 4);
    WaitFrames(0xf);
    _PlaySound(0xae);
    WaitFrames(0x37);
    StopTask(Func_809b588);
    gs2 = gState;
    gs2 += 0x93 << 2;
    if (*(short *)gs2 != 0)
        _Actor_SetSpriteFlags(a, 2);
    else
        _Actor_SetSpriteFlags(a, 1);
    _Actor_SetColorswap(a, 0);
    Func_8003f3c(*(short *)(g + 0x71a));
    _Func_801776c(0x922, 1);
}
