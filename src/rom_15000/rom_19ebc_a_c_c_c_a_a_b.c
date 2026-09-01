typedef struct {
    unsigned char pad[0x600];
    short f600;
    short f602;
    int f604;
} Blk;

extern Blk *galloc_iwram(int tag, int size);
extern void gfree(int tag);
extern unsigned int NumItemIcons(void);
extern void LoadIcon(Blk *b, int n);
extern int L29a10[] __asm__(".L29a10");
extern int L29ee4[] __asm__(".L29ee4");
extern int AllocSpriteSlot(void);
extern int UploadSpriteGFX(int slot, int size, void *gfx);

void LoadItemIconID(int index, int a1, int *p2, int *p3, int a4)
{
    Blk *b;
    int f;

    f = 0;
    b = galloc_iwram(0x11, 0x608);
    if ((unsigned int)index >= NumItemIcons())
        index = 0;
    if (a1 != 0) {
        b->f604 = L29a10[2];
        b->f600 = 2;
        b->f602 = 2;
        LoadIcon(b, 0);
        f = 1;
    }
    b->f604 = L29ee4[index];
    b->f600 = 2;
    b->f602 = 2;
    LoadIcon(b, f);
    if (a4 == 0)
        *p2 = AllocSpriteSlot();
    *p3 = UploadSpriteGFX(*p2, 0x80, (unsigned char *)b + 0x400);
    gfree(0x11);
}
