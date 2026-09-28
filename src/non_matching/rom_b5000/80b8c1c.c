/* Func_80b8c1c (RunResultPhase) -- 0x080b8c1c, the ONLY function in
 * asm/rom_b5000/rom_b8228_c_a_c_a_c_c_c_a.s (datacheck: no data;
 * `grep -ci func_start` = 1).  Whole-file conversion when it closes.
 *
 * NON-MATCHING: 140 encodings of 184 differ (objcmp).
 * NOT A TRUE DISTANCE: 408 bytes against 412, so the stream is two instructions
 * short and the tail is counted as differing whether or not it is.  The
 * relocation list is the better guide: the first nineteen `bl` targets land at
 * the reference's offsets up to 0x76, then drift by 2, 4 and 6 bytes across the
 * three loops.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_b5000/80b8c1c.c \
 *     asm/rom_b5000/rom_b8228_c_a_c_a_c_c_c_a.s --whole
 *
 * THE SHAPE IS SETTLED and is the useful part of this draft; what is left is
 * strength-reduction and spill choices.  The layout was read off the frame:
 * `sub sp, #0x5c` = a 0x54-byte argument block at sp+8 plus TWO reload slots at
 * sp+0 and sp+4, and 0x54 - 0x34 = 0x20 fixes `unsigned char f34[8][4]` with
 * `short f24[8]` at 0x24 before it.
 *
 * LEVERS THAT PAID, each measured alone from the 145/184 starting draft:
 *
 * 1. `s.f14 = 1;` WRITTEN IN BOTH ARMS of the `in->sa <= 7` test, not once after
 *    it.  The reference materialises the 1 separately per arm (`mov r2, #1 /
 *    mov r8, r2` in one, `mov r3, #1` in the other) and CROSS-JUMPS only the
 *    `str`.  One statement after the `if` gives a single `mov` in the join block
 *    and loses the `mov r8, r2` -- which is gcc keeping the constant alive
 *    across the intervening call in that arm only.
 *
 * 2. `d = s.f34[i];` HOISTED TO THE TOP OF THE MIDDLE LOOP BODY, above the
 *    GetBattleActor call, even though it is used only inside the `if`.  143
 *    against 145 and, more importantly, it is what brings the instruction count
 *    from 174 to 180.  It gives `i * 4` an unconditional use so loop.c
 *    strength-reduces THAT giv (the reference's `mov r5, #0` / `add r5, #4`
 *    offset register, with `add r3, r5, r6 / add r2, #0x34` inside the `if` --
 *    the "member array keeps base and offset in separate registers" shape) and
 *    leaves `s.f24[i]` to be recomputed with `lsl r3, r7, #1 / add r3, #0x24`.
 *    Written inside the `if` gcc reduces the OTHER giv instead and the two
 *    instruction sequences swap.
 *
 * 3. `if (in->s0 <= 4) v = 0x80 << 6;` after `v = 0xffffe000;`, not a `?:`.
 *    The reference loads -0x2000 unconditionally and overwrites it in the
 *    fall-through arm.
 *
 * 4. The `f->cur == v` test written as TWO FULL ARMS, each with its own
 *    `f->step = 0x28; WaitFrames(0x28);`.  The reference does not cross-jump
 *    them; a single arm with a conditional `f->cur = v` is three instructions
 *    shorter.
 *
 * MEASURED INERT OR WORSE:
 *  - the first `Func_80b6b40` mode argument written as an inline
 *    `s.fc > 0x7f ? 2 : 1` instead of a local: 147 of 184 and FOUR instructions
 *    shorter (176).  The reference materialises the mode straight into r0 in each
 *    arm AND duplicates `add r7, sp, #0x2c` in both, which neither spelling
 *    reproduces;
 *  - a named `short *u = s.f24;` shared by all three Func_80b6b40 calls (142 of
 *    184, 174 instructions);
 *  - the inner copy loop as `*d++ = (*q++)->b5;` rather than `d[j] = q[j]->b5;`
 *    (144 either way -- gcc reduces the indexed form to the reference's
 *    `ldmia r1!, {r3}` on its own);
 *  - an explicit `k += 4` offset local for `s.f34` (163 of 184);
 *  - the middle loop as `do { } while` under an explicit guard (165 of 184) --
 *    the lever that fixed Func_80b7738's loops makes this one worse, because this
 *    loop's bound is a MEMORY field reloaded every iteration and was never a
 *    reversal candidate;
 *  - the last loop as `do { } while` under a guard (140, unchanged).
 *
 * THE RESIDUE is two instructions, and it is the SAME SHAPE AS Func_80b5f0c's:
 * the reference's last loop pre-header holds
 *      add r5, sp, #8 / ldr r3, [r5, #0x14] / mov r2, r5 / cmp r3, #0 / beq /
 *      mov r6, #0x24
 * and then spills the block address TWICE inside the loop
 *      str r2, [sp, #4] / ldrsh r0, [r2, r6] / str r2, [sp] / bl Func_80b8000 /
 *      ldr r1, [sp, #4] / ldr r3, [r1, #0x14] / ... / ldr r2, [sp]
 * -- two distinct pseudos both holding `&s`, which is what the two reload slots
 * at sp+0 and sp+4 are for.  We produce one pseudo and one slot, hence
 * `sub sp, #0x54` and two instructions fewer.  Func_80b5f0c is two instructions
 * short for the same reason in its own last loop (`mov r0, r2 / mov r2, r4`
 * missing), so this is ONE blocker appearing twice in this batch: a loop whose
 * pre-header holds COPIES of the pointer and the bound address rather than
 * computing them into the loop's own registers.  Worth a class file.
 *
 * NO SHIMS in this draft: no pin, no barrier, no volatile, no .equ, no .sym.
 *
 * ONE THING TO CHECK BEFORE LANDING: `_Func_801f200(p[0x41] & ~1)` gives
 * `mov r3, #2 / neg r3, r3 / and r0, r3` in the reference -- an SImode -2 mask
 * on a byte load.  Ours matches, so nothing is needed; but note this is the same
 * family as the SImode `~0xc` bitfield tell and combine did NOT narrow -2 to
 * 0xfe here, which is worth knowing: the narrowing in
 * simplify_and_const_int needs the AND and the zero_extend to be COMBINED into
 * one insn, and with the result feeding a call argument they are not.
 *
 * STRUCTS: the seven declared here are local inventions sized off the
 * reference's offsets.  docs/structs.md should be checked for existing names --
 * `struct Args` in particular is a battle-animation argument block that other
 * banks almost certainly already declare.
 */
/* Func_80b8c1c (RunResultPhase) -- 0x080b8c1c, only function in
 * asm/rom_b5000/rom_b8228_c_a_c_a_c_c_c_a.s (no data, 1 func_start).
 * Frame 0x5c: struct Args at sp+8 (0x54 bytes), two reload slots at sp+0/sp+4.
 */
struct In {
	short s0;
	short s2;
	short s4;
	short s6;
	short s8;
	short sa;
	short sc;
};

struct Args {
	int f0;
	int f4;
	int f8;
	int fc;
	int f10;
	int f14;
	int f18;
	int f1c;
	int f20;
	short f24[8];
	unsigned char f34[8][4];
};

struct Fade {
	int cur;
	int step;
};

struct Part {
	unsigned char pad[5];
	unsigned char b5;
};

struct Body {
	unsigned char pad[0x27];
	unsigned char count;
	struct Part *parts[4];
};

struct Anim {
	unsigned char pad[0x50];
	struct Body *body;
};

struct Actor {
	struct Anim *anim;
};

extern unsigned int iwram_3001f00;
extern unsigned int iwram_3001e74;
extern void WaitFrames(int n);
extern int Func_80b8808(int id);
extern int Func_80b6b40(int mode, short *list);
extern void _Func_801f200(int flags);
extern struct Actor *GetBattleActor(int id);
extern void _Actor_SetAnim(struct Anim *a, int id);
extern void _Actor_SetAnimSpeed(struct Anim *a, int speed);
extern void _Anim_EPowerUp(struct Args *s);
extern void _Anim_Func(struct Args *s);
extern void Func_80b8000(int id);

int Func_80b8c1c(struct In *in)
{
	struct Args s;
	struct Fade *f;
	unsigned char *p;
	struct Anim *act;
	struct Body *bd;
	struct Part **q;
	unsigned char *d;
	int v;
	int i;
	int j;
	int m;

	f = *(struct Fade **)&iwram_3001f00;
	v = 0xffffe000;
	if (in->s0 <= 4)
		v = 0x80 << 6;
	if (f->cur == v) {
		f->step = 0x28;
		WaitFrames(0x28);
	} else {
		f->cur = v;
		f->step = 0x28;
		WaitFrames(0x28);
	}
	s.f0 = in->s8;
	s.f10 = in->sc;
	s.f8 = in->s0;
	s.fc = in->sa;
	if (Func_80b8808(in->s0) < 0)
		return -1;
	if (s.fc > 0x7f)
		i = 2;
	else
		i = 1;
	s.f14 = Func_80b6b40(i, s.f24);
	p = *(unsigned char **)&iwram_3001e74;
	_Func_801f200(p[0x41] & ~1);
	act = GetBattleActor(s.f8)->anim;
	_Actor_SetAnim(act, 3);
	_Actor_SetAnimSpeed(act, 0x10);
	if ((unsigned short)in->sa <= 7) {
		s.f4 = 1;
		Func_80b6b40(1, s.f24);
		s.f14 = 1;
	} else {
		s.f4 = 0;
		Func_80b6b40(2, s.f24);
		s.f14 = 1;
	}
	for (i = 0; i != s.f14; i++) {
		d = s.f34[i];
		bd = GetBattleActor(s.f24[i])->anim->body;
		m = bd->count - 1;
		if (m != 0) {
			q = bd->parts;
			j = 0;
			do {
				d[j] = q[j]->b5;
				j++;
			} while (j != m);
		}
	}
	s.f0 = 0;
	s.f18 = 0;
	_Anim_EPowerUp(&s);
	s.f0 = 1;
	_Anim_EPowerUp(&s);
	s.f0 = 2;
	_Anim_EPowerUp(&s);
	s.f0 = 3;
	_Anim_EPowerUp(&s);
	s.f0 = 0;
	_Anim_Func(&s);
	_Actor_SetAnim(act, 1);
	for (i = 0; i != s.f14; i++)
		Func_80b8000(s.f24[i]);
	Func_80b8000(s.f8);
	return 0;
}
