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
 *
 * ================================================================
 * BATCH 348 BRIEF C -- THE HEADER'S "pool layout ... exact" IS REFUTED, AND
 * THE WHOLE 33 IS ONE DEFECT: TWO CLEAR MASKS ARE SImode POOL FIXES WHERE THE
 * ROM'S ARE HImode
 * ================================================================
 * RE-DERIVED, ALL THREE LENGTH DIMENSIONS, production flags:
 *     stream encodings  153 / 153
 *     16-bit insns      126 / 126      (objcmp.insn_pool_counts, pads excluded)
 *     32-bit entries    25 / 25        (pool words + every `bl`)
 *     byte size         356 / 356
 *     objcmp            33 differing, first at index 20: ref 4a22 ours 4a21
 *                       *** PLUS AN "XX RELOCATIONS differ" LINE ***
 *     aligncmp          121 aligned-equal (79.1%), 52 in 12 hunks
 *
 * CORRECTION TO THIS HEADER.  It says "Prologue, wait loop, frame, pool layout,
 * the key tests ... are exact" and quotes objcmp without a relocation line.
 * objcmp prints one today: `iwram_3001e40`, `Data_c3734` and `iwram_3001ee4`
 * sit at 0xb8 / 0xbc / 0xc0 in the ROM and at 0xb4 / 0xb8 / 0xbc in ours.  The
 * three length dimensions all agree, so 33 is still a distance -- but the pool
 * layout is the DEFECT, not a thing that is already right, and the header's
 * "THE BLOCKER: register choice in the bitfield-insert block" is the SYMPTOM.
 *
 * ---------------- WHAT THE 33 IS, ITEM BY ITEM ----------------
 * From an LCS-aligned side-by-side (`scratch_elev/b348/C/cdump.py`):
 *   * five `ldr rX,[pc,#N]` encodings, stream indices 20, 25, 28, 30, 43 --
 *     every pool load of a SYMBOL or IO address, each off by exactly one word;
 *   * the three relocation offsets above, -4 against the ROM;
 *   * two pool words transposed: ref carries 0xfffffc00 at stream 79 and
 *     0xfffffe00 at 81, between 0x3ff and 0x1ff; ours carries both at 87/88,
 *     at the very END of the pool;
 *   * two instructions on the wrong side of the minipool barrier -- the ROM's
 *     `ldr r0,[r3] / lsl r0,#12` sit AFTER the `b` over the pool (ref 87-88),
 *     ours BEFORE it (ours 76-77), which is why our pool starts at 0xac and
 *     the ROM's at 0xa8;
 *   * the rest is the r1/r2/r3 rotation this header already describes.
 *
 * ---------------- THE MECHANISM, WITH LINES ----------------
 * THE MINIPOOL IS SORTED BY `fix->address + pool_range`, NOT BY STREAM ORDER.
 * arm.c:4681-4682 states the invariant on the list outright -- entries are held
 * "of increasing max_address" -- and max_address is `fix->address + fix->forwards`
 * (arm.c:4826).  `forwards` is the pattern's pool_range attribute:
 *     *thumb_movsi_insn   pool_range 1020    arm.md:3294
 *     *thumb_movhi_insn   pool_range   64    arm.md:4353  (alternative 1, "mn")
 * gcc prints the classification itself, in the `.26.mach` dump:
 *     ;; HImode fixup for i13; addr 0, range (0,64): 0x3ff
 *     ;; SImode fixup for i24; addr 10, range (0,1020): 0xfffffc00
 *     ;; Emitting minipool after insn 45; address 62
 * So a HImode fix 950-odd bytes LATER in the stream still sorts ahead of an
 * SImode fix, which is why mask constants precede symbol addresses in BOTH
 * bodies.  The two bodies differ only in HOW MANY masks are HImode:
 *     ROM   0x3ff(HI) 0xfffffc00(HI) 0x1ff(HI) 0xfffffe00(HI)
 *           iwram_3001e40 Data_c3734 iwram_3001ee4 0x400004a 0x4000052
 *     ours  0x3ff(HI) 0x1ff(HI)
 *           iwram_3001e40 Data_c3734 iwram_3001ee4 0x400004a 0x4000052
 *           0xfffffc00(SI) 0xfffffe00(SI)
 * Both orders are exactly what `addr + range` predicts.  ONE defect: our two
 * bitfield CLEAR masks are SImode fixes.  Everything else in the 33 follows.
 *
 * WHERE THE TWO MASKS PART COMPANY, INSN BY INSN.  store_fixed_bit_field issues
 * both ANDs in the field's get_best_mode result (HImode here) -- the value mask
 * at expmed.c:756, the clear mask at expmed.c:770 -- and expand_binop force_regs
 * each expensive constant IN HImode at optabs.c:636-640 before widening it to
 * SImode with a paradoxical subreg (widen_operand: `gen_rtx_SUBREG (mode,
 * force_reg (GET_MODE (op), op), 0)`).  So at .00.rtl BOTH masks live in HImode
 * pseudos.  In WaitTextPrompt.c.00.rtl:
 *     (insn 143 (set (reg:HI 65) (const_int -1024)))            ; HImode home
 *     (insn 145 (set (reg:SI 67) (subreg:SI (reg:HI 65) 0)))    ; widening copy
 *     (insn 146 (set (reg:SI 66) (and:SI (subreg:SI (reg:HI 64) 0) (reg:SI 67))))
 * .03.cse and .09.cse2 leave that untouched.  COMBINE then picks one of two
 * mutually exclusive moves and the ROM's build picked the other one:
 *   ours  (.13.combine): insn 143 becomes NOTE_INSN_DELETED and insn 145 becomes
 *         (set (reg:SI 67) (const_int -1024)) -- *thumb_movsi_insn, range 1020.
 *   ROM-shaped: the COPY dies instead and the AND becomes
 *         (and:SI (subreg:SI (reg:HI f) 0) (subreg:SI (reg:HI m) 0)),
 *         leaving the HImode constant load alive -> `ldrh`, range 64.
 *
 * *** THE ROM'S SHAPE EXISTS, BYTE-MATCHING, IN THIS TREE. ***
 * asm/rom_8a000/rom_944ec_a_a_a_a_c_c_a_c_b.s is a GENERATED, byte-matching .s
 * (its source is src/rom_8a000/rom_944ec_a_a_a_a_c_c_a_c_b.c, Task_Earthquake)
 * and it emits this exact block with BOTH masks as HImode pool fixes, and -- as
 * a direct consequence -- the field load scheduled after both mask loads, which
 * is the ROM's order:
 *     ldrh r3, .L18      @ 1023        ldrh r3, .L18+8    @ 511
 *     ldrh r2, .L18+4    @ -1024       mov  r1, fp
 *     and  r1, r1, r3                  and  r1, r1, r3
 *     ldrh r3, [r7, #8]                ldrh r2, .L18+12   @ -512
 *     and  r3, r3, r2                  ldrh r3, [r7, #6]
 *     orr  r3, r3, r1                  and  r3, r3, r2
 *     strh r3, [r7, #8]                orr  r3, r3, r1
 *                                      strh r3, [r7, #6]
 *     .L18: .word 1023 / -1024 / 511 / -512 / ...
 * Found by the shared brief's corpus rule: a scan of all 4,578 generated `.s`
 * in asm/ for an `ldrh rX,.LN+k` whose pool word is NEGATIVE returns 30 hits,
 * and that file is the only one holding BOTH -1024 and -512.  Its park header is
 * worth reading in full before the next attempt -- its lever 4 is a
 * get_alias_set escape on this same OAM-entry struct family.
 *
 * ---------------- WHAT IS NOT THE TRIGGER (measured) ----------------
 * THE DECLARATION IS NOT THE TRIGGER.  Six shapes, all SImode `ldr`
 * (`scratch_elev/b348/C/probe/pb1.c`, `pb2.c`): the twin's `struct Drop`
 * verbatim; this file's `union Ent`; the same struct WITHOUT the union wrapper;
 * the same with the twin's 10/2/4 and 9/5/2 halfword splits instead of 10/6 and
 * 9/7; `unsigned int` bitfields; and a single insert on its own.  A hand-rolled
 * HImode insert (`*h = (*h & 0xfc00) | (v & 0x3ff)`) does not pool the clear
 * mask at all -- gcc builds 0xfc00 with `mov #252 / lsl #8`, two instructions.
 *
 * EIGHT SOURCE VARIANTS ON THE INSTALLED BODY (`scratch_elev/b348/C/v_wtp`):
 *   the x insert moved BEFORE the tile insert                 47 of 153 (WORSE)
 *   the inserted value pre-masked (`UploadSprite2(..) & 0x3ff`) 100 (WORSE)
 *   both inserts moved below the sin() statement        50 at 127 insns (LONG)
 *   the call result in its own `int` local               BIT-IDENTICAL
 *   the twin's halfword splits                           BIT-IDENTICAL
 *   the bitfields reached through a dedicated `struct Oam *`   BIT-IDENTICAL
 *   the x value in its own local                         BIT-IDENTICAL
 *   `unsigned short` mask locals beside the inserts      BIT-IDENTICAL
 * (Those five bit-identical rows are NON-MEASUREMENTS, not negatives -- see the
 * harness lesson below.)
 *
 * ---------------- THE OPEN QUESTION, WITH ITS EVIDENCE ----------------
 * The ROM's shape IS reachable by gcc-2.96 on this exact construct; it needs the
 * clear-mask constant to stay in its HImode pseudo through combine; and it is
 * NOT decided by the type declaration (six shapes measured).  The two combine
 * inputs are isomorphic rtx-for-rtx between this file and the landed twin -- same
 * insn kinds, same modes, same order, same REG_EQUAL notes at .03.cse -- so the
 * divergence is a combine-order or LOG_LINK property of the surrounding
 * function, and the next probe is a side-by-side of the two `.13.combine` dumps
 * with -fdump-rtl-combine-level detail, or simply instrumenting which
 * try_combine attempt fires first.  LOG_LINKS are built by flow.c with
 * PROP_LOG_LINKS and only within one basic block, to the NEXT use, which is the
 * first thing to check: if the HImode constant's def and the widening copy end
 * up in different basic blocks, no link exists and combine must take the other
 * branch -- which is the ROM's.
 *
 * STILL TRUE FROM THE LADDER ABOVE: the goto loop, the `pe` pointer, the
 * non-volatile `ctr`, `int WaitTextPrompt(void)` with `return WaitFrames(1);`,
 * and the SET_IO barrier.  Do not disturb any of them; the 126/126 instruction
 * count and the 356-byte size are all theirs.
 *
 * HARNESS LESSON, applies to every park: detect bit-identical variants by
 * hashing the OBJECT's encoding list and relocation list, NOT the `.s` text.
 * gcc writes `.file "<name>.c"` at the top, so every variant kept in its own
 * file hashes differently and five non-measurements here looked like five clean
 * negatives.  `scratch_elev/b348/C/csweep.py` does it the right way.
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
