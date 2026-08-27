/* OvlFunc_945_200c7cc  --  0x0200c7cc
 *
 * Cut out of goldensun/asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_c.s.
 *
 * Gives each of nine villagers its idle animation and speed.
 *
 * THE CASE ORDER IS READ OFF THE BLOCKS: 0x13, then 0x12/0x14, then 0x16/0x17,
 * then 0x18, then 0x15/0x19, then 0x1a. Numeric order would put 0x12 first and
 * would not match.
 *
 * gcc cross-jumps two arms that share a tail -- case 0x13 (anim 6) and case
 * 0x18 (anim 0xa) both end with speed 8, and the ROM's `.L4832` is the merged
 * `bl __MapActor_SetAnim / ... / bl __MapActor_SetAnimSpeed`. Nothing in the C
 * asks for that; writing both arms out in full is enough.
 *
 * Matched on the first screen.
 */
extern void __MapActor_SetAnim(int slot, int n);
extern void __MapActor_SetAnimSpeed(int slot, int n);

void OvlFunc_945_200c7cc(int slot)
{
    switch (slot) {
    case 0x13:
        __MapActor_SetAnim(slot, 6);
        __MapActor_SetAnimSpeed(slot, 8);
        break;
    case 0x12:
    case 0x14:
        __MapActor_SetAnim(slot, 5);
        __MapActor_SetAnimSpeed(slot, 0x10);
        break;
    case 0x16:
    case 0x17:
        __MapActor_SetAnim(slot, 5);
        __MapActor_SetAnimSpeed(slot, 0x14);
        break;
    case 0x18:
        __MapActor_SetAnim(slot, 0xa);
        __MapActor_SetAnimSpeed(slot, 8);
        break;
    case 0x15:
    case 0x19:
        __MapActor_SetAnim(slot, 5);
        __MapActor_SetAnimSpeed(slot, 4);
        break;
    case 0x1a:
        __MapActor_SetAnim(slot, 9);
        __MapActor_SetAnimSpeed(slot, 4);
        break;
    }
}
