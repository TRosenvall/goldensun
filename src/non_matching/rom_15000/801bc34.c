/* UploadIcon -- NOT MATCHING
 *
 * Source asm: goldensun/asm/rom_15000/rom_1aeec_a_a_c_c.s
 * Best screen: 69 instructions against the ROM's 71.
 *
 * BLOCKER CLASS: a returned value staged through a second register.
 *
 *     rom    bl AllocSpriteSlot / mov r2, r0 / str r2, [sp, #8] / cmp r2, #0x60
 *     ours   bl AllocSpriteSlot / str r0, [sp, #8] / cmp r0, #0x60
 *
 * Two arms do this, so two instructions. The slot variable has its address
 * taken (it is an out-parameter of LoadOldUIIcon and LoadMoveIcon), so it lives
 * on the stack and the store is forced; what differs is that the ROM keeps a
 * register copy and compares THAT, while gcc compares the value still in r0.
 *
 * TRIED AND IDENTICAL AT 38 DIFFERING: naming the returned value in its own
 * `int` local and assigning it to the slot separately, which is the spelling
 * that usually forces the extra copy.
 *
 * Everything else is right and was not obvious: the case order off the blocks
 * is 1/6, 2, 9, 4; the two `return -1` arms are cross-jumped into one
 * `mov r0, #1 / neg r0, r0`; and the initial `slot = -1` is stored to the stack
 * before the switch because its address escapes.
 */
extern int AllocSpriteSlot(void);
extern void LoadOldUIIcon(int a, int b, int *slot, int *out, int e);
extern void LoadMoveIcon(int a, int b, int *slot, int *out, int e);
extern void LoadInventoryIcon(int a, int b);
extern void LoadUIBanner(int a, int b);

int UploadIcon(int kind, int arg)
{
    int slot;
    int out;

    slot = -1;
    switch (kind) {
    case 1:
    case 6:
        LoadOldUIIcon(arg, 0, &slot, &out, 0);
        break;
    case 2:
        slot = AllocSpriteSlot();
        if (slot == 0x60)
            return -1;
        LoadInventoryIcon(arg, 0x1a);
        break;
    case 9:
        slot = AllocSpriteSlot();
        if (slot == 0x60)
            return -1;
        LoadUIBanner(arg, 0);
        break;
    case 4:
        LoadMoveIcon(arg, 1, &slot, &out, 0);
        break;
    }
    return slot;
}
