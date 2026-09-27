/* asm/rom_b5000/rom_b5a0c_c_a.s -- ALL THREE FUNCTIONS.  WHOLE-FILE CONVERSION.
 *
 * EXACT, WHOLE FILE: 760 bytes, 362 encodings and 16 relocations identical
 * (scratch harness wholecmp.py, which assembles the FULL reference .s and
 * diffs .text plus every relocation; 3 runs).  Per function, objcmp:
 *   Func_80b5b18  240 bytes, 115 encodings,  3 relocations identical
 *   Func_80b5c08  308 bytes, 145 encodings,  9 relocations identical
 *   Func_80b5d3c  212 bytes, 102 encodings,  4 relocations identical
 * datacheck reports NO DATA, so landing needs no linker change at all.
 * No pins, no barriers, no shim, no new .sym entry, no flag row.
 *
 * FIVE LEVERS, each confirmed by a SINGLE DROP from this file.
 *
 * 1. *** A `char *` DEREF HAS ALIAS SET 0, SO IT DEPENDS ON EVERY OTHER MEMORY
 *    ACCESS -- ROUTE THE BYTE STORES THROUGH A STRUCT MEMBER INSTEAD. ***
 *    Func_80b5b18's 22 byte stores through `unsigned char *u` are 109 of 115
 *    (6 instructions short); through `struct U *u` / `u->f132[k]` they are
 *    EXACT.  The mechanism is measurable: with alias set 0 the stores conflict
 *    with the `ldrh r0, [r6, r1]` that reloads `buf[i]` for the tail call, so
 *    sched2 gives `add r3, r2, r0` the longer path and issues it first; with a
 *    real alias set the dependence is gone and `mov r1, r8` moves up one slot,
 *    which is the ROM.  The struct ALSO supplies the second const-0 pseudo
 *    (r9) that the ROM keeps in a high register -- with `unsigned char *` a
 *    named `int z = 0` after the clear loop is needed to get it (3 of 115) and
 *    the schedule is still wrong.  -fno-strict-aliasing, -fargument-noalias,
 *    -fno-rerun-cse-after-loop, -fno-schedule-insns2, -fno-gcse and -O1 are
 *    all inert on it, and so are all six declaration-order permutations.
 *
 * 2. A NON-VOID RETURN TYPE IS WHAT PUTS `pop {r1}` IN THE EPILOGUE.  All
 *    three functions return `int` with no `return` statement; gcc reserves r0
 *    for the return value and pops lr into r1 instead.  Declared `void` each
 *    is 2 of its encodings out, in the epilogue only.
 *
 * 3. ONE VARIABLE SHARED BY TWO CONSECUTIVE LOOPS IS WHAT PUSHES IT INTO A
 *    HIGH REGISTER.  Func_80b5d3c's ROM keeps the element index in r8 for BOTH
 *    the 0..3 accumulate loop and the 0..0x1f summon loop, paying `mov r2, r8`
 *    twice per inner iteration and a three-instruction `add r8, r3` increment.
 *    Spelling them as two locals `e` and `s` gives each a cheap low register
 *    and the function collapses to 94 encodings against 102 (83 differing).
 *    The shared `e` is EXACT.  This is the reverse of "naming one level too
 *    many": here the ROM's worse code is the evidence for ONE variable.
 *
 * 4. TWO HALVES OF A FUNCTION THAT USE "THE SAME" POINTER AND COUNTER NEED
 *    SEPARATE LOCALS.  Func_80b5c08 searches a 64-entry table twice.  One `q`
 *    and one `j` across both halves is 151 encodings against 145 (146
 *    differing) -- the second half's uses cross calls, so the single pseudo
 *    conflicts with r0-r3 and takes r5/r7, which spills the loop counter `a`
 *    into r4 (call-clobbered, hence a str/ldr pair around every call) and
 *    drives `p` into r11.  Separate `q2`/`k` for the second half is EXACT.
 *    The greg dump is what settles it: reg 33 is live in basic blocks 7-13 AND
 *    19-24, and its conflict list includes hard regs 0 1 2 3 13 14.
 *
 * 5. AN EXTERN'S RETURN TYPE DECIDES AN ARGUMENT-SETUP SCHEDULE.  Declaring
 *    `_SetDjinni` as returning `int` rather than `void` is the difference
 *    between the ROM's `mov r2,r3 / mov r1,r6 / mov r0,r5` and gcc's
 *    `mov r2,r3 / mov r0,r5 / mov r1,r6` (2 encodings).  The call's r0 result
 *    changes the dependence graph at the call, so sched2 ranks `mov r1, r6`
 *    above `mov r0, r5`.  Measured after eleven other spellings of that block
 *    -- all six orders of the three field loads (best 2 encodings), three
 *    declaration orders, six local types, a `struct E *` entry pointer, both
 *    condition orders and two prototype-less declarations -- were inert on it.
 *
 * THREE MORE THINGS THAT ARE LOAD-BEARING AND SMALLER.
 *
 * - `unsigned int id` for the unit id.  The ROM's `cmp r7,#7 / bls` is an
 *   UNSIGNED compare; an `int id` gives `ble` (1 encoding).  `buf[i] > 7`
 *   without the local is also unsigned (gcc knows the zero_extend is
 *   non-negative) but then gcc re-loads it with `ldrh r0,[r3,r1]` twice and
 *   loses the pointer induction variable, 151 encodings.
 * - THE COMPARE'S OPERAND ORDER.  `a == q->ent[j].a` gives the ROM's
 *   `cmp r6, r3`; `q->ent[j].a == a` gives `cmp r3, r6`, 4 encodings.
 * - THE ENTRY ARRAY AND ITS COUNT LIVE IN A NESTED STRUCT, reached by
 *   `q = &r->q`.  A flat `struct R` with `ent[64]` and `count` as siblings is
 *   151 encodings against 145: gcc then keeps the RECORD pointer live across
 *   the whole search loop and rebuilds `rec + 0x108` each iteration, where the
 *   ROM lets `rec` die and reads the count as `q + 0x100`.  The three field
 *   locals in the second half are needed too -- inlining
 *   `q2->ent[k].c` etc. at both calls re-loads all three bytes twice, 137
 *   encodings against 145.
 *
 * INERT (measured): declaration order of every local in all three functions
 * (6 permutations for Func_80b5b18, 88 for Func_80b5c08), `a < 4`/`b < 20`
 * against `a <= 3`/`b <= 0x13`, a `while` outer loop, a `continue` inversion,
 * `j >= q->count`, an explicit `unsigned short *p` walk over buf, and the
 * literal `0x154` style spellings.
 */
struct U {
	unsigned char pad000[0x118];
	unsigned char f118[4];
	unsigned char pad11c[0x10];
	unsigned char f12c[4];
	unsigned char pad130[2];
	unsigned char f132[14];
	unsigned char pad140;
	unsigned char f141[8];
};

struct E {
	unsigned char a;
	unsigned char b;
	unsigned char c;
	signed char d;
};

struct Q {
	struct E ent[64];
	int count;
};

struct R {
	int mask;
	unsigned char pad04[4];
	struct Q q;
};

struct S {
	unsigned char pad00[4];
	unsigned char req[4];
};

extern int Func_80b6a60(unsigned short *buf);
extern struct U *_GetUnit(int id);
extern void _CalcStats(int id);
extern struct R *_Func_8077330(int side);
extern int _Func_807a1f8(unsigned int id, int a, int b);
extern void _Func_807a458(unsigned int id, int a, int b);
extern int _GetFlag(int flag);
extern void *GetBattleActor(int id);
extern int _SetDjinni(int a, int b, int c);
extern void _Func_807a3a8(int a, int b, int c);
extern struct S *_GetSummonInfo(int id);

int Func_80b5b18(void)
{
	unsigned short buf[10];
	struct U *u;
	int n;
	int i;
	int j;

	n = Func_80b6a60(buf);
	for (i = 0; i < n; i++) {
		u = _GetUnit(buf[i]);
		for (j = 3; j >= 0; j--)
			u->f12c[j] = 0;
		u->f132[0] = 0;
		u->f132[1] = 0;
		u->f132[2] = 0;
		u->f132[3] = 0;
		u->f132[4] = 0;
		u->f132[5] = 0;
		u->f132[6] = 0;
		u->f132[7] = 0;
		u->f132[8] = 0;
		u->f132[9] = 0;
		u->f132[10] = 0;
		u->f132[11] = 0;
		u->f132[12] = 0;
		u->f132[13] = 0;
		u->f141[0] = 0;
		u->f141[1] = 0;
		u->f141[2] = 0;
		u->f141[3] = 0;
		u->f141[4] = 0;
		u->f141[5] = 0;
		u->f141[6] = 0;
		u->f141[7] = 0;
		_CalcStats(buf[i]);
	}
}

int Func_80b5c08(void)
{
	unsigned short buf[10];
	struct R *r;
	struct Q *q;
	struct Q *q2;
	int n;
	int i;
	int a;
	int b;
	int j;
	int k;
	int idx;
	int who;
	int grp;
	unsigned int id;

	n = Func_80b6a60(buf);
	for (i = 0; i < n; i++) {
		id = buf[i];
		for (a = 0; a <= 3; a++) {
			for (b = 0; b <= 0x13; b++) {
				if (_Func_807a1f8(id, a, b) != 0) {
					r = _Func_8077330(id > 7);
					q = &r->q;
					for (j = 0; j < q->count; j++)
						if (a == q->ent[j].a && b == q->ent[j].b)
							break;
					if (j == q->count)
						_Func_807a458(id, a, b);
				}
			}
		}
	}
	if (_GetFlag(0xb6 << 1) == 0) {
		r = _Func_8077330(0);
		q2 = &r->q;
		for (k = 0; k < q2->count; k++) {
			if (q2->ent[k].d == -1 && GetBattleActor(q2->ent[k].c) == 0) {
				who = q2->ent[k].c;
				grp = q2->ent[k].a;
				idx = q2->ent[k].b;
				_SetDjinni(who, grp, idx);
				_Func_807a3a8(who, grp, idx);
			}
		}
	}
}

int Func_80b5d3c(void)
{
	unsigned char acc[4];
	unsigned short buf[10];
	struct S *info;
	int n;
	int e;
	int i;
	int c;
	int mask;

	n = Func_80b6a60(buf);
	mask = 0;
	for (e = 0; e <= 3; e++) {
		acc[e] = 0;
		for (i = 0; i < n; i++)
			acc[e] += _GetUnit(buf[i])->f118[e];
	}
	for (e = 0; e <= 0x1f; e++) {
		info = _GetSummonInfo(e);
		if (info != 0) {
			for (c = 0; c < 4; c++)
				if (acc[c] < info->req[c])
					break;
			if (c == 4)
				mask |= 1 << e;
		}
	}
	_Func_8077330(0)->mask = mask;
}
