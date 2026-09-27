
struct Unit { unsigned char pad_00[0x3a]; short f3a; };
struct MoveInfo { unsigned char pad_00[9]; unsigned char f9; };

extern unsigned char *iwram_3001f2c;
extern struct Unit *_GetUnit(int id);
extern struct MoveInfo *_GetMoveInfo(int id);
extern void _SetTextColor(int c);
extern int Func_80a735c(int id);
extern void Func_80a2324(int count, int first, unsigned int win, int x, int y);
extern void Func_80a21b0(unsigned int win, int total, int perPage, int page, int col);
extern void _Func_8016498(unsigned int win);
extern void _Func_801e41c(unsigned int win, int a, int b, int c, int e);
extern void _Func_801e7c0(int id, unsigned int win, int x, int y);
extern void _Func_801e9d4(int v, int n, unsigned int win, int x, int y);
extern int _MSG_333;

int Func_80a6b64(unsigned int win, int a1, int *d)
{
    unsigned char *state;
    unsigned char n;
    unsigned char i;
    int first;
    int ofs;
    int y;
    int id;
    struct Unit *unit;
    struct MoveInfo *info;

    state = iwram_3001f2c;
    _Func_8016498(win);
    _Func_801e41c(win, 0, 0xb, 0x10, 0xb);
    if (*(unsigned short *)(state + 0x220) & 2)
        _Func_801e7c0(0xae1, win, 0, 0x58);
    else
        _Func_801e7c0(0xb89, win, 0, 0x58);
    first = d[2] * 5;
    n = d[5] - first;
    if (n > 5)
        n = 5;
    Func_80a2324(5, first, win, 0x70, 0x22);
    Func_80a21b0(win, d[5], 5, d[2], 0xf);
    _Func_801e7c0(0xaed, win, 0x60, 0);
    i = 0;
    if (n > i) {
        ofs = first * 2 + 0xe4 * 2;
        do {
            unit = _GetUnit(state[0x21a]);
            info = _GetMoveInfo(*(unsigned short *)(ofs + (int)state) & 0x3fff);
            if (info->f9 > unit->f3a)
                _SetTextColor(2);
            else if (Func_80a735c(*(unsigned short *)(ofs + (int)state) & 0x3fff))
                _SetTextColor(4);
            else
                _SetTextColor(0xf);
            id = (*(unsigned short *)(ofs + (int)state) & 0x3fff) + (int)&_MSG_333;
            y = i * 16 + 8;
            _Func_801e7c0(id, win, 0x10, y);
            _Func_801e9d4(info->f9, 2, win, 0x68, y);
            _SetTextColor(0xf);
            i++;
            ofs += 2;
        } while (n > i);
    }
    return 1;
}
