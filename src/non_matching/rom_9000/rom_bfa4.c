/* Func_800bfa4 -- 0x0800bfa4  (asm/rom_9000/rom_be70_c.s)
 *
 * BLOCKER: register assignment.  12 of 41 differ, EXACT length, exact prologue,
 * exact branch structure, exact constant placement.  Everything that source can
 * express is right; the residue is which register holds which value.
 *
 * What the function does: converts an object's world position to screen space
 * relative to the camera at iwram_3001e70->[0xe4], rejects it if off-screen, and
 * writes {x, y} through an out pointer.  Returns 0 on screen, -1 off.
 *
 * The two-sided range test is gcc's merged compare, not source:
 *     add r3, r1, r0     (dx + 0x1fffff)
 *     ldr r0, =0x12ffffe
 *     cmp r3, r0 / bhi   unsigned, so both bounds in one branch
 * decodes to `dx > -0x200000 && dx < 0x1100000`, which reproduces exactly.
 *
 * PROGRESS -- each step was a separate, isolated edit:
 *
 *   19  naive: cam[0] and cam[1] read at point of use
 *   15  read BOTH camera words up front, so the cam pointer dies early
 *   13  pre-mask cx/cy (fixes the prologue: no more spilled r6)
 *   12  assign cy BEFORE cx  <-- best, scratch/wJ.c
 *
 * The 15 -> 13 step is the reusable one.  Holding the camera POINTER across the
 * object reads kept a register live and forced `push {r5, r6, lr}` where the ROM
 * has `push {r5, lr}`.  Reading both words up front kills the pointer and the
 * prologue matches.  Prologue mismatch is the cheapest tell for this.
 *
 * WHAT REMAINS, and why it is not reachable:
 *
 *     rom   mov r5, r1 / ldr r1, [r3,#0] / ldr r2, [r3,#4]   cx->r1, cy->r2
 *     ours  mov r5, r1 / ldr r1, [r3,#4] / ldr r3, [r3,#0]   cy->r1, cx->r3
 *
 * The ROM loads cam[0] first AND puts it in r1; dx then overwrites r1 and dy
 * overwrites r2, giving the 3-operand `sub r1, r3, r1`.  Ours reaches the same
 * shape with the roles swapped, which degrades one instruction to the 2-operand
 * `sub r2, r3`.  Source order controls the LOAD order and the register order
 * TOGETHER, in opposite directions -- there is no spelling that gets both:
 *
 *     assign cx first    loads in ROM order, registers swapped        14
 *     assign cy first    registers closer, loads swapped              12
 *
 * Also tried, all at 12 or worse -- none touches register assignment:
 *     unsigned cx/cy with an (int) cast on the mask         12
 *     `dx = -(cx & mask) + o[2]` (operand order)            12
 *     `cx &= mask;` as its own statement                    24 (drops to 40 lines)
 *     explicit `p = out` local, before and after the mask   14 / 37
 *     mask as a literal instead of a named local            13
 *
 * This is the same wall documented in docs/elevation.md: gcc's register
 * assignment is decided after every source-level choice has been folded away.
 * Reaching exact length with a matching prologue and only register names left
 * is where this class stops.
 */
extern int iwram_3001e70;

int Func_800bfa4(int *o, int *out)
{
    int *cam;
    int mask;
    int cx;
    int cy;
    int dx;
    int dy;

    cam = (int *)(iwram_3001e70 + 0xe4);
    mask = 0xffff0000;
    cy = cam[1];
    cx = cam[0];
    dx = o[2] - (cx & mask);
    dy = o[4] - (cy & mask);
    if (dx > -0x200000 && dx < 0x1100000 && dy > 0 && dy < 0xe00000) {
        *out++ = dx >> 16;
        *out = dy >> 16;
        return 0;
    }
    *out++ = 0;
    *out = 0;
    return -1;
}
