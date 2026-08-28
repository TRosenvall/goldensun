/* OvlFunc_881_200808c and OvlFunc_881_20080d4 -- 0x0200808c / 0x020080d4,
 * asm/overlays/rom_77a7c8/ovl_30_a_a_a_c_a.s
 *
 * A twin pair: identical but for two immediates (0x809/0x2a against
 * 0x80a/0x18), so one solution elevates both.  Candidate at scratch/L808c.c.
 *
 * Best screen: ours 28 lines against the ROM's 29 -- ONE instruction, and ours
 * is the SHORTER one.  Everything else agrees, including the derived offset
 * (`mov r2,#0x8e / lsl r2,#2 / sub r2,#0x8c`, 0x238 -> 0x1ac), which reproduces
 * from a plain mutated variable exactly as the batch-123 rule predicts, because
 * 0x238 is forced into a register by the `add r5, r2` that builds the gState
 * address.
 *
 * BLOCKER: gcc folds `base + offset` into a reg+reg access where the ROM
 * materialises the address first.
 *
 *      rom   add r3, r6, r2 / ldr r3, [r3, #0x0]
 *      ours  ldr r3, [r6, r3]
 *
 * and the same at the store in the `if` body.
 *
 * WHAT MAKES THIS WORTH RECORDING is that the identical construct went the
 * OTHER way one round earlier.  `OvlFunc_881_20084a0` (now elevated,
 * src/overlays/rom_77a7c8/ovl_30_c_a_c_a_c_a_c.c) contains
 *
 *      r2 = 0xb8; r2 <<= 1; p = (short *)(base + r2); *p = a;
 *
 * and gcc emitted the ROM's `add r3, r1, r2 / strh r7, [r3]` from it.  The very
 * same shape here folds.  So "name the destination pointer" is NOT a lever that
 * decides this -- whether gcc materialises the address or uses the addressing
 * mode is downstream of register allocation and pressure, and the source can
 * ask for it only indirectly.
 *
 * The docs/elevation.md note "the named-pointer lever needs the offset to be
 * mutated afterwards" is therefore necessary but not sufficient: here the offset
 * IS reassigned afterwards (inside the `if`) and gcc folded anyway.
 *
 * TRIED: named `int *p` for both accesses; the integer-local chain
 * (`a = (unsigned int)base; a += off; v = *(int *)a;`); `base` as an unsigned
 * int for the whole function; the ROM's exact statement order (which did fix
 * the position of the gState pool load); -O1, --no-sched2, --no-rerun-cse.
 * All 28 lines, none closer.
 *
 * Note also `bl __divsi3` against the ROM's `bl _divsi3_RAM` in the screen --
 * that one is not a difference.  overlays/rom_77a7c8/overlay.ld already carries
 * the `__divsi3 = _divsi3_RAM;` alias, so it resolves at link time; tryc.py
 * compares text and cannot see that.
 */
