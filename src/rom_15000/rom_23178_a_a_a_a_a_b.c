/* Func_8025180  --  0x08025180
 *
 * Cut out of goldensun/asm/rom_15000/rom_23178_a_a_a_a_a.s.
 *
 * Classifies an item for the equip menu: 1 means "not usable here", 2 means
 * "usable but its move is passive", 0 means the move is active. The four early
 * returns of 1 are why the ROM keeps re-issuing `mov r0, #1` -- gcc preloads
 * the constant at each point it can reach the shared exit from.
 *
 * The `&&` in the equip test is one statement, not two nested ifs: the ROM has
 * `ldrb r3, [r5, #2] / cmp r3, #0 / beq .L251b8` falling THROUGH to the call
 * and jumping past it when the byte is zero, which is short-circuit evaluation
 * of a single condition.
 *
 * Matched on the first screen.
 */
struct Item {
    unsigned char pad00[2];
    unsigned char f2;
    unsigned char pad03[0xc - 3];
    unsigned char fc;
    unsigned char pad0d[0x28 - 0xd];
    unsigned short f28;
};

struct Move {
    unsigned char pad00[1];
    unsigned char f1;
};

extern struct Item *_GetItemInfo(int item);
extern struct Move *_GetMoveInfo(int id);
extern int _CanEquipItem(int unit, int item);

int Func_8025180(int unit, int item)
{
    struct Item *info;

    if (item == 0)
        return 1;
    info = _GetItemInfo(item);
    if (info->fc == 3)
        return 1;
    if (info->f28 == 0)
        return 1;
    if (info->f2 != 0 && _CanEquipItem(unit, item) == 0)
        return 1;
    if ((_GetMoveInfo(info->f28)->f1 & 0x80) == 0)
        return 2;
    return 0;
}
