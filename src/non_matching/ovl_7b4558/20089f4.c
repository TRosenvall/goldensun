/* OvlFunc_927_20089f4  [ovl_7b4558]  --  0x020089f4
 *
 * Source asm: goldensun/asm/overlays/rom_7b4558/ovl_30_a_a_c_c_c_c.s
 *
 * HEAD OF A 5-MEMBER FAMILY, one copy per overlay plus one in
 * overlays/common/common0.s. Spawns an actor, clears two sprite flag bits,
 * sets two per-actor bytes, and runs three configuration calls. Returns the
 * actor, or 0 if the spawn failed.
 *
 * Thirty-nine instructions against thirty-nine, sixteen of them identical.
 *
 * THREE THINGS ARE ALREADY SETTLED and should be kept in any further attempt:
 *
 * 1. THE MASK OPERAND ORDER. `m & s->flags`, not `s->flags & m`. The ROM's
 *    combine is `and r3, r2` with the mask as destination. This is the finding
 *    from batch 12's narrow_constant work and it applies unchanged here.
 * 2. THE MASK IS A NAMED INT. `int m = ~0xc;` -- written inline gcc narrows it
 *    to a byte immediate and the mov/neg pair disappears.
 * 3. THE SUCCESS PATH IS THE `if` BODY. Written as an early `return 0` on
 *    failure, gcc hoists the zero to the top of the function and the output is
 *    one instruction long. The ROM branches away on failure, so the spawn path
 *    is the fall-through.
 *
 * Blocker: ONE REORDERING, at instruction 17. The ROM copies the actor pointer
 * into a scratch register BEFORE storing the masked sprite flags, then adds
 * the field offset after:
 *
 *     rom    mov r2, r5 / strb r3,[r1,#9] / add r2,#0x55 / mov r3,#0 / strb ...
 *     ours   strb r3,[r1,#9] / mov r1,r5 / mov r2,#0 / add r1,#0x55 / strb ...
 *
 * The two byte stores are a POINTER WALK in the ROM -- `add r2, #4` advances
 * from +0x55 to +0x59 rather than recomputing the address -- and writing them
 * that way is what brought this from 40 instructions to 39. What remains is
 * only where the base copy lands.
 *
 * TRIED for that last step, both still 39-vs-39 at 17:
 *   1. `q = (unsigned char *)a;` placed before the flags store, with the
 *      `q += 0x55` after it
 *   2. the same with q assigned before the sprite pointer as well
 *
 * gcc emits the store first regardless. This is the same shape as the hoist in
 * src/non_matching/ovl_7b4558/2008ab0.c -- two independent operations that the
 * ROM's compiler ordered the other way -- and the two files are adjacent in
 * this same .s, which makes them worth attempting together if a scheduling
 * lever is ever found. Eleven functions ride on the pair.
 */
#include "actor.h"

struct Spr { unsigned char pad_00[9]; unsigned char flags; };

extern Actor *__CreateActor(int a, int b, int c, int d);
extern void __Actor_SetSpriteFlags(Actor *a, int f);
extern void __Func_80929d8(Actor *a, int n);
extern void __Func_800c548(Actor *a, int n);

Actor *OvlFunc_927_20089f4(int a0, int a1, int a2, int a3)
{
    Actor *a;
    struct Spr *s;
    unsigned char *q;
    int m;

    a = __CreateActor(a3, a0, a1, a2);
    if (a != 0) {
        s = (struct Spr *)a->sprite;
        m = ~0xc;
        s->flags = m & s->flags;
        q = (unsigned char *)a + 0x55;
        *q = 0;
        q += 4;
        *q = 8;
        __Actor_SetSpriteFlags(a, 0);
        __Func_80929d8(a, 0xe);
        __Func_800c548(a, 1);
        return a;
    }
    return 0;
}
