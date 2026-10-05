/* Func_8020b64 (0x08020b64) -- LANDS EXACT.  Batch 327 brief C, 50 of 57 -> 0.
 *
 *   116 bytes, 57 encodings, 1 relocation, all identical.  Reference 56
 *   instructions; ours 56.  --whole green against the post-split reference.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/rom_15000/rom_20198_c_c_c_a_a_a_a_c_b.c asm/rom_15000/rom_20198_c_c_c_a_a_a_a_c_b.s --whole
 *
 * Split: asm/rom_15000/rom_20198_c_c_c_a_a_a_a_c.s holds four functions and
 * Func_8020b64 is the first, so split_s.py writes _b.s (this function, 66
 * lines) and _c.s (the other three, 910 lines) and removes the original.
 * datacheck.py on the original is silent -- no data section.  Pins 0, devices 0,
 * no flag group.
 *
 * ===================== WHAT THE WHOLE COPY-COLLAPSE FAMILY GOT WRONG HERE ====
 *
 * Four parks and six batches read this function's residue as a COPY CHAIN
 * COLLAPSED BY cse, and chased it through names, types, scopes, separator
 * statements, read counts, role swaps, a volatile instrument and four flags --
 * twenty-plus whole bodies, every one exactly flat at 50.  The diagnosis was
 * reproduced every time and it was still the wrong frame, because every one of
 * those bodies KEPT THE `while` LOOP.
 *
 *   ROM    strb r2,[r5] / add r1,#1 / ldrb r3,[r1] / mov r2,r3 / mov r3,r2 / ...
 *   park   strb r2,[r5] / add r1,#1 / ldrb r2,[r1] /    ---    / mov r3,r2 / ...
 *
 * ** THE ROM'S THREE INSNS ARE NOT THREE C NAMES.  THEY ARE THREE SEPARATE C
 * EXPRESSIONS THAT EACH READ `*src`: **
 *
 *      if (*src != 0)        -> the entry test     (prologue ldrb r2 / mov r3,r2)
 *      buf[n] = *src         -> the carried value  (mov r2,r3)
 *      while (*src != 0)     -> the bottom test    (mov r3,r2)
 *
 * With a `while` loop there are only TWO such expressions in the source, and
 * `expand_end_loop` (stmt.c) manufactures the third by ROTATING the loop: it
 * moves the original top test to the bottom and emits a COPY of it at the entry,
 * marking the copies' registers REG_LOOP_TEST_P (printed `/s` in the dumps).
 * The rotation's two QI pseudos then live and die inside the loop body's own cse
 * block, so `make_regs_eqv` (cse.c:1002) never promotes the second one, cse
 * substitutes the first for it, and the copy becomes a dead store.  No name,
 * type, scope, separator or liveness edit inside the loop can reach that,
 * because the pseudos are not the ones the source named.
 *
 * ** WRITING THE ROTATION BY HAND -- `if (...) do { ... } while (...)` -- GIVES
 * THREE INDEPENDENT EXPRESSIONS AND NOTHING COLLAPSES. **  That is the whole
 * edit, and it takes the function from 50 of 57 to byte-identical in one step.
 *
 * COROLLARY FOR THE FAMILY, worth more than this landing: when a residue is ONE
 * MISSING COPY at the bottom of a `while` loop, the first question is not which
 * pseudo cse promoted -- it is WHETHER THE SOURCE OR THE COMPILER PERFORMED THE
 * LOOP ROTATION.  `/s` on a chain member in `.02.jump` is the tell: those
 * registers are stmt.c's duplicate, not the source's.
 *
 * MEASURED HERE (all at ref 56 insns unless noted; reference memory profile
 * ldrb=2 strb=7 matched by every row, so no row is a figure bought by doing
 * less work than the ROM):
 *   if/do-while, buf[n]=*src; src++; n++        ** 0, EXACT **
 *   while (*src != 0) { buf[n]=*src; src++; n++ }   46  (55 insns, 1 short)
 *   while (*src != 0) { buf[n]=*src; n++; src++ }   46  (55)
 *   while (*src)      { buf[n]=*src; src++; n++ }   46  (55)
 *   while (*src != 0) { buf[n++]=*src++ }           46  (55)
 *   while (*src != 0) { buf[n]=*src++; n++ }        46  (55)
 *   while (src[0]!=0) { buf[n]=src[0]; src++; n++ } 46  (55)
 *   for (n=0; *src!=0; n++) { buf[n]=*src; src++ }  46  (55)
 *   c=*src; while (c!=0){buf[n]=c; src++; c=*src; n++}      52  (53)
 *   c=*src; while (c!=0){buf[n]=c; src++; n++; c=*src}      52  (53)
 *   while ((c=*src)!=0){buf[n]=c; src++; n++}               52  (53)
 *   while (*src!=0){ uchar x=*src; buf[n]=x; src++; n++}    55  (54)
 *   the shipped park body (c + t, two names)                50  (55)
 *   the park's body E (block uchar x + store separator)     53  (55)
 *
 * > ** A LOWER FIGURE IS NOT A BETTER STARTING POINT, AND HERE THE REVERSE OF
 * > THE USUAL TRAP APPLIES: the park's 50 was a LOCAL OPTIMUM OF THE WRONG
 * > FAMILY.**  The naive three-statement `while` body, which the park never
 * > measured in twenty tries because it carries no named char at all, reads 46
 * > and is EXACT in all 56 instructions but one -- including the duplicated
 * > `mov r0,sp`, the `add r2,r4,r0` pad setup and the `sub r4,r3,r4` trip count.
 * > One instruction from zero, with the park reporting four blocker sites.
 *
 * Builds a small display string: copy a null-terminated source into a stack
 * buffer, append two control bytes, pad with 0x5f out to offset 7, append two
 * more and a terminator, then hand it to the text layer.
 */
void Func_801e858(unsigned char *dest, int b, int c, int d);

void Func_8020b64(int a, unsigned char *src)
{
	unsigned char buf[0x14];
	int n;

	n = 0;
	if (*src != 0) {
		do {
			buf[n] = *src;
			src++;
			n++;
		} while (*src != 0);
	}
	buf[n] = 8;
	n++;
	buf[n] = 2;
	n++;
	while (n < 7) {
		buf[n] = 0x5f;
		n++;
	}
	buf[n] = 8;
	n++;
	buf[n] = 0xf;
	n++;
	buf[n] = 0;
	Func_801e858(buf, a, 0, -2);
}
