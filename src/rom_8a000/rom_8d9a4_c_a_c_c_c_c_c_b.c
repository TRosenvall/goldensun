/* Cluster Func_808f1c0..Func_808f1c0 extracted from goldensun/asm/rom_8a000/rom_8d9a4_c_a_c_c_c_c_c.s.
 *
 * Total .text for this TU = 204 bytes (= 0xcc). Never attempted before batch 274.
 * No pins, no flags, and it screened OK ON THE FIRST COMPILE.
 *
 * It matched first time because src/non_matching/overlays/common1_1608.c -- a PARK -- is a
 * near-twin: same `galloc_iwram(0x11, 0xc1 * 8)`, same `_LoadItemIcon`, same
 * `UploadSpriteGFX(s[0x1c], 0x80, buf)` shape. Its mask lever transferred on the first
 * try. Second function this batch to land off a park rather than an elevated file, which
 * is worth repeating: A PARK IS A FILE-MATE SOURCE.
 *
 * TWO MASK READINGS KEPT:
 *   - `m = -0x21; x = x & m;` keeps the 32-bit mask, confirming common1_1608's finding
 *     (and `-0xd` behaves the same way).
 *   - TWO `& K` MASKS ON ONE BYTE MUST BE SPLIT ACROSS STATEMENTS. `(x & 0xf) & ~0xc`
 *     folds to `x & 3`; `b = x & 0xf;` then `(b & m2) | 4` gives the ROM's
 *     `and 0xf / and ~0xc / orr 4`.
 *
 * And `gState + K` folded into the pool word is fixed by TWO named pointers --
 * `gp = gState; g = gp + (0xfa << 1);` restores `ldr =gState / mov #0xfa / lsl #1 / add`,
 * matching the 8096140 park's idiom.
 */
extern unsigned char gState[];
extern unsigned char *GetFieldActor(int slot);
extern void *galloc_iwram(int tag, int size);
extern unsigned char *_CreateActor(int kind, int x, int y, int z);
extern void _LoadItemIcon(int item);
extern int UploadSpriteGFX(int a, int b, void *c);
extern void gfree(int tag);
extern void Func_808eee4(void);
extern void Func_808f0d8(unsigned char *a);
extern void WaitFrames(int n);
extern void _Actor_SetAnim(unsigned char *a, int n);
extern void _DeleteActor(unsigned char *a);

void Func_808f1c0(int item, int flags)
{
	unsigned char *gp;
	unsigned char *g;
	unsigned char *a;
	unsigned char *n;
	unsigned char *s;
	unsigned char *buf;
	int zero;
	int m;
	int m2;
	int b;

	gp = gState;
	g = gp + (0xfa << 1);
	a = GetFieldActor(*(int *)g);
	buf = (unsigned char *)galloc_iwram(0x11, 0xc1 << 3);
	n = _CreateActor(0x16, *(int *)(a + 8), *(int *)(a + 0xc) + (0x90 << 14),
	                 *(int *)(a + 0x10));
	if (n != 0) {
		s = *(unsigned char **)(n + 0x50);
		zero = 0;
		s[0x26] = zero;
		s[0x27] = zero;
		m = -0x21;
		s[5] = s[5] & m;
		b = s[9] & 0xf;
		m2 = -0xd;
		s[9] = (b & m2) | 4;
		_LoadItemIcon(item);
		UploadSpriteGFX(s[0x1c], 0x80, buf + (0x80 << 3));
		gfree(0x11);
		if ((flags & 1) != 0)
			*(void **)(n + 0x6c) = Func_808eee4;
		if ((flags & 2) != 0)
			Func_808f0d8(n);
		WaitFrames(0x50);
		_Actor_SetAnim(a, 1);
		_DeleteActor(n);
	}
}
