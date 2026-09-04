/* OvlFunc_928_2008500 -- 0x02008500
 *
 * A shove check: while the leader stands in a particular doorway band and this
 * actor is pushing back into it, count frames, and past a threshold set a map
 * word and reset the counter.
 *
 * TWO LEVERS, and the second one CORRECTS AN EXISTING PARK NOTE.
 *
 *  - THE HALFWORD STORE USES A TYPED FIELD. The cast form pools the constant
 *    (`ldr r3, =0xc8`) where the ROM builds it (`mov r3, #0xc8`) -- the
 *    narrow-store table again, and the typed field is again the row that fits.
 *
 *  - THE POOLED ZERO IS A PLAIN LITERAL `0`, NOT A LINKER SYMBOL. The ROM's
 *    `ldr r2, .L58c @ 0` reads exactly like the pooled-small-constant tell, and
 *    `(int)&_AREA_00` does reproduce the pool TEXT -- so the tell fires and is
 *    WRONG here. Writing the field assignment as a bare `0` makes gcc emit a
 *    NARROW pool reference (`ldrh r2, .L8` against `.word 0`), which encodes
 *    the same halfword the ROM's `ldr` does, and that one change fixes three
 *    separate residues at once: it dumps the pool inside the function with the
 *    ROM's branch over it, it sorts `.word 0` AHEAD of `.word iwram_3001ebc`
 *    even though its instruction comes later (the max_address ordering rule),
 *    and it forces the address temporary out of the low registers into ip,
 *    producing the ROM's `mov r2, #0x62 / add r2, r5 / ldrb ... mov r12, r2`.
 *
 *    src/non_matching/ovl_7d6418/2008dd0.c records "byte stores have no QImode
 *    analogue of the halfword pooling exception. MEASURED: they do not." One
 *    plainly does here, and its narrow pool range is load-bearing. That park
 *    should be re-measured with a bare `0`.
 *
 * TEN spellings were measured around the address temporary -- naming it in the
 * guarded block, hoisting it to a dominating block, naming the base pointer,
 * naming the stored constant, and the --no-rerun-cse / --no-sched2 / --O1
 * probes -- and every one of them tied at EXACTLY 27 differing or was worse
 * (67, 72). That exact tie is itself the tell that the variable's existence was
 * never the problem; the whole residue was the one token.
 *
 * ON VERIFYING THIS ONE: tryc.py reports six differing lines that are not real.
 * gcc emits its pool-dump target and the epilogue label at the SAME address, and
 * the ROM's disassembly can only show one label there. Assembling both sides
 * gives 156 bytes against 156, identical encodings and identical relocations at
 * identical offsets.
 */
#include "gba/types.h"
#include "actor.h"

struct Blk {
    unsigned char pad000[0x182];
    short f182;
};

extern struct Blk *iwram_3001ebc;

extern struct Actor *__MapActor_GetActor(int slot);
extern int OvlFunc_928_2008408(struct Actor *a, struct Actor *o, int d, int f);

int OvlFunc_928_2008500(struct Actor *a)
{
    struct Actor *o;
    int f;

    f = 0;
    if (a->targetX == (0x80 << 24) && a->targetZ == (0x80 << 24))
        return 0;

    o = __MapActor_GetActor(0);
    if ((o->pos.x >> 20) > 0x10 && (o->pos.x >> 20) < 0x13
        && (o->pos.z >> 20) == 0xe
        && (a->pos.x >> 20) <= 0x13
        && a->motion.x <= 0) {
        if (o->pos.x <= a->pos.x) {
            a->tickFast++;
            f = 1;
        }
    } else {
        a->tickFast = 0;
    }

    if (f != 0 && a->tickFast > 0x77) {
        iwram_3001ebc->f182 = 0xc8;
        a->tickFast = 0;
    }

    OvlFunc_928_2008408(a, o, 0x12, f);
    return 0;
}
