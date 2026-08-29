/* Cluster OvlFunc_899_200c698..OvlFunc_899_200c698 extracted from goldensun/asm/overlays/rom_794ac0/ovl_30_c_c_c_c_c_c_a.s.
 *
 * Total .text for this TU = 98 bytes (= 0x62).
 * Placed in the run in goldensun/overlays/rom_794ac0/overlay.ld.
 *
 * Byte-identical to OvlFunc_902_2008570 in overlays/rom_7987ac; this C is shared verbatim, with only the symbol changed.
 *
 * Spawns the item-icon actor: creates it, clears two sprite bytes and one flag
 * bit, keeps only the low nibble of another, seeds two actor bytes, then
 * borrows an IWRAM block under tag 0x11 to stage the icon graphics before
 * uploading them and releasing the block.
 *
 * THE TWO MASKS HAVE DIFFERENT WIDTHS AND THEREFORE DIFFERENT SPELLINGS, which
 * is the batch-71 rule doing its job in one function:
 *
 *   sprite +5   `mov r3, #0x21 / neg r3, r3`   32-bit -> a BITFIELD, `f5_b5 = 0`
 *   sprite +9   `mov r3, #0xf`                 byte   -> hand-written, `f9 &= 0xf`
 *
 * Read the width off the ROM and the spelling follows. Matched on the first
 * screen once both were written that way.
 *
 * The zero stored to three different fields is a named local, because the ROM
 * keeps it in r5 across the whole body rather than re-materialising it.
 */

struct S {
    unsigned char pad00[5];
    unsigned char f5_lo : 5;
    unsigned char f5_b5 : 1;
    unsigned char f5_hi : 2;
    unsigned char pad06[3];
    unsigned char f9;
    unsigned char pad0a[0x12];
    unsigned char f1c;
    unsigned char pad1d[9];
    unsigned char f26;
    unsigned char f27;
};

struct A {
    unsigned char pad00[0x50];
    struct S *spr;
    unsigned char pad54;
    unsigned char f55;
    unsigned char pad56[6];
    unsigned char f5c;
};

extern struct A *__CreateActor(int kind);
extern void *__galloc_iwram(int tag, int size);
extern void __LoadItemIcon(int id);
extern void __UploadSpriteGFX(int slot, int n, void *src);
extern void __gfree(int tag);

void OvlFunc_899_200c698(int id)
{
    struct A *act;
    struct S *s;
    void *buf;
    int z;

    z = 0;
    act = __CreateActor(0x16);
    if (act == 0)
        return;
    s = act->spr;
    s->f26 = z;
    s->f27 = z;
    s->f5_b5 = 0;
    s->f9 &= 0xf;
    act->f55 = z;
    act->f5c = 1;
    buf = __galloc_iwram(0x11, 0xc1 << 3);
    __LoadItemIcon(id);
    __UploadSpriteGFX(s->f1c, 0x80, (char *)buf + (0x80 << 3));
    __gfree(0x11);
}
