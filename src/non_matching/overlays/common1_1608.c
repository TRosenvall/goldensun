/* OvlFunc_common1_1608 -- asm/overlays/common/common1_a_c_c_c.s
 *
 * WORTH TWO FUNCTIONS: x2 duplicate group with OvlFunc_948_200a0c4
 * (asm/overlays/rom_7d30e0/ovl_30_c_c_c_c_c_c_c_c_c_c_a.s).
 *
 * BLOCKER: two constant-width residues. 16 of 84, one line long.
 *
 * Reloads a map actor's item icon: allocate a 0x608-byte scratch, DMA-clear
 * 0x80 bytes at +0x400, load the icon, upload it, free, then rewrite the
 * sprite's layer/palette fields. Prologue, all six calls, the high-register
 * save/restore and every field store reproduce.
 *
 * THIS FUNCTION IS WHY THE r8-r11 REJECT WAS REMOVED. It uses r8, r9 and r10,
 * and it came within 16 lines on a second attempt from ordinary C -- no
 * pinning, no tricks. What puts values in high registers is simply having more
 * call-crossing locals than r5/r6/r7 can hold: here the item id across
 * __MapActor_GetActor, the status byte across four calls, and a zero used by
 * five later stores.
 *
 * THE LEVER THAT MOVED IT MOST, 28 differing to 20, first divergence 22 -> 57:
 * the ROM births the zero BETWEEN the two halves of the buffer offset
 * computation. Splitting `buf = galloc(...) + 0x400` into
 *
 *     buf = galloc(...);
 *     zero = 0;
 *     buf += 0x80 * 8;
 *
 * is the statement-split lever from Func_80b9a70, and it applies here because
 * `zero` is a LIVE LOCAL -- used by five stores after the split -- which is
 * exactly the precondition docs/elevation.md records for it.
 *
 * WHAT REMAINS, both constant-width:
 *
 *   1. rom  `mov r3, #0x21 / neg r3, r3 / and r3, r2`   (mask -0x21, 32-bit)
 *      ours `mov r3, #0xdf / and r3, r2`                (narrowed to a byte)
 *      The store target is `unsigned char`, so gcc narrows the mask and saves
 *      an instruction. Naming the mask in an int local produces the ROM's form
 *      but costs lines elsewhere.
 *
 *   2. rom  loads 0x3ff FIRST, masks the gfx handle, THEN loads 0xfffffc00
 *      ours loads 0xfc00 (narrowed from 0xfffffc00) and masks the halfword
 *      first.
 *
 * MEASURED:
 *   baseline                                        85 lines, 28 differ
 *   + zero split out of the buffer offset           85 lines, 20 differ
 *   + halfword mask through an int temp             84 lines, 18 differ
 *   + byte mask through a named int local           85 lines, 16 differ  <- best
 *   both masks sharing ONE temp                     85 lines, 36 differ
 *   byte mask alone                                 86 lines, 25 differ
 *   OR operands swapped (gfx first)                 85 lines, 16 differ (tie)
 *   gfx masked in its own statement                 85 lines, 21 differ
 *
 * Sharing one temp between the two masks is the sharpest negative: it undoes
 * the zero-split gain and sends the first divergence back to instruction 22.
 * The two fixes need separate locals.
 *
 * NOT INSTALLED. The reference keeps its pool inside the function, so per
 * docs/elevation.md the count is advisory in both directions -- and the tail
 * differences here are the double-label artifact, not real. But 16 includes
 * two genuine constant-width clusters, so this is a real miss, not an
 * artifact-only one.
 *
 * ROUND-2 UPDATE -- THE HALFWORD MASK IS NOT REACHABLE BY SPELLING.
 * Five further forms, all 16 differing and byte-identical to each other:
 *
 *   computation split from the store (`hw = ...; *(u16 *)(s+8) = hw;`)
 *                                             83 lines, 19 differ (WORSE)
 *   the mask applied in its own statement      85 lines, 16 differ
 *   `~0x3ff` instead of the literal            85 lines, 16 differ
 *   `hw` unsigned with an unsigned mask        85 lines, 16 differ
 *   `-0x400` instead of the literal            85 lines, 16 differ
 *
 * gcc canonicalises every spelling of 0xfffffc00 to one rtx and then narrows
 * it to 0xfc00, because the destination is a halfword store and the upper bits
 * are provably dead. The ROM pool-loads the full 32-bit constant. Nothing in
 * the source decides this -- it is a width inference on the STORE, not on the
 * expression, and the byte mask above (-0x21) only came out right because
 * naming it in an int local kept a live 32-bit value that gcc could not sink
 * into the store.
 *
 * So the two constant-width residues are NOT the same problem, and that is
 * the useful distinction: the byte one is reachable (name the mask), the
 * halfword one is not (the narrowing happens at the store regardless).
 *
 * Remaining after all of the above: the halfword mask, and one scheduling swap
 * where the ROM stores the byte before loading the next constant. Everything
 * else, including the whole high-register prologue and epilogue, is exact.
 */
#include "dma.h"

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
    int hw;

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
    hw = *(unsigned short *)(s + 8);
    *(unsigned short *)(s + 8) = (gfx & 0x3ff) | (hw & 0xfffffc00);
    s[0x25] = zero;
    s[0x26] = zero;
}
