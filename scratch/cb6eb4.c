extern unsigned short Lc2a1c[] __asm__(".Lc2a1c");
extern unsigned short Lc2a2a[] __asm__(".Lc2a2a");
extern unsigned short Lc2a38[] __asm__(".Lc2a38");
extern unsigned short Lc2a46[] __asm__(".Lc2a46");
extern unsigned short Lc2a54[] __asm__(".Lc2a54");

extern unsigned char *_GetUnit(int id);
extern int _GetEquippedItem(int unit, int slot);
extern int GetWeaponType(int id);

int GetWeaponSpriteID(int unit)
{
    unsigned char *u;
    int item;
    int type;
    int off;
    int r;
    unsigned short *tbl;

    u = _GetUnit(unit);
    item = _GetEquippedItem(unit, 1);
    r = 0;
    if (item >= 0) {
        off = item * 2 + 0xd8;
        type = GetWeaponType(*(unsigned short *)(u + off) & 0x1ff);
        switch (u[0x94 << 1]) {
        case 0:
            tbl = Lc2a1c;
            break;
        case 1:
            tbl = Lc2a2a;
            break;
        case 2:
            tbl = Lc2a38;
            break;
        case 3:
            tbl = Lc2a46;
            break;
        case 5:
            tbl = Lc2a54;
            break;
        default:
            return r;
        }
        r = tbl[type];
    }
    return r;
}
