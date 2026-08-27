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
