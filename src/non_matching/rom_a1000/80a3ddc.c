/* Func_80a3ddc -- 0x080a3ddc, asm/rom_a1000/rom_a1814_c_a_c_c_c_c_c_a.s
 *
 * BLOCKER: movhi-load form and register allocation are in direct tension.
 * Best screen 39 of 39 lines (instruction COUNT exact, whole shape right);
 * every remaining difference is one of two things, and fixing either breaks
 * the other.
 *
 * What the function does: zero 32 destination halfwords, then walk 15 slots of
 * rec+0xd8 copying the non-zero ones down into the front of dst, returning the
 * count.  Note the second loop re-zeroes dst[i] before the copy, and the write
 * pointer always trails i, so the ordering is load-bearing.
 *
 * SOLVED, and worth reusing:
 *   - Loop 1 is a do/while over a POINTER compared with (int) casts.  The ROM
 *     compares with `bge` (signed); a plain pointer comparison gives `bcs`.
 *     Casting both sides to int is what produces the signed branch.
 *   - Loop 2 is a `for`, NOT a goto loop.  The ROM hoists `ldr r3,=0` into r12
 *     ahead of the loop and copies `mov r3, r12` per iteration -- that hoist IS
 *     the loop optimiser running, so this loop was a for/while in the original.
 *     (Contrast the goto-loop lever: absence of a hoist is the goto tell, and
 *     its presence is equally informative in the other direction.)
 *   - `for (k = 0xe; k >= 0; k--, off += 2)` gets the ROM's `sub r1,#1` before
 *     `add r6,#2`; incrementing off in the body reverses that pair.
 *   - `ldr r2, =0` for the constant zero reproduces on its own.  It looked like
 *     a tell and is not one -- gcc-2.96 emits a pool load for 0 here unprompted.
 *
 * THE TENSION.  The ROM's loop body is
 *       mov  r3, r12         ; the hoisted 0
 *       strh r3, [r6, r5]
 *       ldrh r2, [r0, #0]
 *       mov  r3, r2          ; <-- a copy, tested while r2 is stored
 *       add  r0, #2
 *       cmp  r3, #0
 * and needs BOTH r2 and r3 live as scratch.  That second scratch register is
 * what pushes the hoisted 0 out to r12 and, downstream, puts dst in r5 and the
 * counter in r1.  So the copy is not cosmetic: the whole allocation depends
 * on it.
 *
 *   - Declaring the loaded value `int` gives the ROM's `ldrh r2,[r0,#0]` but no
 *     copy, so only r3 is scratch: the 0 stays in a low register, dst stays in
 *     r1, and the allocation diverges everywhere.
 *   - Declaring it `short`/`unsigned short` gives the copy and reproduces the
 *     ROM's allocation EXACTLY (r12 for the zero, r7 = n, r4 = out, r6 = off) --
 *     but the load becomes `mov r3,#0 / ldrsh r2,[r0,r3] / lsl r3,r2,#0x10`.
 *
 * VERIFIED GENERAL FINDING (see docs/elevation.md): gcc-2.96 thumb loads a
 * HImode *local* with `ldrsh rD,[rB,rI]` -- sign-extending, and needing a zero
 * index register because ldrsh has no immediate form -- and tests it for zero
 * with `lsl #16`.  A plain `ldrh rD,[rB,#0]` only ever comes from loading into
 * an SImode destination.  So in any function the ROM both `ldrh`s a value and
 * keeps a redundant copy of it, the copy cannot be produced by the type of the
 * destination, and must come from somewhere I have not identified.
 *
 * TRIED AND DID NOT MOVE IT: splitting into `t = *src; v = t;` in both
 * directions (41 lines, worse); testing and storing through `*src` twice and
 * letting CSE fold the loads (38 lines); casting at the test rather than the
 * declaration.
 */
