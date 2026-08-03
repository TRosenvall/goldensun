/* Overlay 968: forward an actor's turn target, masked to its low nibble.
 *
 * Split out of asm/overlays/rom_7f2f14/ovl_30_a_a_a_c_c.s; the neighbouring
 * parts stay as assembly and are listed around this one in
 * overlays/rom_7f2f14/overlay.ld, so the ROM layout is unchanged.
 *
 * NOTE this overlay has -O1 rules in the Makefile for other stems
 * (ovl_30_c_a_c_a_c_c% and ovl_30_c_a_c_a_c_a%). This file matches neither, so
 * it builds at the default -O2 -- checked before landing, because screening a
 * -O1 translation unit at -O2 produces a clean-looking match that then fails
 * the build.
 */
#include "actor.h"

extern void __Func_80929d8(Actor *actor, int value);

/* The low nibble of the turn target at +0x64 is the 16-way direction (see the
 * note on `facing` in actor.h); the upper bits are dropped here.
 */
int OvlFunc_968_2008594(Actor *actor)
{
    __Func_80929d8(actor, actor->goalFacing & 0xf);
    return 0;
}
