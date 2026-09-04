// fakematch
/* OvlFunc_947_200a040  --  0x0200a040
 *
 * From goldensun/asm/overlays/rom_7d0e88/ovl_1528_a_c_a.s, which held this
 * function alone, so no split was needed.
 *
 * Copies the actor's position into a stack temp, raises z, and hands the temp
 * to a test that picks one of two follow-ups.
 *
 * PARKED AT 11 OF 29 ON A REGISTER ROLE SWAP, with the length already exact.
 * The ROM moves the actor pointer OUT of r0 the moment it is returned and
 * builds the temp's address in r0 instead:
 *
 *     rom   mov r1, r0 / ldr r3, [r1, #8] / mov r0, sp / str r3, [r0, #0] ...
 *     ours  ldr r3, [r0, #8] / mov r2, sp / str r3, [r2, #0] ... / mov r0, r2
 *
 * Both are three field copies and a call; they disagree only about WHICH
 * register holds the actor and which holds the temp. gcc keeps the returned
 * pointer where it landed and moves the temp into r0 at the end, one
 * instruction later than the ROM and with the roles exchanged throughout.
 *
 * ONE PIN ON THE ACTOR CLOSES IT. Declaring the actor `register Actor *pa
 * __asm__("r1")` forces the `mov r1, r0` immediately after the call, which
 * leaves r0 free for the temp and the other ten instructions fall into place
 * on their own.
 *
 * TORN DOWN, because the first three forms that matched were all larger:
 *
 *     pin the actor (r1), the temp (r0) and the constant (r2)   OK
 *     pin the actor and the temp                                OK
 *     pin the temp ALONE                                        5 differing
 *     pin the actor ALONE                                       OK   <- kept
 *
 * The asymmetry is the finding. Pinning the temp alone does NOT work, because
 * it says nothing about where the actor goes and gcc still leaves it in r0,
 * forcing the temp somewhere else. Pinning the actor alone DOES, because
 * moving the actor off r0 is the whole decision -- everything after it follows
 * from r0 being free at the right moment. Anchor the value that has to MOVE,
 * not the one that has to arrive.
 *
 * NOT A FLAGS CASE, though it was reached by a sweep looking for one. The
 * sweep matched parks to translation units by address SUFFIX, and two
 * functions in different overlays end in _200a040; this park holds
 * OvlFunc_947_200a040 in rom_7d0e88, and it was screened against
 * OvlFunc_964_200a040's file in rom_7ed0a0. No wildcard rule covers
 * rom_7d0e88 at all. Match a park to its .s by the function NAME its header
 * gives, never by the address in its filename.
 */

#include "gba/types.h"
#include "actor.h"

extern Actor *__MapActor_GetActor(int slot);
extern int OvlFunc_947_2008350(vec3_t *p);
extern void OvlFunc_947_2009fd4(void);
extern int OvlFunc_947_2009268(void);

void OvlFunc_947_200a040(void)
{
    vec3_t v;
    register Actor *pa __asm__("r1");

    pa = __MapActor_GetActor(0);
    v.x = pa->pos.x;
    v.y = pa->pos.y;
    v.z = pa->pos.z + (0x80 << 13);
    if (OvlFunc_947_2008350(&v)) {
        OvlFunc_947_2009fd4();
    } else if (OvlFunc_947_2009268() == 0) {
        __Func_8093e28();
    }
}
