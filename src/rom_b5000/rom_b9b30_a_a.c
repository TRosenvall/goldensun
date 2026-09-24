/* Func_80b9b30 (RunApproachSequence)  --  0x080b9b30, the ONLY function in
 * asm/rom_b5000/rom_b9b30_a_a.s.
 * EXACT: 516 bytes, 215 encodings and 39 relocations identical (objcmp, 3 runs).
 * datacheck reports NO DATA, so this is a WHOLE-FILE CONVERSION -- no split, no
 * linker change; stage1.ld:1517 already names the single .o.
 *
 * ONE SCAFFOLD, and it is minimal by measurement: the single
 *     register int q0 __asm__("r0");
 * holding `0xff << 17` at the INDIRECT call site.  Both calls in that block take
 * the same 0x1fe0000 and the ROM rebuilds `mov r0,#0xff / lsl r0,#17` twice;
 * plain C commons it into a callee-saved pseudo (13 of 215).  The ladder:
 * pinning BOTH sites' r0/r1/r2 is exact, pinning the first site alone is exact,
 * pinning the SECOND site alone is 13 -- so the load-bearing pin is the FIRST
 * one, and once that use cannot be substituted the second constant has a single
 * use and needs nothing.  Two literal `int a1`/`int a2` locals do NOT separate
 * it (17), and -fno-rerun-cse-after-loop is byte-identical to the default, which
 * places this in the FIRST cse pass rather than the cse2 rerun that
 * docs/elevation.md's "prefer the flag" rule addresses.  Worth adding to that
 * section: the flag route does not exist for a same-block pair, and ONE pin on
 * the earlier use is enough -- a hard register is not a substitution target, so
 * the pseudo never forms and the later use stays a literal.
 *
 * FIVE MORE THINGS, all load-bearing, all measured by removing them singly.
 *
 * - THE SOURCE'S case ORDER IS THE ROM'S BLOCK ORDER, NOT 1..9.  gcc emits case
 *   bodies in source order, so reading the jump table plus the block addresses
 *   recovers it: 1, 2, 5, 9, 3, 6, 8, 4, 7.  Written 1..9 it is 115 of 215.
 *   The ROM's cross-jump falls out for free -- cases 3 and 8 share the tail
 *   because both reach 0x654 through r3, while case 6 uses r2 and keeps its own
 *   copy.  (115 differing from nothing but a case permutation is worth knowing:
 *   a jump-table switch that is nearly right may only be mis-ORDERED.)
 * - `int last = 0xff; *arg0 = last;`  A bare `*arg0 = 0xff` pools the constant,
 *   because gcc-2.96 has no immediate alternative for an HImode constant: 88 of
 *   215 and two instructions long.  The int carrier is the documented cure and
 *   it is what makes the length converge.
 * - THE FUNCTION POINTER.  `ldr r3, =Func_80008ac / bl _call_via_r3`; a direct
 *   call is 58 of 215.  Its RETURN TYPE is part of the declaration: `int` is
 *   exact, `void` is 6 of 215.
 * - LOCAL DECLARATION ORDER IS THE SLOT MAP, AND IT IS REVERSED.  The ROM has
 *   the 28-byte queue buffer at sp+0 and the 4-byte pair at sp+0x1c; gcc lays
 *   the FIRST-declared object at the HIGHER address, so `pair` must be declared
 *   before `buf`.  Declared the other way round it is 2 of 215 -- a two-encoding
 *   diff that is purely an addressing mode (`mov r0, sp` vs `add r0, sp, #0x1c`).
 * - STATEMENT ORDER AT THE VIEW LOAD.  `*(int *)(b + 0x644) = 0x80 << 9;` must
 *   come BEFORE `view = *(void **)((char *)iwram_3001f00 - 0x80);`.  The other
 *   way round sched2 issues the view load ahead of the address add and takes the
 *   store's base register with it: 4 of 215.
 *
 * The three iwram pointers follow src/rom_c9000/rom_cd508_b.c's idiom -- one
 * pool word for &iwram_3001f00 with `sub #0x8c` / `sub #0x80` off it, and a
 * SECOND, separate pool word for iwram_3001e74 in the tail block, which is what
 * the reference's two distinct pool entries for the same address say.
 */
extern unsigned char iwram_3001f00[];
extern unsigned char *iwram_3001e74;

extern unsigned char *_GetUnit(int id);
extern void Func_80bd424(unsigned short *p, int n);
extern void InitMatrixStack(void);
extern void MatrixSetLook(void *a, void *b);
extern int Func_80008ac(int a, int b);
extern void Func_8005258(int a, int b, int c);
extern void WaitFrames(int n);
extern void Func_80c10e8(unsigned short *p, int n);
extern int Func_80be378(unsigned short *p, void *base);
extern void Func_80ba27c(void *base, int n);
extern void Func_80ba2c0(void *base, int n);
extern void Func_80b9ec0(void *base, int n);
extern void Func_80ba978(void *base, int n);
extern void Func_80ba6ac(void *base, int n, unsigned short *p);
extern int Func_80b9dc4(void *base);
extern void WaitTextPrompt(void);
extern void Func_80b7e7c(void);
extern void Func_80bfba4(void *base);
extern void Func_80b6c90(void);
extern int Func_80b6c08(int n, unsigned short *buf);
extern void Func_80b8000(int id);
extern void Func_80c0774(int a, unsigned short b, int c);

int Func_80b9b30(unsigned short *arg0, int frames)
{
	unsigned short pair[2];
	unsigned short buf[14];
	int (*fn)(int, int);
	unsigned char *u;
	int *g;
	unsigned char *b;
	void *view;
	int flag;
	int last;
	int r;
	int n;
	int i;

	flag = 0;
	if (*(short *)arg0 == 0xff)
		return 0;
	u = _GetUnit(*(short *)arg0);
	if (*(short *)(u + 0x38) == 0)
		return -1;
	if (u[0x129] == 0)
		Func_80bd424(arg0, 1);
	g = *(int **)iwram_3001f00;
	g[1] = 0x3c;
	b = *(unsigned char **)((char *)iwram_3001f00 - 0x8c);
	g[5] = flag;
	*(int *)(b + 0x644) = 0x80 << 9;
	view = *(void **)((char *)iwram_3001f00 - 0x80);
	InitMatrixStack();
	MatrixSetLook(view, (char *)view + 0xc);
	fn = Func_80008ac;
	{
		register int q0 __asm__("r0");
		q0 = 0xff << 17;
		Func_8005258(0xff << 17, fn(q0, 0xc0 << 8), 0x7fff0000);
	}
	if (frames != 0) {
		g[0] = 0x80 << 6;
		WaitFrames(frames);
	}
	pair[0] = *arg0;
	pair[1] = 0xff;
	Func_80c10e8(pair, 1);
	r = Func_80be378(arg0, b + 0x654);
	if (r == 0) {
		switch (*(int *)(b + (0xd5 << 3))) {
		case 1:
			Func_80ba27c(b + 0x654, 0);
			break;
		case 2:
			Func_80ba2c0(b + 0x654, 0);
			break;
		case 5:
			Func_80b9ec0(b + 0x654, 1);
			break;
		case 9:
			Func_80b9ec0(b + 0x654, 0);
			break;
		case 3:
			Func_80ba978(b + 0x654, 0);
			break;
		case 6:
			Func_80ba978(b + 0x654, 1);
			break;
		case 8:
			Func_80ba978(b + 0x654, 2);
			break;
		case 4:
			Func_80ba6ac(b + 0x654, 0, arg0);
			break;
		case 7:
			if (Func_80b9dc4(b + 0x654) != 0)
				flag = 1;
			if (flag != 0)
				goto tail;
			break;
		}
	} else {
		if (r == -1) {
			WaitTextPrompt();
			WaitFrames(3);
		}
		Func_80c10e8(0, 0);
	}
	Func_80b7e7c();
	Func_80bfba4(b + 0x654);
	Func_80b6c90();
	n = Func_80b6c08(3, buf);
	for (i = 0; i < n; i++)
		Func_80b8000(buf[i]);
	last = 0xff;
	*arg0 = last;
tail:
	Func_80c0774(2, *(unsigned short *)(iwram_3001e74 + (0xc9 << 3)), 0);
	return flag;
}
