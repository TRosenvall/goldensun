/* Cluster Func_8092a1c..Func_8092a1c extracted from goldensun/asm/rom_8a000/rom_92950_a_a_c_a.s.
 *
 * Total .text for this TU = 88 bytes.
 *
 * THE PARKED C WAS ALREADY CORRECT and had been written off on the retired
 * branch-over-pool claim. Nothing about it needed changing; the mid-body
 * `.word 0` behind `.pool_aligned` reproduces exactly, with the `b` over it.
 *
 * tools/tryc.py reports 6 differing here and all six are label-naming
 * artifacts -- ours emits `b L2 / L2: / L0:` where the ROM has `b L0 / L0:`,
 * both at the same address. The assembled .text is byte-identical.
 */
#include "gba/types.h"
#include "actor.h"

extern struct Actor *GetFieldActor(int slot);
extern void _Actor_SetScript(struct Actor *a, void *script);

void Func_8092a1c(int slot, int packed, void *script)
{
    struct Actor *a;
    struct Actor *t;

    a = GetFieldActor(slot);
    t = GetFieldActor(packed & 0xff);
    if (a != 0 && t != 0) {
        a->unk_68 = (u32)t;
        if ((packed & (0x80 << 9)) == 0) {
            a->goalFacing = 0x28;
            a->accel = t->accel << 1;
            a->speed = t->speed;
            a->interactFlags = 0;
        }
        _Actor_SetScript(a, script);
    }
}
