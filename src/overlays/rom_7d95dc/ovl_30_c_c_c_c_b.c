/* OvlFunc_953_200ab1c  --  0x0200ab1c, cut from
 * goldensun/asm/overlays/rom_7d95dc/ovl_30_c_c_c_c.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7d95dc/ovl_30_c_c_c_c_a.o and
 * asm/overlays/rom_7d95dc/ovl_30_c_c_c_c_c.o in
 * goldensun/overlays/rom_7d95dc/overlay.ld.
 *
 * Assigns palettes 3, 0, 4, 1, 5, 2, 6 to slots 0xc..0x12 -- the ordering is
 * the ROM's and is transcribed as found -- then gives five of them staggered
 * animation speeds ten frames apart and starts slot 0xf on animation 0.
 *
 * Thirteen calls in a straight line with nothing but immediates; it matched on
 * the first screen. The reference keeps its literal pool inside the function,
 * so this went to `make compare` rather than being trusted from the screen.
 */
extern void __Func_8092950(int slot, int pal);
extern void __MapActor_SetAnimSpeed(int slot, int s);
extern void __MapActor_SetAnim(int slot, int a);

void OvlFunc_953_200ab1c(void)
{
    __Func_8092950(0xc, 3);
    __Func_8092950(0xd, 0);
    __Func_8092950(0xe, 4);
    __Func_8092950(0xf, 1);
    __Func_8092950(0x10, 5);
    __Func_8092950(0x11, 2);
    __Func_8092950(0x12, 6);
    __MapActor_SetAnimSpeed(0xd, 0xa);
    __MapActor_SetAnimSpeed(0xe, 0x14);
    __MapActor_SetAnim(0xf, 0);
    __MapActor_SetAnimSpeed(0x10, 0x28);
    __MapActor_SetAnimSpeed(0x11, 0x32);
    __MapActor_SetAnimSpeed(0x12, 0x3c);
}
