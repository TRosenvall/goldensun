/* Cluster OvlFunc_956_2008a44..OvlFunc_956_2008a44 extracted from goldensun/asm/overlays/rom_7e0928/ovl_30_c_c_a_a.s.
 *
 * The .s held ONLY this function and no data, so no split was needed -- the .o
 * keeps its name and its slot in goldensun/overlays/rom_7e0928/overlay.ld is
 * unchanged.
 *
 * Sets an actor moving on a script: clear its interact byte, give it a speed
 * and an acceleration, start an animation, attach the script, set a flag.
 *
 * Two details worth the note.
 *
 * THE SPEED AND ACCELERATION ARE THE SAME VALUE, loaded once and stored twice:
 *
 *     ldr r3, =0x19999 / str r3, [r5, #0x34] / str r3, [r5, #0x30]
 *
 * so it goes through a single named local. That is NOT the constant-CSE class
 * -- gcc CSEs it here and so does the ROM, because both stores are on one path
 * with no call between them. The class only bites when the repetitions are
 * separated by a call, which is the rule recorded in
 * src/non_matching/overlays/constant_reuse.c.
 *
 * ONE DECLARATION. __Actor_SetScript wants r0 filled before r1 and gcc filled
 * it after:
 *
 *     rom    mov r0, r5 / ldr r1, =gScript_956__0200cc48
 *     ours   ldr r1, =gScript_956__0200cc48 / mov r0, r5
 *
 * __Actor_SetAnim needs no declaration, because r0 still holds the actor from
 * __MapActor_GetActor and nothing reloads it -- the ROM does not emit a `mov r0`
 * there at all.
 */
#include "gba/types.h"
#include "actor.h"

extern void __Actor_SetScript(void *a, void *s);
extern Actor *__MapActor_GetActor(int slot);
extern unsigned char gScript_956__0200cc48[];

void OvlFunc_956_2008a44(void)
{
    Actor *a;
    u8 *p;
    fx32 v;

    a = __MapActor_GetActor(0x1e);
    p = (u8 *)a + 0x55;
    *p = 0;
    v = 0x19999;
    a->accel = v;
    a->speed = v;
    __Actor_SetAnim(a, 2);
    __Actor_SetScript(a, gScript_956__0200cc48);
    __SetFlag(0x363);
}
