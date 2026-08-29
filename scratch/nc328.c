extern char *iwram_3001ebc;
extern void __PlaySound(int id);
extern void __Func_808f1c0(int a, int b);
extern void __Func_801776c(int a, int b);
extern int __FindEmptyInventorySlot(int bag);
extern int __UI_SellMenu(int *a, int *b);
extern void __Func_8078948(int a, int b);
extern void __GiveItem(int id);

void OvlFunc_896_200c328(void)
{
    char *p;
    short save;
    int n;
    int a;
    int b;

    p = iwram_3001ebc;
    save = *(short *)(p + (0xec << 1));
    __PlaySound(0x53);
    __Func_808f1c0(0xe0, 3);
    __Func_801776c(0x111b, 1);
    for (;;) {
        n = 0x1e - __FindEmptyInventorySlot(0);
        n -= __FindEmptyInventorySlot(1);
        if (n > 3)
            break;
        __Func_801776c(0x111c, 1);
        if (__UI_SellMenu(&a, &b) != -1)
            __Func_8078948(a, b);
    }
    __GiveItem(0xe0);
    __GiveItem(0xe0);
    __GiveItem(0xe0);
    __GiveItem(0xe0);
    *(short *)(p + (0xec << 1)) = save;
}
