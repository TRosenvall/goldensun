/* Cluster OvlFunc_901_2008350..OvlFunc_901_2008350 extracted from
 * goldensun/asm/overlays/rom_797990/ovl_314_a_a_c_a.s.
 *
 * PARKED ON "REGISTER PRESSURE" AND THAT WAS THE WRONG DIAGNOSIS.  The old note
 * read the r8-r11 spills off the prologue, observed that the ROM keeps four
 * pointers live where we keep three, and concluded the allocator was the
 * blocker -- adding that "the structure is believed right and is not the
 * problem".  The structure was the problem.  Nothing about the register
 * allocation had to be argued with.
 *
 * THE GUARD WAS INVERTED.  The ROM reads
 *      cmp r0, r10 / blt .L384        <- angle work
 *      mov r3, r11 / cmp r3, #0 / beq .L3da   <- the anim-2 arm
 * which is `if (n < lim || force != 0) { angle } else { anim2 }`.  The park
 * wrote the contrapositive, `if (n >= lim && force == 0) { anim2 } else
 * { angle }` -- the same predicate, the arms swapped -- and every difference
 * after it, including the four-versus-three pointer allocation, followed from
 * that.  Written the ROM's way round it matches exactly, r8-r11 and all.
 *
 * HOW IT WAS FOUND: tools/solved_twins.py.  OvlFunc_898_2009674
 * (src/overlays/rom_793768/ovl_314_c_c_c_a_c_c_c.c) is the same function in
 * another overlay and had already been elevated; this file is that .c with the
 * callee renamed, a third argument added to it, and nothing else.  The scan
 * costs seconds and it beat five rounds of reasoning about the prologue.
 *
 * The lesson for the parked set: a park that names a REGISTER-ALLOCATION
 * blocker and says the structure is fine has usually not tested the structure
 * against anything -- there was no second opinion to test it against.  Run
 * solved_twins.py over the parks, not just over the fresh candidates.
 */
extern int OvlFunc_901_2008314(int *a, int *b, int n);
extern int __atan2(int y, int x);
extern void __Actor_SetAnim(char *a, int anim);

int OvlFunc_901_2008350(char *a, char *b, int lim, int force)
{
	int *bp;
	int *ap;
	int ret;
	int ang;
	int dir;
	int base;
	int plus;
	int minus;

	ret = 0;
	bp = (int *)(b + 8);
	ap = (int *)(a + 8);
	if (OvlFunc_901_2008314(bp, ap, 0) < lim || force != 0) {
		ang = (unsigned short)__atan2(*(int *)(b + 0x10) - *(int *)(a + 0x10),
					      *bp - *ap);
		minus = (ang - (0x80 << 5)) & (0xf0 << 8);
		plus = (ang + (0x80 << 5)) & (0xf0 << 8);
		base = ang & (0xf0 << 8);
		dir = *(unsigned short *)(a + 6) & (0xf0 << 8);
		if (base == dir || plus == dir || minus == dir || force != 0) {
			a[0x5b] = 1;
			__Actor_SetAnim(a, 1);
			ret = 1;
		}
	} else {
		a[0x5b] = ret;
		__Actor_SetAnim(a, 2);
	}
	return ret;
}
