/* Func_808ddb8 -- 0x0808ddb8, from goldensun/asm/rom_8a000/rom_8d9a4_a_a.s.
 *
 * A key-value lookup over the -1 terminated halfword pair table at .L9e686:
 * walk the pairs, and on a key match return the value beside it, otherwise the
 * default 0x10.
 *
 * Preserves the original ROM layout when slotted between
 * asm/rom_8a000/rom_8d9a4_a_a_a.o and asm/rom_8a000/rom_8d9a4_a_a_c.o in
 * goldensun/stage1.ld.
 *
 * THE LEVER IS THE MIRROR OF BATCH 178'S, and that is worth recording. There
 * the fix was to REMOVE a name so a second read would appear; here the fix is
 * to ADD one so a second read does not.
 *
 * The ROM loads each entry once and uses it twice -- `ldrsh r3, [r2, r4]` for
 * the sentinel test and then `cmp r0, r3` for the key. Written with two reads
 * through the pointer (`while (*p != -1) { p++; if (key == p[-1]) ... }`) gcc
 * cannot prove the second is the same object at the same address, and RELOADS
 * it through a recomputed address: `sub r3, r2, #0x2 / mov r4, #0x0 /
 * ldrsh r3, [r3, r4]`, three extra instructions and 29 lines against 26.
 * Reading the entry once into a named local and testing that local twice
 * matches on the first screen.
 *
 * > THE DIAGNOSTIC IS WHICH WAY THE EXTRA INSTRUCTION POINTS. An extra COPY in
 * > the ROM means the source read twice and we named it -- remove the name. An
 * > extra LOAD in OURS means the source read once and we did not -- add one.
 *
 * The register-offset `ldrsh` needs nothing written for it: Thumb has no
 * immediate-offset form of the signed halfword load, so `*p` on a `short *`
 * always emits `mov rN, #0` beside it, in the ROM as well.
 *
 * MEASURED (rom 26 lines):
 *   two reads through the pointer, `p[-1]` for the key   29 lines, 11 differing
 *   one read into a named local, tested twice            26, MATCH
 */
extern short L9e686[] __asm__(".L9e686");

int Func_808ddb8(int key)
{
	short *p;
	int v;
	int e;

	p = L9e686;
	v = 0x10;
	for (;;) {
		e = *p;
		if (e == -1)
			break;
		p++;
		if (key == e) {
			v = *p;
			break;
		}
		p++;
	}
	return v;
}
