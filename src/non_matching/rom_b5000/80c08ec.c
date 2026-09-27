/* AnimTransitionIn -- NON-MATCHING, 119 encodings of 125, AND THREE INSTRUCTIONS SHORT:
 * ref 125 encodings / 312 bytes, ours 129 / 320.  106 instructions against 103.  Do not read
 * the 119 as a distance -- the stream length disagrees, so most of it measures the shift.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_b5000/80c08ec.c \
 *     asm/rom_b5000/rom_bffb8_a_c_c.s --func AnimTransitionIn
 *
 * THIS PARK CARRIES A SHIM AND IT IS NAMED: `__asm__(".equ _SIZE_80b5138, 0x230");`.
 * A park is self-verifying and legitimately keeps its shims; ONLY A LANDED FILE MUST NOT.
 * Delete it if this ever lands, and add the real entry instead.
 *
 * A NEW size.sym ENTRY IS REPORTED AND WITHHELD: `_SIZE_80b5138 = 0x230;`.
 *
 * IN-FUNCTION CONTROL, which is strong: the ROM has `ldr r5,=0x230` and builds the DMA
 * control word AT RUNTIME (`mov r2,#0x84 / lsr r5,#2 / lsl r2,#24 / orr r2,r5`).  With the
 * plain literal, 0x230 is `0x8c << 2` so gcc BUILDS it and then folds the whole control word
 * to a pooled `=0x8400008c` -- 100 instructions, structurally wrong.  With the symbol that
 * window is BYTE-EXACT (106).  The arithmetic is verified: Func_80b5138 @ 0x080b5138 to
 * Func_80b5368 @ 0x080b5368 is exactly 0x230, and it is the ARM routine this function
 * DMA-copies into a galloc'd buffer -- size.sym's exact class.
 *
 * IT IS WITHHELD BECAUSE IT DOES NOT COMPLETE THE FUNCTION, which is size.sym's own rule and
 * this tree's line.  Note it is DISTINCT from LoadGS1CreditsBG's long-standing
 * `_SIZE_80f0024 = 0x230` -- same value, different subject -- and it is the same situation
 * as the two 0x7c entries already in the file.
 *
 * BLOCKER, NAMED AND A CATCH-22: `&iwram_3001f00` IS COMMONED INTO A CALLEE-SAVED REGISTER
 * AND THE ROM REMATERIALISES IT at each of three uses.  That seventh long-lived pseudo is
 * ALL THREE missing instructions.
 *
 *   one symbol name       -> the ROM's instructions (`sub r3,#0x8c`, `[r2,#0x14]`) with the
 *                            WRONG register
 *   separate names, or symbols, or __asm__ aliases
 *                         -> gcc folds each offset into its OWN pool word: wrong
 *                            instructions AND wrong relocations
 *
 * THE REFERENCE CARRIES EXACTLY ONE `R_ARM_ABS32` TO iwram_3001f00, which PROVES the first
 * form with the address rematerialised -- so the answer is a way to deny that pseudo a
 * register, not a different spelling of the address.
 *
 * Not a flag: -fno-gcse, -fno-cse-follow-jumps, -fno-rerun-cse-after-loop and
 * -fno-expensive-optimizations are all inert.  Not this function either -- a 12-line minimal
 * case commons it too.
 *
 * WHAT IS ALREADY RIGHT: the three DMA blocks are plain include/dma.h macros, so NO
 * register-asm fakematch is needed here, unlike the two `// fakematch` siblings in this
 * family; and the whole tail is instruction-identical.
 */
/* AnimTransitionIn (0x080c08ec) -- NOT MATCHING.  First of two in
 * asm/rom_b5000/rom_bffb8_a_c_c.s (sibling Func_80c0a24 also attempted, see
 * c0a24_best.c).  103 ROM instructions against 106 here; objcmp: ref 125
 * encodings / 312 bytes, ours 129 / 320.  datacheck reports NO DATA.
 *
 * *** CONTAINS A VERIFICATION SHIM.  THE LINE
 *         __asm__(".equ _SIZE_80b5138, 0x230");
 *     IS A SHIM AND MUST BE DELETED BEFORE LANDING.  It exists only so objcmp
 *     can resolve the symbol locally instead of emitting a relocation.  The
 *     real build input is a NEW size.sym entry:  _SIZE_80b5138 = 0x230;  ***
 *
 * THE size.sym ENTRY IS IDENTIFIED AND VERIFIED, AND IT DOES *NOT* COMPLETE THE
 * FUNCTION -- so by the rule size.sym's own header and
 * src/non_matching/rom_f0000/LoadGS1CreditsBG.c both state ("a build input is
 * worth adding when it COMPLETES a function") it should NOT be added yet.
 *   - IN-FUNCTION CONTROL: the ROM has `ldr r5, =0x230` and then builds the DMA
 *     control word at RUNTIME -- `mov r2,#0x84 / lsr r5,#2 / lsl r2,#24 /
 *     orr r2,r5`.  0x230 is 0x8c << 2, so gcc BUILDS it from a literal and then
 *     folds the whole control word to a single pooled `=0x8400008c`: with the
 *     literal this function is 100 instructions, with the symbol it is 106 and
 *     that four-instruction window becomes byte-exact.  Exactly size.sym's tell.
 *   - ARITHMETIC: Func_80b5138 @ 0x080b5138 (asm/rom_b5000/rom_b5138.s) and the
 *     next routine Func_80b5368 @ 0x080b5368, so the size is the gap, 0x230.
 *     The routine is the ARM decompressor this function DMA-copies into a
 *     galloc'd scratch buffer and calls there -- size.sym's exact class.
 *   - It is a DIFFERENT routine that happens to share 0x230 with
 *     LoadGS1CreditsBG's _SIZE_80f0024 (Func_80f0024), the same situation
 *     size.sym already documents for the two 0x7c entries, so it needs its own
 *     name and must not be merged with that one.
 *
 * NAMED BLOCKER: &iwram_3001f00 IS COMMONED INTO A CALLEE-SAVED REGISTER AND
 * THE ROM REMATERIALISES IT.  The ROM loads the ONE pool word three times --
 * `ldr r2,=iwram_3001f00 / ldr r2,[r2]`, `ldr r3,=iwram_3001f00 / sub r3,#0x8c
 * / ldr r6,[r3]`, `ldr r2,=iwram_3001f00 / ldr r3,[r2,#0x14]` -- into
 * short-lived r2/r3.  gcc keeps the address in one callee-saved register across
 * both intervening calls, which is a SEVENTH long-lived pseudo where the ROM has
 * six, so `b` and `file` swap between r6 and r8, `mode` is pushed to r11, and
 * the prologue grows the `mov r7,r8 / push {r7}` pair the ROM does not have.
 * That accounts for ALL THREE remaining instructions.
 *   - It is not a flag: -fno-gcse, -fno-cse-follow-jumps,
 *     -fno-rerun-cse-after-loop and -fno-expensive-optimizations are all inert.
 *   - It is not this function: a TWELVE-LINE minimal case (three uses of one
 *     extern array's address separated by calls) commons it too, into r6.
 *   - IT IS A CATCH-22, and that is the useful part.  With ONE symbol name gcc
 *     commons the address and then expresses the offsets as `sub r3,#0x8c` /
 *     `[r2,#0x14]` off the surviving register -- the ROM's INSTRUCTIONS, wrong
 *     register.  With separate names or separate symbols (`iwram_3001e74`,
 *     `iwram_3001f14`, or `__asm__("iwram_3001f00")` aliases) gcc instead FOLDS
 *     each offset into its own pool word, giving `ldr r3,POOL / ldr r3,[r3]` --
 *     the right register class, wrong instructions and wrong relocations.  Both
 *     reach 103 instructions; neither reaches the ROM.  The reference object
 *     carries exactly ONE R_ARM_ABS32 to iwram_3001f00, which proves the ROM is
 *     the first form with the address rematerialised, i.e. a pseudo that carries
 *     a constant equivalence and was NOT given a hard register even though r11
 *     was free.  Reaching that needs a seventh call-crossing value gcc does not
 *     have, or a rule about REG_LIVE_LENGTH for constant-equivalent pseudos that
 *     is not in the notebook.
 *
 * WHAT IS RIGHT, and is worth keeping: the three DMA3 blocks are plain
 * include/dma.h macros (DMA3_COPY twice, DMA3_COPY16 once) -- no register-asm
 * fakematch is needed here, unlike the two sibling `// fakematch` files in this
 * family; the indirect call through `*(fp *)(iwram_3001f00 + 0x14)` gives the
 * ROM's `ldr r3,[r2,#0x14] / bl _call_via_r3`; `f2 = Func_80008d4; f2(...)`
 * gives `ldr r3,=Func_80008d4 / bl _call_via_r3`; the whole tail from the third
 * DMA block to `REG_BG1CNT = 0x1f83` is instruction-for-instruction identical,
 * including the bare `REG_BG1CNT = 0x1f83` pooling the halfword constant.
 */
#include "dma.h"

extern unsigned char iwram_3001f00[];
extern unsigned char Func_80b5138[];
extern unsigned char _SIZE_80b5138[];

/* VERIFICATION SHIM ONLY -- the real landing needs `_SIZE_80b5138 = 0x230;` in
   size.sym.  Remove this line when the symbol exists. */
__asm__(".equ _SIZE_80b5138, 0x230");
extern unsigned char *GetFile(int id);
extern void *galloc_iwram(int tag, int size);
extern void gfree(int tag);
extern void UploadBGPalette(void *src, void *dst, int amount, int count);
extern int StartTask(void *fn, int pri);
extern void Func_80c0098(void *p);
extern void Func_80c00d8(void *p);
extern int Func_80008d4(void *dst, int len);
extern void Func_80c0130(void);

void AnimTransitionIn(int mode, int id, int pal)
{
	int *g;
	unsigned char *b;
	unsigned char *file;
	unsigned char *p;
	void *scratch;
	int size;
	int v;
	int (*fn)(void *, void *);
	int (*f2)(void *, int);

	g = *(int **)iwram_3001f00;
	file = GetFile(id);
	b = *(unsigned char **)((char *)iwram_3001f00 - 0x8c);
	size = (int)&_SIZE_80b5138;
	scratch = galloc_iwram(0x31, size);
	DMA3_COPY(Func_80b5138, scratch, size);
	fn = *(int (**)(void *, void *))((char *)iwram_3001f00 + 0x14);
	fn(file + 0x100, (void *)0x6008000);
	gfree(0x31);
	p = b + 0x544;
	DMA3_COPY(file, p, 0x100);
	if (pal >= 0) {
		v = (0x80 << 9) - pal * 1092;
		*(int *)(b + 0x644) = v;
		UploadBGPalette(p, (void *)0x50000c0, v, 0x80);
	}
	DMA3_COPY16((void *)0x5000200, (void *)0x50000a0, 0x40);
	*(unsigned short *)0x50000bc = *(unsigned short *)0x50001e8;
	Func_80c0098((void *)0x6003800);
	Func_80c00d8((void *)0x600f800);
	f2 = (int (*)(void *, int))Func_80008d4;
	f2((void *)0x600ffc0, 0x40);
	if (g[2] == 0)
		StartTask(Func_80c0130, 0x4ff);
	g[2] = mode;
	if (mode == 1)
		REG_BG1CNT = 0x1f83;
}
