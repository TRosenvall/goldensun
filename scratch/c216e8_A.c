extern int _FILE_f1;
#define FILE_f1 ((int)&_FILE_f1)

extern void *galloc_ewram(int slot, int size);
extern unsigned char *GetFile(int id);
extern void DecompressLZ1(unsigned char *src, void *dst);
extern void _Func_800f9cc(void *p, int n);
extern void UploadSpriteGFX(int slot, int size, void *p);
extern void gfree(int slot);

void StartMenu_AddOption(int idx, int slot, int flag)
{
    void *buf;
    unsigned char *file;
    int size;
    int off;

    size = 0x80 << 3;
    buf = galloc_ewram(0xe, size);
    file = GetFile(FILE_f1);
    if (slot <= 0x5f) {
        off = idx * 2;
        DecompressLZ1(file + *(unsigned short *)(off + file), buf);
        if (flag)
            _Func_800f9cc(buf, 0xc0 << 2);
        UploadSpriteGFX(slot, size, buf);
        gfree(0xe);
    }
}
