/* GiveItemTo (0x08078588) -- NON-MATCHING.
 * Blocker class: SCRATCH-REGISTER SELECTION (see docs/elevation.md).
 *
 * 73 lines against the ROM's 73, 54 differing, and after the one real fix
 * below every remaining difference is which of r1/r2/r3/r4 carries a value.
 *
 * THE ONE REAL FIX, and it is worth keeping. The ROM reads inventory slot 0
 * with a REGISTER offset -- `mov r3, #0xd8 / ldrh r3, [r6, r3]` -- where
 * `u + 0xd8 + i * 2` with `i` folded to zero gives pointer arithmetic and an
 * immediate-offset load (`mov r1, r6 / add r1, #0xd8 / ldrh r3, [r1]`).
 * Naming the offset in an int local produces the ROM's form. That is the
 * register-offset rule applied to a constant index, which is a case the
 * recorded note does not cover -- it is written for a computed one.
 *
 * With that fix the loads match and the residue is purely allocation:
 *
 *     rom    ldr r2, =0x1ff ... and r3, r2 ... mov r1, r2
 *     ours   ldr r4, =0x1ff ... and r3, r4
 *
 * MEASURED:
 *   pointer arithmetic for slot 0 (as first written)     73 lines, 54 differing
 *   the offset named, giving the register-offset load     72 lines, 58
 *
 * The named-offset version is one line SHORT because the ROM's `mov r1, r2`
 * copy of the mask has nowhere to come from -- the same
 * copy-into-a-register shape recorded three times already, and inert to source
 * spelling in all of them.
 *
 * The C below is the exact-length version. Per the recognition rule added in
 * batch 171, this got one probe past the point where the diff became a column
 * of `mov rN` against `mov rM`, and then stopped.
 *
 * WHAT IS RIGHT: the two search loops, the stackable-flag test on info[3] & 0x10,
 * the quantity field packed at bits 11..15 with its 0x1d ceiling, the 0x1ff id
 * mask, and the shared `-1` exit reached from both the overflow check and the
 * full-inventory case.
 *
 * NEXT: nothing source-level.
 */
extern unsigned char *GetUnit(int who);
extern unsigned char *GetItemInfo(int item);

int GiveItemTo(int who, int item)
{
    unsigned char *u;
    unsigned char *info;
    unsigned short *p;
    int i;
    int off;
    int mask;
    int v;
    int q;

    u = GetUnit(who);
    info = GetItemInfo(item);
    if ((info[3] & 0x10) != 0) {
        mask = 0x1ff;
        for (i = 0; i <= 0xe; i++) {
            if (((*(unsigned short *)(u + 0xd8 + i * 2) ^ item) & mask) == 0)
                break;
        }
        if (i != 0xf) {
            off = i * 2 + 0xd8;
            v = *(unsigned short *)(u + off);
            q = (v >> 11) + 1;
            if (q > 0x1d)
                return -1;
            *(unsigned short *)(u + off) = (v & 0x7ff) | (q << 11);
            return i;
        }
    }
    p = (unsigned short *)(u + 0xd8);
    off = 0xd8;
    for (i = 0; i <= 0xe; i++) {
        if (*p++ == 0) {
            *(unsigned short *)(u + off) = item;
            return i;
        }
        off += 2;
    }
    return -1;
}
