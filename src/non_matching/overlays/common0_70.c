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
 * NOT TRIED YET: anything that would stop gcc copying the actor into r5. That
 * is the whole remaining problem, and nothing in docs/elevation.md addresses
 * which of two live copies of a value gcc keeps in the return register.
 */
#include "gba/types.h"
#include "actor.h"

struct Spr { u8 pad_00[9]; u8 f9; };

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
