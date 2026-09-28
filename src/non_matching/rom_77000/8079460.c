/* InitEnemyUnit -- 0x08079460, from goldensun/asm/rom_77000/rom_79460_a.s.
 *
 * NON-MATCHING: 31 encodings of 197 differ (objcmp).
 *
 * objcmp --whole, verbatim:
 *   XX InitEnemyUnit                31 of 197 differ (ours 197), first at index 68
 * No SIZE line and no RELOCATIONS line: 412 bytes both sides, 197 encodings both
 * sides, and all 8 relocations identical at identical offsets.  So 31 is a TRUE
 * DISTANCE, not a count against a differently shaped object.
 *
 * THE .s HOLDS ONE FUNCTION AND NO DATA SECTION, so this would convert it whole.
 *
 * SEVEN CONSTRUCTS ARE ESTABLISHED, each by a single drop, and they took the
 * function from 136 of 197 at the wrong size to 31 at the right one:
 *
 * 1. THE NAME COPY IS A GUARDED do-while THROUGH A `char *`.
 *    `d = (char *)u; i = 0; if (buf[i] != 0) { do { d[i] = buf[i]; if (++i > 13)
 *    break; } while (buf[i] != 0); }`.  The `char *` is what makes the byte store
 *    invalidate the halfword load, so the ROM's TWO `ldrh` per iteration appear
 *    (a struct `char[15]` member does NOT do it -- the struct's alias set
 *    governs).  The guarded do-while is what puts the bound test before the zero
 *    test in the body and makes the zero test the `bne` latch; a plain
 *    `while (buf[i] != 0) { d[i] = buf[i]; if (++i > 13) break; }` rolls the
 *    OTHER way and lets gcc reuse the condition's load for the store.
 *    146 of 197 -> and it fixed the instruction count.
 *
 * 2. ONE COUNTER SERVES THE NAME COPY AND THE FOUR-ELEMENT OUTER LOOP.
 *    The ROM keeps kind-8, the name index, the element index and the u+0x128
 *    pointer all in r5 and never touches r4.  A separate `k` costs 96 -> and,
 *    more tellingly, moved the first differing encoding from index 8 to 16.
 *
 * 3. *** THE INNER COUNT IS COPIED INTO A SECOND VARIABLE. ***  `cnt =
 *    e->cnt[i]; if (cnt != 0) { m = cnt; do { ... } while (--m != 0); }`.  The
 *    ROM's `ldrb r3,[r2] / cmp r3,#0 / beq / ... / mov r1,r3` has that `mov` as
 *    a live-range split: the tested value and the decrementing counter are two
 *    pseudos.  Decrementing `cnt` itself is one instruction short and 32 places
 *    worse.  **This single token took the function from 96 to 32.**
 *
 * 4. THE RANGE TEST AT THE END IS TWO STATEMENTS WITH A `goto`, NOT `&&`.
 *    `v = u->f128; if (v > 0xab) goto done; if (v >= 0x9e) u->f12a = 2; done: ;`
 *    Written `if (u->f128 <= 0xab && u->f128 >= 0x9e)` gcc folds it into an
 *    unsigned range test (`add r3,#98 / lsl #24 / lsl #20 / cmp / bhi`), and
 *    written as nested `if`s it still narrows both compares to UNSIGNED
 *    (`bhi`, `bls`) because combine sees the QImode zero-extend.  The goto form
 *    is the only one of five spellings that reproduces the ROM's SIGNED
 *    `cmp r3,#171 / bgt`.
 *
 * 5. THE ELEMENT LOOP AND THE INNER COUNT LOOP ARE WRITTEN COUNTING UP /
 *    DECREMENTING RESPECTIVELY.  `for (i = 0; i < 4; i++)` is reversed by
 *    check_dbra_loop into the ROM's `sub r5,#1 / bge` on its own (the counter is
 *    unused in the body), and `do { } while (--m != 0)` is the ROM's
 *    `sub r1,#1 / cmp r1,#0 / bne`.
 *
 * 6. THE `Func_80008d4` CALL IS INDIRECT THROUGH A TYPEDEF THAT RETURNS `int`.
 *    `bl _call_via_r3` needs the callee in a local of function-pointer type; the
 *    `int` return is batch 290's Func_80c1c54 lever (a void pointer type
 *    reorders the argument moves).
 *
 * 7. THE THREE ENTRY GUARDS ARE THREE SEPARATE `return 0;` STATEMENTS and
 *    `t = kind - 8` is `unsigned int`, which is what makes `cmp r5,#242 / bls`
 *    and `cmp r5,#164 / bls` unsigned.  An `int` with two casts costs 151 and
 *    two instructions.
 *
 * FIVE RESIDUE CLUSTERS, all register-allocation or scheduling, none reachable
 * by any spelling tried:
 *
 *   a. *** THE TWO STRENGTH-REDUCED POINTERS ARE IN THE OPPOSITE HIGH
 *      REGISTERS. ***  The ROM puts the element pointer (ent+0x28) in r14 and
 *      the count pointer (ent+0x30) in r12; gcc does the reverse.  REG_ALLOC_ORDER
 *      is {3,2,1,0,12,14,...}, so r12 goes to the higher-priority allocno; both
 *      givs have exactly 3 refs, so this is the EXACT-TIE case batch 287
 *      identified, broken on pseudo number -- and both pseudos are created by
 *      loop.c, not by the source, so there is no declaration order to change.
 *      This is the largest cluster and it drags the surrounding `mov r2,r14` /
 *      `add r12,r3` pairs with it.
 *   b. The pool load of 0x28f goes to r0 in the ROM and to r3 here, which is
 *      what lets the ROM's scheduler hoist it above the `ldrb r3,[r7,#0x1d]`.
 *      Two encodings, and the same allocation question as (a).
 *   c. The ROM copies `suffix` out of r8 ONCE and uses that copy for both the
 *      `cmp #8` and the `+0x31`; gcc copies it twice.  A named `int sfx = suffix;`
 *      local is fully inert.
 *   d. `cmp r3,#158 / blt` against our `cmp r3,#157 / ble`.  gcc-2.96's
 *      simplify_comparison canonicalises `LT const` to `LE const-1` and
 *      `GE const` to `GT const-1` unconditionally, so NEITHER `v < 0x9e` nor
 *      `v >= 0x9e` can emit a `blt` against 158.  One encoding, and it looks
 *      structurally unreachable from C for the same reason the const.sym pool
 *      tell is: the canonicalisation has no source-level switch.
 *
 * MEASURED (ref 197 encodings, 412 bytes):
 *   first candidate (while-loop copy, struct name store)  136   (400 bytes)
 *   + guarded do-while copy loop                          146   (412 bytes)
 *   + goto-form range test                                151   (408 bytes)
 *   + shared i/k counter                                   96   (412 bytes)
 *   + `m = cnt` live-range split                           32   (412 bytes)
 *   + signed first compare via the goto form               31   (412 bytes)
 *   `n = 0` before the _DecompressString call              92 (a different mix)
 *   `n = 0` at the top of the function                    127
 *   an explicit `unsigned short *q = buf;`                186-193, 416-424 bytes
 *   `d = (char *)u;` moved before the call                      inert
 *   `int sfx = suffix;`                                          inert
 *   `if (e->cnt[i] != 0) { cnt = e->cnt[i]; ... }` (re-read)  139   (424 bytes)
 *   `(signed char)` on the range test                      179   (420 bytes)
 *
 * No pins, no barriers, NO SHIM, no flag row, no .sym entry, no fakematch.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_77000/79460_InitEnemyUnit.park.c \
 *     asm/rom_77000/rom_79460_a.s --func InitEnemyUnit
 */
struct Ent {
	unsigned char pad00[0x0f];
	unsigned char f0f;
	unsigned short f10;
	unsigned short f12;
	unsigned short f14;
	unsigned short f16;
	unsigned short f18;
	unsigned char f1a;
	unsigned char f1b;
	unsigned char f1c;
	unsigned char f1d;
	unsigned char pad1e[2];
	unsigned int f20;
	unsigned char pad24[4];
	unsigned short el[4];
	unsigned char cnt[4];
	unsigned char pad34[0x54 - 0x34];
};

struct Unit {
	unsigned char name[15];
	unsigned char f0f;
	unsigned short f10;
	unsigned short f12;
	unsigned short f14;
	unsigned short f16;
	unsigned short f18;
	unsigned short f1a;
	unsigned short f1c;
	unsigned char f1e;
	unsigned char f1f;
	unsigned char f20;
	unsigned char f21;
	unsigned char pad22[2];
	unsigned char f24[0x34 - 0x24];
	unsigned short f34;
	unsigned short f36;
	unsigned short f38;
	unsigned short f3a;
	unsigned char pad3c[0xd8 - 0x3c];
	unsigned short psy[15];
	unsigned char padf6[0x120 - 0xf6];
	unsigned int f120;
	unsigned char pad124[4];
	unsigned char f128;
	unsigned char f129;
	unsigned char f12a;
};

typedef int (*ZeroFn)(void *p, int n);

extern struct Unit *GetUnit(int id);
extern int Func_80008d4(void *p, int n);
extern void _DecompressString(int msg, unsigned short *dst, int max);
extern void Func_80798e0(int id, void *p);
extern void CalcStats(int id);
extern struct Ent Data_80ec8[];

int InitEnemyUnit(int id, int kind, int suffix)
{
	unsigned short buf[16];
	struct Unit *u;
	struct Ent *e;
	ZeroFn zero;
	char *d;
	unsigned int t;
	int i;
	int n;
	int cnt;
	int m;
	int v;

	t = kind - 8;
	if (id < 0x80)
		return 0;
	if (id > 0x86)
		return 0;
	if (t > 0xf2)
		return 0;
	u = GetUnit(id);
	zero = Func_80008d4;
	zero(u, 0x14c);
	if (t > 0xa4)
		t = 0;
	e = &Data_80ec8[t];
	u->f0f = e->f0f;
	u->f10 = e->f10;
	u->f38 = e->f10;
	u->f34 = e->f10;
	u->f12 = e->f12;
	u->f3a = e->f12;
	u->f36 = e->f12;
	u->f14 = 0x4000;
	u->f16 = 0x4000;
	u->f18 = e->f14;
	u->f1a = e->f16;
	u->f1c = e->f18;
	u->f1e = e->f1a;
	u->f1f = e->f1b;
	u->f20 = e->f1c;
	u->f21 = e->f1d;
	_DecompressString(0x28f + t, buf, 0xf);
	d = (char *)u;
	i = 0;
	if (buf[i] != 0) {
		do {
			d[i] = buf[i];
			if (++i > 13)
				break;
		} while (buf[i] != 0);
	}
	if (suffix <= 8) {
		d[i] = suffix + 0x31;
		i++;
	}
	d[i] = 0;
	u->name[14] = 0;
	n = 0;
	for (i = 0; i < 4; i++) {
		if (e->el[i] != 0) {
			cnt = e->cnt[i];
			if (cnt != 0) {
				m = cnt;
				do {
					if (n <= 14) {
						u->psy[n] = e->el[i];
						n++;
					}
				} while (--m != 0);
			}
		}
	}
	u->f120 = e->f20;
	u->f129 = 0;
	u->f128 = kind;
	Func_80798e0(id, u->f24);
	CalcStats(id);
	u->f12a = 1;
	v = u->f128;
	if (v > 0xab)
		goto done;
	if (v >= 0x9e)
		u->f12a = 2;
done:
	;
	return 1;
}
