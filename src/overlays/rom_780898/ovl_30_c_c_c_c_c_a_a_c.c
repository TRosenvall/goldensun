/* OvlFunc_883_200d950  --  0x0200d950
 *
 * Was goldensun/asm/overlays/rom_780898/ovl_30_c_c_c_c_c_a_a_c.s, which held
 * it alone.
 *
 * Redraws a signpost-style map marker: place the default icon, pick the actor
 * to read (flag 0x87a selects slot 0x15 over 0x14), clear all three state
 * flags, then set exactly one of them and redraw with the matching icon
 * according to which of three bands the actor's x coordinate falls in.
 *
 * Found by tools/templated.py at a perfect 1.00; the neighbour supplied the
 * whole extern block.
 *
 * NEEDS CSE_CFLAGS (-fno-rerun-cse-after-loop). Three flag ids -- 0xc5 << 2,
 * 0x315 and 0x316 -- are each read once before the branches, to clear them, and
 * once again inside one arm, to set one. The first use DOMINATES the second, so
 * this is the recorded guard/set shape the flag group exists for, and the
 * Makefile already carries several of these.
 *
 * THE SYMPTOM WAS A WIDER PROLOGUE, and it is the second function today to show
 * it. At plain -O2 the candidate is 85 instructions against the ROM's 78, with
 * `push {r5, r6, r7, lr}` plus an r8 save against the ROM's `push {r5, lr}`:
 * gcc had parked all three flag ids in callee-saved registers rather than
 * rebuilding them in each arm. With the flag, 85 differing collapses to 6 and
 * the instruction count lands exactly. -fno-gcse, -O1 and -fno-schedule-insns2
 * were also tried; -O1 gives 30, the other two do not help.
 *
 * THE LAST SIX INSTRUCTIONS WERE THE TWO STACK ARGUMENTS. __Func_8010704 takes
 * six, so the last two are stored to the stack, and the ROM materialises BOTH
 * before storing either:
 *
 *     mov r3, #0x16 / mov r2, #0x24 / str r3, [sp] / str r2, [sp, #4]
 *
 * Written as literals in the call, gcc completes each argument in turn and
 * reuses r3 for both. Naming them as a pair of locals assigned together makes
 * both live at once and reproduces the ROM. This is the eager-issue face of the
 * named-local rule applied to stack arguments, which is a new place to look for
 * it -- the two arms that pass `t` need no help, because `t` is already a
 * variable and only the literal pair had to be named.
 *
 * The shared __MapActor_GetActor call is cross-jumped, not shared in the
 * source: the call is written inside BOTH arms of the flag test, and gcc merges
 * the identical tails. Written as a single call on a selected slot value, gcc
 * if-converts the choice instead and the branch disappears -- the recorded
 * "look for a CALL that can be moved inside the arms" lever.
 */

struct A {
    unsigned char pad00[8];
    int f8;
};

extern struct A *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_883_200d950(void)
{
    struct A *a;
    int t;
    int p, q;

    p = 0x16;
    q = 0x24;
    __Func_8010704(0x11, 0, 3, 1, p, q);
    if (__GetFlag(0x87a))
        a = __MapActor_GetActor(0x15);
    else
        a = __MapActor_GetActor(0x14);
    if (a == 0)
        return;
    __ClearFlag(0xc5 << 2);
    __ClearFlag(0x315);
    __ClearFlag(0x316);
    t = a->f8 >> 20;
    if (t == 0x16) {
        __Func_8010704(0x11, 1, 1, 1, t, 0x24);
        __SetFlag(0xc5 << 2);
    } else if (t == 0x17) {
        __Func_8010704(0x11, 1, 1, 1, t, 0x24);
        __SetFlag(0x315);
    } else {
        p = 0x18;
        q = 0x24;
        __Func_8010704(0x11, 1, 1, 1, p, q);
        __SetFlag(0x316);
    }
}
