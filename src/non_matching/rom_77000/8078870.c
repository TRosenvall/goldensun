/* Func_8078870 (0x08078870) -- NON-MATCHING.
 * Blocker class: PROLOGUE INSTRUCTION SCHEDULE (see docs/elevation.md).
 *
 * 41 lines against the ROM's 41, FIVE differing, and all five are the position
 * of ONE instruction inside a nine-instruction prologue. Everything from the
 * loop head to the epilogue is byte-identical.
 *
 * This is the third member of the 0x080787dc..0x08078870 family. The other two
 * -- GetEquippedItem and Func_807882c -- both matched this round, using the
 * register-offset lever this file's older sibling park (8078588.c) had already
 * written down. Func_8078870 differs from them in one way: its ROM keeps a
 * SINGLE base (`ldrh r3, [r5]`, `add r5, #0xd8` folded into the pointer)
 * instead of the base/index pair, so the register-offset lever has nothing to
 * act on here and the body comes out right without it.
 *
 * THE RESIDUE, in full:
 *
 *     rom                        ours
 *     mov  r2, #0x80             mov  r2, #0x80
 *     ldr  r3, =0x1ff            ldr  r3, =0x1ff
 *     lsl  r2, #0x2         <->  mov  r5, r0
 *     mov  r5, r0           <->  lsl  r2, #0x2
 *     mov  r7, r1                mov  r7, r1
 *     mov  r6, #0x0              mov  r6, #0x0
 *     mov  r8, r2           <->  add  r5, #0xd8
 *     mov  r10, r3          <->  mov  r8, r2
 *     add  r5, #0xd8        <->  mov  r10, r3
 *
 * The ROM sinks `add r5, #0xd8` -- the pointer's initial value -- BELOW the two
 * pseudo-to-high-register copies. We emit it above them. That is a scheduling
 * decision about three independent instructions with no data dependence
 * between them, and it is not reachable from the source: the copies into r8/r10
 * are emitted by reload at the point it chooses, not at any point the C names.
 *
 * MEASURED (all 41 lines against the ROM's 41):
 *   the version below, `p = a + 0xd8` before the loop        5 differing
 *   `p` declared before `i`, so the add is written first     6
 *   `p = a;` then `p += 0xd8;` as two statements             5
 *   `p` typed `unsigned short *`, walked with `p++`          5
 *   the parameter itself walked, no separate `p`             5
 *   the two loop constants named in int locals              10
 *
 * FLAGS RULED OUT (all 5 differing, i.e. inert):
 *   -fno-strict-aliasing, -fno-gcse, -fno-strength-reduce,
 *   -fno-rerun-cse-after-loop, -fno-schedule-insns, -fno-peephole,
 *   -fomit-frame-pointer
 * and two that make it worse: -fno-sched2 (8 differing), -O1 (10).
 *
 * So: nine source spellings and nine flag settings, and the count never leaves
 * 5. Per the recognition rule from batch 171, a diff that has collapsed to a
 * fixed permutation of instructions with no dependence between them is done.
 *
 * WHAT IS RIGHT: the whole loop, the 0x200 equipped test, the info[2] kind
 * comparison, the 0x1ff id mask on the hit path, the 0..0xe bound, and the
 * bottom-tested do/while entry. Only the prologue's ordering is wrong.
 */
extern unsigned char *GetItemInfo(int id);

int Func_8078870(unsigned char *a, int kind)
{
	int i;
	unsigned char *p;
	unsigned char *info;

	i = 0;
	p = a + 0xd8;
	do {
		if (*(unsigned short *)p & 0x200) {
			info = GetItemInfo(*(unsigned short *)p);
			if (info[2] == kind)
				return *(unsigned short *)p & 0x1ff;
		}
		i++;
		p += 2;
	} while (i <= 0xe);
	return 0;
}
