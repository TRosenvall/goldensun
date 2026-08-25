/* Cluster LoadUIIcon..LoadUIIcon extracted from goldensun/asm/rom_15000/rom_23178_a_a_a_a_c_c.s.
 *
 * Slotted between rom_23178_a_a_a_a_c_c_a.o and the rest of stage1.ld.
 *
 * Allocates a buffer, decompresses one entry of an icon file into it, uploads
 * it and frees the buffer.
 *
 * THE INDEX IS THE LOAD'S BASE AND THE FILE POINTER ITS OFFSET, which is the
 * reverse of how the expression reads:
 *
 *     rom    ldrh r3, [r5, r0]     r5 = idx << 1, r0 = file
 *     ours   ldrh r3, [r0, r5]     written as `f + (idx << 1)`
 *
 * One instruction of 31, and the fix is to write the arithmetic the way the ROM
 * allocated it -- the index cast to a pointer and the file added to it. This is
 * the "name the pointer to move a load's base and offset" lever from batch 48
 * used in the opposite direction: there an array-of-structs declaration made
 * the TABLE the base, here the index has to be.
 *
 * The size 0x400 and the destination both live in high registers (r8, r10)
 * across three calls. Nothing in the C asks for that; it falls out of pressure.
 */
extern int _FILE_f1;
extern void *Func_8004938(int size);
extern void *GetFile(int id);
extern void DecompressLZ1(void *src, void *dst);
extern void UploadSpriteGFX(void *dst, int size, void *src);
extern void free(void *p);

void LoadUIIcon(void *dst, int idx)
{
    int size;
    void *buf;
    unsigned char *f;
    int off;

    size = 0x80 << 3;
    buf = Func_8004938(size);
    f = (unsigned char *)GetFile((int)&_FILE_f1);
    off = *(unsigned short *)((unsigned char *)(idx << 1) + (unsigned int)f);
    DecompressLZ1(f + off, buf);
    UploadSpriteGFX(dst, size, buf);
    free(buf);
}
