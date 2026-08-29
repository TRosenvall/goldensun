struct Ui {
    unsigned char pad0[0x8e];
    short count;
};

extern unsigned char *iwram_3001f38;

extern int AllocSpriteSlot(int id);
extern void LoadUIIcon(int slot, int id);

void AddMenuBarOption(int id)
{
    struct Ui *u;
    unsigned char *p;
    int n;
    int s;
    int k;
    int v;

    u = (struct Ui *)iwram_3001f38;
    n = u->count;
    if (n > 5)
        return;
    u->count++;
    s = AllocSpriteSlot(id);
    LoadUIIcon(s, id);
    p = (unsigned char *)(n * 20 + (unsigned int)u);
    *(short *)(p + 0xc) = n * 24 + 0x20;
    v = 0x88;
    *(short *)(p + 0xe) = v;
    k = n + 0x84;
    *(short *)(p + 0x12) = s;
    *((unsigned char *)u + k) = id;
}
