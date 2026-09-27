/* Func_80a76d0  --  0x080a76d0, split out of asm/rom_a1000/rom_a7380_a_c_a.s;
 * Func_80a77a4 (parked) and the rest stay in _c.s. Matched from scratch.
 *
 * `state = r != -1;` came out as `state = 0; if ...; state = 1` sharing a tail
 * with another case; routing it through a block-scoped `int more = r != -1;
 * state = more;` restores the ROM's mvn/neg/orr/lsr idiom (measured, mechanism
 * in expr.c not traced).
 */
extern int _GetFlag(int id);
extern int Func_80a77a4(int which);
extern int Func_80a8114(void);
extern int Func_80a90bc(void);
extern int Func_80a96d8(void);

struct Cursor {
    unsigned char pad0[5];
    unsigned char visible;
    unsigned char pad6[6];
    unsigned short timer;
};

struct StatusState {
    unsigned char pad0[0x10];
    unsigned int win;
    struct Cursor *cursor[2];
    signed char sel[4];
    unsigned char pad20[0x154];
    unsigned short f174;
};
extern struct StatusState *iwram_3001f2c;

int Func_80a76d0(void)
{
    struct StatusState *st = iwram_3001f2c;
    int state = 0;
    int done = 0;
    int r = 0;

    while (!done && !_GetFlag(0x150)) {
        switch (state) {
        case 0:
            st->f174 = 0;
            if (Func_80a77a4(0) == -1) {
                r = -1;
                done = 1;
            }
            state = 1;
            break;
        case 1:
            st->cursor[0]->visible = 0xd;
            r = Func_80a8114();
            state = (r != -1) * 2;
            break;
        case 2:
            st->cursor[0]->visible = 0xd;
            r = Func_80a90bc();
            state = 0;
            if (r != -1)
                state = 3;
            break;
        case 3:
            st->cursor[0]->visible = 0xd;
            r = Func_80a96d8();
            {
                int more = r != -1;
                state = more;
            }
            break;
        default:
            done = 1;
            break;
        }
    }
    if (_GetFlag(0x150))
        r = -1;
    return r;
}
