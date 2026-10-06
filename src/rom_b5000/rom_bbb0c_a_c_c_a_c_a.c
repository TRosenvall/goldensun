/* Func_80bf574 (0x080bf574) -- LANDING, byte-identical.  Was parked at
 * 3 differing encodings of 24; this body reads 0.  PIN-FREE, SHIM-FREE,
 * FLAG-FREE, DEVICE-FREE.  No split and no export: the reference .s holds
 * this function alone (one .thumb_func_start) and tools/datacheck.py is
 * silent, so landing it is a plain whole-file conversion.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/rom_b5000/rom_bbb0c_a_c_c_a_c_a.c asm/rom_b5000/rom_bbb0c_a_c_c_a_c_a.s --func Func_80bf574
 *
 * objcmp --func and --whole both read
 *   OK Func_80bf574 -- 52 bytes, 24 encodings and 1 relocations identical
 *
 * WHAT THE PARK HAD RIGHT, AND THE ONE AXIS IT NEVER VARIED.
 *
 * The park's five-step causal chain is CORRECT and is what this body obeys:
 * the ROM needs the first store to be a LEAF off the add, which needs a
 * SEPARATE truncation insn, which needs the stored value NOT to be the
 * zero-extended one -- and every body that got that shape put `u` in r1,
 * which left the 0x146 offset live in r0, which let reload_cse_move2add
 * (reload1.c:8891) derive 0x147 from it as `adds r0, #1` and delete the
 * ROM's pooled `.word 0x147`.  17 int-temp bodies, `u` last and in r1 in
 * all 17.
 *
 * The axis none of those 17 varied is the TYPE CONSTRUCTOR of the sum
 * carrier.  `promote_mode` (explow.c:895-902) applies PROMOTE_MODE only to
 * INTEGER / ENUMERAL / BOOLEAN / CHAR / REAL / OFFSET types, so a plain
 * `unsigned char` local becomes an SImode pseudo holding the ZERO-EXTENDED
 * value -- which is why `*p = v` could never be a leaf off the add -- while
 * a RECORD_TYPE is not in that switch and stays QImode.  The sum carrier
 * here is therefore a one-member struct of `unsigned char`:
 *
 *   s.b = *p + 0xff;   QImode, so `*p = s.b` is a LEAF off the add
 *   v   = s.b;         a PROMOTED unsigned char -- the real lsl/lsr pair
 *   if (v != 0) ...    one use of the extended value
 *   *(u + 0x147) = v;  the SECOND use, which is why combine cannot collapse
 *                      the extension into the lsl-only `(x << 24) != 0` form
 *
 * Two uses of the extended pseudo is the part that matters for the shifts:
 * with one use combine substitutes the extension into the comparison and
 * emits `lsl #24 / cmp` instead of the ROM's `lsl #24 / lsr #24 / cmp`.
 *
 * AND THE ALLOCATION COMES FOR FREE, measured in .18.greg.  The QImode
 * carrier restores the block-0 double-read copy (`ldrb r2, [r1] / mov r3,
 * r2`), so local-alloc again parks block-local pseudos in r3 and BOTH
 * long-lived conflictors of `u` inherit the hard-r3 conflict -- exactly the
 * requirement the park stated and could not reach:
 *     5 regs to allocate: 41 47 36 33 32
 *     32 conflicts: 32 33 36 37 41 43 3 13      (32 = u, allocated LAST)
 *     33 conflicts: 32 33 36 37 41 3 13
 *     36 conflicts: 32 33 36 37 3 13
 * r3 is blocked by u's own hard conflict, 41/36 take r2, 33 takes r1, and
 * `u` lands in r0 BY ELIMINATION -- REG_ALLOC_ORDER on ARM is {3,2,1,0}
 * (arm.h:989, descending).  With `u` in r0 the 0x146 offset is built in r3
 * instead, move2add has no r0 constant to derive from, and the ROM's pooled
 * 0x147 survives.  One edit, and all three symptoms of the chain go.
 *
 * THE MECHANISM IS CONFIRMED FOUR WAYS (same body, carrier type only):
 *   struct { unsigned char b; } s;      0   <- shipped
 *   union  { unsigned char b; } s;      0
 *   struct { unsigned char b:8; } s;    0
 *   unsigned char s[1];                 0
 *   unsigned char s;                    3   <- PROMOTE_MODE applies; the park
 *   int s;                              9   <- the int-temp horn, pool lost
 * so the figure tracks the TYPE CONSTRUCTOR and nothing else.  A one-member
 * aggregate is a LEVER in this tree, not a device (see the standing bound on
 * `union { int i; }`), and the carrier is never aliased, addressed or
 * type-punned here -- it is read and written through its own member only.
 *
 * Also measured on the way, all worse, so nobody repeats them: the int-temp
 * sum carrier with a separate promoted carrier 9; with `unsigned int v` 20;
 * `v = t & 0xff` 9; `u[0x147] = v` 9; `if (v)` 9; the final store taking the
 * raw sum 19; a second pointer named early 21; `*(p + 1) = v` 20; the cast
 * only in the test 19.
 */
extern unsigned char *_GetUnit(void);

int Func_80bf574(void)
{
	unsigned char *u;
	unsigned char *p;
	struct { unsigned char b; } s;
	unsigned char v;

	u = _GetUnit();
	p = u + (0xa3 << 1);
	if (*p == 0)
		goto fail;
	s.b = *p + 0xff;
	*p = s.b;
	v = s.b;
	if (v != 0)
		goto fail;
	*(u + 0x147) = v;
	return 1;
fail:
	return 0;
}
