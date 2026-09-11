/* OvlFunc_905_2008bd0  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_799abc/ovl_30_c_c_a_a_a_c_a.s
 * Best screen: 38 of 121.  Reached as a bonus file-mate while closing
 *   OvlFunc_905_2008ecc in the same original .s.
 *
 * BLOCKER CLASS: register-role alternation inside a hand-written multiply
 * ladder.  The ladder itself is now correct -- see below -- but the ROM
 * alternates which register holds the running sum between successive terms, and
 * the spelling that fixes one term moves the alternation to the next.
 *
 * THE FINDING THAT GOT IT HERE, and it generalises well beyond this function:
 *
 *   gcc-2.96 at this tree's flags NEVER synthesizes a constant multiply.
 *
 * The ROM's `n * 6553` is the classic synth_mult shift-and-add ladder.  Our gcc
 * emits `ldr r3, =6553 / mul` instead.  Probed directly rather than inferred,
 * and the same holds for `n * 13`, so it is not a threshold effect.  The ladder
 * comes back byte-for-byte ONLY when the shifts and adds are written out in the
 * source.
 *
 *   A `mul` against a pooled constant in OUR output where the ROM has a
 *   shift/add chain is a SOURCE-LEVEL question, not a codegen one.
 *
 * That is worth re-screening other parks against: the residue signature is
 * distinctive (one pooled word plus a mul, against a run of lsl/add) and any
 * park showing it has been misfiled as a codegen blocker.  Writing the ladder
 * out took this function from ten instructions SHORT of the ROM to the exact
 * length, which is what exposed the alternation underneath.
 *
 * WHY IT IS PARKED HERE RATHER THAN IN rom_799abc's split: the original
 * asm/overlays/rom_799abc/ovl_30_c_c_a_a_a_c.s held three functions and only the
 * third (2008ecc) closed.  2008bd0 and 2008ce0 remain in _a.s.  2008ce0 was not
 * started; its tail is the same block as this function's.  Closing BOTH would
 * turn that landing from "split first" into a WHOLE conversion and remove the
 * split entirely -- so they are worth attempting together, not separately.
 */
#include "actor.h"

extern int iwram_3001e40;
extern struct Actor *__MapActor_GetActor(int slot);
extern unsigned int __Random(void);
extern void OvlFunc_905_2008a68(int a, int b, int c, int d, int e, int f, int g);

void OvlFunc_905_2008bd0(void)
{
    int z;
    unsigned int n;
    unsigned int t;
    unsigned int d;

    z = iwram_3001e40 & 7;
    if (z == 0) {
        OvlFunc_905_2008a68(
            __MapActor_GetActor(9)->pos.x + ((__Random() * 12 >> 16) << 16),
            __MapActor_GetActor(9)->pos.y,
            __MapActor_GetActor(9)->pos.z + (0xc0 << 11),
            0,
            -(int)((((n = __Random() * 5), (n >>= 16),
                     (t = n * 2 + n), (t = t * 4 + n),
                     (d = t * 64 - t), (d = d * 8)), d + n)),
            __Random() * 2 >> 16,
            z);
    }
    z = iwram_3001e40 & 0xf;
    if (z == 0) {
        OvlFunc_905_2008a68(
            __MapActor_GetActor(9)->pos.x + ((__Random() * 12 >> 16) << 16),
            __MapActor_GetActor(9)->pos.y,
            __MapActor_GetActor(9)->pos.z + (0xc0 << 11),
            0,
            -(int)((((n = __Random() * 5), (n >>= 16),
                     (t = n * 2 + n), (t = t * 4 + n),
                     (d = t * 64 - t), (d = d * 8)), d + n)),
            __Random() * 2 >> 16,
            z);
    }
}
