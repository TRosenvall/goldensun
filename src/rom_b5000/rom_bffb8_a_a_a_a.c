/* Cluster Func_80bffb8..Func_80bffb8 extracted from goldensun/asm/rom_b5000/rom_bffb8_a_a_a.s.
 *
 * Total .text for this TU = 224 bytes (= 0xe0). Never attempted before batch 274.
 * No pins, no flags. The reference keeps its pool INSIDE the function and this reproduces
 * it -- 99 encodings and 7 relocations identical.
 *
 * A mosaic screen transition: save the four BG control words, set their mosaic bits, ramp
 * REG_MOSAIC over 16 frames, then restore.
 *
 * FOUR STACK ADDRESSES CARRIED IN CALLEE-SAVED REGISTERS MEANS THE ADDRESSES ARE NAMED
 * POINTERS, ASSIGNED ONE PER GROUP. The ROM's `mov r9, r0 / mov r1, r9` round-trip per
 * slot is unreachable from plain locals (gcc keeps them in registers and emits no
 * `sub sp` at all), from a local array (one base plus `[r0, #imm]`, 71 lines against 93),
 * and from `volatile` locals accessed without pointers (91 lines -- the two
 * high-register copies are simply missing).
 *
 * The address has to be computed BEFORE the `ldrh` it conflicts with, and only a separate
 * `p = &x;` statement per group puts it there. Assigning all four pointers up front also
 * fails, at 82 differing -- THE INTERLEAVING IS THE LEVER, not the pointers.
 *
 * `volatile` on the locals, on the pointers, or on neither all reach exact and tie at 224
 * bytes; the plain form is landed as the least misleading.
 */
#include "gba/types.h"
#include "gba/io.h"

extern void Func_8003b70(int a);
extern unsigned int Random(void);
extern void WaitFrames(int n);

int Func_80bffb8(void)
{
	unsigned short a;
	unsigned short b;
	unsigned short c;
	unsigned short e;
	unsigned short *pa;
	unsigned short *pb;
	unsigned short *pc;
	unsigned short *pe;
	int t;
	int i;

	pa = &a;
	t = REG_BG0CNT;
	*pa = t;
	REG_BG0CNT = t | 0x40;
	pb = &b;
	t = REG_BG1CNT;
	*pb = t;
	REG_BG1CNT = t | 0x40;
	pc = &c;
	t = REG_BG2CNT;
	*pc = t;
	REG_BG2CNT = t | 0x40;
	pe = &e;
	t = REG_BG3CNT;
	*pe = t;
	REG_BG3CNT = t | 0x40;
	REG_BLDCNT = 0x3eee;
	Func_8003b70(0x10);
	for (i = 0; i <= 0xf; i++) {
		Random();
		Random();
		Random();
		Random();
		REG_MOSAIC = (i << 8) | i;
		WaitFrames(1);
	}
	REG_DISPCNT = 1;
	WaitFrames(4);
	REG_BG0CNT = *pa;
	REG_BG1CNT = *pb;
	REG_BG2CNT = *pc;
	REG_BG3CNT = *pe;
	return 0;
}
