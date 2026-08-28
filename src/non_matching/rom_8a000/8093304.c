/* Func_8093304 (SetDialoguePortrait) -- 0x08093304,
 * asm/rom_8a000/rom_93304_a_a_a_a_a_a.s
 *
 * Best screen: ours 32 lines against the ROM's 33, candidate at
 * scratch/L93304.c.  ONE instruction missing, and it is the last store.
 *
 * This was the first function written with the integer-local address idiom
 * (docs/elevation.md, "Address arithmetic in unsigned int locals") and it
 * reproduced the ROM's gState access exactly, first try:
 *     r3 = (unsigned int)&gState; r1 = 0x83; r1 <<= 2; r3 += r1;
 *     value = L9fc28[*(unsigned char *)r3];
 * giving `ldr r3,=gState / mov r1,#0x83 / lsl r1,#2 / add r3,r1 / ldrb r3,[r3]`.
 *
 * A REFINEMENT OF THE "ROM DERIVES, GCC FOLDS" PARK (see rom_15000/80160fc.c).
 * That park recorded that `off = 0xea6; ... off -= 3;` cannot produce the ROM's
 * `sub r2,#3`, because gcc folds both values at each use.  Here the ROM does
 * the same thing --
 *     ldr r1, =0x12f4 ... add r1, #0x2
 * -- and it DOES reproduce, from exactly the spelling that failed there:
 *     off = 0x12f4;  q = (short *)(base + off);  off += 2;
 *
 * The difference is that 0x12f4 is first used in a RUNTIME add against a
 * pointer (`base + off`, base being loaded from memory), so gcc must
 * materialise it into a register; once it is in a register, CSE derives 0x12f6
 * from it with an `add`.  In Func_80160fc both offsets were folded into
 * addresses and never needed a register, so there was nothing to derive from.
 *
 *   Rule: a derived constant is reachable when the base constant is already
 *   forced into a register by a runtime use.  It is not reachable when both
 *   values are only ever compile-time addresses.
 *
 * REMAINING BLOCKER: the store at the join.  The ROM computes the address
 * separately --
 *     L1:  add  r3, r5, r1
 *          strh r2, [r3, #0x0]
 * -- and gcc folds it into one reg+reg store, `strh r1, [r5, r2]`.
 *
 * The named-destination-pointer lever does not reach this.  It works INSIDE a
 * branch (path 2 gets the ROM's `add r3, r5, r1 / strh r0, [r3]`) only because
 * `off` is mutated afterwards, so the old sum has to be materialised.  At the
 * join `off` is dead after the store, nothing forces materialisation, and
 * gcc always prefers the addressing mode.
 *
 * TRIED: naming the pointer at the join; computing it via the integer idiom
 * (`r3 = (unsigned int)base; r3 += off;`); making `base` an unsigned int for
 * the whole function; duplicating the final store into both branches to invite
 * cross-jumping to merge the tails (34 lines, no merge); -O1, --no-sched2,
 * --no-rerun-cse (all 32 lines, none closer).
 */
