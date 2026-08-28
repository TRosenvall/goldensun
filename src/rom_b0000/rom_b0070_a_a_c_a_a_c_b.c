extern unsigned char gState[];

extern int _GiveItemTo(int who, int item);
extern int _EquipItem(int who, int slot);
extern unsigned char *_GetUnit(int n);
extern int Func_80b0278(int a, int b);

int Debug_TestEquipAndStatus(void)
{
    unsigned char *g;
    int v;
    unsigned char *u;
    int item;

    g = gState;
    *(int *)(g + 0x10) = 0x30d40;
    g += 0x11c;
    v = 0x1c;
    *g = v;

    item = _GiveItemTo(1, 0x48d);
    _EquipItem(1, item);
    item = _GiveItemTo(0, 0x40b);
    _EquipItem(0, item);
    _GiveItemTo(2, 0xe7);

    u = _GetUnit(3);
    *(u + 0x131) = 1;
    u = _GetUnit(5);
    *(u + 0x131) = 1;
    *(_GetUnit(2) + 0x140) = 1;

    Func_80b0278(1, 0x1e);
    return 0;
}
