/* LoadOldUIIcon -- EXACT MATCH.
 *   OK LoadOldUIIcon -- 180 bytes, 80 encodings and 8 relocations identical
 * Verify with:
 *   python3 tools/objcmp.py src/rom_15000/rom_19ebc_a_c_c_a.c \
 *     asm/rom_15000/rom_19ebc_a_c_c_a.s
 * ONE function, no data sections -- converts whole, no split.  NO SHIMS.
 *
 * FIRST TRY, and the reason is the sibling: this is LoadMoveIconID
 * (src/rom_15000/rom_19ebc_a_c_c_c_c_a.c) with two table lookups instead of one.
 * Reusing that file's `Blk` struct -- pad[0x600] / short f600 / short f602 /
 * int f604 -- and its field WRITE ORDER (f604, then f600, then f602) is the whole
 * solution.  Writing f604 first is what makes gcc pool 0x604 and DERIVE 0x602 from
 * it (`ldr r2,=0x604` ... `ldr r3,=0x602`), and the struct is what keeps 0x600 as
 * `mov r3,#0xc0 / lsl r3,#3`.
 *
 * Note the header comment in the reference .s is off by one function: the file
 * named `rom_19ebc_a_c_c_a.s` carries LoadOldUIIcon (its comment says
 * "LoadPortrait"), and `rom_19ebc_a_c_c_c_c_c.s` carries LoadPortrait (its comment
 * says "LoadNamedGraphic").  The `.thumb_func_start` line is the authority.
 */
typedef struct {
    unsigned char pad[0x600];
    short f600;
    short f602;
    int f604;
} Blk;

extern Blk *galloc_iwram(int tag, int size);
extern void gfree(int tag);
extern void LoadIcon(Blk *b, int n);
extern int L29a10[] __asm__(".L29a10");
extern int L29e00[] __asm__(".L29e00");
extern int AllocSpriteSlot(void);
extern int UploadSpriteGFX(int slot, int size, void *gfx);

void LoadOldUIIcon(int a0, int a1, int *p2, int *p3, int a4)
{
    Blk *b;

    b = galloc_iwram(0x11, 0x608);
    b->f604 = L29a10[a1];
    b->f600 = 2;
    b->f602 = 2;
    LoadIcon(b, 0);
    b->f604 = L29e00[a0];
    b->f600 = 2;
    b->f602 = 2;
    LoadIcon(b, 1);
    if (a4 == 0)
        *p2 = AllocSpriteSlot();
    *p3 = UploadSpriteGFX(*p2, 0x80, (unsigned char *)b + 0x400);
    gfree(0x11);
}
