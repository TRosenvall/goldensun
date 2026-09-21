/* Cluster OvlFunc_common1_1608..OvlFunc_common1_1608 extracted from
 * goldensun/asm/overlays/common/common1_a_c_c_c.s.
 *
 * Total .text for this TU = 196 bytes (= 0xc4). Parked at 16 of 84; elevated in batch 278.
 * No pins, no volatile, no flag change -- ordinary C.
 *
 * IDENTICAL TWIN: OvlFunc_948_200a0c4, at
 * src/overlays/rom_7d30e0/ovl_30_c_c_c_c_c_c_c_c_c_c_a.c -- 90 normalised lines, identical,
 * found with tools/dupfuncs.py and verified by objcmp against its OWN reference. If you edit
 * one, edit both.
 *
 * FLAGS, CHECKED RATHER THAN ASSUMED, because getting this wrong makes every measurement in
 * this file meaningless. `common1` is NOT covered by any override: the COMMON2_CFLAGS rule is
 * the pattern `asm/overlays/common/common2_%.o`, which `common1_*` does not match, so this
 * builds under the generic rule with full GCC296_CFLAGS -- `-mthumb-interwork` AND
 * `-fcall-used-r4`. The one-grep test agrees: `push {r4` appears in ZERO of the 31
 * `asm/overlays/common/common1*.s` files. Re-verified at landing.
 *
 * ===== A BITFIELD INSERTION IS NOT AN EXPRESSION STORE, AND THAT REFUTES THIS PARK. =====
 *
 * The park concluded the halfword mask was "NOT reachable by spelling", because a `(u16)`
 * store target narrows `0xfffffc00` to `0xfc00` however the expression is written. THAT IS
 * TRUE OF AN EXPRESSION STORED TO A HALFWORD. It is not true of a bitfield write: gcc's
 * `store_bit_field` builds the read-modify-write in SImode, so the ROM's
 * `ldr =0x3ff` / `ldr =0xfffffc00` pair comes back and the byte store ahead of it stops being
 * scheduled after the pool load.
 *
 * Replacing
 *
 *     hw = *(unsigned short *)(s + 8);
 *     *(unsigned short *)(s + 8) = (gfx & 0x3ff) | (hw & 0xfffffc00);
 *
 * with a 10-bit bitfield write took it from 16 differing to EXACT in one step.
 *
 * So the recorded rule that a HImode/QImode STORE TARGET TRUNCATES A MASK IN THE RHS stands,
 * and this is its escape: where the ROM's mask is WIDER than the store and the field is a
 * contiguous bit range, write a BITFIELD, not a masked expression. The other recorded escape
 * -- assign into an SImode local first and store that -- is for when the value is not a
 * clean field.
 *
 * It also fixed the pool, which is the corroboration: ours is now
 * [0x3ff, REG_DMA3SAD, 0x85000020, 0xfffffc00], the ROM's order. The 0x3ff still sorts first
 * as a HImode entry and the 0xfffffc00 is now SImode and sorts last -- exactly what the
 * batch-277 "pool entries sort HImode before SImode" reading predicts.
 *
 * One reading note: `ldrh rX, .Lk` (gcc's HImode pool load) and the ROM's `ldr rX, =0x3ff`
 * assemble to the SAME encoding. tryc normalises them and objcmp confirms the bytes -- the
 * same Thumb-1 fact that retired const.sym's _CONST_1f objection in batch 277.
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

void OvlFunc_common1_1608(int slot, int item)
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
