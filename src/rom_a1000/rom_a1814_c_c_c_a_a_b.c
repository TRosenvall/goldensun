/* Func_80a448c  --  0x080a448c, split out of asm/rom_a1000/rom_a1814_c_c_c_a_a.s;
 * Func_80a414c (a jump-table function) stays in _a.s. Matched on the FIRST try.
 * `out` is `signed char *`. (The .s header's "ComputeTargetAvailability" text was
 * a duplicate that did not describe this function; it writes 6 bytes.)
 */
struct Item {
    unsigned char pad00[2];
    unsigned char f2;
    unsigned char f3;
};

struct State {
    unsigned char pad00[0x178];
    unsigned short f178;
    unsigned char pad17a[0x219 - 0x17a];
    unsigned char f219;
    unsigned char f21a;
};

extern struct State *iwram_3001f2c;
extern struct Item *_GetItemInfo(int item);
extern int Func_80a46b4(int unit, int item);
extern int _CanEquipItem(int unit, int item);
extern int _Func_808e990(int item);

void Func_80a448c(signed char *out)
{
    struct State *st;
    struct Item *info;
    int r;

    st = iwram_3001f2c;
    info = _GetItemInfo(st->f178 & 0x1ff);
    if (info->f2 == 0) {
        out[0] = 1;
        out[1] = -1;
    } else {
        out[0] = -1;
        out[1] = 1;
    }
    r = Func_80a46b4(st->f21a, st->f178);
    if (r != -1)
        out[0] = 1;
    else
        out[0] = r;
    if (st->f178 & 0x400)
        out[0] = -1;
    if (_CanEquipItem(st->f21a, st->f178 & 0x1ff) == 0)
        out[1] = -1;
    out[3] = 1;
    out[5] = 1;
    out[2] = 1;
    if (st->f178 & 0x200) {
        out[4] = 1;
        out[1] = -1;
    } else {
        out[4] = -1;
    }
    if (info->f3 & 2) {
        out[4] = -1;
        if (st->f178 & 0x200) {
            out[3] = -1;
            out[5] = -1;
        }
    }
    if (_Func_808e990(st->f178 & 0x1ff) != 0)
        out[0] = 1;
    if (st->f219 <= 1)
        out[3] = -1;
    if (info->f3 & 8)
        out[5] = -1;
}
