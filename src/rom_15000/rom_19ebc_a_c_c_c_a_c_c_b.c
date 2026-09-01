typedef struct {
    unsigned char pad[0x600];
    short f600;
    short f602;
    int f604;
} Blk;

extern Blk *iwram_3001e94;
extern unsigned char *_GetItemInfo(int id);
extern int L29ee4[] __asm__(".L29ee4");
extern void LoadIcon(Blk *b, int n);

void LoadItemIcon(int id)
{
    Blk *b;
    unsigned char *info;

    b = iwram_3001e94;
    info = _GetItemInfo(id & 0x1ff);
    if (id != 0)
        b->f604 = L29ee4[*(unsigned short *)(info + 6)];
    else
        b->f604 = L29ee4[0];
    b->f600 = 2;
    b->f602 = 2;
    LoadIcon(b, 0);
}
