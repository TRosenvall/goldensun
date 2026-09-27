/* Func_802281c  --  0x0802281c, split out of asm/rom_15000/rom_21dfc_a_c_c_c.s;
 * MenuBar and Func_8022768 (parked) stay in _a.s. Matched from scratch.
 *
 * - The outer loop is a `goto` loop (`if (list[0] != 0xff) { top: ... i++;
 *   if (i <= 3 && list[i] != 0xff) goto top; }`): a for or while lets loop.c
 *   strength-reduce the list pointer.
 * - Party slots read as the member `party->member[j]`; a raw
 *   `*(short *)(party + 0x58 + j*2)` lets the 0x58 hoist.
 */
struct Party { unsigned char pad[0x58]; short member[4]; };
extern struct Party *iwram_3001e74;
extern int _Func_80b6c08(int, int);
extern void Func_8022768(int x, int y, int w, int h, int flag);

int Func_802281c(unsigned short *list)
{
    struct Party *party = iwram_3001e74;
    int n;
    int i, j;

    n = _Func_80b6c08(1, 0);
    Func_8022768(0x1d - n * 6, 0, 0x19, 5, 0xf);
    i = 0;
    if (list[0] != 0xff) {
    top:
        for (j = 0; j < 4; j++) {
            int v = party->member[j];
            if (v == list[i])
                break;
            if (v == 0xff) {
                j = 4;
                break;
            }
        }
        if (j != 4)
            Func_8022768(0x1d - (n - j) * 6, 0, 7, 5, 0xe);
        i++;
        if (i <= 3 && list[i] != 0xff)
            goto top;
    }
    return 0;
}
