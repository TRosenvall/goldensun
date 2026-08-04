/* OvlFunc_common0_70  [overlays/common]
 *
 * Source asm: goldensun/asm/overlays/common/common0.s
 *
 * NOT SPLIT. common0.s holds many functions and the overlay linker scripts
 * reference it as a unit.
 *
 * Creates an actor, clears two per-actor bytes, sets a sprite flag nibble and a
 * bit at +0x23, and returns the actor or NULL.
 *
 * WORTH FIVE FUNCTIONS. This body appears verbatim five times -- it is the
 * largest UNPARKED group tools/find_twins.py reports. Solving it elevates all
 * five.
 *
 * Forty-eight instructions against forty-eight, ten positions differing, and
 * every one of them follows from a single register-allocation choice.
 *
 *     rom    ldr r1, [r5, #0x50]  ... mov r1, #0 / bl __Actor_SetSpriteFlags
 *     ours   ldr r0, [r5, #0x50]  ... mov r0, r5 / bl __Actor_SetSpriteFlags
 *
 * The ROM puts the sprite pointer in r1 and leaves the ACTOR sitting in r0
 * where __CreateActor returned it, so the later call needs no reload. gcc
 * copies the actor to r5, treats r0 as dead, allocates the sprite pointer
 * there, and then has to restore r0 before the call. Same instruction count,
 * different registers throughout the block.
 *
 * A SECOND, SMALLER THING, and it is the constant-CSE class again: the ROM
 * materialises 0 TWICE -- `mov r3, #0` for the byte store and `mov r1, #0` for
 * the call argument -- where gcc builds it once in r1 and reuses it. That is
 * what frees the instruction gcc then spends on `mov r0, r5`, which is why the
 * counts come out equal.
 *
 * PROGRESS MADE, and worth keeping:
 *
 *   1. The body must sit INSIDE `if (act != 0) { ... return act; } return 0;`
 *      rather than after an early `if (act == 0) return 0;`. The early-return
 *      form makes gcc hoist `mov r0, #0` above the branch; the ROM sets it at
 *      the tail label. That change alone took the diff from 35 positions to 10.
 *   2. The ~0xc mask is a named `int` local. As a literal it narrows to a byte
 *      and the `mov #0xd / neg` pair disappears -- the narrow_constant lever,
 *      and it is doing its job here.
 *   3. The argument shuffle is real: the ROM calls
 *      __CreateActor(d, a, b, c) from OvlFunc_common0_70(a, b, c, d).
 *
 * DIAGNOSIS REFINED. The register swap is not the cause, it is downstream of
 * CONSTANT-CSE, which is a class already known to be unsolved.
 *
 * The ROM materialises 0 TWICE -- `mov r3, #0` for the byte store at +0x55 and
 * `mov r1, #0` for the __Actor_SetSpriteFlags argument. gcc builds it once, in
 * r1, and uses it for both. That occupies r1, so the sprite pointer goes to r0
 * instead; r0 then no longer holds the actor, so gcc has to emit `mov r0, r5`
 * before the call. One CSE, three differing instructions.
 *
 * So this is not a new problem. It is src/non_matching/ovl_7f2f14/20087d8.c and
 * src/non_matching/ovl_794ac0/200852c.c again, in a function where the knock-on
 * effect happens to be a register reassignment rather than a spill.
 *
 * ALSO TRIED, after the two findings above:
 *   4. the sprite access inline with no local, twice -- 49 lines, 33 differ,
 *      gcc loads the pointer twice
 *   5. the sprite local in an inner block to shorten its live range -- no
 *      change, 10 differ
 *   6. `extern void __Actor_SetSpriteFlags(Actor *, int);` -- no change
 *   7. `extern void __Func_80929d8(Actor *, int);` -- 10 differ down to 8, and
 *      it is in the file below because it is a genuine improvement
 *   8. both declarations together -- also 8, so (7) is doing the work
 *
 * What would finish it is whatever defeats constant-CSE, and that is worth far
 * more than these five functions.
 */
#include "gba/types.h"
#include "actor.h"

struct Spr { u8 pad_00[9]; u8 f9; };

extern void __Func_80929d8(Actor *a, int n);
extern Actor *__CreateActor(int a, int b, int c, int d);

Actor *OvlFunc_common0_70(int a, int b, int c, int d)
{
    Actor *act;
    struct Spr *s;
    u8 *p;
    u8 *q;
    int m;

    act = __CreateActor(d, a, b, c);
    if (act != 0) {
        s = (struct Spr *)act->sprite;
        m = ~0xc;
        s->f9 = (s->f9 & m) | 4;
        p = (u8 *)act + 0x55;
        *p = 0;
        p += 4;
        *p = 8;
        __Actor_SetSpriteFlags(act, 0);
        __Func_80929d8(act, 0xf);
        q = (u8 *)act + 0x23;
        *q = (*q & 0xfe) | 2;
        return act;
    }
    return 0;
}
