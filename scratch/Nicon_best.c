extern int AllocSpriteSlot(void);
extern void LoadOldUIIcon(int a, int b, int *c, int *d, int e);
extern void LoadInventoryIcon(int a, int b);
extern void LoadUIBanner(int a, int b);
extern void LoadMoveIcon(int a, int b, int *c, int *d, int e);

int UploadIcon(int id, int a)
{
    int v4;
    int v8;

    v8 = -1;
    switch (id) {
    case 1:
    case 6:
        LoadOldUIIcon(a, 0, &v8, &v4, 0);
        break;
    case 2:
        v8 = AllocSpriteSlot();
        if (v8 == 0x60)
            return -1;
        LoadInventoryIcon(a, 0x1a);
        break;
    case 9:
        v8 = AllocSpriteSlot();
        if (v8 == 0x60)
            return -1;
        LoadUIBanner(a, 0);
        break;
    case 4:
        LoadMoveIcon(a, 1, &v8, &v4, 0);
        break;
    }
    return v8;
}
