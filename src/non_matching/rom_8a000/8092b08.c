/* Func_8092b08 (SetSlotDrawPriority) -- NON-MATCHING.
 * Blocker class: REGISTER-ROLE SWAP. 17 of 38, SAME LENGTH, and the structure
 * is right throughout -- the two guards, both sprite-byte masks, the flags
 * clear, and the shared exit all reproduce.
 *
 *     rom    mov r5, r1  (priority)  ... mov r6, r0  (actor)
 *     ours   mov r6, r1              ... mov r5, r0
 *
 * r5 and r6 are exchanged and every later reference follows. Nothing else
 * differs.
 *
 * Tried: saving the parameter into an explicit local before the call, so the
 * two values are created in the ROM's order rather than gcc's. Byte-identical,
 * 17 differing. Which callee-saved register a value lands in is not selected
 * by the source, which is the register-pressure category HANDOFF.md describes.
 *
 * The actor.h form is right and is worth keeping: drawKind at 0x54 masked to
 * its low nibble, sprite at 0x50, the paired priority writes at sprite bytes
 * 9 and 0x15 with an int intermediate for the negated mask, and flags at 0x23
 * cleared of bit 0. Three functions elevated first-screen on exactly this
 * shape this batch, so the shape is not the problem here.
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
        if ((a->drawKind & 0xf) == 1) {
            s = (unsigned char *)a->sprite;
            m = -0xd;
            prio &= 3;
            v = prio << 2;
            s[9] = (s[9] & m) | v;
            s[0x15] = (s[0x15] & m) | v;
            a->flags &= 0xfe;
        }
    }
}
