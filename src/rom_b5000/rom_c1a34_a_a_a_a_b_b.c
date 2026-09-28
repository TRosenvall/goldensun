/* Func_80c1c54 -- 0x080c1c54, from goldensun/asm/rom_b5000/rom_c1a34_a_a_a_a_b.s.
 *
 * EXACT.  objcmp, verbatim:
 *   OK Func_80c1c54 -- 416 bytes, 189 encodings and 17 relocations identical
 *
 * Applies a level gain to one unit: snapshot the 0x24-byte stat block, grow
 * each stat by a per-stat multiple of the level delta, floor every result at
 * 70% of the snapshot, clamp it, then bump the level byte and recompute.
 *
 * THE .s HOLDS TWO FUNCTIONS (Func_80c1afc first, then this one) and NO data,
 * so landing this one alone needs tools/split_s.py.  Func_80c1afc does NOT
 * match (20 of 158 encodings; see Func_80c1afc.park.c), so the file does NOT
 * convert whole.
 *
 * FIVE LOAD-BEARING CONSTRUCTS, each confirmed by a SINGLE DROP from this file.
 *
 * 1. *** THE FUNCTION-POINTER TYPEDEF RETURNS int, NOT void. ***  That one
 *    token is the whole difference between 4 differing encodings and exact.
 *    With `typedef void (*CopyFn)(...)` the argument setup at the indirect call
 *    comes out `mov r0,r9 / movs r2,#0x24 / ldr r3,=Func_8001af8 / mov r1,r6`;
 *    with `int` it is the ROM's `movs r2,#0x24 / ldr r3 / mov r1,r6 / mov r0,r9`.
 *    A void call does not set r0 in RTL, so the write to r0 has no dependent
 *    and rank_for_schedule issues it first.  Changing ONLY the typedef (leaving
 *    `extern void Func_8001af8`) is also exact, so the callee's own declared
 *    return type is inert here -- it is the POINTER's type that schedules.
 *    This is the batch-286 "a callee's return type orders moves" lever reaching
 *    an INDIRECT call for the first time.
 *
 * 2. THE INDIRECT CALL ITSELF.  `bl _call_via_r3` needs the callee in a local
 *    of function-pointer type (the rom_e0524.c LoadVFXFile idiom).  A direct
 *    `Func_8001af8(...)` is 170 of 189 differing at 408 bytes against 416.
 *
 * 3. *** EVERY FIELD IS REACHED THROUGH `u`, NOT THROUGH A `struct Stats *`. ***
 *    Naming the block (`st = &u->st;` then `st->hp`, `st->pp`, ...) rebases the
 *    whole function on u+0x10, so `u` itself dies and gets a FOURTH stack slot
 *    -- `sub sp,#0x10` against the ROM's `sub sp,#0xc`, 159 of 189 differing.
 *    Writing `u->st.hp` and passing `&u->st` only to the copy is exact: cse
 *    reuses the `add r6,#0x10` it built for the call argument at the ONE access
 *    whose offset is exactly 0x10, and reaches the rest as `[r7,#0x12]` etc.
 *    off u.  The ROM's mixed bases are the tell that the source had no
 *    intermediate pointer.
 *
 * 4. THE SIGNEDNESS OF EACH FIELD IS VISIBLE.  hp/pp are `short` (ldrsh) and
 *    atk/def/agi are `unsigned short` (ldrh).  Making atk/def/agi signed is 122
 *    of 189 at 428 bytes; making hp/pp unsigned is 168 of 189 at 408.
 *
 * 5. THE ELEMENT LOOP COUNTS UP.  `for (i = 0; i < 4; i++)` is exact -- gcc
 *    reverses it itself into the ROM's `sub r4,#1 / bge`.  Writing the
 *    countdown in the source is 42 of 189.
 *
 * Also load-bearing: the `int` return type with no `return` statement.  `void`
 * is 2 of 189, both in the epilogue (`pop {r0}` for `pop {r1}`).
 *
 * The multipliers come out of gcc's own mult expansion and are what share the
 * subexpressions the ROM spills: lv*97 = ((lv*3)<<5)+lv and lv*51 =
 * ((lv*3)<<4)+lv*3 share `lv*3` in r11; lv*15 = (lv<<4)-lv and the loop's
 * second lv*15 share `lv<<4`, which is the word at [sp,#4]; lv*123 =
 * (((lv<<5)-lv)<<2)-lv and lv*33 = (lv<<5)+lv share `lv<<5`.  Nothing had to
 * be written to make that happen.
 *
 * MEASURED (ref 189 encodings, 416 bytes):
 *   named `struct Stats *st` for every field            159   (404 bytes)
 *   fields through `u`, void CopyFn typedef               4
 *   direct call instead of the pointer                  170   (408 bytes)
 *   atk/def/agi signed                                  122   (428 bytes)
 *   hp/pp unsigned                                      168   (408 bytes)
 *   element loop written counting down                   42
 *   void return type                                      2
 *   int CopyFn typedef                                  EXACT
 *
 * No pins, no barriers, no shim, no new .sym entry, no flag row.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py <this file> \
 *     asm/rom_b5000/rom_c1a34_a_a_a_a_b.s --func Func_80c1c54
 */
struct El {
	short v;
	short w;
};

struct Stats {
	short hp;
	short pp;
	unsigned char pad04[4];
	unsigned short atk;
	unsigned short def;
	unsigned short agi;
	unsigned char pad0e[6];
	struct El el[4];
};

struct U {
	unsigned char pad00[0xf];
	unsigned char lvl;
	struct Stats st;
};

typedef int (*CopyFn)(void *dst, void *src, int len);

extern void *Func_8004970(int size);
extern struct U *_GetUnit(int id);
extern int Func_8001af8(void *dst, void *src, int len);
extern void _CalcStats(int id);
extern void free(void *p);

int Func_80c1c54(int id, int lv)
{
	struct U *u;
	struct Stats *old;
	CopyFn copy;
	int v;
	int i;

	old = Func_8004970(0x24);
	u = _GetUnit(id);
	copy = Func_8001af8;
	copy(old, &u->st, 0x24);
	v = u->st.hp + lv * 97 / 10;
	if (v < old->hp * 7 / 10)
		v = old->hp * 7 / 10;
	if (v > 0x270f)
		v = 0x270f;
	u->st.hp = v;
	v = u->st.pp + lv * 15 / 10;
	if (v < old->pp * 7 / 10)
		v = old->pp * 7 / 10;
	if (v > 0x270f)
		v = 0x270f;
	u->st.pp = v;
	v = u->st.atk + lv * 123 / 10;
	if (v < old->atk * 7 / 10)
		v = old->atk * 7 / 10;
	if (v > 0x3e7)
		v = 0x3e7;
	u->st.atk = v;
	v = u->st.def + lv * 33 / 10;
	if (v < old->def * 7 / 10)
		v = old->def * 7 / 10;
	if (v > 0x3e7)
		v = 0x3e7;
	u->st.def = v;
	v = u->st.agi + lv * 51 / 10;
	if (v < old->agi * 7 / 10)
		v = old->agi * 7 / 10;
	if (v > 0x3e7)
		v = 0x3e7;
	u->st.agi = v;
	for (i = 0; i < 4; i++) {
		v = u->st.el[i].v + lv * 15;
		if (v < old->el[i].v * 7 / 10)
			v = old->el[i].v * 7 / 10;
		if (v > 0xc8)
			v = 0xc8;
		u->st.el[i].v = v;
	}
	u->lvl += lv;
	_CalcStats(id);
	free(old);
}
