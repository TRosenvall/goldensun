/* Func_80b6e30 (ComputeSlotPosition) -- NON-MATCHING.
 * Blocker class: LABEL/POOL STRUCTURE at the loop tail. 35 lines against the
 * ROM's 34, 15 differing, and the loop body is exact.
 *
 * TWO LEVERS GOT IT FROM 17 TO 15 AND FIXED THE BODY. Both are reusable.
 *
 * 1. THE POOLED ZERO NEEDS THE LITERAL, NOT A VARIABLE. The ROM holds a
 *    pooled zero in r10 across the loop (`ldr r2, =0x0 / mov r10, r2`) and
 *    stores it with `strh`. Written as `int z = 0;` gcc emits `mov r2, #0`
 *    and the pool is lost; written as the literal `*(short *)a = 0;` gcc
 *    pools it and hoists it into r10 by itself. That is the third
 *    confirmation of the const.sym exception -- a small constant meeting a
 *    HALFWORD is pooled, and naming it in an int variable defeats that.
 *
 * 2. INVERTING WHICH OPERAND IS THE POINTER FIXES THE ADDRESSING BASE. The
 *    ROM emits `ldrsh r3, [r5, r7]` with the byte OFFSET as the addressing
 *    base and the struct pointer as the index. Written naturally --
 *    `char *p; int off; *(short *)(p + off)` -- gcc emits `[r7, r5]`, the
 *    other way round, and no amount of reordering the addition changes it,
 *    because gcc normalises `a + b`. Declaring the OFFSET as the pointer and
 *    the base as an int:
 *
 *        int p; char *off;
 *        p = (int)iwram_3001e74;
 *        off = (char *)4;
 *        ... *(short *)(off + p) ...
 *
 *    puts them in the ROM's roles and the addressing matches exactly. It
 *    reads backwards, but the ROM's own register assignment says which one
 *    gcc treats as the base, and that is the only thing that decides it.
 *
 * WHAT REMAINS: an extra label and branch at the loop tail, where the ROM
 * jumps over its mid-function pool and ours emits `b L2 / L2: / L1:`, one
 * instruction long. The epilogue also differs -- the ROM does `pop {r1} /
 * bx r1`, which by the epilogue-register tell means r0 carries a return
 * value. Declaring the function `int` and falling off the end does NOT change
 * ours, so that tell is not reachable this way here.
 */
extern char *iwram_3001e74;
extern void _PreloadSpriteGFX(int a, int b, int c, int d);

void Func_80b6e30(int slot)
{
    int p;
    char *off;
    int i;

    p = (int)iwram_3001e74;
    i = 0;
    off = (char *)4;
    do {
        if (*(short *)(off + p) == slot) {
            _PreloadSpriteGFX(i, 0, 0, 0);
            *(short *)(off + p) = 0;
        }
        i++;
        off += 2;
    } while (i <= 5);
}
