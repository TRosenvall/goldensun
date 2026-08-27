extern int L29a10[] __asm__(".L29a10");
extern int L29e00[] __asm__(".L29e00");
extern unsigned char *galloc_iwram(int tag, int size);
extern void LoadIcon(unsigned char *blk, int n);
extern int AllocSpriteSlot(void);
extern int UploadSpriteGFX(int a, int b, unsigned char *c);
extern void gfree(int tag);

void LoadOldUIIcon(int a, int b, int *c, int *d, int e)
{
    unsigned char *blk;
    int *dst;
    unsigned short *p600;
    unsigned short *p602;
    int two;

    blk = galloc_iwram(0x11, 0xc1 << 3);
    dst = (int *)(blk + 0x604);
    *dst = L29a10[b];
    p600 = (unsigned short *)(blk + 0x600);
    p602 = (unsigned short *)(blk + 0x602);
    two = 2;
    *p600 = two;
    *p602 = two;
    LoadIcon(blk, 0);
    *dst = L29e00[a];
    *p600 = two;
    *p602 = two;
    LoadIcon(blk, 1);
    if (e == 0)
        *c = AllocSpriteSlot();
    *d = UploadSpriteGFX(*c, 0x80, blk + (0x80 << 3));
    gfree(0x11);
}
