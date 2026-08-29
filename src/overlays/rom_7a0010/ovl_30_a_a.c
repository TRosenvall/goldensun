/* Overlay 912: initialise a fifteen-entry slot table.
 *
 * Whole-file conversion of asm/overlays/rom_7a0010/ovl_30_a_a.s.
 */

struct Slot {
    /* 0x00 */ unsigned short id;
    /* 0x02 */ unsigned char pad_02[2];
    /* 0x04 */ int flags;
    /* 0x08 */ unsigned char pad_08[14];
    /* 0x16 */ unsigned char kind;
    /* 0x17 */ unsigned char pad_17;
};

/* Fills all fifteen entries with the same defaults, then overrides the id on
 * entries 4 and 7 -- so those two are a different kind of thing in an
 * otherwise uniform table.
 *
 * The counter is unsigned: the ROM ends the loop with `bls`, not `ble`.
 */
void OvlFunc_912_2008030(struct Slot *slot)
{
    unsigned int i;

    for (i = 0; i <= 0xe; i++) {
        slot->kind = 2;
        slot->flags = 1;
        slot->id = 0x69;
        if (i == 4 || i == 7)
            slot->id = 0x6e;
        slot++;
    }
}
