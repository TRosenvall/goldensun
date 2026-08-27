extern unsigned char gState[];
extern int _GiveItemTo(int unit, int item);
extern void _EquipItem(int unit, int slot);
extern unsigned char *_GetUnit(int id);
extern void Func_80b0278(int a, int b);

int Debug_TestEquipAndStatus(void)
{
    unsigned char *g;
    int off;
    int one;

    g = gState;
    *(int *)(g + 0x10) = 0x30d40;
    g += 0x8e << 1;
    *g = 0x1c;
    _EquipItem(1, _GiveItemTo(1, 0x48d));
    _EquipItem(0, _GiveItemTo(0, 0x40b));
    _GiveItemTo(2, 0xe7);
    off = 0x131;
    one = 1;
    _GetUnit(3)[off] = one;
    _GetUnit(5)[off] = one;
    _GetUnit(2)[0xa0 << 1] = one;
    Func_80b0278(1, 0x1e);
    return 0;
}
