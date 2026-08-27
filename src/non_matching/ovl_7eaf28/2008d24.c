/* OvlFunc_960_2008d24 -- asm/overlays/rom_7eaf28/ovl_314_c_c_a.s
 *
 * BLOCKER: ARG INTERLEAVE on the LAST call + store ordering
 *
 * 8 of 65 differing, same length.  57 lines are exact.  Best C: scratch/n8d24.c.
 *
 * TWO THINGS THAT WORKED, both worth reusing:
 *
 *  1. The ROM POOLS 0xa5 (`ldr r3, =0xa5 / cmp r2, r3`) where a literal gives
 *     `cmp r2, #0xa5`.  0xa5 fits in a thumb cmp immediate, so the pool load is
 *     the symbol tell.  `_AREA_a5` is already in area.sym; spelling the test as
 *     `== (int)(&_AREA_a5)` took the screen from 62 differing to 17.
 *  2. Two of the four __Func_8010704 sites pass DIFFERENT values in the two
 *     stack slots, and the ROM materialises BOTH before storing either
 *     (`mov r3,#0xf / mov r2,#0x2c / str r3,[sp] / str r2,[sp,#4]`).  A pair of
 *     named locals per site -- not shared between sites -- took 17 to 8.  The
 *     third site passes 0x7f in both slots and matches with plain literals,
 *     which is the control that shows the lever is about the values differing.
 *
 * WHAT IS LEFT (8 lines, two spots):
 *
 *   call 4, __Func_8010704(0xb, 0x47, 1, 1, 0xc, 0x47):
 *     rom  mov r1,#0x47 / mov r2,#1 / mov r3,#1 / mov r0,#0xb
 *     ours mov r3,#1 / mov r2,#1 / mov r0,#0xb / mov r1,#0x47
 *   the tail store:
 *     rom  ldrh r2,[r3] / ldr r3,=0x500019e / strh r2,[r3]
 *     ours ldr r2,=0x500019e / ldrh r3,[r3] / strh r3,[r2]
 *   and the consequent pop {r1}/bx r1 vs pop {r0}/bx r0.
 *
 * Call 3 -- the same callee, same shape, plain literals -- matches exactly, so
 * this is specific to call 4 and not to the callee or the argument count.
 *
 * MEASURED (all 65 lines unless noted):
 *   named pair per stack-arg site                              8   <- best
 *   ... + volatile on the 0x500019e store                      8
 *   ... + arg2 spelled as q1 (sharing the stack arg's local)   8
 *   ... + unsigned short temp for L1a00[0]                     8   (66 lines, worse)
 *   ... + 0xb assigned in the dominating entry block           8   (66 lines)
 *   ... + 0x47 assigned in the dominating entry block         12
 *   ... + q0/q1 hoisted to the dominating entry block         12
 *
 * The basic-block lever IS available here -- the entry block dominates the
 * whole if-body -- and it makes things WORSE at both positions tried.  That is
 * a useful negative: it separates this from the straight-line arg-interleave
 * parks (2009df8, 20087dc) where the lever is merely unreachable.
 */
