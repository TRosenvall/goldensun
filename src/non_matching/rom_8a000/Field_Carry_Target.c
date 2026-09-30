/* Field_Carry_Target  --  0x08099da4, was asm/rom_8a000/rom_97b54_c_c_c_a_a.s.
 *
 * SPLIT SHAPE: NONE NEEDED.  `grep -c func_start` on the reference is 1 and
 * tools/datacheck.py reports NO data section, so this file converts WHOLE --
 * no tools/split_s.py run, no extra `.global` exports.
 *
 * BUT NOTE, AND DO NOT "TIDY" IT: `stage1.ld` names this object TWICE --
 * line 1126 `(.text)` AND line 1193 `(.rodata)` -- even though the `.s` has NO
 * rodata at all.  This is the batch-300 double-naming trap arriving from the
 * other direction: there the second line existed because a DIFFERENT function
 * in the file carried a jump table; here the `.rodata` line is simply reserved
 * and currently contributes nothing.  LEAVE BOTH LINES ALONE.  The `.c` emits
 * no rodata either, so line 1193 keeps contributing nothing; if a future
 * spelling ever grows a switch table, that is the slot it must land in, and
 * deleting the line would move the ROM.  I checked this by grepping the script
 * for the object stem, which is the batch-300 rule, and datacheck CANNOT see
 * it because the requirement lives in the script, not the `.s`.
 * (The two `.L9f0bc` / `.L9f0d4` script blobs it references live in ANOTHER
 * object already and are reached the way four landed siblings reach them, with
 * `extern unsigned char L9f0xx[] __asm__(".L9f0xx");`.)
 *
 * NON-MATCHING, 500 of 549 encodings differ.
 *
 * SIZE and COUNT are BOTH INEXACT -- ref 1264 bytes / 549 encodings against
 * ours 1292 / 562 -- so objcmp's 500 is a SATURATED figure and cannot rank
 * anything.  Every number quoted below is therefore tools/aligncmp.py's, and
 * the baseline is: aligned-equal 310 of 549 (56.5%), 367 differing/ins/del in
 * 92 hunks.  (Proof the saturation is real, not a guess: fct1..fct4 in
 * scratch_elev/b302g -- four genuinely different compilations, four different
 * .s md5sums -- ALL report objcmp 513 of 549 and aligncmp 271/413/99 to the
 * digit.  A probe that inserted one extra WaitFrames moved objcmp to 523 and
 * aligncmp to 272, which is what proves the tools are sensitive and the
 * insensitivity belongs to this candidate's distance, not to the tools.)
 *
 * Shims: NONE -- tools/shimcount.py reports 0.  No pins, no "+r" barriers, no
 * volatile, no do{}while(0), no .equ, default flags.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_8a000/Field_Carry_Target.c \
 *     asm/rom_8a000/rom_97b54_c_c_c_a_a.s --func Field_Carry_Target
 *   -> XX SIZE  ref 1264 bytes, ours 1292
 *      XX ENCODINGS differ in 500 place(s) (ref 549, ours 562)
 * and rank any successor with
 *   ... python3 tools/aligncmp.py src/non_matching/rom_8a000/Field_Carry_Target.c \
 *     asm/rom_8a000/rom_97b54_c_c_c_a_a.s Field_Carry_Target
 *   -> aligned-equal 310  (56.5% of ref)   differing/ins/del 367 in 92 hunks
 *
 * READ src/rom_8a000/rom_97b54_c_c_c_a_b.c FIRST.  Field_Carry is this
 * function's NEAR TWIN and its header/body is the template: the same
 * `g = (int *)iwram_3001f30` read, the same
 * `Func_809a3c4(v[0] + (0x80<<14), v[1], v[2], 0x80<<8)` /
 * `Func_809a3c4(v[0] - 0x200000, v[1], v[2], 0)` pair off one `int v[3]`, and a
 * particle loop that is Field_Carry's verbatim -- `_Actor_SetScript(q, L9f0d4)`,
 * `*(int *)(q+0x30) = Random() + (0x80<<10)`, `*(int *)(q+0x34) = 0x80<<10`,
 * `*(q+0x55) = 0`, `Func_8096bec(q, Random()*24 + (0x80<<12), Random())`.  Every
 * one of those transferred unchanged and none of them is in the residue.
 *
 * LEVERS THAT PAID, in the order they paid (aligncmp aligned-equal of 549):
 *
 * 1. THE TWO ACTORS ARE ONE `unsigned char *act[2]`, NOT TWO SCALARS.
 *    [271 -> 311, and the hunk count 99 -> 93]  The frame says so before any
 *    measurement does: the ROM's `sub sp, #0x7c` decomposes as
 *    `arr[20]` at sp+0x2c (0x50 bytes), `int v[3]` at sp+0x20, and the two
 *    actor pointers at sp+0x14 / sp+0x18 -- and the ROM NEVER holds either in a
 *    register, reloading each from its slot at all eleven use sites.  That
 *    always-in-memory behaviour is what gcc-2.96 does to an array element and
 *    what it does NOT do to a scalar: as two scalars one of them wins r11 and
 *    the reference's r11 (`g`, the iwram_3001f30 struct base) is pushed to a
 *    spill slot.  Declaring them as an array takes both out of the allocno pool
 *    and lets `g` keep r11, which is the reference's assignment.
 *      MEASURED AND REJECTED, each one compile:
 *        act[3] with the caster as act[2]  -> 241  (all THREE stack pointers
 *            in one array; the caster's own accesses then cost index work)
 *        act[3] / act[4] with only [0],[1] used -> 233 both.  Field_Force's
 *            "the array must hold one MORE pointer than it uses" lever does NOT
 *            transfer here; the extra element is worth -78 aligned lines, and
 *            [3] and [4] scoring identically says the cost is the array's
 *            declared size crossing a boundary, not the element count.
 *        three one-element arrays (`unsigned char *ll[1]` etc., accessed
 *            `ll[0]`), declared aa/rr/ll after `v` so the expand_decl rule puts
 *            them at 0x1c/0x18/0x14 -> 271, i.e. BYTE-FOR-BYTE the scalar
 *            result.  So the expand_decl corollary's "give it an array type of
 *            the right width" is INERT for a POINTER scalar here: `T *x[1]`
 *            read as `x[0]` is allocated exactly like `T *x`.  That is a new
 *            bound on that corollary, which was measured on `u32`/`u8[4]`.
 *
 * 2. THE MAIN LOOP'S 0x100000 MUST BE A NAMED LOCAL.  [288 -> 311 -- dropping
 *    `kk` and writing the literal `0x80 << 13` costs 23 aligned lines.]  The
 *    reference sets `mov r3, #0x80 / lsl r3, #0xd / mov r9, r3` in the main
 *    loop's PREHEADER and then uses r9 four times inside, in BOTH arms of the
 *    `d == 0xffff` test.  A bare literal is never hoisted, because the two arms
 *    give loop.c's move_movables no position that dominates all four uses
 *    (`reg_in_basic_block_p` fails); a local assigned in the preheader is
 *    loop-invariant BY CONSTRUCTION and gets the high register.
 *    NOTE THE DIRECTION IS THE OPPOSITE OF Func_809a3c4's, two files away
 *    (src/non_matching/rom_8a000/Field_Force.c): there a NAMED invariant was
 *    rematerialised per iteration and only the bare literal reached the hoist.
 *    Both are true.  The discriminator is whether the uses share a dominator:
 *    Func_809a3c4's single-arm loop gives the literal one, this two-arm loop
 *    does not, and naming is the only way to put the set where a hoist can see
 *    it.  Do not carry either result across without checking the arms.
 *
 * 3. A NAMED `step` FOR THE RAISE LOOP AND A SECOND `r2` COPY OF act[1] FOR THE
 *    SUCCESS BLOCK.  [311 -> 310 aligned, but 516 -> 500 objcmp, size
 *    1308 -> 1292, count 570 -> 562.]  Near-inert on the ranking view and kept
 *    only because all three of the other three figures move toward the
 *    reference and both constructs are read straight off the reference's frame:
 *    the raise loop's `0xc0 << 7` is hoisted to r1 and SPILLED to sp+0 across
 *    its `WaitFrames`, and act[1] has a SECOND stack home at sp+8 written in
 *    the main loop's preheader and read only in the success block.  A one-line
 *    aligned difference is inside this candidate's noise; treat lever 3 as
 *    untested rather than as established.
 *
 * ALSO MEASURED, ALL INERT ON THE SATURATED BASELINE (fct1..fct4, all
 * 513/549 and 271/413/99): the short-circuit `hit != 0 || (comma, i > 0)` form
 * against nested `if`s; an explicit `continue` closing the blocked arm; naming
 * the 0x100000.  Each of these is INERT ONLY AGAINST THE SCALAR BASE -- lever 2
 * is the same edit as one of them and is worth 23 once act[2] is in place.
 * This is the "an inert spelling is UNTESTED, not disproved" rule paying out
 * inside one function: re-measure all three on top of lever 1.
 *
 * MEASURED AND REJECTED (on top of levers 1-3): splitting the success block's
 * saved v[0]/v[2] pair into their own `sv0`/`sv2` locals and unifying the
 * particle counter with the two wait-loop counters into one `i` -- 232, the
 * worst result of the round.  The reference DOES put all three counters in r8
 * across disjoint ranges, so "counters unify across disjoint loops" looks like
 * it should apply; it does not here, and the pair of saved values wants to be
 * the same two variables the counters are.
 *
 * WHAT IS ALREADY RIGHT.  The relocation SEQUENCE is 75 entries on both sides
 * and every R_ARM_THM_CALL is present; the residue in the sequence is
 * positional only (our pool block lands at a different offset because the
 * function is 28 bytes longer, which is a CONSEQUENCE, not a residue -- and a
 * relocation FORM difference is never one, objcmp compares unlinked objects).
 * Landed and out of the residue: the `if (e == 0) return` opening off
 * `g[4]`/`g[5]`; both `Func_809a3c4` calls; the `l == 0 || r == 0` early
 * `Func_809748c` exit; the whole `while (WaitFrames(1), (gKeyPress & 0x303) == 0)`
 * COMMA-CONDITION loop -- the reference's `b` INTO a bottom test that ends
 * `bne <exit> / b <body>` is exactly what a while-condition containing a CALL
 * gives, because duplicate_loop_exit_test (jump.c:1137) refuses a test with
 * side effects and so the test is never copied, only relocated; the two
 * `while (*(int *)(e+0x28) >= 0) { WaitFrames(1); k++; if (k > 0x59) break; }`
 * wait loops, guard-plus-bottom-test, which ARE duplicate_loop_exit_test's
 * output and so must be written `while`, never `do`; and the entire particle
 * loop.
 *
 * THE BLOCKER, by pass: `reload1.c` frame construction, four excess spill
 * slots, and one `jump.c` block relocation.
 *
 *   ref   sub sp, #0x7c = 5 spill words (sp+0..0x10) + a 12-byte declared
 *                         object at sp+0x14 + v[3] at 0x20 + arr[20] at 0x2c
 *   ours  sub sp, #0x88 = 9 spill words (sp+0..0x20) + act[2] at 0x24
 *                         + v[3] at 0x2c + arr[20] at 0x38
 *
 * The reference's frame admits exactly two readings and I could not settle it:
 *   (i) EIGHT spill words (0..0x1c) with v[3] and arr[20] the only declared
 *       objects -- 0x20 + 0xc + 0x50 = 0x7c.  This is the scalar reading; it is
 *       what fct4.c builds and it scores 271.
 *   (ii) FIVE spill words plus a TWELVE-byte declared object at 0x14 --
 *       0x14 + 0xc + 0xc + 0x50 = 0x7c.  act[2] (eight bytes) is the closest
 *       any spelling came, and every twelve-byte candidate measured WORSE
 *       (act[3] 233, act[3]-with-caster 241).
 * Settling that is the next move, and the cheap discriminator is a 12-byte
 * declared object that is NOT three pointers -- an `int` triple, or a struct --
 * since the two pointer spellings of twelve bytes are both eliminated.
 *
 * The `jump.c` half is independent and is worth about forty instructions of
 * misalignment on its own: our success block (`_PlaySound(0xaf)` .. the final
 * `WaitFrames(0xa)`) is RELOCATED to sit immediately after the early-return
 * block, where the reference leaves it at the end of the loop body between the
 * blocked arm's back-jump and the loop's bottom test.  Both blocks have the
 * same properties -- one predecessor, ending in an unconditional jump -- so the
 * reference's source does NOT trigger whatever moves ours.  INVERTING the test
 * to `hit == 0 && (comma, i <= 0)` so the success arm is the FALLTHROUGH DOES
 * put the block back where the reference has it (verified in the generated .s:
 * the label moves from line 77 to line 330) and scores 311, i.e. IDENTICAL to
 * the un-inverted form: the placement gain and the branch-sense loss cancel
 * exactly.  That candidate is scratch_elev/b302g/fctD.c and it is the other
 * half of the answer -- whatever fixes the frame should be measured on BOTH
 * senses, because one of them is carrying a 40-instruction credit the aligned
 * figure is currently hiding.
 */
extern unsigned char *iwram_3001f30;
extern int iwram_3001e40;
extern int gKeyHeld;
extern int gKeyPress;
extern void Func_8097384(void);
extern void Func_809748c(void);
extern void Func_8096b88(void);
extern void Func_8099d18(void);
extern void Func_809a6b8(unsigned char *a);
extern unsigned char *Func_809a3c4(int a, int b, int c, int d);
extern void Func_8096bec(unsigned char *a, int b, int c);
extern unsigned short Func_8097b54(int keys);
extern void WaitFrames(int n);
extern void _PlaySound(int id);
extern void StartTask(void *fn, int prio);
extern void StopTask(void *fn);
extern void _Actor_SetScript(unsigned char *a, unsigned char *s);
extern void _Actor_SetSpriteFlags(unsigned char *a, int f);
extern void _Actor_SetAnim(unsigned char *a, int n);
extern void _Actor_SetColorswap(unsigned char *a, int n);
extern void _Actor_TravelTo(unsigned char *a, int x, int y, int z);
extern void _Actor_WaitMovement(unsigned char *a);
extern void _DeleteActor(unsigned char *a);
extern unsigned char *CreateParticleActor(int id, int x, int y, int z);
extern unsigned int Random(void);
extern void vec3_translate(int d, int ang, int *v);
extern int _Func_800d924(unsigned char *a, int *v);
extern int _TestCollision(unsigned char *a, int *v);
extern unsigned char L9f0bc[] __asm__(".L9f0bc");
extern unsigned char L9f0d4[] __asm__(".L9f0d4");

void Field_Carry_Target(void)
{
	unsigned char *arr[20];
	int v[3];
	unsigned char *act[2];
	int *g;
	unsigned char *a;
	unsigned char *e;
	unsigned char *q;
	unsigned char **w;
	unsigned char *p55;
	unsigned char *p22;
	unsigned short d;
	int kk;
	int step;
	unsigned char *r2;
	int i;
	int k;
	int hit;

	g = (int *)iwram_3001f30;
	a = (unsigned char *)g[4];
	e = (unsigned char *)g[5];
	if (e == 0)
		return;
	Func_8097384();
	*(unsigned char **)(a + 0x68) = e;
	_Actor_SetScript(a, L9f0bc);
	v[0] = g[1];
	v[1] = g[2] + (0x80 << 13);
	v[2] = g[3];
	act[0] = Func_809a3c4(v[0] + (0x80 << 14), v[1], v[2], 0x80 << 8);
	act[1] = Func_809a3c4(v[0] - 0x200000, v[1], v[2], 0);
	if (act[0] == 0 || act[1] == 0) {
		Func_809748c();
		return;
	}
	WaitFrames(0xf);
	v[0] = *(int *)(e + 8);
	v[1] = *(int *)(e + 0xc) + (0x80 << 13);
	v[2] = *(int *)(e + 0x10);
	_Actor_TravelTo(act[0], v[0] + (0x80 << 13), v[1], v[2]);
	_Actor_TravelTo(act[1], v[0] - 0x100000, v[1], v[2]);
	_Actor_WaitMovement(act[0]);
	_Actor_WaitMovement(act[1]);
	*(int *)(act[0] + 8) = v[0] + (0x80 << 13);
	*(int *)(act[0] + 0x24) = 0;
	*(int *)(act[1] + 8) = v[0] - 0x100000;
	*(int *)(act[1] + 0x24) = 0;
	*(void **)(e + 0x6c) = (void *)Func_8096b88;
	StartTask((void *)Func_8099d18, 0xc8 << 4);
	_PlaySound(0x82);
	p55 = e + 0x55;
	*p55 = 4;
	_Actor_SetSpriteFlags(e, 0);
	step = 0xc0 << 7;
	if (act[0] != 0 && act[1] != 0) {
		while (*(int *)(e + 0xc) - *(int *)(e + 0x14) <= (0xc0 << 13)) {
			*(int *)(act[0] + 0xc) += step;
			*(int *)(act[1] + 0xc) += step;
			*(int *)(e + 0xc) += step;
			WaitFrames(1);
		}
	}
	*(int *)(act[0] + 0x30) = 0x80 << 11;
	*(int *)(act[0] + 0x34) = 0x80 << 8;
	*(int *)(act[1] + 0x30) = 0x80 << 11;
	*(int *)(act[1] + 0x34) = 0x80 << 8;
	*(int *)(e + 0x30) = 0x6666;
	*(int *)(e + 0x34) = 0x3333;
	*(unsigned char *)(e + 0x5a) = 0;
	p22 = e + 0x22;
	*p22 = 2;
	kk = 0x80 << 13;
	r2 = act[1];
	while (WaitFrames(1), (gKeyPress & 0x303) == 0) {
		d = Func_8097b54(gKeyHeld);
		if (d == 0xffff) {
			v[0] = *(int *)(e + 8);
			v[1] = *(int *)(e + 0xc) + kk;
			v[2] = *(int *)(e + 0x10);
			_Actor_TravelTo(act[0], v[0] + kk, v[1], v[2]);
			_Actor_TravelTo(act[1], v[0] - 0x100000, v[1], v[2]);
			_Actor_SetAnim(act[0], 1);
			_Actor_SetAnim(act[1], 1);
			continue;
		}
		v[0] = *(int *)(e + 8);
		v[1] = *(int *)(e + 0xc) + kk;
		v[2] = *(int *)(e + 0x10);
		vec3_translate(0x80 << 10, d, v);
		_Actor_TravelTo(act[0], v[0] + kk, v[1], v[2]);
		_Actor_TravelTo(act[1], v[0] - 0x100000, v[1], v[2]);
		_Actor_WaitMovement(act[0]);
		_Actor_WaitMovement(act[1]);
		v[0] = *(int *)(e + 8);
		v[1] = *(int *)(e + 0x14);
		v[2] = *(int *)(e + 0x10);
		vec3_translate(kk, d, v);
		hit = _Func_800d924(e, v);
		if (hit != 0
		    || (*(int *)(e + 0x14) += 0x80 << 13,
			i = _TestCollision(e, v),
			*(int *)(e + 0x14) -= 0x100000,
			i > 0)) {
			_Actor_SetAnim(act[0], 4);
			_Actor_SetAnim(act[1], 4);
			if ((iwram_3001e40 & 0xf) == 0)
				_PlaySound(0x72);
			continue;
		}
		{
			_PlaySound(0xaf);
			k = v[0];
			i = v[2];
			_Actor_SetAnim(act[0], 4);
			_Actor_SetAnim(act[1], 4);
			WaitFrames(0xf);
			*(unsigned char *)(e + 0x5b) = hit;
			*(int *)(e + 0x30) = 0x3333;
			*(int *)(e + 0x34) = 0x3333;
			_Actor_TravelTo(e, v[0], v[1], v[2]);
			*(int *)(act[0] + 0x30) = 0x3333;
			*(int *)(act[0] + 0x34) = 0x3333;
			*(int *)(r2 + 0x30) = 0x3333;
			*(int *)(r2 + 0x34) = 0x3333;
			_Actor_TravelTo(act[0], v[0] + kk, v[1], v[2]);
			_Actor_TravelTo(act[1], v[0] - 0x100000, v[1], v[2]);
			_Actor_WaitMovement(e);
			*(int *)(e + 8) = k;
			*(int *)(e + 0x10) = i;
			*(int *)(e + 0x24) = hit;
			*(int *)(e + 0x2c) = hit;
			WaitFrames(0xa);
			break;
		}
	}
	_Actor_SetAnim(act[0], 4);
	_Actor_SetAnim(act[1], 4);
	StopTask((void *)Func_8099d18);
	_PlaySound(0x87);
	WaitFrames(0xf);
	_PlaySound(0x87);
	WaitFrames(0xf);
	v[0] = *(int *)(e + 8);
	v[1] = *(int *)(e + 0xc) + (0x80 << 13);
	v[2] = *(int *)(e + 0x10);
	w = arr;
	for (i = 19; i >= 0; i--) {
		q = CreateParticleActor(0x11d, v[0], v[1], v[2]);
		*w++ = q;
		if (q != 0) {
			_Actor_SetScript(q, L9f0d4);
			*(int *)(q + 0x30) = Random() + (0x80 << 10);
			*(int *)(q + 0x34) = 0x80 << 10;
			*(unsigned char *)(q + 0x55) = 0;
			Func_8096bec(q, Random() * 24 + (0x80 << 12), Random());
		}
	}
	_PlaySound(0x83);
	_DeleteActor(act[0]);
	_DeleteActor(act[1]);
	_Actor_SetColorswap(e, *((unsigned char *)g + 0x44));
	_Actor_SetScript(e, (unsigned char *)g[15]);
	*(void **)(e + 0x6c) = (void *)g[14];
	*p55 = 3;
	*(int *)(e + 0x28) = 0xa0 << 12;
	*(int *)(e + 0x44) = 0x3333;
	*p22 = 0;
	*(int *)(a + 0x6c) = 0;
	_Actor_SetColorswap(a, 0);
	if (*(signed char *)((char *)g + 0x34) != 0) {
		k = 0;
		while (*(int *)(e + 0x28) >= 0) {
			WaitFrames(1);
			k++;
			if (k > 0x59)
				break;
		}
		WaitFrames(1);
		k = 0;
		while (*(int *)(e + 0x28) < 0) {
			WaitFrames(1);
			k++;
			if (k > 0x59)
				break;
		}
		Func_809a6b8(e);
		Func_809748c();
		WaitFrames(0x1e);
	} else {
		Func_809748c();
	}
}
