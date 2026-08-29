/* StartMenu_AddOption  --  0x080216e8
 *
 * Cut out of goldensun/asm/rom_15000/rom_20198_c_c_c_a_a_c_a.s.
 *
 * Decompresses one menu icon out of file 0xF1 into a scratch buffer, optionally
 * recolours it, uploads it to the given sprite slot, and frees the buffer.
 *
 * A REGISTER-OFFSET LOAD'S OPERAND ORDER SAYS ARRAY SUBSCRIPT OR POINTER
 * ARITHMETIC, and that was the whole function -- one instruction of forty-five:
 *
 *      rom    ldrh r0, [r3, r2]      <- r3 is idx*2, r2 is the file base
 *      ours   ldrh r0, [r2, r3]
 *
 * `LDRH Rd, [Rn, Rm]` computes the same address either way but encodes Rn and
 * Rm in fixed positions, so the two are different bytes. Writing the access as
 * pointer arithmetic on the base -- `*(unsigned short *)(file + off)` -- puts
 * the BASE first. Writing it as an array subscript -- `((unsigned short *)file)[idx]`
 * -- puts the SCALED INDEX first, which is the ROM.
 *
 * Four spellings were measured: `file + off` and `(int)file + off` both give the
 * base-first form; `off + (int)file` and the subscript both give the ROM's. The
 * subscript is kept because it is what the original plainly meant, and because
 * it does not need the separate `off` local at all.
 *
 * This is worth carrying: a `ldr/ldrh/ldrb rD, [rA, rB]` whose FIRST register
 * holds a scaled index rather than a base is a tell for a subscript, and the
 * base-first form is what naive pointer arithmetic produces.
 *
 * The 0x400 buffer size is a named local because it is passed to both
 * galloc_ewram and UploadSpriteGFX; the ROM keeps it in r10 across the two.
 */
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

    size = 0x80 << 3;
    buf = galloc_ewram(0xe, size);
    file = GetFile(FILE_f1);
    if (slot <= 0x5f) {
        DecompressLZ1(file + ((unsigned short *)file)[idx], buf);
        if (flag)
            _Func_800f9cc(buf, 0xc0 << 2);
        UploadSpriteGFX(slot, size, buf);
        gfree(0xe);
    }
}
