/* Func_80aaf58 -- 0x080aaf58, from goldensun/asm/rom_a1000/rom_aa538_c_c_a_c.s.
 *
 * Runs the menu's per-entry evaluator over the visible rows, writing each
 * result into the status byte array at +0xa0, and re-reading the row count on
 * every pass because the evaluator can change it.
 *
 * Preserves the original ROM layout when slotted between
 * asm/rom_a1000/rom_aa538_c_c_a_c_a.o and rom_aa538_c_c_a_c_c.o in
 * goldensun/stage1.ld.
 *
 * TWO INDEPENDENT LEVERS, and the second is new.
 *
 * THE EPILOGUE TELL APPLIED TO THIS FUNCTION'S OWN SIGNATURE. The ROM ends
 * `pop {r1} / bx r1`, avoiding r0, which is gcc-2.96's tell for a
 * value-returning function. Declaring this `int` -- with no `return` statement,
 * because the ROM does not set r0 either -- flips `pop {r0} / bx r0` to the
 * ROM's form. The notebook records this lever for CALLEES; here it is the
 * function's own declaration.
 *
 * A SECOND, GIV-ONLY INDEX VARIABLE RELOCATES THE INDUCTION UPDATES RELATIVE
 * TO A STORE. The residue was five lines of `strb r0, [r7]` landing four slots
 * early. Because `-fcall-used-r4` is in the production flags the loop counter
 * has no callee-saved home and is spilled around the call, and post-reload
 * sched2 cannot disambiguate a `char` store from either the spill slot or the
 * `u8` count load -- so their order is frozen at whatever RTL order was.
 * `-fno-strict-aliasing` changes nothing, which confirms it is not
 * type-based aliasing.
 *
 * The ROM's schedule needs BOTH the counter reload and the count re-read to
 * precede the store in RTL, while the store still uses the pre-increment index.
 * Two induction variables give exactly that: `i` is incremented before the
 * store and survives only in the loop compare, so it stays the biv in r4;
 * `j` is incremented after the store and appears only inside address
 * expressions, so biv elimination deletes `j` itself and keeps its three givs
 * (`+= 0x14`, `+= 2`, `+= 1`) placed after the `strb`, where the ROM has them.
 *
 * MEASURED (rom 44 lines):
 *   plain `for` over `p[0x219]`, void                    44 lines, 7 differing
 *   the same declared `int`                              44, 5
 *   `int`, with a named temp for the call result         44, 5
 *   `int`, `a[0xa0 + i++] = v;`                          44, 5 (byte-identical)
 *   an explicit walking pointer for the store            44, 13
 *   the same with `i++` before the store                 44, 14
 *   all three pointers walked destructively              45, 41
 *   two indices, count re-read before the store          44, MATCH
 *   the 7-differing form with --no-sched2                44, 28
 *
 * The duplicate-constant warning on this candidate was a false alarm: 0x219 is
 * pooled twice, but the module pointer is dereferenced once into a local which
 * pins it across the call, and the plain spelling reproduced both pool loads.
 */
typedef unsigned char u8;
typedef unsigned short u16;

extern u8 *iwram_3001f2c;
extern int Func_80ac8fc(void *a, int b, int c);

int Func_80aaf58(u8 *a)
{
	u8 *p = iwram_3001f2c;
	int i = 0;
	int j = 0;
	int n = p[0x219];

	while (i < n) {
		int v = Func_80ac8fc(a + j * 0x14, ((u16 *)(p + 0x208))[j], -1);
		i++;
		n = p[0x219];
		a[0xa0 + j] = v;
		j++;
	}
}
