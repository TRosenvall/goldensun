/* BattleIntro  --  0x080941e0, split out of asm/rom_8a000/rom_93304_c_a.s;
 * Func_942e0 (parked) stays in _c.s. Matched from scratch.
 *
 * - LOOP.C HOISTS A CONSTANT STORE ADDRESS BY ITS LOAD'S LIFETIME: move_movables
 *   needs threshold*savings*lifetime >= insn_count (loop.c:1803).
 *   `*(u16 *)K = expr` loads K before computing expr (lifetime 5, hoisted into
 *   a callee-saved register); `w = expr; *(u16 *)K = w;` puts it beside the
 *   store (lifetime 1, stays): the ROM's per-iteration `ldr r2,=K`. ~48 -> ~17.
 * - WHEN THE ROM REVERSES ONE LOOP AND NOT ITS TWIN, THAT IS check_dbra_loop:
 *   no_use_except_counting needs giv_count == 0 (loop.c:7896). Loop 1 keeps c
 *   as a giv of i (not reversed); loop 2 has three bivs and a bare counter
 *   (reversed), and its biv order g, b, c picks the ROM's spill.
 * - One `do { } while (0)` after the first palette store stops `mov r1,#0x10`
 *   hoisting above the strh (3 off without). `int` return, no return
 *   statement (`pop {r1}`); 0x7fff through an `int`.
 */
extern unsigned char gState[];
extern unsigned char *iwram_3001ebc;
extern void _PlaySound(int id);
extern void ScreenTransitionOut(int a, int b);
extern void WaitFrames(int n);

int BattleIntro(void)
{
    unsigned char *s;
    unsigned char *gs;
    int i;
    int c;
    int w;
    int g, b;

    s = iwram_3001ebc;
    gs = gState;
    gs += 0xf7 << 1;
    _PlaySound(*(short *)gs);
    _PlaySound(0x90 << 1);
    _PlaySound(0x93);
    if (*(short *)(s + (0xcf << 1)) == 3) {
        w = 0x7fff;
        *(unsigned short *)0x50001e6 = w;
        do { } while (0);
        ScreenTransitionOut(0x401, 0x10);
        { unsigned char *q = s + (0xe3 << 1); int z = 0; *(unsigned short *)q = z; }
        WaitFrames(0x10);
        for (i = 0; i < 16; i++) {
            c = 0x1e - i * 2;
            w = (c << 10) | (c << 5) | c;
            *(unsigned short *)0x50001e6 = w;
            WaitFrames(1);
        }
    } else {
        { int w2 = 0x7fff; *(unsigned short *)0x5000000 = w2; }
        ScreenTransitionOut(0x207, 0x10);
        { unsigned char *q = s + (0xe3 << 1); int z = 0; *(unsigned short *)q = z; }
        WaitFrames(0x10);
        g = 0x3c0;
        b = 0x7800;
        c = 0x1e;
        for (i = 0; i < 16; i++) {
            *(unsigned short *)0x5000000 = b | g | c;
            WaitFrames(1);
            g -= 0x40;
            b -= 0x800;
            c -= 2;
        }
    }
}
