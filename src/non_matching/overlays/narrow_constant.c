/* THE 34-FUNCTION BLOCKER, MOSTLY SOLVED. Read this before attempting any of
 * them.
 *
 * The shape, from OvlFunc_927_20089dc and 33 others (docs/elevation.md has the
 * search that lists them):
 *
 *     rom    mov r3, #0xd / neg r3, r3      (~0xc as 0xfffffff3, 32-bit)
 *     ours   mov r3, #0xf3                  (~0xc narrowed to a byte)
 *
 * THE FIX: put the mask in a NAMED INT LOCAL.
 *
 *     int f = s->flags;
 *     int m = ~0xc;              <-- this line is the whole trick
 *     s->flags = (f & m) | ...;
 *
 * Written inline as `s->flags & ~0xc`, gcc sees a value it has proved is
 * 0..255 and picks the cheaper 8-bit immediate. Through a named int it commits
 * to 32-bit width and emits the mov/neg pair. Confirmed by probe: of six ways
 * to hide the width -- volatile field, volatile read, bitfield, union with an
 * int, a plain int local, and a named mask -- ONLY THE NAMED MASK WORKS. The
 * volatile and union forms change the load instead, which is worse.
 *
 * WHAT IS STILL OPEN, and it is only ordering. This version is 11 instructions
 * against the ROM's 11, with the last five identical and three transposed:
 *
 *     rom    mov r3, #3 / ldrb r2 / and r1, r3 / mov r3, #0xd / neg r3, r3
 *     ours   mov r3, #3 / and r1, r3 / mov r2, #0xd / ldrb r3 / neg r2, r2
 *
 * The ROM loads the field between building the 3 and using it. SEVEN
 * statement orders have now been tried and this one is still closest:
 *
 *   1. mask inline, field in an int local                 11, diff at 1
 *   2. priority masked into its own local first           11, diff at 1
 *   3. priority modified in place, field between          11, diff at 2  <-- here
 *   4. priority masked and shifted in one statement       12, diff at 1
 *   5. the 3 in a named local, field read after it        10, diff at 1
 *   6. as 5 but mask and shift combined                   12, diff at 1
 *   7. shift folded into the final expression             11, diff at 1
 *
 * Note 5 and 6: naming the 3 as a local makes gcc fold it away entirely and
 * the function comes out a whole instruction SHORT, which is a worse failure
 * than the transposition. The 3 has to stay a literal.
 *
 * So the mask-width half is solved and stable, and only the placement of the
 * ldrb resists. Every order that puts the field read where the ROM has it
 * either loses the named-mask effect or changes the instruction count.
 *
 * That last step is worth someone else's fresh eyes, because it is now the
 * only thing between here and thirty-four functions.
 */
struct Spr { unsigned char pad_00[9]; unsigned char flags; };
void OvlFunc_927_20089dc(Actor *actor, int priority) {
    struct Spr *s = (struct Spr *)actor->sprite;
    int f;
    int m;
    priority &= 3;
    f = s->flags;
    m = ~0xc;
    priority <<= 2;
    s->flags = (f & m) | priority;
}
