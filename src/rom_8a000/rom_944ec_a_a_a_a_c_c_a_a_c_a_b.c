/* Task_Thunder -- 0x080949a8.  EXACT.
 * ref: asm/rom_8a000/rom_944ec_a_a_a_a_c_c_a_a_c_a.s  (2 functions: Task_Thunder
 *      0x080949a8, StartRain 0x08094ac8).  NOTE: the retired park's header said
 *      "7 functions"; the file holds TWO.  grep -c thumb_func_start = 2.
 *
 * objcmp:
 *   OK Task_Thunder -- 288 bytes, 117 encodings and 21 relocations identical
 * 117 encodings / 288 bytes.  PINS: 0.  No shims, no volatile, no .equ, no
 * per-file flags, no device.
 *
 * SPLIT SHAPE.  Task_Thunder is the FIRST of the two, so split_s.py puts it in
 * the _b piece and StartRain in _c (the _a piece is empty and not written):
 *   `python3 tools/split_s.py --dry-run asm/rom_8a000/rom_944ec_a_a_a_a_c_c_a_a_c_a.s \
 *        Task_Thunder`
 *     [dry-run] would write asm/rom_8a000/rom_944ec_a_a_a_a_c_c_a_a_c_a_b.s  (1 function(s), 124 lines)
 *     [dry-run] would write asm/rom_8a000/rom_944ec_a_a_a_a_c_c_a_a_c_a_c.s  (1 function(s), 107 lines)
 *     [dry-run] would REMOVE asm/rom_8a000/rom_944ec_a_a_a_a_c_c_a_a_c_a.s
 *     [dry-run] would rewrite stage1.ld
 *   `python3 tools/datacheck.py asm/rom_8a000/rom_944ec_a_a_a_a_c_c_a_a_c_a.s` prints
 *   NOTHING: the only data in the file is the switch jump table, which gcc emits
 *   itself, and there are no `.global` data exports.  exports: [].
 *
 * >>> COORDINATOR: this split MOVES StartRain to
 * >>> asm/rom_8a000/rom_944ec_a_a_a_a_c_c_a_a_c_a_c.s, so
 * >>> src/non_matching/rom_8a000/StartRain.c's `Verify with:` recipe must be
 * >>> REPOINTED at the new path.  Its current recipe names _c_a.s, which this
 * >>> landing deletes.  The landed sibling
 * >>> src/rom_8a000/rom_944ec_a_a_a_a_c_c_a_a_c_b.c (Task_Snow) already warned
 * >>> that these two parks' headers need repointing after each split; this is
 * >>> the second time round.  A park whose recipe names a deleted .s is the
 * >>> Task_ScreenWindowTransition failure mode in this same bank.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_8a000/rom_944ec_a_a_a_a_c_c_a_a_c_a_b.c \
 *     asm/rom_8a000/rom_944ec_a_a_a_a_c_c_a_a_c_a_b.s --func Task_Thunder
 *   (BEFORE the split, substitute the pre-split reference
 *    asm/rom_8a000/rom_944ec_a_a_a_a_c_c_a_a_c_a.s -- that is how it was measured.)
 *
 * ================ WHAT THIS FUNCTION DOES ================
 *
 * The thunder weather task.  Counts a HImode frame timer down from 0x80 at the
 * map buffer's +0x1f80, and on timer values 0, 5, 10 and 1, 6, 11 drives two
 * different lightning-flash blits, re-seeding the timer on 0 with
 * `Random()*400/65536 - Random()*100/65536 + 0x96` frames and playing one of two
 * thunder sounds depending on the flag at +0x1f84.  Note the DELIBERATE
 * FALLTHROUGH from `case 0` into `case 5`/`case 10`.
 *
 * ================ THE RESIDUE, AND WHAT CLOSED IT ================
 *
 * The park reached 19 of 117 with the LENGTH ALREADY IDENTICAL and indices
 * 0-94 exact; the whole residue was the two arms' shared store tail.  The ROM
 * keeps the second address constant INSIDE the merged tail, so tail
 * cross-jumping (.25.jump2, which runs after .23.sched2) merges five insns:
 *
 *   rom  arm-1 tail   ldr r3, =0x2a01 / add r2, r7, r3 / mov r3, #0xc / b L7
 *   rom  arm-2 tail   ldr r1, =0x2a01 / mov r3, #1 / add r2, r7, r1
 *   rom  merged tail  L7: strb r3,[r2] / ldr r2,=0x2a02 / mov r1,#0 / add r3,r7,r2 / strb r1,[r3]
 *
 * Ours hoisted `ldr r1, =0x2a02` into BOTH arms, so only four insns merged and
 * the second `Func_8091200` relocation landed at 0xe8 against the reference's
 * 0xe6.  Note the ROM reloads the 0x2a02 constant INTO r2 -- the register the
 * first `strb` reads -- which is itself the output dependence that keeps it
 * below the store.  Ours put it in r1, a register nothing else wanted, leaving
 * sched2 free to hoist it.
 *
 * WHAT CLOSED IT: writing the two byte stores as MEMBER ACCESSES OF AN
 * AGGREGATE rather than as plain pointer indexing.  `((struct Buf *)y)->f1`
 * instead of `y[0x2a01]`.  One edit, 19 -> 0.
 *
 * ================ WHY, AND A CORRECTION TO THIS BANK'S LEVER 5 ================
 *
 * This is the bank's star lever in substance -- src/rom_8a000/rom_9a44c_c_c_a_a.c
 * (Field_Whirlwind) lever 5 -- but TWO PARTS OF ITS STATED MECHANISM DO NOT
 * HOLD HERE, and both are the kind of propagated claim the brief template warns
 * about.  Reported as measurements, with the evidence attached:
 *
 * (a) IT IS NOT ALIAS SET 0, AND IT IS NOT THE UNION.  Field_Whirlwind records
 *     "Only the union is alias set 0".  For THESE stores the compiler source
 *     says the baseline is ALREADY alias set 0: `lang_get_alias_set`
 *     (c-common.c:3348-3351) returns 0 for ANY reference whose type is an
 *     INTEGER_TYPE of char precision -- "If this is a char *, the ANSI C
 *     standard says it can alias anything" -- and `unsigned char` is exactly
 *     that.  So `y[0x2a01]`, the union form and the struct form ALL have alias
 *     set 0, and the alias set cannot be what distinguishes them.  MEASURED
 *     ACCORDINGLY: a plain `struct` member access is EXACT (0), identical to
 *     the union.  A struct member access is NOT alias set 0 by the union rule
 *     at c-common.c:3340-3345, and it works anyway.
 *
 * (b) IT IS NOT DIRECTIONAL HERE.  The brief carried Field_Whirlwind's finding
 *     that "typing one of two stores works, typing both cancels".  Measured on
 *     this function: typing BOTH stores is ALSO EXACT (0).  The direction that
 *     matters is WHICH store -- see the inert list.
 *
 * WHAT THE MEASUREMENTS DO PIN is a boundary nobody had drawn: the aggregate
 * must have AT LEAST TWO MEMBERS.  Exhaustively, on this function:
 *
 *   construct on the first store                                 objcmp
 *   struct { u8 pad[0x2a01]; u8 f1; u8 f2; }  at base y            0   <-- this file
 *   union  { u8 bb; int ii; }                                      0
 *   union  { u8 bb; int ii; } on BOTH stores                       0
 *   union  { u8 bb; u8 cc; }            (size 1, align 1)          0
 *   union  { u8 bb; short hh; }         (size 2, align 2)          0
 *   union  { u8 bb; int ii; } __attribute__((packed)) (align 1)    0
 *   struct { u8 bb; int ii; }                                      0
 *   struct { u8 bb; u8 pad[3]; }        (size 4, align 1)          0
 *   union  { u8 bb; }                   ONE member                19  INERT
 *   struct { u8 bb; }                   ONE member                19  INERT
 *   *(char *)(y + 0x2a01)               plain cast                19  INERT
 *   nothing (the park's y[0x2a01])                                 19  BASE
 *
 * So size, alignment, packedness and struct-vs-union are ALL inert; member
 * count 1 vs 2 is the whole discriminator.  I did NOT identify the pass.
 * `MEM_IN_STRUCT_P` is set for a COMPONENT_REF at expr.c:8845 and read by
 * `fixed_scalar_and_varying_struct_p` (alias.c:1529-1540), which is the
 * obvious suspect, but that flag is set for a single-member aggregate too and
 * so cannot by itself explain the 1-vs-2 boundary.  Dump diffing could not
 * settle it: the extra member shifts every insn UID and pseudo number in the
 * TU, so .00.rtl onward differs by renumbering alone and no pass can be
 * fingered without an alignment pass over the dumps.  STATED AS A MEASUREMENT,
 * NOT A MECHANISM -- the next reader should not build on a cause here.
 *
 * ================ crossfire: BASE SHOWS A *TRUE* MEM FLAG ================
 *
 * The brief asked to be told if BASE ever shows a flag.  It does here, and it
 * is NOT a tool defect -- it is a true positive the park should have acted on.
 * `crossfire.py --depth 1` on the park's body reports
 *     BASE figure 19  (ref 117 / ours 117)   flags: RELOC MEM
 *     reference memory profile: ldr=12 ldrh=1 ldrsh=3 strb=2 strh=3
 * and counting the compiled streams confirms it: the park's body has
 * ldr=13 / mov=20, the reference and this file have ldr=12 / mov=21.  The
 * duplicated `ldr r1, =0x2a02` is a REAL extra memory-class opcode, exactly
 * cancelled in the TOTAL instruction count by the `mov r3, #1` the park's arm-2
 * does not need -- which is why 117 tied 117 and nothing noticed.
 *
 * >>> So the MEM screen was pointing straight at this residue from the start.
 * >>> A MEM flag at a TIED instruction count is not only a wrong-program
 * >>> warning; it localises the residue to an opcode class.
 *
 * The RELOC flag on BASE is expected and benign: the park's relocations are the
 * same symbols at a shifted offset, which is the consequence of the residue.
 *
 * ================ MEASURED AND INERT / WORSE ================
 *
 * - The aggregate on the SECOND store only (`y[0x2a01]` plain, 0x2a02 typed):
 *   19, exactly the base.  THIS IS THE DIRECTION THAT MATTERS: the dependence
 *   has to be created at the store whose register the later constant reuses.
 * - The aggregate on the first store in ONE ARM only: 51 of 117 and 123
 *   encodings -- it destroys the cross-jump outright.  Both arms or neither.
 * - A two-byte `struct Pair` based at `y + 0x2a01` with fields at +0 and +1:
 *   46 of 117, 118 encodings.  gcc computes ONE base and uses `strb rX,[rY,#1]`,
 *   where the ROM computes both addresses from `y`.  So the aggregate must be
 *   based at `y`, which is also the honest reading of the data.
 * - A plain `*(char *)` cast on the first store: 19 (see the table).
 *
 * INHERITED FROM THE PARK, NOT RE-MEASURED (its observations were sound; only
 * its verdict on the tail was open).  These three are load-bearing:
 *   1. `int u = *p` on an `unsigned short *`, THEN `t = (short)u`, is the only
 *      spelling giving `ldrh / sub #1 / lsl / asr / strh`.  `short *p` with
 *      `t = *p` gives `ldrsh` (91); `unsigned short u` gives
 *      `ldr =0xffff / lsl / lsr / add / asr` (99); `unsigned int u` gives
 *      `add rX, 0xffff` (92).
 *   2. Hoisting the value out of the halfword store -- `sum = ...; w = ...;
 *      *w = sum;` -- schedules the address ahead of the multiply chain.  31->23.
 *   3. `c = 0x80; *p = c;` is required; bare `*p = 0x80` pools the constant
 *      (the HImode-store class, same as StartRain's `c1`).
 * Also inherited, and still true: `synth_mult` is NOT capped at three
 * operations, so plain `* 400` and `* 100` are correct and need no composite
 * spelling (recorded in docs/elevation.md by this function).
 *
 * THE PARK'S ONE WRONG CALL, for the record: it reported the `q1`/`q2` variant
 * as "16 but WRONG -- gcc notices `0x2a01 & 0xff == 1` and stores the LOW BYTE
 * OF THE ADDRESS CONSTANT instead of `mov r3, #1`".  Storing the low byte of a
 * register that holds 0x2a01 stores 1, which is the right value; it is a legal
 * optimisation, not a wrong program.  The park's own base body relies on it --
 * arm-2 of the 19-of-117 baseline is `ldr r3,=0x2a01 / add r2,r7,r3 /
 * strb r3,[r2]`.  Worth correcting because "wrong program" is the one verdict
 * that stops anyone re-testing a variant.
 */
#include "dma.h"

struct Buf {
    unsigned char pad[0x2a01];
    unsigned char f1;
    unsigned char f2;
};

extern unsigned char *iwram_3001ec8;
extern int _GetFlag(int id);
extern unsigned int Random(void);
extern void _PlaySound(int id);
extern void Func_8091200(void *p, int n);

void Task_Thunder(void)
{
	unsigned char *b;
	unsigned char *y;
	unsigned short *p;
	int u;
	int t;
	unsigned int ra;
	unsigned int rb;
	int c;
	int sum;
	short *w;

	b = iwram_3001ec8;
	p = (unsigned short *)(b + (0xfc << 5));
	y = (&iwram_3001ec8)[2];
	if (*(short *)p < 0)
		return;
	if (_GetFlag(0xb3 << 1)) {
		c = 0x80;
		*p = c;
	}
	u = *p;
	*p = u - 1;
	t = (short)u;
	switch (t) {
	case 0:
		if (*(short *)(b + 0x1f82) != 0) {
			ra = Random();
			rb = Random();
			sum = ((ra * 400) >> 16) - ((rb * 100) >> 16) + 0x96;
			w = (short *)(b + (0xfc << 5));
			*w = sum;
			if (*(short *)(b + 0x1f84) != 0)
				_PlaySound(0xac);
			else
				_PlaySound(0xab);
		}
	case 5:
	case 10:
		Func_8091200(b, 1);
		DMA3_COPY(b + (0xa8 << 5), y + (0xc4 << 5), 0xa80);
		((struct Buf *)y)->f1 = 0xc;
		((struct Buf *)y)->f2 = 0;
		break;
	case 1:
	case 6:
	case 11:
		Func_8091200(b + (0xa8 << 4), 1);
		((struct Buf *)y)->f1 = 1;
		((struct Buf *)y)->f2 = 0;
		break;
	}
}
