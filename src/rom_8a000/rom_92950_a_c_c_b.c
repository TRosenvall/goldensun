/* SetSlotDrawPriority -- 0x08092b08.  LANDING, BYTE EXACT, pin-free.
 *
 * Writes bits 2-3 of both actor byte +0x09 and byte +0x15 -- the OAM priority
 * of the main sprite and of its companion/shadow -- so the pair stays on the
 * same layer, then clears bit 0 of the entity's +0x23.  Draw kind 1 only.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_8a000/rom_92950_a_c_c_b.c \
 *     asm/rom_8a000/rom_92950_a_c_c_b.s --whole
 *
 * Split: tools/split_s.py asm/rom_8a000/rom_92950_a_c_c.s Func_8092b08
 *   -> _b.s (this function, 47 lines) and _c.s (Func_8092b54, 33 lines); no _a.
 *   tools/datacheck.py is silent on the file; exports [].
 * Pins 0.  Devices 0.  Default flags.
 *
 * TWO EDITS CLOSED 17 OF 37.  Both are recorded in the retired park.
 *   `int three = 3;` set OUTSIDE the block that holds the `and` makes
 *   reg_is_remote_constant_p (regmove.c:856-901) true, so fixup_match_1
 *   (regmove.c:1651) refuses to move the `and`'s destination off prio's
 *   pseudo; prio keeps four refs, and allocno_compare (global.c:607) then
 *   ranks it above the actor pointer and it takes r5.  That is 17 -> 6.
 *   Assigning `s` last, below `v`, is the other 6 -- a local-alloc
 *   (local-alloc.c:1496 QTY_CMP_PRI) ordering between `s` and the byte it
 *   loads.
 */
#include "gba/types.h"
#include "actor.h"

extern struct Actor *GetFieldActor(int slot);

void Func_8092b08(int slot, int prio)
{
    struct Actor *a;
    unsigned char *s;
    int m, v;

    a = GetFieldActor(slot);
    if (a != 0) {
        int three = 3;
        if ((a->drawKind & 0xf) == 1) {
            prio &= three;
            m = -0xd;
            v = prio << 2;
            s = (unsigned char *)a->sprite;
            s[9] = (s[9] & m) | v;
            s[0x15] = (s[0x15] & m) | v;
            a->flags &= 0xfe;
        }
    }
}
