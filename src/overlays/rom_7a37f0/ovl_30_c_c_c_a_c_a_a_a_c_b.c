/* OvlFunc_916_2008a90  --  0x02008a90
 *
 * Walks a -1-terminated table of 12-byte spawn records. Each record names an
 * actor id, a tile x and y, and a flag that swaps which of the two half-tile
 * offsets goes on which axis; the spawned actor is written back into the
 * record, given anim 1 and no sprite flags, and has its z raised by the
 * terrain height under it.
 *
 * THREE LEVERS.
 *
 * 1. THE GUARD IS A SEPARATE `if`, NOT A `while`. Written `while (p->id != -1)`
 *    the preheader loads the halfword ONCE and derives both the guard compare
 *    and the loop-carried raw value from it. The ROM has TWO loads there --
 *    `mov r2,#0 / ldrsh r3,[r6,r2]` for the guard and a separate `ldrh r0,[r6]`
 *    for the body -- because jump.c's duplicate_loop_exit_test never ran: a
 *    hand-written guard is generated on its own, so it gets the memory-form
 *    ldrsh, and PRE inserts the raw ldrh separately for the body. Worth
 *    80 differing -> 27.
 *
 * 2. THE TWO COORDINATES MUST BE NAMED. Passing `(p->x << 20) + K` straight
 *    into the call lets gcc coalesce the shift's pseudo with the argument
 *    register and emit the two-address `add r1, r2`; the ROM's three-operand
 *    `add r1, r3, r2` needs the sum to be a pseudo of its own. 27 -> 23.
 *
 * 3. THE CALL GOES *AFTER* THE IF/ELSE -- the one place where the standing
 *    "put the call in every arm and let gcc cross-jump it" rule is INVERTED,
 *    and it is a SCHEDULING lever rather than a cross-jump one. The tail is the
 *    same five instructions either way; what changes is the order inside the
 *    arms. sched2 breaks a priority tie on the number of dependent insns in
 *    rank_for_schedule, BEFORE falling through to insn order. With the call in
 *    the arm, argument 3's `mov r2, #0` sits in the same block and hands the
 *    constant's `lsl r2,#0xc` a third dependent (an output dep on r2), so the
 *    constant is issued first and the value shift sinks into the tail. With the
 *    call outside, the block ends at `add r3, r2`, both shifts have two
 *    dependents, the tie falls through to insn order, and the ROM's
 *    `lsl r3,#0x14 / lsl r2,#0xc` comes out. The x term keeps the opposite
 *    order in both, because there the constant really does have more dependents
 *    -- the y load's clobber and its own mov write r2 later. Worth the last 23,
 *    and confirmed against -fsched-verbose=6.
 *
 * WHAT THE 23-DIFFERING PLATEAU WAS. Ten spellings sat there: operand order on
 * either axis, naming one coordinate or the other, hoisting the constant,
 * `unsigned short id` with casts, -fno-strength-reduce, -ffixed-r7. They all
 * shared the assumption that the call had to be duplicated into both arms. None
 * of them moved the residue, because the residue was never an operand question.
 * Also load-bearing but unremarkable: `short f20` and `unsigned char f59` as
 * typed fields (the narrow-store lever), and `h` named once for the two uses.
 */

struct Actor {
    unsigned char pad00[0xa];
    short fa;
    int fc;
    unsigned char pad10[0x12 - 0x10];
    short f12;
    int f14;
    unsigned char pad18[0x20 - 0x18];
    short f20;
    unsigned char pad22[0x59 - 0x22];
    unsigned char f59;
};

struct Entry {
    short id;
    short x;
    short y;
    short f6;
    struct Actor *actor;
};

extern struct Actor *__CreateActor(int id, int x, int y, int z);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void __Actor_SetSpriteFlags(struct Actor *a, int f);
extern int __Func_8011f54(int a, int b, int c);

void OvlFunc_916_2008a90(struct Entry *p)
{
    struct Actor *a;
    int h;
    int u;
    int v;

    if (p->id == -1)
        return;
    do {
        if (p->f6 == 0) {
            u = (p->x << 20) + (0x80 << 14);
            v = (p->y << 20) + (0x80 << 12);
        } else {
            u = (p->x << 20) + (0x80 << 12);
            v = (p->y << 20) + (0x80 << 14);
        }
        a = __CreateActor(p->id, u, 0, v);
        if (a == 0)
            break;
        p->actor = a;
        __Actor_SetAnim(a, 1);
        __Actor_SetSpriteFlags(a, 0);
        a->f59 = 0;
        a->f20 = 0x20;
        h = __Func_8011f54(0, a->fa, a->f12) << 16;
        a->fc += h;
        a->f14 = h;
        p++;
    } while (p->id != -1);
}
