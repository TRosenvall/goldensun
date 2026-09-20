/* Cluster Func_80b1dec..Func_80b1dec extracted from goldensun/asm/rom_b0000/rom_b0070_a_a_c_c_c.s.
 *
 * Total .text for this TU = 148 bytes (= 0x94).
 * Preserves the original ROM layout when slotted between
 * asm/rom_b0000/rom_b0070_a_a_c_c_c_c_a.o and asm/rom_b0000/rom_b0070_a_a_c_c_c_c_c.o in
 * goldensun/stage1.ld.
 *
 * Never attempted before batch 273. No pins, no flags.
 *
 * THE FIRST CANDIDATE WAS 2 DIFFERING AND THE FIX WAS STATEMENT ORDER: `x = 8; y = 0;`
 * rather than `y = 0; x = 8;`. Worth recording as a calibration -- a residue of exactly
 * two adjacent prologue `mov`s is usually just the order the source assigns them, not a
 * scheduling problem and not worth a flag or a pin. Try the swap before reading a dump.
 *
 * The struct came from the file-mate src/rom_b0000/rom_b0070_a_a_c_c_c_a_c.c, which
 * already declares `Unit { u8 pad00[0xd8]; u16 items[1]; }`; this landed in ONE
 * candidate after that read.
 */
typedef struct { unsigned char pad00[0xd8]; unsigned short items[1]; } Unit;

extern Unit *_GetUnit(int unit);
extern void _Func_8016478(int box);
extern int _FindEmptyInventorySlot(int unit);
extern void _DrawSmallText(int msg, int box, int x, int y);
extern unsigned char *_Func_801eb90(int item, int a, int box, int x, int y);

void Func_80b1dec(int box, int unit)
{
    Unit *u;
    int i;
    int x;
    int y;

    u = _GetUnit(unit);
    x = 8;
    y = 0;
    if (box == 0)
        return;
    _Func_8016478(box);
    if (_FindEmptyInventorySlot(unit) == 0) {
        _DrawSmallText(0xc91, box, 8, 0x14);
        return;
    }
    for (i = 0; i <= 0xe; i++) {
        if (u->items[i] != 0) {
            _Func_801eb90(u->items[i], 0x1b, box, x, y)[0xf] = 0xfc;
        }
        x += 0x10;
        if (i == 4) {
            x = 8;
            y += 0x10;
        }
        if (i == 9) {
            x = 8;
            y += 0x10;
        }
    }
}
