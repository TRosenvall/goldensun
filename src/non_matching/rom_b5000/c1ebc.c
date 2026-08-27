/* Func_80c1ebc -- asm/rom_b5000/rom_c1a34_a_a_a.s
 *
 * BLOCKER: MULTIPLE, INDEPENDENT -- at least four, which is why the count
 * stays high while the structure is right.  Best 66 of 75 (ours 70 lines).
 *
 * The semantics are settled and almost certainly correct:
 *   b = iwram_3001e74; n = b[0x40]; u = _GetUnit();
 *   if (u[0x129]) return;
 *   t = u[0x94 << 1];
 *   find i in [0,n) with *(u16 *)(b + 0x10 + i*2) == t;  if none, return;
 *   if (*(int *)(b + 0x1c + i*4) == 0) return;
 *   k = leading nonzero bytes of u, capped at 14;
 *   bit = k ? u[k-1] - 0x31 : 0x20;
 *   *(int *)(b + 0x1c + i*4) &= ~(1 << bit);
 *
 * THE FOUR:
 *
 *  1. CONSTANT CSE, 0x128 FROM 0x129.  ROM builds the second offset fresh
 *     (`mov r3,#0x94 / lsl r3,#1 / add r3,r12`); gcc derives it from the
 *     pooled 0x129 with `sub r2,#1`.  There IS a control-flow boundary between
 *     the two uses (the `if (u[0x129])` early return), so this should be the
 *     reachable half of the documented rule -- but the flag does not close it.
 *     MEASURED: -fno-rerun-cse-after-loop 66 (no change); -fno-gcse 66;
 *     routing the second read through a separate `unsigned char *p = u;` 68.
 *  2. REG+REG ADDRESSING.  ROM `mov r3,r1 / add r3,#0x1c / ldr r3,[r6,r3]`;
 *     ours `add r3,r1,r7 / ldr r3,[r3,#0x1c]` -- 0x1c fits a thumb immediate
 *     so gcc forms a pointer.  A named `int o = i*4 + 0x1c;` local (the lever
 *     that worked on Func_80f7df0) does NOT reproduce it here: 68.
 *  3. u LIVES IN r12.  ROM keeps the _GetUnit result in r12, which is
 *     call-clobbered and free because there are no further calls; we spend a
 *     callee-saved register and an extra push.  Everything downstream renames.
 *  4. BOTH LOOPS ARE PEELED DIFFERENTLY.  ROM checks element 0 via
 *     `ldrh r3,[r6,#0x10]` and only then materialises the walking pointer;
 *     and the second loop's first `ldrb` sits above the loop top.  A plain
 *     indexed `for` gives 70 lines, a pointer-walking `for` gives 67, and the
 *     ROM is 75 -- neither spelling reaches the rotation.
 *
 * MEASURED, whole-function:
 *   indexed for, target hoisted, named offset, separate pointer   68 (69 lines)
 *   indexed for, target hoisted                                   66 (70 lines)
 *   pointer-walking for                                           68 (67 lines)
 *   indexed for, target read inside the loop condition            77 (79 lines)
 *   -fno-rerun-cse-after-loop / -fno-gcse                         66 / 66
 *
 * This is a good candidate to re-attempt AFTER the r12 question is understood:
 * blocker 3 is upstream of most of the count, and the other three are each
 * worth only a handful of lines.
 */
