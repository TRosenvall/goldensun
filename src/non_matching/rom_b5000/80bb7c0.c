/* Func_80bb7c0 (CloseBattleHud) -- NON-MATCHING, 97 encodings of 119.
 * 0x080bb7c0, third of three in asm/rom_b5000/rom_bb588_a.s.
 *
 * objcmp: XX SIZE ref 280 bytes, ours 272 / XX ENCODINGS differ in 97 place(s)
 * (ref 119, ours 115).  tryc --align: 32 instructions in disagreeing regions,
 * of 112.  Reproduced on three runs.  NO SHIM, NO PIN, NO FLAG in this file.
 * datacheck reports no data in the .s, so landing would be a plain text split.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_b5000/80bb7c0.c \
 *     asm/rom_b5000/rom_bb588_a.s --func Func_80bb7c0
 *
 * NOTE THE SIZE LINE: this park is 8 BYTES SHORT with 115 instructions against 119, so
 * the 97 is NOT a distance to exact -- most of it is the stream shifting past the four
 * missing instructions.  Quote it only beside the size and count, per the measurement
 * rule in docs/elevation.md.
 *
 * EVERYTHING EXCEPT ONE VALUE IS EXACT: the wait loop, the sprite-slot
 * allocation, both OAM bitfield inserts with all four mid-function pool masks in
 * the reference's order, the byte store, the key test, the loop's back edge and
 * the whole epilogue.  The residue is FOUR INSTRUCTIONS in one place.
 *
 * ===================== THE BLOCKER, NAMED =====================
 * PASS: loop.c's invariant motion, guarded by true_dependence against the
 * loop's own stores -- NOT cse, NOT combine, NOT the scheduler.
 *
 * The ROM hoists ONE `ldrh` of the x parameter's stack home out of the sprite
 * loop and keeps the zero-extended value in r9 for the whole loop:
 *     mov r2, sp / ldrh r2, [r2] / ... / mov r9, r2     (preheader)
 *     ... add r3, r9 ...                                (body)
 * That extra live value is what pushes the y parameter out to sp+4 and the mask
 * constant 4 up into r8, which is why the ROM copies `mov r2, r8` before each
 * `and` and we do not.  ONE cause, four instructions, three symptoms.
 *
 * `*thumb_zero_extendhisi2` accepts a MEM only, so the ROM's `ldrh` PROVES the
 * value was in memory at that point -- a register source gives lsl+lsr instead.
 * Two ways to put it in memory were measured:
 *
 *   (a) `unsigned short x0` as the parameter.  gcc gives the SImode parm pseudo
 *       a register, expands the zero_extend as lsl+lsr, hoists only the lsl and
 *       leaves `lsr r2, r1, #0x10` in the loop: 54 of 112, 120 encodings.
 *   (b) `int pos[2]; pos[0] = x0; pos[1] = y0;` -- a local ARRAY always has a
 *       memory home, and its two `str`s, the `ldr [sp,#4]` of y each iteration
 *       and the 0x14 frame are all the ROM's.  THIS IS THE SHIPPED FORM (32 of
 *       112).  But `(unsigned short)pos[0]` is absorbed by combine's
 *       force_to_mode into the 9-bit bitfield mask that consumes it, so gcc
 *       reads the WORD; and spelled `*(unsigned short *)pos` to force a genuine
 *       HImode load the load is NOT hoisted (104 of 119, +3 insns, plus a
 *       frame-pointer copy) -- loop.c will not move it above the `strh [r5,#6]`
 *       bitfield store, which has the same `unsigned short` alias set and whose
 *       base is a pseudo loop.c created itself.  Hoisting it by hand
 *       (`xx = *(unsigned short *)pos;` before the loop) costs a frame pointer
 *       and 0x18 of stack: 45 of 112.  Three placements tried, all worse.
 *
 * So the two halves are in tension: the absorbable cast hoists and reads a word;
 * the unabsorbable HImode load reads a halfword and does not hoist.  A spelling
 * that is BOTH is what this park needs, and nothing in the corpus offers one.
 *
 * ===================== WHAT DID PAY, FOR THE NEXT ATTEMPT =====================
 * From 111 of 119 down to 97, four levers, each measured alone:
 *
 * 1. `volatile unsigned int iwram_3001e40`.  The ROM re-reads it between the two
 *    bitfield stores; non-volatile gcc reads once and shifts the same value
 *    twice.  This is NOT a fakematch dodge: `(iw & 4) >> 1` and `>> 2` are a
 *    one-pixel jitter off bit 2, i.e. a counter an interrupt writes.  111 -> 97.
 * 2. `unsigned int`, not `int`: the ROM shifts with `lsr`, not `asr`.
 * 3. `int Func_80bb7c0(...)` with `return WaitFrames(1);`.  `pop {r1} / bx r1`.
 * 4. THE TWO ADDITIVE CONSTANTS ARE ALREADY-NARROWED, AND WRITING THEM NARROWED
 *    IS FREE.  The reference adds 0xfffc before a 9-bit bitfield insert and 0xf8
 *    before an `unsigned char` store -- force_to_mode's own reduction of -4 and
 *    -8 to the store's mode.  `+ 0xfffc`/`+ 0xf8`, `- 4`/`- 8`, and
 *    `(unsigned short)(... - 4)`/`(unsigned char)(... - 8)` ALL compile
 *    identically, so the spelling is inert -- but read the pool placement, not
 *    the value: 0x3ff, ~0x3ff, 0x1ff and ~0x1ff are in the MID-function pool
 *    (HImode, 60-byte range) and 0xfffc is in the END pool (SImode, 1020), which
 *    is what says the add is SImode and only the mask is narrow.
 *
 * STILL WRONG, AND DOWNSTREAM OF THE BLOCKER: `mov r2, #0x10` for the BLDALPHA
 * store comes out as `ldr r2, =0x10` (the HImode-constant rule) because the
 * `int bld` carrier is loop-invariant and gcc hoists it into a register instead
 * of rematerialising it.  Declaring it inside the loop, assigning inside the
 * loop, and sharing it with the adjacent `Func_800393c(reg, bld)` argument all
 * measure the same.  Expect this to resolve once the register pressure is the
 * ROM's; it is one instruction.
 *
 * A NAMED `void *reg` LOCAL FOR 0x400004a IS WRONG -- the bare literal is
 * hoisted into r7 as the ROM has it, the named local reloads the pool at both
 * call sites.
 */
extern volatile unsigned int iwram_3001e40;
extern int gKeyPress;

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

extern void *_Func_8021bc8(int n);
extern int _Func_8017364(void);
extern int WaitFrames(int n);
extern int AllocUploadSpriteGFX(int size);
extern void Func_80039fc(void *reg, int val);
extern void Func_800393c(void *reg, int val);
extern int UploadSprite2(int slot, void *gfx);
extern void Func_8003dec(union Ent *e, int n);
extern void Func_8003f3c(int slot);

int Func_80bb7c0(int x0, int y0)
{
	int pos[2];
	union Ent e;
	void *gfx;
	int slot;
	int bld;

	gfx = _Func_8021bc8(0);
	pos[0] = x0;
	pos[1] = y0;
	while (_Func_8017364() == 0)
		WaitFrames(1);
	slot = AllocUploadSpriteGFX(0x80);
	while (1) {
		Func_80039fc((void *)0x400004a, 4);
		bld = 0x10;
		Func_800393c((void *)0x400004a, bld);
		*(volatile unsigned short *)0x4000052 = bld;
		e.w[1] = 0x80 << 23;
		e.w[2] = 0;
		e.f.tile = UploadSprite2(slot, gfx);
		e.f.x = ((iwram_3001e40 & 4) >> 1) + (unsigned short)pos[0] + 0xfffc;
		e.f.f4 = pos[1] - ((iwram_3001e40 & 4) >> 2) + 0xf8;
		Func_8003dec(&e, 0xf0);
		if ((gKeyPress & 0x303) != 0)
			break;
		WaitFrames(1);
	}
	Func_8003f3c(slot);
	return WaitFrames(1);
}
