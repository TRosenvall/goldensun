/* Func_801c0dc  --  0x0801c0dc, split out of asm/rom_15000/rom_1aeec_c_a_a_a_a_a_a.s;
 * Func_801c154 (parked) stays in _c.s. Matched from scratch.
 *
 * The OAM attribute fields must be assigned in the ROM's order (tileNum,
 * objMode, mosaic, bpp, affineMode, matrixNum, size, shape, priority), with
 * `void *src = L342f8;` first. .L342f8 is exported by rom_1aeec_c_c_b.s.
 */
struct OamSprite {
    unsigned char pad[4];
    /* attr0 */
    unsigned int y:8;
    unsigned int affineMode:2;
    unsigned int objMode:2;
    unsigned int mosaic:1;
    unsigned int bpp:1;
    unsigned int shape:2;
    /* attr1 */
    unsigned int x:9;
    unsigned int matrixNum:5;
    unsigned int size:2;
    /* attr2 */
    unsigned int tileNum:10;
    unsigned int priority:2;
    unsigned int paletteNum:4;
};
extern unsigned char L342f8[] __asm__(".L342f8");
extern int AllocSpriteSlot(void);
extern int UploadSpriteGFX(int slot, int n, void *src);

void Func_801c0dc(struct OamSprite *o, int *slot)
{
    void *src = L342f8;
    int t;
    *slot = AllocSpriteSlot();
    t = UploadSpriteGFX(*slot, 0x80, src);
    o->tileNum = t;
    o->objMode = 0;
    o->mosaic = 0;
    o->bpp = 1;
    o->affineMode = 0;
    o->matrixNum = 0;
    o->size = 0;
    o->shape = 2;
    o->priority = 0;
}
