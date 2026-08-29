extern int AllocSpriteSlot(void);
extern void LoadOldUIIcon(int a, int b, int *slot, int *out, int e);
extern void LoadMoveIcon(int a, int b, int *slot, int *out, int e);
extern void LoadInventoryIcon(int a, int b);
extern void LoadUIBanner(int a, int b);

int UploadIcon(int kind, int arg)
{
    int slot;
    int out;

    slot = -1;
    switch (kind) {
    case 1:
    case 6:
        LoadOldUIIcon(arg, 0, &slot, &out, 0);
        break;
    case 2:
        slot = AllocSpriteSlot();
        if (slot == 0x60)
            return -1;
        LoadInventoryIcon(arg, 0x1a);
        break;
    case 9:
        slot = AllocSpriteSlot();
        if (slot == 0x60)
            return -1;
        LoadUIBanner(arg, 0);
        break;
    case 4:
        LoadMoveIcon(arg, 1, &slot, &out, 0);
        break;
    }
    return slot;
}
