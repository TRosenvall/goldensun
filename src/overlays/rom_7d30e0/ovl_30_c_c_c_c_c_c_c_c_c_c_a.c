/* Cluster OvlFunc_948_200a0c4..OvlFunc_948_200a0c4 extracted from
 * goldensun/asm/overlays/rom_7d30e0/ovl_30_c_c_c_c_c_c_c_c_c_c_a.s.
 *
 * Total .text for this TU = 196 bytes (= 0xc4). Never attempted before batch 278.
 * No pins, no flags. This file needs no split -- the function is alone in its `.s`.
 *
 * IDENTICAL TWIN OF OvlFunc_common1_1608 (src/overlays/common/common1_a_c_c_c_b.c) -- read
 * that file for the derivation, which turns on writing the 10-bit field as a BITFIELD rather
 * than a masked expression, because `store_bit_field` builds the read-modify-write in SImode
 * and so keeps the ROM's 32-bit mask. Each source was verified by objcmp against its OWN
 * reference rather than inferred from the other. If you edit one, edit both.
 */
#include "dma.h"

struct SprBits {
    unsigned short gfx : 10;
    unsigned short rest : 6;
};

extern unsigned char *__MapActor_GetActor(int slot);
extern void *__galloc_iwram(int tag, int size);
extern void __LoadItemIcon(int item);
extern int __UploadSpriteGFX(int a, int b, void *c);
extern void __gfree(int tag);
extern void __DeleteSpriteLayer(int layer);

void OvlFunc_948_200a0c4(int slot, int item)
{
    unsigned char *a;
    unsigned char *s;
    unsigned char *buf;
    int st;
    int zero;
    int gfx;
    int m;

    a = __MapActor_GetActor(slot);
    if (a == 0)
        return;
    st = a[0x54];
    if (st != 1)
        return;
    s = *(unsigned char **)(a + 0x50);
    buf = (unsigned char *)__galloc_iwram(0x11, 0xc1 * 8);
    zero = 0;
    buf += 0x80 * 8;
    DMA3_CLEAR(buf, 0x80);
    __LoadItemIcon(item);
    gfx = __UploadSpriteGFX(s[0x1c], 0x80, buf);
    __gfree(0x11);
    a[0x5c] = st;
    __DeleteSpriteLayer(*(int *)(s + 0x28));
    *(int *)(s + 0x28) = zero;
    s[0x27] = zero;
    m = -0x21;
    s[5] = s[5] & m;
    ((struct SprBits *)(s + 8))->gfx = gfx;
    s[0x25] = zero;
    s[0x26] = zero;
}
