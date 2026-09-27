/* Func_80a4800  --  0x080a4800, split out of asm/rom_a1000/rom_a47b4_a_c_a.s;
 * Func_80a4924 (a jump-table function) stays in _c.s. Matched from scratch.
 *
 * `sel = 0;` is written at the TOP, beside `redraw = 1;`, although the ROM
 * materialises `mov r6,#0` much later: the longer live range makes sel conflict
 * with the pseudo holding 0xd, and the whole allocation rotates to the ROM's
 * (st r8, id r9, redraw r10, 0xd r5, sel r6, box r7). With the assignment where
 * the ROM places it, 123 of 127 differ.
 */
struct Spr {
    unsigned char pad00[5];
    unsigned char f5;
};

struct State {
    unsigned char pad00[0x10];
    int f10;
    unsigned char pad14[0x17c - 0x14];
    struct Spr *f17c;
    unsigned char pad180[0x21c - 0x180];
    struct Spr *f21c;
};

extern struct State *iwram_3001f2c;
extern volatile unsigned int gKeyRepeat;
extern volatile unsigned int gKeyPress;
extern int _CreateUIBox(int a, int b, int c, int d, int e);
extern void StopTask(void *fn);
extern int StartTask(void *fn, int prio);
extern void Func_80a19a0(void);
extern void Func_80a22f4(void);
extern void WaitFrames(int n);
extern int _GetFlag(int id);
extern void Func_80a4924(int box, int id);
extern void _Func_8016498(int box);
extern void _CloseUIBox(int box, int a);
extern void Func_80a2144(int a);
extern void _Func_80170f8(int a, int b, int c, int d);

int Func_80a4800(int id)
{
    struct State *st;
    int redraw;
    int sel;
    int box;

    redraw = 1;
    sel = 0;
    st = iwram_3001f2c;
    st->f21c->f5 = 0xd;
    box = _CreateUIBox(0, 0, 0x1e, 0xa, 2);
    StopTask(Func_80a19a0);
    st->f17c->f5 = 0xd;
    Func_80a22f4();
    WaitFrames(1);
    while (_GetFlag(0x150) == 0) {
        if (redraw) {
            redraw = 0;
            sel = (sel + 5) % 5;
            Func_80a4924(box, id);
        }
        if (gKeyPress & 1)
            break;
        if (gKeyPress & 2) {
            sel = -1;
            break;
        }
        if (gKeyRepeat & 0x40) {
            sel--;
            redraw = 1;
        }
        if (gKeyRepeat & 0x80) {
            sel++;
            redraw = 1;
        }
        WaitFrames(1);
    }
    _Func_8016498(box);
    WaitFrames(1);
    _CloseUIBox(box, 1);
    _Func_8016498(st->f10);
    Func_80a2144(0xe);
    StartTask(Func_80a19a0, 0xc80);
    st->f17c->f5 = 1;
    _Func_80170f8(0xd, 0, 0x11, 0xa);
    return sel;
}
