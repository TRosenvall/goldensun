/* Func_8093e28 -- NON-MATCHING, 48 encodings of 168.  SIZE AND RELOCATIONS IDENTICAL,
 * instruction count identical (168 = 168).
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_8a000/8093e28.c \
 *     asm/rom_8a000/rom_93304_a_c_c_c_c.s --func Func_8093e28
 *
 * BLOCKER: CALLEE-SAVED ROLE PERMUTATION in `global_alloc`, and DECLARATION ORDER IS
 * MEASURED INERT AGAINST IT.  Every one of the 48 differing encodings is the same
 * rotation, with the SAME COUNT of callee-saved registers:
 *
 *     ROM   a=r6, x=r5, g=r8,  tz=r9,  tx=r10
 *     ours  a=r5, x=r6, g=r9,  tx=r8,  tz=r10
 *
 * THE MEASURED NEGATIVE IS THE POINT: 60 RANDOM PERMUTATIONS OF THE SEVEN RELEVANT
 * DECLARATIONS ALL PRODUCED BYTE-IDENTICAL OUTPUT -- 60 of 60 at exactly 121 differing
 * (pre-tail-fix).  So THE DECLARATION-ORDER LEVER DOES NOTHING WHEN THE LOCALS ARE ALL
 * REGISTER-RESIDENT, because there is no stack slot to reorder; that rule is about SLOTS.
 * Four statement-order variants of the x/z/tx/tz block were also inert to the rotation.
 *
 * Under allocno_compare's floor_log2(n_refs)*n_refs/live_length the ROM's priority order is
 * `x > a` and `g > tx > tz`; this candidate's is `a > x` and `tx > tz > g`.  A next attempt
 * should change REFERENCE COUNTS or LIVE LENGTHS, not names or order.
 *
 * WHAT GOT IT FROM 121 TO 48 IS WORTH REUSING -- A `bl` THAT APPEARS TWICE IN THE ROM'S
 * RELOCATION LIST AT ADJACENT ADDRESSES MEANS ONE SHARED `goto` TAIL, NOT TWO INLINE
 * COPIES.  The ROM put both CutsceneEnd calls at 0x142/0x14a; this candidate had one at
 * 0x9a.  Replacing two inline `CutsceneEnd(); return -1;` with `goto bad;` and a single
 * tail took it 170 lines / 121 differing -> 169 / 48 WITH SIZE AND RELOCATIONS EXACT.
 * THE RELOCATION LIST IS A CHEAP STRUCTURAL ORACLE FOR TAIL SHARING, readable before you
 * look at a single encoding.
 *
 * Also read off correctly here: `if (x<0) x += 0xffff; x >>= 16` is `/ 0x10000`, and a
 * `+ 0x17` before `>> 4` is `/ 16` WITH THE ROUNDING ADDEND ALREADY FOLDED -- the ROM's
 * `mov r3,r5 / add r3,#0x17 / asr r2,r3,#4` is `(x + 8) / 16` where `tx = x + 8` is a
 * separately live variable and combine folded the +15 in.  Reading it as a shift loses the
 * two-arm branch.
 */
typedef unsigned char u8;

struct Actor {
	u8 pad00[6];
	unsigned short f06;
	int f08;
	int f0c;
	int f10;
	u8 pad14[4];
	int f14b;
	u8 pad1c[0x28 - 0x1c];
	int f28;
	u8 pad2c[4];
	int f30;
	u8 pad34[0x55 - 0x34];
	u8 f55;
	u8 pad56[0x5a - 0x56];
	u8 f5a;
};

extern u8 gState[];
struct Cell {
	u8 pad00[2];
	u8 b2;
	u8 pad03[1];
};

extern struct Cell gBuffer[];
extern struct Cell ewram_2010200[];

extern struct Actor *MapActor_GetActor(int id);
extern void CutsceneStart(void);
extern void CutsceneEnd(void);
extern void CutsceneWait(int frames);
extern int _Func_801219c(int *v);
extern void Func_8092158(int id, int x, int z);
extern void Func_8092adc(int id, int a, int b);
extern void MapActor_WaitScript(int id);
extern void MapActor_WaitMovement(int id);
extern void _Actor_SetSpriteFlags(struct Actor *a, int flags);
extern void _Actor_SetAnim(struct Actor *a, int anim);
extern void _Actor_TravelTo(struct Actor *a, int x, int y, int z);

int Func_8093e28(void)
{
	u8 *g;
	int *id;
	struct Actor *a;
	int x;
	int z;
	int tx;
	int tz;
	int idx;
	int r;
	int one;
	int v[6];

	g = gState;
	id = (int *)(g + (0xfa << 1));
	a = MapActor_GetActor(*id);
	x = *(short *)((char *)a + 0xa) & 0xfff0;
	tx = x + 8;
	z = *(short *)((char *)a + 0x12) & 0xfff0;
	tz = z + 8;
	CutsceneStart();
	g += (0xf9 << 1);
	if (*g == 0) {
		idx = (tx / 16) + ((tz / 16) << 7);
		if (gBuffer[idx].b2 != ewram_2010200[idx].b2)
			goto bad;
		v[0] = a->f08;
		v[1] = a->f0c - 0x100000;
		v[2] = a->f10;
		r = _Func_801219c(v);
		if (r != 0)
			goto bad;
		Func_8092158(*id, tx, tz);
		a->f30 = 0x80 << 9;
		Func_8092adc(*id, 0xc0 << 8, 0);
		MapActor_WaitScript(*id);
		one = 1;
		a->f5a = one;
		a->f55 = r;
		_Actor_SetSpriteFlags(a, 0);
		_Actor_SetAnim(a, 0xd);
		_Actor_TravelTo(a, tx << 16, a->f0c - 0x100000, (tz << 16) + (0x80 << 13));
		MapActor_WaitMovement(*id);
		*g = one;
	} else {
		_Actor_SetAnim(a, 0xa);
		a->f55 = 3;
		a->f28 = 0x80 << 11;
		a->f14b = a->f0c;
		_Actor_SetSpriteFlags(a, 1);
		CutsceneWait(6);
		one = 0;
		*g = one;
		a->f5a = 1;
		a->f06 = 0xc0 << 8;
	}
	CutsceneEnd();
	return 0;
bad:
	CutsceneEnd();
	return -1;
}
