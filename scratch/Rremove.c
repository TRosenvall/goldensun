extern char *GetUnit(int who);
extern unsigned char *GetItemInfo(int id);

int CanRemoveItem(int who, int slot)
{
    char *u;
    int off;
    int id;
    unsigned char *info;
    int flags;

    off = slot * 2;
    off += 0xd8;
    u = GetUnit(who);
    id = *(unsigned short *)(u + off) & 0x1ff;
    info = GetItemInfo(id);
    if (id == 0)
        return -1;
    flags = info[3];
    if (flags & 8)
        return -4;
    if ((*(unsigned short *)(u + off) & (0x80 << 2)) && (flags & 2))
        return -3;
    return 0;
}
