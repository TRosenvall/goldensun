/* WaitTextPrompt -- NON-MATCHING, 33 encodings of 153.
 * 0x080bb65c, first of two in asm/rom_b5000/rom_bb588_a_c.s (Func_80bb7c0
 * follows; parked separately in src/non_matching/rom_b5000/80bb7c0.c).
 * objcmp: size identical (356 bytes), 153 encodings, 33 differing.
 * tryc --align: 24 instructions in disagreeing regions, of 145, ALL inside one
 * block (the two OAM bitfield inserts after UploadSprite2, and the reloads
 * around the sin() call).  Prologue, wait loop, frame, pool layout, the key
 * tests, the i counter spilled to [sp], and the epilogue are exact.
 * NO SHIM, NO PIN, NO FLAG.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_b5000/WaitTextPrompt.c asm/rom_b5000/rom_bb588_a_c.s \
 *     --func WaitTextPrompt
 *
 * THE LADDER (each step measured):
 * - 145 differing at 376 bytes (first transcription) -> 126 at 352: a
 *   `union Ent *pe = &e` before AllocUploadSpriteGFX (the ROM's `add r7,sp,#4`
 *   is one pseudo; `&e` at each use made two), iwram_3001e40 NON-volatile and
 *   read through a named `ctr`.
 * - 126 -> 36: THE LOOP IS A goto LOOP, not while(1)/for(;;).  With a real loop
 *   loop.c hoists &iwram_3001e40 out, the pseudo is then spilled and
 *   rematerialised, and i gets a register.  In the ROM nothing is hoisted
 *   (Data_c3734 and 0x400004a are reloaded every pass), &iwram_3001e40 is
 *   CSE'd inside the body into r8 across calls, and i lives at [sp] -- which is
 *   exactly what `loop: ... goto loop;` with `goto out` exits gives.
 * - `int WaitTextPrompt(void)` + `return WaitFrames(1);`: `pop {r1}`.
 * - 36 -> 33: SET_IO(REG_BLDALPHA, 0x10) -- the do{}while(0) barrier puts the
 *   `mov r2,#0x10` before the address load, as the ROM has it.
 *
 * THE BLOCKER: register choice in the bitfield-insert block.  The ROM loads both
 * masks (0x3ff, ~0x3ff) before the `ldrh [r7,#8]`, keeps ~0x3ff in r2, and so
 * builds the x value in r1 with r2 as the reload register for a (r10) and c
 * (r9); the a->fc / c->f4 loads are then scheduled ABOVE the tile strh.  We load
 * the field first and reuse r3 for ~0x3ff, which moves x to r2, the reloads to
 * r1/r3, the `*ctr` reload to r2 and the 0x7fff to r3.  One allocation decision,
 * five symptoms.
 *
 * INERT: -fno-strict-aliasing (so it is NOT alias sets); struct A/C vs raw
 * byte offsets for iwram_3001ee4's two pointers; `unsigned int` bitfields; all
 * six spellings of the x expression (shift vs *8, grouping, temp, unsigned);
 * `v = sin(..) / 0x8000` placement (three forms); a temp for UploadSprite2's
 * result.  WORSE: `e.` instead of `pe->` (38); hand-masked `h[4]` inserts
 * (42-97); `bld` carrier before Func_800393c (117); volatile i (115); the ctr
 * pointer hoisted before the loop (inert, measured under while(1) only).
 */
#include "gba/io.h"
extern unsigned int iwram_3001e40;
struct A { int f0[3]; unsigned short fc; unsigned short fe; };
struct C { int f0; unsigned short f4; unsigned short f6; };
struct AC { struct A *a; struct C *c; };
extern struct AC *iwram_3001ee4;
extern unsigned char Data_c3734[];
extern volatile unsigned int gKeyHeld;
extern volatile unsigned int gKeyPress;

union Ent {
	int w[3];
	struct {
		int f0;
		unsigned char f4;
		unsigned char f5;
		unsigned short x : 9;
		unsigned short f6hi : 7;
		unsigned short tile : 10;
		unsigned short f8hi : 6;
	} f;
};

extern int _Func_8017364(void);
extern int WaitFrames(int n);
extern int AllocUploadSpriteGFX(int size);
extern void Func_80039fc(void *reg, int val);
extern void Func_800393c(void *reg, int val);
extern int UploadSprite2(int slot, void *gfx);
extern void Func_8003dec(union Ent *e, int n);
extern void Func_8003f3c(int slot);
extern int sin(int x);
extern void _PlaySound(int id);

int WaitTextPrompt(void)
{
	int i;
	union Ent e;
	union Ent *pe;
	unsigned char *gfx;
	struct A *a;
	struct C *c;
	unsigned int *ctr;
	int slot;
	int v;

	while (_Func_8017364() == 0)
		WaitFrames(1);
	pe = &e;
	slot = AllocUploadSpriteGFX(0x80);
	i = 0;
loop:
	{
		ctr = &iwram_3001e40;
		gfx = Data_c3734 + ((*ctr >> 2) & 7) * 0x80;
		a = iwram_3001ee4->a;
		c = iwram_3001ee4->c;
		Func_80039fc((void *)0x400004a, 4);
		Func_800393c((void *)0x400004a, 0x10);
		SET_IO(REG_BLDALPHA, 0x10);
		pe->w[1] = 0xa4 << 8;
		pe->w[2] = 0;
		pe->f.tile = UploadSprite2(slot, gfx);
		pe->f.x = a->fc * 8 + (c->f4 >> 8) + 4;
		v = sin(*ctr << 12);
		pe->f.f4 = v / 0x8000 + a->fe * 8 + (c->f6 >> 8) + 6;
		Func_8003dec(pe, 0xf0);
		if (gKeyHeld & 2)
			goto out;
		if (gKeyPress & 0x303)
			goto out;
		if (i > 15 && (gKeyHeld & 0x303))
			goto out;
		WaitFrames(1);
		i++;
	}
	goto loop;
out:
	_PlaySound(0x6f);
	Func_8003f3c(slot);
	return WaitFrames(1);
}
