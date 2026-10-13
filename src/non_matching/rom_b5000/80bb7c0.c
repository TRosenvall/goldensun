/* Func_80bb7c0 (CloseBattleHud) -- NON-MATCHING, 94 encodings of 119 differ.
 *
 * 0x080bb7c0, SECOND of the two functions in asm/rom_b5000/rom_bb588_a_c.s.
 * ours 120 encodings / 96 16-bit instructions against ref 119 / 96 -- the
 * INSTRUCTION COUNTS AGREE -- pool/32-bit entries 22 against 21, 284 bytes
 * against 280.  Honest residues: tryc --align 27 of 112; aligncmp 57 (raw, 80 aligned-equal of 119; and
 * INFLATED by the one extra pool word, which shifts every PC-relative load --
 * aligncmp's own docstring says it counts those).  0 pins, 0 devices.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_b5000/80bb7c0.c \
 *     asm/rom_b5000/rom_bb588_a_c.s --func Func_80bb7c0
 * Rank variants on tryc --align here, NOT aligncmp, until the pool count agrees.
 *
 * SPLIT SHAPE.  datacheck.py exits silent: no data section, no EXPORTS line.
 * split_s.py asm/rom_b5000/rom_bb588_a_c.s Func_80bb7c0 --dry-run:
 *   would write rom_bb588_a_c_a.s (1 function, 164 lines)   <- WaitTextPrompt
 *   would write rom_bb588_a_c_b.s (1 function, 129 lines)   <- this function
 *   would REMOVE rom_bb588_a_c.s, rewrite stage1.ld
 * Plain two-way TEXT split; no exports.  (The piece's other member,
 * WaitTextPrompt, is parked separately at 33 of 153.)
 *
 * ========= THREE CORRECTIONS TO THE PREVIOUS HEADER (batch 348, brief D) =====
 *
 * 1. "115 instructions against 119 ... the residue is FOUR INSTRUCTIONS in one
 *    place" was wrong on both halves.  119/115 was the STREAM length; the
 *    16-bit instruction counts were 96/94 and the 21 pool/`bl` entries were
 *    EQUAL, so the gap was TWO instructions plus the two alignment pad
 *    halfwords that follow from it.  And it was TWO SITES OF 1, measured:
 *      preamble after AllocUploadSpriteGFX   ref 10, ours 9    -1
 *      second OAM bitfield insert            ref  3, ours 2    -1
 *    Site 2 is downstream of site 1 exactly as the old header argued: the extra
 *    long-lived value pushes the mask constant 4 out to r8, so each `and` pays
 *    a `mov r2, r8` first.  One cause, two instructions.
 *
 * 2. THE NAMED BLOCKER IS REFUTED BY THE COMPILER'S OWN SOURCE.  The old header
 *    said "PASS: loop.c's invariant motion, guarded by true_dependence against
 *    the loop's own stores", and that loop.c would not move the HImode load
 *    above the `strh [r5,#6]` bitfield store because of a shared alias set.
 *    true_dependence IS NEVER REACHED for this loop.  loop.c:2444-2447
 *        else if (GET_CODE (insn) == CALL_INSN)
 *          { if (! CONST_CALL_P (insn)) unknown_address_altered = 1;
 *    fires for ANY non-const call in the loop, and loop.c:3265-3267
 *        if (RTX_UNCHANGING_P (x) ? unknown_constant_address_altered
 *                                 : unknown_address_altered) return 0;
 *    returns 0 for every non-RTX_UNCHANGING_P MEM BEFORE the loop_store_mems /
 *    true_dependence walk at loop.c:3269-3277.  This loop holds FIVE calls
 *    (Func_80039fc, Func_800393c, UploadSprite2, Func_8003dec, WaitFrames), so
 *    NO ordinary memory load can EVER be hoisted out of it and no alias set can
 *    matter.  The old header's "a spelling that is BOTH is what this park needs,
 *    and nothing in the corpus offers one" rests on a guard that does not run.
 *
 *    Corollary and the lever: the ROM's `mov r2, sp / ldrh r2, [r2] / mov r9, r2`
 *    is NOT A HOIST.  It is a read written BEFORE the loop in the source.
 *    `xx = (unsigned short)pos[0];` placed right after AllocUploadSpriteGFX --
 *    the ROM's own position -- takes the instruction count to the reference's 96
 *    exactly, the pad count to the reference's 2, and tryc --align 32 -> 27.
 *    (aligncmp moves 41 -> 57 the other way, which is the extra pool word
 *    shifting every PC-relative load, not a regression in the code.)
 *
 * 3. "a frame-pointer copy" was a misreading.  `mov r2, sp` is there because
 *    Thumb-1 `ldrh` cannot use sp as a base register; there is no frame pointer.
 *
 * ======================= WHAT REMAINS, TWO NAMED THINGS =====================
 * a) Ours reads the WORD and shifts:
 *      ldr r2, [sp] / lsl r3, r2, #16 / lsr r3, r3, #16 / mov r9, r3     (4)
 *    against the ROM's
 *      mov r2, sp / ldrh r2, [r2] / mov r9, r2                           (3)
 *    combine's force_to_mode pushes the 9-bit bitfield mask into the PLUS
 *    operands (combine.c:6855-6860) and the `(unsigned short)` disappears.
 *    A UNION memory home does NOT help: `union { int w[2]; unsigned short h[4]; }`
 *    read as `pos.h[0]` is BIT-IDENTICAL to this body, so the narrowing is
 *    force_to_mode and not aliasing.  `*(unsigned short *)pos` forces the HImode
 *    load but costs +2 instructions and +1 pool (tryc --align 45).
 * b) The one extra pool entry is `ldrh r2, .L12` for
 *      *(volatile unsigned short *)0x4000052 = bld;
 *    where the ROM has `mov r2, #0x10`.  Brief lever 3 / arm.md:4269-4271,
 *    pool_range 64: with no free high register left, reload no longer parks the
 *    `int bld` carrier in fp (which is what the OLD body did -- `mov r2, fp`)
 *    and the literal folds into the HImode store instead.  The ROM materialises
 *    0x10 TWICE -- `mov r1, #0x10` for the call argument and `mov r2, #0x10` for
 *    the store -- so the carrier is rematerialised there, not shared.
 *    `bld` declared and assigned INSIDE the loop in its own block is
 *    BIT-IDENTICAL, which re-confirms the old header's negative at the new,
 *    higher pressure.
 *
 * ================= KEEP ALL OF THIS (unchanged, still measured) =============
 * 1. `volatile unsigned int iwram_3001e40`.  The ROM re-reads it between the two
 *    bitfield stores; non-volatile gcc reads once and shifts the same value
 *    twice.  NOT a fakematch dodge: `(iw & 4) >> 1` and `>> 2` are a one-pixel
 *    jitter off bit 2, i.e. a counter an interrupt writes.
 * 2. `unsigned int`, not `int`: the ROM shifts with `lsr`, not `asr`.
 * 3. `int Func_80bb7c0(...)` with `return WaitFrames(1);`.  `pop {r1} / bx r1`.
 * 4. `+ 0xfffc` / `+ 0xf8`, `- 4` / `- 8` and the cast forms ALL compile
 *    identically, so the spelling is inert -- but read the POOL PLACEMENT: 0x3ff,
 *    ~0x3ff, 0x1ff, ~0x1ff sit in the MID-function pool (HImode, 60-byte range)
 *    and 0xfffc in the END pool (SImode, 1020), which is what says the add is
 *    SImode and only the mask is narrow.
 * 5. `int pos[2]` is the memory home: a local ARRAY always has one, and its two
 *    `str`s, the `ldr [sp,#4]` of y each iteration and the 0x14 frame are all
 *    the ROM's.  `unsigned short x0` as the parameter is NOT (120 encodings).
 * 6. A NAMED `void *reg` LOCAL FOR 0x400004a IS WRONG -- the bare literal is
 *    hoisted into r7 as the ROM has it; the named local reloads the pool twice.
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
	unsigned int xx;

	gfx = _Func_8021bc8(0);
	pos[0] = x0;
	pos[1] = y0;
	while (_Func_8017364() == 0)
		WaitFrames(1);
	slot = AllocUploadSpriteGFX(0x80);
	xx = (unsigned short)pos[0];
	while (1) {
		Func_80039fc((void *)0x400004a, 4);
		bld = 0x10;
		Func_800393c((void *)0x400004a, bld);
		*(volatile unsigned short *)0x4000052 = bld;
		e.w[1] = 0x80 << 23;
		e.w[2] = 0;
		e.f.tile = UploadSprite2(slot, gfx);
		e.f.x = ((iwram_3001e40 & 4) >> 1) + xx + 0xfffc;
		e.f.f4 = pos[1] - ((iwram_3001e40 & 4) >> 2) + 0xf8;
		Func_8003dec(&e, 0xf0);
		if ((gKeyPress & 0x303) != 0)
			break;
		WaitFrames(1);
	}
	Func_8003f3c(slot);
	return WaitFrames(1);
}
