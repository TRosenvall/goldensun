/* Task_Rain  --  0x08094820, split out of asm/rom_8a000/rom_944ec_a_a_a_a_c_c_a_a.s;
 * Task_Thunder, StartRain and Task_Snow (all parked) stay in _c.s. Matched from
 * scratch.
 *
 * - Hoisting the 0x3f mask hinged on ONE insn of lifetime: loop.c had it at 8
 *   (fails), 9 is hoisted (threshold 15 against 132 insns); `(unsigned char)p[1]`
 *   adds exactly that insn (6 -> exact).
 * - Reusing x and y for the respawn coordinates lengthens their ranges so they
 *   spill as in the ROM, leaving the mask its register (155 -> 6).
 * - The halfword-store `int h` is declared inside each block; one function-scope h
 *   is allocated function-wide and lands in r1.
 */
struct Drop {
    int f00;
    unsigned char y;
    unsigned char a0lo : 6;
    unsigned char shape : 2;
    unsigned short x : 9;
    unsigned short a1mid : 5;
    unsigned short size : 2;
    unsigned short tile : 10;
    unsigned short prio : 2;
    unsigned short pal : 4;
    unsigned char pad0a[2];
    int px;
    int py;
    int pz;
    int f18;
    unsigned short t;
    unsigned short pad1e;
};

struct Rain {
    int f00;
    int tilebase;
    struct Drop d[32];
};

struct Map {
    int *actor;
    unsigned char pad04[0xe4 - 4];
    int camx;
    int camy;
};

extern struct Map *iwram_3001e70[];
extern short L9ef84[] __asm__(".L9ef84");

extern int _GetFlag(int id);
extern int Random(void);
extern int _Func_8011f54(int a, int b, int c);
extern void Func_8003dec(struct Drop *p, int n);

void Task_Rain(void)
{
    struct Map *m;
    struct Rain *s;
    struct Drop *e;
    short *p;
    int camx, camy;
    unsigned int i;
    int x, y;
    int *w;
    int *c;

    m = iwram_3001e70[0];
    s = (struct Rain *)iwram_3001e70[0x15];
    c = &m->camx;
    camx = c[0];
    camy = c[1];
    e = s->d;
    for (i = 0; i < 0x20; i++, e++) {
        if (--e->t == 0xffff)
            continue;
        if (_GetFlag(0x166))
            e->t++;
        p = &L9ef84[e->t * 5];
        x = (e->px - camx) / 0x10000 + *p++;
        y = (e->pz - e->py - camy) / 0x10000 + *p++;
        if ((unsigned)(x + 0x10) <= 0xff && y >= -0x20 && y <= 0x9f) {
            e->prio = 1;
            e->x = x;
            e->y = y;
            e->tile = s->tilebase + (unsigned short)*p++;
            e->shape = (unsigned char)p[0];
            e->size = (unsigned char)p[1];
            Func_8003dec(e, 0xf0);
        }
        if (e->t == 0) {
            w = m->actor;
            x = w[0] + (Random() << 8) - 0x800000;
            y = w[2] + (Random() << 8) - 0x800000;
            e->px = x;
            e->pz = y;
            e->py = _Func_8011f54(0, x >> 16, y >> 16) << 16;
            e->t = 0x10;
        }
    }
}
