/* Cluster OvlFunc_941_2008094 split out of goldensun/asm/overlays/rom_7c5efc/ovl_30_c_a_c_c_a_c_a.s.
 *
 * MATCHING, byte-identical.  64 bytes, 29 encodings, 3 relocations identical,
 * whole-file clean.  ZERO register pins, no fakematch row, no flag group.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_7c5efc/ovl_30_c_a_c_c_a_c_a_a.c \
 *     asm/overlays/rom_7c5efc/ovl_30_c_a_c_c_a_c_a_a.s --whole
 *
 * THE SPLIT.  The leaf asm/overlays/rom_7c5efc/ovl_30_c_a_c_c_a_c_a_a.s already
 * exists and holds EXACTLY this one function with no data section; datacheck is
 * silent, so no `.global` is needed and no further split_s run is needed.
 *
 * HOW IT LANDED (batch 332).  The park's observation was exactly right and its
 * verdict was exactly wrong, which is the standing pattern.  It observed that
 * the ROM rebuilds BOTH byte-store addresses from the actor pointer
 * (`mov r1,r0 / add r1,#0x23` then `mov r2,r0 / add r2,#0x55`) where gcc chains
 * them, advancing the first address by the difference of 0x32 and so emitting
 * one instruction fewer.  It then concluded "nothing at the expression level --
 * the two stores are independent in the source and gcc relates them anyway",
 * having tried two named pointer locals and the array form.
 *
 * The answer was neither: it is STRUCT FIELD ASSIGNMENT through this tree's own
 * include/actor.h, whose `flags` sits at 0x23 and `interactFlag` at 0x55.
 *
 *     actor->flags = 1;
 *     actor->interactFlag = 0;
 *
 * WHY THE FIELD FORM IS A DIFFERENT PROGRAM.  Pointer arithmetic gives gcc one
 * address pseudo that both stores can share, so cse and the address reload
 * derive the second address from the first.  Two COMPONENT_REFs off the same
 * pointer are two independent addresses built from the base, and there is no
 * shared pseudo to derive from -- which is the reference's own shape, including
 * its register assignment.
 *
 * THE METHOD IS REUSABLE AND IS THE LESSON HERE.  The shape was found by
 * searching the tree's own GENERATED assembly -- the files in asm/ bearing
 * gcc's banner are byte-matching by construction -- for the reference's exact
 * instruction window (a base copy, a value mov, an immediate add, a byte store,
 * then a SECOND copy of the same base).  Nine functions in the tree already
 * emit it; the nearest, OvlFunc_949_2008ca8 in
 * src/overlays/rom_7d4af4/ovl_30_c_c_c_c_b.c, uses the SAME two field offsets
 * and the same `__Func_8010704` tail with its last two arguments named in
 * locals.  Reading one matched neighbour was worth more than every new spelling
 * of the offsets.
 *
 * The two trailing stack arguments stay named locals, as the neighbour has
 * them: that is what puts them in the reference's `str r3,[sp] / str r2,[sp,#4]`
 * order rather than the reverse.
 */
#include "actor.h"

extern void *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __SetFlag(int id);

void OvlFunc_941_2008094(void)
{
    struct Actor *actor;
    int s1;
    int s2;

    actor = (struct Actor *) __MapActor_GetActor(9);
    if (actor != 0)
    {
        actor->flags = 1;
        actor->interactFlag = 0;
    }
    s1 = 8;
    s2 = 0x20;
    __Func_8010704(7, 0x20, 1, 1, s1, s2);
    __SetFlag(0x81 << 2);
}
