/* Func_8093fa0 -- NON-MATCHING, 140 encodings of 192.  Instruction count identical
 * (192 = 192) and SIZE IDENTICAL once `_CONST_1` is spelled -- see below.  Relocations
 * differ in pool ORDER only.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_8a000/8093fa0.c \
 *     asm/rom_8a000/rom_93304_a_c_c_c_c.s --func Func_8093fa0
 *
 * A POOLED `1` IN THIS BANK IS `_CONST_1`, AND IT ALREADY EXISTS -- const.sym:149, with its
 * validation note at const.sym:140.  NO NEW SYMBOL IS NEEDED.  The ROM's
 * `ldr r5, .L9411c @ 1` is the const.sym tell described at docs/elevation.md:4727, and
 * spelling it `(int)&_CONST_1` closed the exact four-byte size gap: 436 against 432 became
 * SIZE IDENTICAL, 192 = 192 encodings.  `extern int _CONST_1;` is already used by four
 * landed files, two of them in this same bank.
 *
 * BLOCKER: the same CALLEE-SAVED ROLE PERMUTATION as its twin Func_8093e28 -- read that
 * park first, including its measured negative that 60 declaration permutations are
 * byte-identical.  This function shows the identical rotation.
 *
 * ITS ONE EXTRA RESIDUE is the MID-FUNCTION POOL DUMP placing the `bad:` tail AFTER the
 * pool, which is what the remaining relocation-order difference is.  That is the
 * pool-placement question in docs/elevation.md, and it is a LAYOUT fact rather than an
 * allocation one, so it should be attacked separately from the rotation.
 */
typedef unsigned char u8;

struct Sprite {
	u8 pad00[0x26];
	u8 b26;
};

struct Actor {
	u8 pad00[8];
	int f08;
	int f0c;
	int f10;
	u8 pad14[4];
	int f14b;
	u8 pad1c[0x28 - 0x1c];
	int f28;
	u8 pad2c[4];
	int f30;
	u8 pad34[0x50 - 0x34];
	struct Sprite *f50;
	u8 f54;
};

struct Cell {
	u8 pad00[2];
	u8 b2;
	u8 pad03[1];
};

extern u8 gState[];
extern struct Cell gBuffer[];
extern struct Cell ewram_200fe00[];
extern int ewram_2000434;
extern int _CONST_1;

extern struct Actor *MapActor_GetActor(int id);
extern void CutsceneStart(void);
extern void CutsceneEnd(void);
extern void CutsceneWait(int frames);
extern void WaitFrames(int frames);
extern int _Func_801219c(int *v);
extern void Func_8092158(int id, int x, int z);
extern void MapActor_WaitMovement(int id);
extern void _Actor_SetSpriteFlags(struct Actor *a, int flags);
extern void _Actor_SetAnim(struct Actor *a, int anim);
extern void _Actor_TravelTo(struct Actor *a, int x, int y, int z);

int Func_8093fa0(void)
{
	u8 *g;
	struct Actor *a;
	int x;
	int z;
	int tx;
	int tz;
	int idx;
	int r;
	int flags;
	int one;
	u8 *p55;
	u8 *p5a;
	int v[5];

	g = gState;
	a = MapActor_GetActor(ewram_2000434);
	x = *(short *)((char *)a + 0xa) & 0xfff0;
	z = *(short *)((char *)a + 0x12) & 0xfff0;
	flags = 1;
	tx = x + 8;
	tz = z + 8;
	CutsceneStart();
	if (a->f54 == 1)
		flags = a->f50->b26;
	g += (0xf9 << 1);
	if (*g == 0) {
		idx = (tx / 16) + ((tz / 16) << 7);
		if (gBuffer[idx].b2 != ewram_200fe00[idx].b2)
			goto bad;
		v[0] = a->f08;
		v[1] = a->f0c;
		v[2] = a->f10;
		r = _Func_801219c(v);
		if (r != 0)
			goto bad;
		p5a = (u8 *)a + 0x5a;
		*p5a = r;
		Func_8092158(ewram_2000434, tx, tz);
		_Actor_SetAnim(a, 6);
		WaitFrames(4);
		_Actor_SetAnim(a, 7);
		a->f28 = 0x80 << 11;
		WaitFrames(4);
		*((u8 *)a + 0x55) = r;
		flags &= 0xfe;
		_Actor_SetSpriteFlags(a, flags);
		a->f30 = 0x80 << 9;
		a->f28 = r;
		_Actor_SetAnim(a, 0xc);
		WaitFrames(4);
		one = 1;
		*g = one;
		*p5a = one;
		WaitFrames(8);
	} else {
		p55 = (u8 *)a + 0x55;
		r = 0;
		*p55 = r;
		_Actor_SetAnim(a, 0xb);
		_Actor_TravelTo(a, tx << 16, a->f0c + (0x80 << 12),
				(tz << 16) + 0xfff00000);
		MapActor_WaitMovement(ewram_2000434);
		*p55 = 3;
		one = (int)&_CONST_1;
		flags |= one;
		a->f14b = a->f0c;
		_Actor_SetSpriteFlags(a, flags);
		CutsceneWait(4);
		*g = r;
		*((u8 *)a + 0x5a) = one;
	}
	CutsceneEnd();
	return 0;
bad:
	CutsceneEnd();
	return -1;
}
