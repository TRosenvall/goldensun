/* UI_NameEntry  --  0x08020bd8, was asm/rom_15000/rom_20198_c_c_c_a_a_a_a_c.s.
 *
 * SPLIT SHAPE: a PLAIN 3-WAY TEXT SPLIT, NO EXTRA EXPORTS.
 *   The file holds FOUR functions in this order -- Func_8020b64, UI_NameEntry,
 *   Func_802106c, Func_8021228 -- so UI_NameEntry is the middle cut:
 *     python3 tools/split_s.py asm/rom_15000/rom_20198_c_c_c_a_a_a_a_c.s UI_NameEntry
 *   gives _a (Func_8020b64), _b (UI_NameEntry -> this .c), _c (Func_802106c +
 *   Func_8021228).  tools/datacheck.py reports NO data section, and `stage1.ld`
 *   names the object EXACTLY ONCE, at line 538, with `(.text)` only and no
 *   `(.rodata)` line -- so the batch-300 "the script can name an object twice
 *   because of a DIFFERENT function in the file" trap does NOT apply here; I
 *   grepped for it.  NO `.global` is needed anywhere: `.thumb_func_start` in
 *   include/macros.inc already emits `.global` for every function in the file,
 *   `Func_8020b14` is in the sibling object rom_20198_c_c_c_a_a_a_a_b.s and is
 *   already global there, and `.L371f6` is ALREADY `.global` at
 *   asm/rom_15000/rom_20198_c_c_c_c_c.s:162.  Run split_s, confirm `make compare`
 *   is still green BEFORE writing any .c, then delete the _b.s.
 *
 * NON-MATCHING, 488 of 525 encodings differ.
 *
 * SIZE and COUNT are BOTH INEXACT -- ref 1172 bytes / 525 encodings against
 * ours 1216 / 547 -- so objcmp's figure is SATURATED and does not rank.  Rank
 * with aligncmp: this candidate is aligned-equal 253 of 525 (48.2%), 384
 * differing/ins/del in 107 hunks.
 *
 * Shims: NONE -- tools/shimcount.py reports 0.  The only non-plain construct is
 * `DMA3_COPY` from include/dma.h, which is the tree's established `static
 * inline` helper and is what the FILE-MATE park below uses for the same
 * register block.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/8020bd8.c \
 *     asm/rom_15000/rom_20198_c_c_c_a_a_a_a_c.s --func UI_NameEntry
 *   -> XX SIZE  ref 1172 bytes, ours 1216
 *      XX ENCODINGS differ in 488 place(s) (ref 525, ours 547)
 *
 * READ src/non_matching/rom_15000/802106c.c FIRST.  Func_802106c is the NEXT
 * FUNCTION IN THIS SAME .s and its park is this function's template -- it is
 * where `struct Spr { int f0; int f4; int f8; int fc; }`, the
 * `extern volatile int gKeyRepeat` declaration, the
 * `DMA3_COPY((void *)0x50001e0, pal, 0x20); pal[4] = 0x6318;` palette block,
 * the `AllocSpriteSlot / UploadSpriteGFX / Func_801eadc / _Func_80b0a20`
 * sprite-record idiom and the `gKeyRepeat` bit meanings (0x40 up, 0x80 down,
 * 0x20 left, 0x10 right, 2 cancel, 1 confirm; sounds 0x6f move, 0x70 confirm,
 * 0x71 cancel) all come from.  Every one of those transferred unchanged and
 * NONE of them is in this residue.  A park is a file-mate source, a third time.
 *
 * WHAT IS ALREADY RIGHT.  The RELOCATION SEQUENCE is 51 entries against the
 * reference's 53 and every entry present is in the SAME ORDER -- symbol for
 * symbol, including both pool blocks, both `Data_31xxxx` sprite sheets and the
 * seven `_PlaySound` sites.  That fixes the whole control-flow skeleton, and it
 * says the two missing entries are ONE merged basic block, not a mis-read
 * program (see the blocker).  Landed and out of the residue: the whole
 * 0x60-byte frame decomposition (`unsigned char nb[16]` at sp+0x50,
 * `struct Spr o1` at 0x40, `struct Spr o2` at 0x30, twelve spill words below);
 * both `CreateUIBox` calls with their fifth argument through sp+0;
 * `tbl[0xea3] = redraw; nb[0] = ret;` -- which must assign FROM THE INT LOCALS,
 * because the reference does `ldrb` out of redraw's and ret's own stack slots
 * and a literal 1 / 0 would be `mov`; the four direction handlers including
 * both `x = 5 - (x != 3)` / `x = (x != 6) + 4` boolean forms that gcc emits as
 * `eor / neg / orr / lsr #31`; `(unsigned)(row - 4) > 1` as the two-row test;
 * the `((box1[7]+row+1) * 32 + (box1[6]+col+1)) * 2` tile lookup as a BYTE
 * index into tbl; and the two-`goto` exit shape the file-mate park also needs.
 *
 * LEVERS THAT PAID, in the order they paid (aligncmp aligned-equal of 525):
 *
 * 1. NO SEPARATE COUNTER FOR THE COPY-OUT LOOP -- reuse the `t` scratch.
 *    [215 -> 252, and objcmp 530 -> 489]  The single biggest edit on this
 *    function, and it is the low-register rotation rule read forwards: one
 *    EXTRA short-lived quantity pushed `col`/`row` out of r6/r7 into r7/r8 and
 *    then pushed `e1` (`&o1`) out of a register ENTIRELY, into a thirteenth
 *    spill word.  Deleting the counter gave the quantity back.
 *
 * 2. THE COPY-IN LOOP'S BOUND IS A SIGNED COMPARE: `while ((int)p <= (int)&nb[14])`.
 *    [252 -> 253, objcmp 489 -> 488]  Small but it is a CORRECTNESS-shaped
 *    result, not a tuning one.  The reference ends that loop with `ble`; a plain
 *    C pointer comparison is UNSIGNED and gives `bls`.  So the reference's loop
 *    latch was written on signed ints, and the cast is the honest spelling of
 *    that.  The obvious alternative -- an INDEXED loop (`for (t = 0; t <= 13;
 *    t++) { c = u[t]; np[t] = c; ... }`) on the theory that loop.c's strength
 *    reduction would carry the signed condition onto the derived pointer -- was
 *    MEASURED AND IS WRONG: 249, three worse, and it does not restore `ble`.
 *
 * MEASURED AND INERT (each one compile; all three land on 252/385/107, i.e.
 * byte-for-byte the base):
 *   - merging `slot` into `t` so one variable carries the AllocSpriteSlot
 *     result AND the Func_801eadc result, which is exactly what the reference's
 *     r5 does.  gcc had already coalesced them.
 *   - renaming `w` (the cursor-width local).
 * MEASURED AND REJECTED:
 *   - dropping the `pal` local and writing `(unsigned short *)0x50001c0` /
 *     `*(unsigned short *)0x50001c8` inline: 211, FOUR WORSE than keeping it.
 *     The palette base is a real held quantity (the reference's r4), even though
 *     it is used only twice and dies immediately.
 *   - dropping BOTH `pal` and the copy-out counter: 249 against 252 for the
 *     counter alone, so the two edits are not additive.
 *   - the 0x19 bit clear through an int carrier, two ways -- `c = *p & -13;`
 *     [235] and `c = *p; c &= ~0xc;` [236] -- against `*p &= ~0xc;` [252].
 *     The reference builds the mask in SImode (`mov r3, #0xd / neg r3, r3`,
 *     which is `& ~0xc`, NOT `& ~0xd` -- the batch-295 `neg` trap) where ours
 *     narrows to QImode and emits one `mov r3, #0xf3`.  Ours is one instruction
 *     SHORTER and every attempt to buy the reference's two costs 16 aligned
 *     lines elsewhere, so the narrowing is NOT the thing to fix next.
 *
 * THE BLOCKER, in two parts, both attributable.
 *
 * (a) `jump.c` CROSS-JUMPING MERGES TWO TAILS THE REFERENCE KEEPS DISTINCT, and
 *     this is the ONLY difference in the relocation sequence.  The reference has
 *     TWO separate `Func_8016478(box2); Func_8020b64(box2, np); redraw = 1;`
 *     blocks -- one closing the backspace path at 0x20f76 and one closing the
 *     character-entry path at 0x21020 -- and ours has ONE, which is why our 51
 *     relocations are the reference's 53 minus exactly that pair.  THE REFERENCE
 *     DOES NOT MERGE THEM BECAUSE ITS TWO TAILS ARE NOT IDENTICAL AT THE INSN
 *     LEVEL: one ends `mov r2, #1 / str r2, [sp, #0xc]` and the other
 *     `mov r3, #1 / str r3, [sp, #0xc]`.  A one-register difference in a
 *     reload-chosen scratch is the whole reason do_cross_jump declines.
 *     That makes this residue DOWNSTREAM of register allocation and NOT
 *     addressable from the source: no spelling of two identical statement
 *     sequences can make gcc choose two different scratch registers.  It will
 *     resolve when (b) does, and not before -- so do not spend spellings on it.
 *
 * (b) `reload1.c` gives `e1` (`&o1`) a STACK SLOT where the reference keeps it
 *     in r11, and splits the frame-address computation.  Ours emits
 *     `mov r3, sp / add r3, #0x44 / str r3, [sp, #0x18]` at BOTH assignment
 *     sites against the reference's `add r2, sp, #0x40 / mov r11, r2`, and that
 *     costs the thirteenth spill word (`sub sp, #0x64` against #0x60), which
 *     shifts EVERY frame offset by 4 and is most of the 384 differing lines.
 *     This is the SAME blocker the file-mate park books as its item (b), where
 *     it reads: "`&obj` in the taken branch costs three instructions against
 *     the reference's two -- RELOAD SPLITS IT BECAUSE THE PSEUDO IS ALLOCATED TO
 *     r8, and `add rd, sp, #imm` needs a LOW destination".  Here it is one step
 *     worse: the pseudo does not reach a high register at all.  The rotation is
 *       ref   r6 col  r7 row  r8 box1  r9 flag  r10 e2  r11 e1
 *       ours  r7 col  r8 row  r9 e2    r10 box1 r11 flag  e1 -> sp+0x18
 *     -- six callee-saved quantities in the reference against five in ours, so
 *     this is the MISSING-quantity form of the rotation rule, and lever 1 above
 *     already bought one of the two back.  There is at least one more
 *     short-lived low-register quantity to find; the reference frees r6 right
 *     after `nb[0] = ret` and immediately reuses it for `col`, so the thing to
 *     look for lives in the window between the frame-address setup and
 *     AllocSpriteSlot.  That, and NOT the QImode mask or the cross-jump, is the
 *     next move.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

struct Spr { int f0; int f4; int f8; int fc; };

extern volatile int gKeyRepeat;
extern int gKeyPress;
extern unsigned char *iwram_3001e8c;
extern unsigned int iwram_3001800;
extern unsigned char Data_310a4[];
extern unsigned char Data_317e4[];
extern unsigned char Data_73864[];
extern signed char L371f6[] __asm__(".L371f6");

extern unsigned char *_GetUnit(int id);
extern void Func_800479c(void);
extern unsigned short *CreateUIBox(int a, int b, int c, int d, int e);
extern int GetPortrait(int id);
extern void Func_8019da8(int a, int b, int c, int d);
extern void Func_80209d0(unsigned short *box, unsigned char *p);
extern void Func_801e41c(unsigned short *box, int b, int c, int d, int e);
extern void Func_8020b64(unsigned short *box, void *p);
extern int AllocSpriteSlot(void);
extern void UploadSpriteGFX(int slot, int size, unsigned char *gfx);
extern int Func_801eadc(int a, unsigned int b, unsigned short *c, int d, int e);
extern void _Func_80b0a20(struct Spr *o, int x, int y);
extern void _Func_80b09fc(struct Spr *o, int x, int y, int d);
extern void _Func_80b08b8(struct Spr *o);
extern void _Func_80b0958(struct Spr *o);
extern int Func_8020b14(unsigned char *s);
extern void Func_8020a60(unsigned short *box, int b, int c, int d, int e, int f);
extern void _PlaySound(int n);
extern void WaitFrames(int n);
extern void Func_8016478(unsigned short *box);
extern void CloseUIBox(unsigned short *box, int mode);
extern void Func_8019e48(int id);

int UI_NameEntry(int arg)
{
	unsigned char nb[16];
	struct Spr o1;
	struct Spr o2;
	unsigned short *box1;
	unsigned short *box2;
	unsigned short *pal;
	unsigned char *tbl;
	unsigned char *u;
	unsigned char *np;
	unsigned char *cur;
	unsigned char *p;
	unsigned char *s;
	unsigned char *q;
	struct Spr *e1;
	struct Spr *e2;
	int ret;
	int na;
	int len;
	int redraw;
	int flag;
	int slot;
	int col;
	int row;
	int w;
	int t;
	int c;

	ret = 0;
	na = 0;
	len = 0;
	np = &nb[1];
	u = _GetUnit(arg);
	tbl = iwram_3001e8c;
	redraw = 1;
	flag = 1;
	Func_800479c();
	box1 = CreateUIBox(3, 6, 0x18, 9, 2);
	box2 = CreateUIBox(8, 3, 8, 3, 2);
	Func_8019da8(GetPortrait(arg), 0, 3, 1);
	Func_80209d0(box1, Data_73864);
	Func_801e41c(box1, 0x12, 0, 0x12, 7);
	tbl[0xea3] = redraw;
	nb[0] = ret;
	p = np;
	s = u;
	do {
		c = *s++;
		*p++ = c;
		if (c != 0) {
			na++;
			len++;
		}
	} while ((int)p <= (int)&nb[14]);
	np[14] = 0;
	Func_8020b64(box2, u);
	slot = AllocSpriteSlot();
	col = 0x12;
	row = 5;
	if (slot <= 0x5f) {
		UploadSpriteGFX(slot, 0x80, Data_310a4);
		t = Func_801eadc(slot, 0x80 << 23, box1, 0, 0);
		e1 = &o1;
		e1->f0 = t;
		_Func_80b0a20(e1, box1[6] * 8 + 0x8c, box1[7] * 8 + 0x34);
	} else {
		e1 = &o1;
	}
	slot = AllocSpriteSlot();
	if (slot <= 0x5f) {
		UploadSpriteGFX(slot, 0x80, Data_317e4);
		t = Func_801eadc(slot, 0x80 << 23, box1, 0, 0);
		e2 = &o2;
		e2->f0 = t;
		*(unsigned char *)(t + 0xf) = 0xff;
		*(unsigned char *)(t + 0x19) &= ~0xc;
		_Func_80b0a20(e2, Func_8020b14(np) + 0x46, 0x16);
	} else {
		e2 = &o2;
	}
	pal = (unsigned short *)0x50001c0;
	DMA3_COPY((void *)0x50001e0, pal, 0x20);
	pal[4] = 0x6318;
	cur = np + na;
	for (;;) {
		w = 1;
		if (col == 0x12) {
			if (row == 4)
				w = 3;
			if (row == 5)
				w = 3;
		}
		Func_8020a60(box1, col, row, w, 1, 0xe);
		WaitFrames(1);
		Func_8020a60(box1, col, row, w, 1, 0xf);
		if (flag != 0) {
			flag = 0;
			_Func_80b09fc(e1, (box1[6] + col) * 8 - 7,
				      (box1[7] + row) * 8 + 0xf, 3);
		}
		if (redraw != 0) {
			redraw = 0;
			_Func_80b09fc(e2, Func_8020b14(np) + 0x46, 0x16, 3);
		}
		_Func_80b08b8(e1);
		_Func_80b0958(e2);
		t = (iwram_3001800 >> 1) & 7;
		q = (unsigned char *)e2->f0;
		*(unsigned short *)(q + 0x16) =
			(*(unsigned short *)(q + 0x16) & 0xfffffe00) |
			((*(unsigned short *)(q + 6) + L371f6[t]) & 0x1ff);
		t = (t + 5) & 7;
		q[0x14] = q[8] + (unsigned char)L371f6[t];
		if ((gKeyRepeat & 0x40) != 0) {
			_PlaySound(0x6f);
			flag = 1;
			row--;
			if (col == 0x12)
				row = 5 - (row != 3);
			else if (row == -1)
				row = 5;
		}
		if ((gKeyRepeat & 0x80) != 0) {
			_PlaySound(0x6f);
			flag = 1;
			row++;
			if (col == 0x12)
				row = (row != 6) + 4;
			else if (row == 6)
				row = 0;
		}
		if ((gKeyRepeat & 0x20) != 0) {
			_PlaySound(0x6f);
			col--;
			flag = 1;
			if (col == -1) {
				col = 0x12;
				if ((unsigned int)(row - 4) > 1)
					col = 0x10;
			} else if (col == 5 || col == 0xb || col == 0x11) {
				col--;
			}
		}
		if ((gKeyRepeat & 0x10) != 0) {
			_PlaySound(0x6f);
			col++;
			flag = 1;
			if (col == 0x13)
				col = 0;
			else if (col == 5 || col == 0xb || col == 0x11)
				col++;
			if (col == 0x12 && (unsigned int)(row - 4) > 1)
				col = 0;
		}
		if ((gKeyPress & 8) != 0) {
			_PlaySound(0x6f);
			flag = 1;
			col = 0x12;
			row = 5;
		}
		if ((gKeyRepeat & 2) != 0) {
			_PlaySound(0x71);
			goto back;
		}
		if ((gKeyRepeat & 1) == 0)
			continue;
		_PlaySound(0x70);
		if (col == 0x12) {
			if (row == 5) {
				if (len == 0) {
					*(unsigned char *)(e2->f0 + 5) = 0xd;
					Func_8016478(box2);
					Func_8020b64(box2, u);
					WaitFrames(0xa);
					goto done;
				}
				p = np;
				s = u;
				t = 0;
				do {
					c = *p++;
					*s++ = c;
					t++;
				} while (t <= 0xe);
				goto done;
			}
			if (row != 4)
				continue;
back:
			if (len == 0) {
				ret = -1;
				goto done;
			}
			len--;
			cur--;
			*cur = 0;
			Func_8016478(box2);
			Func_8020b64(box2, np);
			redraw = 1;
			continue;
		}
		t = ((box1[7] + row + 1) * 32 + (box1[6] + col + 1)) * 2;
		c = tbl[t];
		if (len == 5)
			continue;
		*cur++ = c;
		*cur = 0;
		len++;
		if (len == 5) {
			flag = 1;
			col = 0x12;
			row = 5;
		}
		Func_8016478(box2);
		Func_8020b64(box2, np);
		redraw = 1;
	}
done:
	CloseUIBox(box1, 2);
	CloseUIBox(box2, 2);
	Func_8019e48(arg);
	WaitFrames(1);
	return ret;
}
