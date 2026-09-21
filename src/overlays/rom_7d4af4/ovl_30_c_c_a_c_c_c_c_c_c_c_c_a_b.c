/* Cluster OvlFunc_949_2008728..OvlFunc_949_2008728 extracted from
 * goldensun/asm/overlays/rom_7d4af4/ovl_30_c_c_a_c_c_c_c_c_c_c_c_a.s.
 *
 * Total .text for this TU = 364 bytes (= 0x16c). Never attempted before batch 277.
 * FOUR REGISTER PINS -- one fakematch row. 139 instructions; ten candidates plus two
 * sweeps, 58 -> 27 -> 11 -> 7 -> 4 -> 0.
 *
 * THE PINS ARE THE MEASURED MINIMUM FOR EXACTNESS, and the ladder is on file:
 *
 *     p0 + p1 + p2 + one      0
 *     p1 + p2 + one           2   (`lsl r1` transposed past `mov r0`)
 *     p2 + one                2   (same)
 *     one only                3
 *     none                    7
 *
 * ELEVEN OF THE TWELVE CALL SITES NEEDED NO PIN -- they take ordinary named locals. That
 * is three fewer pins than the immediate sibling spends: ovl_30_c_c_a_c_c_c_c_c_c_c_c_b.c
 * is a fakematch that pins r0/r1/r2 and writes six calls' assignments in ROM order. THE
 * PIN IS THE FALLBACK FOR BLOCK 0 SPECIFICALLY, NOT THE DEFAULT FOR THE INTERLEAVE. Read
 * that file with this one.
 *
 * BOTH RESIDUE CLASSES WERE PRICED, NOT GUESSED.
 *
 * 1. `__MapActor_SetPos(0x10, 0xb0 << 17, 0xb0 << 17)` passes the SAME expensive constant
 *    twice, in the function's FIRST basic block. `.03.cse` shows one surviving
 *    `(set (reg) (const_int 23068672))` with the copies carrying REG_EQUAL, and `.07.gcse`
 *    shows cprop did not restore them. That is docs/elevation.md's own decision procedure
 *    ("is there a branch that DOMINATES the repeated uses?") answering NO -- there is no
 *    branch before this call -- and its recorded conclusion is that only a `register`
 *    declaration, which forms no pseudo for cse1 to common, reaches it. Confirmed.
 *
 * 2. The `*(short *)(a + 0x64) = one;` store is a 4-instruction r2/r3 swap plus an adjacent
 *    transposition. At `.19.flow2` the PRE-SCHEDULE order is already the ROM's (`mov #1`
 *    before the address `add`), so it is the register assignment that drives sched2 to
 *    invert it. NINE non-pin spellings measured BYTE-IDENTICAL at 4 differing: `one` at the
 *    top of the function (6, worse), assigned before the GetActor call (9, worse), declared
 *    first / last / next to the pointer, the address walked into its own `short *q`
 *    statement, a `struct Actor` with a real `short f64` member, a bare literal 1 (15 -- it
 *    pools), `unsigned short one`, and `one` reused as a later call argument.
 *
 * ALSO MEASURED AND WORSE: the return-type sweep on `__MapActor_SetPos`. Declaring it to
 * return `int`, and leaving it UNDECLARED so it is an implicit `int` call, are BOTH 16
 * differing against a 2-differing baseline. The fill order here is not reachable through
 * the callee's return type -- another instance of batch 276's rule that an
 * argument-fill-order difference can be a consequence rather than a cause.
 */
extern char *iwram_3001ebc;
extern unsigned char gScript_949__02008ec0[];
extern unsigned char gScript_949__02008f90[];
extern void OvlFunc_949_2008170(void);
extern void OvlFunc_949_20086e8(void);
extern void OvlFunc_949_2008ca8(void);
extern void OvlFunc_949_2008224(void);
extern int __GetFlag(int id);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern char *__MapActor_GetActor(int slot);
extern int __MapActor_SetAnim(int slot, int n);
extern void __Func_8092adc(int a, int b, int c);

int OvlFunc_949_2008728(void)
{
	char *p;
	register int p0 __asm__("r0");
	register int p1 __asm__("r1");
	register int p2 __asm__("r2");
	char *a;
	register int one __asm__("r3");
	int zero;
	int cx, cy;
	int x1, y1, x2, y2, x3, y3, x4, y4, x5, y5, x6, y6, x7, y7;
	int dx, dy;
	int dv;

	cx = 0x9e << 17;
	cy = 0xa4 << 17;
	x1 = 0x82 << 18; y1 = 0x8c << 18;
	x2 = 0x82 << 18; y2 = 0x8c << 18;
	x3 = 0x82 << 18; y3 = 0x8c << 18;
	x4 = 0x82 << 18; y4 = 0x8c << 18;
	x5 = 0x82 << 18; y5 = 0x8c << 18;
	x6 = 0x82 << 18; y6 = 0x8c << 18;
	x7 = 0x82 << 18; y7 = 0x8c << 18;
	dx = 0x8c << 17;
	dy = 0xa0 << 15;
	dv = 0x80 << 6;
	zero = 0;

	p = iwram_3001ebc;
	*(int *)(p + (0xe0 << 1)) = 0x100;
	p1 = 0xb0;
	p2 = 0xb0;
	p2 <<= 17;
	p0 = 0x10;
	p1 <<= 17;
	__MapActor_SetPos(p0, p1, p2);
	__MapActor_SetBehavior(0x10, gScript_949__02008ec0);
	a = __MapActor_GetActor(0x10);
	one = 1;
	*(short *)(a + 0x64) = one;
	*(void **)(a + 0x6c) = OvlFunc_949_2008170;
	__MapActor_SetPos(0x11, 0xb8 << 17, 0xa0 << 17);
	__MapActor_SetBehavior(0x11, gScript_949__02008f90);
	a = __MapActor_GetActor(0x11);
	*(short *)(a + 0x64) = zero;
	*(void **)(a + 0x6c) = OvlFunc_949_2008170;
	a = __MapActor_GetActor(0xe);
	*(void **)(a + 0x6c) = OvlFunc_949_20086e8;
	if (__GetFlag(0x8c1))
		__MapActor_SetPos(0x1c, cx, cy);
	if (__GetFlag(0x201))
		OvlFunc_949_2008ca8();
	if (__GetFlag(0x80 << 2)) {
		OvlFunc_949_2008224();
		__MapActor_SetAnim(8, 4);
	}
	if (__GetFlag(0x95 << 4)) {
		__MapActor_SetPos(0x14, x1, y1);
		__MapActor_SetPos(0x15, x2, y2);
		__MapActor_SetPos(0x16, x3, y3);
		__MapActor_SetPos(0x18, x4, y4);
		__MapActor_SetPos(0x19, x5, y5);
		__MapActor_SetPos(0x1a, x6, y6);
		__MapActor_SetPos(0x1b, x7, y7);
	} else if (__GetFlag(0x962)) {
		__MapActor_SetPos(0x1b, dx, dy);
		__Func_8092adc(0x1b, dv, 0);
		__MapActor_SetAnim(0x1b, 1);
	}
	return 0;
}
