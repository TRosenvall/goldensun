/* Cluster Func_8021620..Func_8021620 extracted from goldensun/asm/rom_15000/rom_20198_c_c_c_a_a_c_a.s.
 *
 * Total .text for this TU = 148 bytes (= 0x94). Never attempted before batch 273.
 * No pins, no flags.
 *
 * THE +0x18 FIELD IS A 10-BIT BITFIELD, and that is the whole function. The first
 * candidate had 40 instructions right and the entire residue was that one update.
 *
 *     unsigned short cnt : 10;   ...   n2->cnt = n2->cnt + 8;
 *
 * gives the ROM's `lsl #22 / lsr #22` extract and its full-width insert mask.
 * Hand-written masking reaches the same instruction COUNT but differs twice: the
 * extract becomes `ldr 0x3ff / and`, and the insert mask narrows to 0xfc00 because gcc
 * knows the value it is inserting. The bitfield spelling is not a cosmetic choice --
 * the width is what makes gcc pick shifts over a pooled mask.
 *
 * Worth adding to the pooled-constant family: a mask that appears as `ldr rX, =0x3ff`
 * where the ROM shifts is the tell for a BITFIELD, not for a symbol.
 */
/* Func_8021620  --  0x08021620
 *
 * AttachIconGraphic.  Reserves a sprite slot, then attaches two display nodes
 * one row (0x20) apart and marks both alive (+0x0F = 0xFD).  The second node's
 * 10-bit counter field at +0x18 is advanced by 8; that field is a BITFIELD --
 * gcc's `lsl #22 / lsr #22` extract and `and 0x3ff` / `and 0xfffffc00` / `orr`
 * insert are extract_bit_field + store_bit_field, not hand-written masking.
 * Spelling `n2->f18 = (v & ~0x3ff) | (((v & 0x3ff) + 8) & 0x3ff)` gets the same
 * 11 instructions with the first mask as `ldr 0x3ff / and` instead of the
 * shift pair, and the insert mask narrowed to 0xfc00.
 */
struct Node {
    unsigned char pad00[0xf];
    unsigned char f0f;
    unsigned char pad10[8];
    unsigned short cnt : 10;
};

extern int AllocSpriteSlot(void);
extern void Func_80215e0(int a, int slot);
extern struct Node *Func_801eadc(int slot, unsigned int k, unsigned int a,
                                 unsigned int b, unsigned int c);

struct Node *Func_8021620(int a0, unsigned int a1, unsigned int a2, unsigned int a3)
{
    int slot;
    struct Node *n1;
    struct Node *n2;

    slot = AllocSpriteSlot();
    if (slot > 0x5f)
        return 0;
    Func_80215e0(a0, slot);
    n1 = Func_801eadc(slot, 0x80004000, a1, a2, a3);
    n1->f0f = 0xfd;
    n2 = Func_801eadc(slot, 0x80004000, a1, a2 + 0x20, a3);
    n2->f0f = 0xfd;
    n2->cnt = n2->cnt + 8;
    return n1;
}
