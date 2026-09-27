/* Field_Reveal  --  0x080983a0, was asm/rom_8a000/rom_97b54_a_c_a_a_a_c_c_c_c.s
 * (this function alone), so it converts whole. Matched from scratch.
 *
 * - `do { } while (0);` after the 0xcb8 store keeps the `*px` load below it.
 * - `*(short *)(m + 0xcba) = one * 600;` with `one` the int local already
 *   holding 1: the bare 600 pools as `ldr =0x258`, a fresh int local takes the
 *   wrong register.
 * - StartTask declared `int` (batch 286's callee-return-type lever) settles the
 *   StartTask argument tie; gState is built at runtime; the /0x10000 is written
 *   `if (t < 0) t += 0xffff; t >>= 16;`.
 */
extern unsigned char *iwram_3001ebc[];
extern unsigned char gState[];
extern void _PlaySound(int id);
extern int StartTask(void *fn, int priority);
extern void Func_80982dc(void);
extern void Func_8098294(int a);
extern void Func_808fe38(int a);
extern void Func_808f32c(void);
extern unsigned char *GetFieldActor(int id);
extern void Func_8091200(int a, int b);
extern void Func_8091254(int a);
extern void Func_8091220(int a, int b);
extern void WaitFrames(int n);
extern int Func_808e4b4(int a, int b, int *out);
extern void Func_8096b28(int a, int b, int c);

void Field_Reveal(void)
{
    unsigned char *m;
    unsigned char *e;
    unsigned char *a;
    unsigned char *g;
    int *px;
    int *pz;
    int v;
    int r;
    int i;
    int one;
    int t;

    m = iwram_3001ebc[0];
    Func_8098294(6);
    Func_808fe38(8);
    g = gState;
    e = iwram_3001ebc[4];
    a = GetFieldActor(*(int *)(g + (0xfa << 1)));
    px = (int *)(e + 0x52c);
    *px = *(int *)(a + 8);
    pz = (int *)(e + 0x530);
    *pz = *(int *)(a + 0x10) - *(int *)(a + 0xc);
    Func_8091220(0x80 << 9, 0);
    Func_8091200(0x10001, 1);
    Func_8091254(1);
    WaitFrames(1);
    r = Func_808e4b4(0x50000005, 8, &v);
    if (r != 0)
        Func_8096b28(r, *(int *)(g + (0xfa << 1)), v);
    _PlaySound(0x83);
    one = 1;
    *(short *)(m + 0xcb8) = one;
    do { } while (0);
    t = *px;
    if (t < 0)
        t += 0xffff;
    t >>= 16;
    *(short *)(m + 0xcbc) = t;
    t = *pz;
    if (t < 0)
        t += 0xffff;
    t >>= 16;
    *(short *)(m + 0xcbe) = t;
    *(short *)(m + 0xcba) = one * 600;
    *(short *)(m + 0xcc0) = one;
    Func_808f32c();
    for (i = 0; i <= 0x12; i++) {
        WaitFrames(1);
        *(short *)(e + 0x52a) = i;
    }
    StartTask(Func_80982dc, 0xc80);
}
