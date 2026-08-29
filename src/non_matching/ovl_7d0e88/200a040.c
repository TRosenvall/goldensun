/* OvlFunc_947_200a040  [ovl_7d0e88]
 *
 * Source asm: goldensun/asm/overlays/rom_7d0e88/ovl_1528_a_c_a.s
 *
 * NOT SPLIT, and it would not need one -- the .s holds only this function.
 *
 * Builds a position vector on the stack from the player actor, offset upward,
 * and dispatches on what a test of it returns. Twenty-nine instructions against
 * twenty-nine, eleven positions differing, and all eleven follow from one
 * choice.
 *
 * Blocker: WHICH VALUE LIVES IN r0. The ROM moves the ACTOR out to r1 and puts
 * the outgoing struct address in r0 immediately:
 *
 *     rom    mov r1, r0 / ldr r3, [r1, #8] / mov r0, sp / str r3, [r0] ...
 *     ours   ldr r3, [r0, #8] / mov r2, sp / str r3, [r2] ... / mov r0, r2
 *
 * gcc keeps the actor in r0, builds the struct through r2, and reloads r0
 * before the call. Same instruction count; every difference is that register
 * assignment carried through the six stores.
 *
 * This is the SAME residue as src/non_matching/overlays/common0_70.c, which is
 * worth five functions -- there the ROM leaves a returned pointer in r0 and
 * puts a derived pointer in r1, and gcc does the reverse. Two independent
 * functions now sit on "which of two live pointers gcc leaves in r0", and
 * nothing in docs/elevation.md addresses it.
 *
 * TRIED:
 *   1. the stack object addressed directly, `&v` at the call (the form below)
 *   2. a named `vec3_t *pv = &v;` used for all three stores and the call --
 *      byte-identical to (1)
 *
 * (2) failing is consistent with the note already in docs/elevation.md that the
 * named-intermediate lever does not generalise to stack-object addresses. That
 * is now confirmed rather than inferred.
 */
#include "gba/types.h"
#include "actor.h"

extern Actor *__MapActor_GetActor(int slot);
extern int OvlFunc_947_2008350(vec3_t *p);
extern void OvlFunc_947_2009fd4(void);
extern int OvlFunc_947_2009268(void);

void OvlFunc_947_200a040(void)
{
    Actor *a;
    vec3_t v;

    a = __MapActor_GetActor(0);
    v.x = a->pos.x;
    v.y = a->pos.y;
    v.z = a->pos.z + (0x80 << 13);
    if (OvlFunc_947_2008350(&v)) {
        OvlFunc_947_2009fd4();
    } else if (OvlFunc_947_2009268() == 0) {
        __Func_8093e28();
    }
}
