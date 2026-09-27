/* Func_80a9598 (DrawAbilityPage2)  --  0x080a9598, split out of
 * asm/rom_a1000/rom_a8604_c_a_a.s; Func_80a93a4 stays in _a.s and Func_80a96d8
 * (parked) in _c.s. Matched from scratch.
 *
 * Its message base is _MSG_af7 (message.sym, added this batch by value): used as
 * `m` then `m + 1`, and as a CONST_INT sched2 hoists the load above the preceding
 * call, because a constant set of a call-saved register after reload has no
 * dependence on the call. As a SYMBOL_REF it stays put. _MSG_182 does the same
 * job in this function and was already admitted.
 */
extern unsigned char *iwram_3001f2c;
extern int _MSG_182;
extern int _MSG_af7;
extern unsigned char *_GetUnit(int id);
extern void _Func_80164d4(unsigned int win, int x, int y, int w, int h);
extern void _Func_801e7c0(int msg, unsigned int win, int x, int y);
extern void _Func_801e8b0(void *unit, unsigned int win, int x, int y);
extern void _Func_801ea08(int val, int digits, unsigned int win, int x, int y);
extern void Func_80a2324(int count, int first, unsigned int win, int x, int y);
extern void Func_80a21b0(unsigned int win, int total, int perPage, int page, int col);

int Func_80a9598(unsigned int win, int a1, int *d)
{
    unsigned char *state;
    unsigned char *unit;
    unsigned short *p;
    int first;
    unsigned char count;
    unsigned char i;
    int m;

    state = iwram_3001f2c;
    unit = _GetUnit(*(unsigned char *)(state + 0x21a));
    _Func_80164d4(win, 0x80, 8, 0xe0, 0x60);
    first = d[2] * 5;
    count = d[5] - first;
    if (count > 5)
        count = 5;
    Func_80a2324(5, first, win, 0x77, 0x34);
    Func_80a21b0(win, d[5], 5, d[2], 0x1c);
    if (*(unsigned char *)(state + 0x218) == 0) {
        _Func_801e7c0(0xad7, win, 0x78, 8);
    } else {
        i = 0;
        if (count > i) {
            p = (unsigned short *)(state + first * 2 + 0xe4 * 2);
            do {
                _Func_801e7c0((*p & 0x1ff) + (int)&_MSG_182, win, 0x80, i * 16 + 8);
                p++;
                i++;
            } while (count > i);
        }
    }
    _Func_801e8b0(unit, win, 0x28, 0);
    m = (int)&_MSG_af7;
    _Func_801e7c0(m, win, 0x20, 0x10);
    m++;
    _Func_801e7c0(m, win, 0x20, 0x18);
    _Func_801ea08(*(unsigned short *)(unit + 0x3c), 3, win, 0x50, 0x10);
    _Func_801ea08(*(unsigned short *)(unit + 0x3e), 3, win, 0x50, 0x18);
    return 1;
}
