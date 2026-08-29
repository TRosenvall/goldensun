#include "gba/types.h"

struct Panel {
    u8  pad_00[8];
    u16 kind;     /* 0x08 */
    u16 handle;   /* 0x0a */
    u16 tiles;    /* 0x0c */
    u16 gfx;      /* 0x0e */
    u8  pad_10[0x12];
    u16 w;        /* 0x22 */
    u16 h;        /* 0x24 */
    u16 f26;      /* 0x26 */
};

struct Entry {
    u8  pad_00[8];
    u16 file;     /* 0x08 */
    u16 kind;     /* 0x0a */
};

extern u8 *iwram_3001e98;
extern struct Entry *Func_801b36c(u8 *base);
extern void *galloc_iwram(int tag, int size);
extern void *GetFile(int id);
extern void DecompressLZ1(void *src, void *dst);
extern int AllocSpriteSlot(void);
extern int UploadSpriteGFX(int slot, int size, void *gfx);
extern void gfree(int tag);
extern int _FILE_f1;

void Func_801c188(void)
{
    u8 *base;
    struct Entry *ent;
    struct Panel *panel;
    u8 *buf;
    u8 *file;
    u8 *src;
    int n;
    u8 **dst;

    base = iwram_3001e98;
    ent = Func_801b36c(base);
    if (ent->kind == 1 || ent->kind == 6) {
        buf = (u8 *)galloc_iwram(0x11, 0xc1 << 3);
        panel = (struct Panel *)(base + 0x30c);
        n = ent->file;
        file = (u8 *)GetFile((int)&_FILE_f1);
        dst = (u8 **)(buf + 0x604);
        src = file + ((u16 *)file)[ent->file];
        *dst = src;
        DecompressLZ1(src, buf);
        if (panel->handle == 0)
            panel->tiles = AllocSpriteSlot();
        panel->gfx = UploadSpriteGFX(panel->tiles, 0x80 << 3, buf);
        panel->handle = 1;
        panel->kind = n;
        panel->w = 0x28;
        panel->h = 0x28;
        panel->f26 = 0xf0;
        gfree(0x11);
    }
}
