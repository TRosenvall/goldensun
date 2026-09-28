/* Func_80b7738 (StepCombatantActors) -- 0x080b7738, the ONLY function in
 * asm/rom_b5000/rom_b7410_a_a_c_c_a_a_a.s (datacheck: no data;
 * `grep -ci func_start` = 1).  Whole-file conversion when it closes.
 *
 * NON-MATCHING: 45 encodings of 205 differ (objcmp).
 * THIS IS A TRUE DISTANCE: 428 bytes against 428 and 205 encodings against 205,
 * so the streams are aligned and every counted difference is real.  What is left
 * is register BINDING, not shape: no instruction is missing, extra or reordered
 * except the two noted below.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_b5000/80b7738.c \
 *     asm/rom_b5000/rom_b7410_a_a_c_c_a_a_a.s --whole
 *
 * FIVE LEVERS GOT IT FROM 185/205 (wrong length) TO 45/205 (right length).
 * Each was measured by putting it in alone; the ladder is in order.
 *
 * 1. THE FIRST LOOP IS A GOTO LOOP.  185 -> 164.  The reference recomputes the
 *    byte offset with `lsl r3, r7, #1` at the bottom of the loop and loads
 *    buf[i] TWICE per iteration (once for the `!= 0xff` test, once for the call
 *    argument).  Both are the signature of a loop loop.c never saw: written
 *    `while (buf[i] != 0xff) { ... i++; if (i > 0xd) break; }` gcc strength-
 *    reduces the access to a walking pointer and commons the two loads.
 *    docs/elevation.md's "A ROM loop that hoists nothing was a goto loop" holds
 *    here, with the addition that the DOUBLE LOAD is the second tell.
 *
 * 2. *** THE SECOND AND THIRD LOOPS MUST BE `do { } while` UNDER AN EXPLICIT
 *    GUARD, NOT `for`. ***  This is the new mechanism and it is worth carrying
 *    out of this batch.  Written `for (i = 0; i < n; i++)` gcc REVERSES both
 *    loops -- `sub r7, #1 / cmp r7, #0 / bne` where the ROM has
 *    `add r7, #1 / cmp r7, r9 / blt` -- and the guard collapses from the ROM's
 *    `mov r7, #0 / mov r9, r0 / cmp r7, r9 / bge` to `cmp r0, #0 / ble`.
 *    Written
 *        i = 0;
 *        if (i < n) {
 *            do { ... i++; } while (i < n);
 *        }
 *    the reversal does not happen and the guard keeps the ROM's four
 *    instructions.
 *
 *    MECHANISM, read out of /opt/camelot-gcc/gcc-2.96/gcc/loop.c.
 *    check_dbra_loop reverses through one of two paths (loop.c:8073 onwards).
 *    The "vanilla" path needs `GET_CODE (comparison_value) == CONST_INT`; the
 *    bound here is the variable `n`, so that path is closed.  The only other
 *    path is loop.c:8097
 *        else if (add_val == 1 && loop->vtop
 *                 && (bl->biv_count == 0 || no_use_except_counting))
 *    and `loop->vtop` is the NOTE_INSN_LOOP_VTOP that stmt.c emits ONLY when it
 *    rotates a loop's entry test to the bottom.  A do-while has no entry test to
 *    rotate, so it gets no VTOP note and a variable-bound loop cannot be
 *    reversed at all.  `no_use_except_counting` is 1 here on the SECOND loop
 *    pass (-frerun-loop-opt is on at -O2): the dump
 *    `Insn 678: possible biv, reg 170, const =2` shows the first pass's
 *    strength-reduced address giv coming back as a BIV, which leaves the
 *    counter's class with giv_count == 0 and hence no use except counting.
 *    Diagnostic confirmation: `-fno-rerun-loop-opt` alone takes the `for`
 *    version from 201 instructions to 202 of 203 -- i.e. the second loop pass IS
 *    the reverser.  The do-while spelling gets the same result with no flag.
 *    GENERAL RULE: a ROM count-UP loop with a VARIABLE bound and a counter used
 *    for nothing else is a `do { } while` under its own `if` guard; a `for` will
 *    be reversed.
 *
 * 3. THE {1,2} PAIR IS AN `int t[2]` ARRAY READ ONCE PER ITERATION INTO A LOCAL.
 *    The reference keeps `mov r3, #3 / and r3, r1` in the loop pre-header.  Two
 *    plain `int` locals assigned 1 and 2 in the two arms of the `if` DROP that
 *    `and` (4 instructions short, 166 of 205): combine's
 *    set_nonzero_bits_and_sign_copies ORs the nonzero_bits of every set of a
 *    pseudo, 1|2 == 3, and simplify_and_const_int then deletes `x & 3` as
 *    redundant.  A stack ARRAY element has unknown nonzero_bits, so the mask
 *    survives -- and the frame arithmetic agrees: 0x2c = buf[14] (28) + t[2] (8)
 *    + two reload slots (8).  But the array must be read ONCE, at the top of the
 *    loop body, into a local (`v1 = t[0];`): read separately in the two switch
 *    arms it is two loads that no pass commons (mutually exclusive blocks), and
 *    gcc then hoists only one of them and keeps the constant 3 alive in a high
 *    register (163 of 205, 215 instructions).  Read once at the top it is
 *    loop-invariant, loop.c hoists load + `and` + `lsl` into the pre-header, and
 *    the switch arms use the hoisted values exactly as the ROM does.
 *    147 of 205 at this point.
 *
 * 4. *** A SEPARATE LOCAL FOR THE ACTOR IN THE SECOND AND THIRD LOOPS. ***
 *    147 -> 49, and this is what made the length exact.  The first loop needs
 *    the GetBattleActor result in a callee-saved register (it survives the
 *    `Func_80b7994` call), so the ROM has `mov r6, r0`.  The other two loops use
 *    it only for `->anim`, so the ROM leaves it in r0: `cmp r0, #0 / beq /
 *    ldr r5, [r0]`.  Sharing ONE local `b` across all three loops makes gcc
 *    copy in all three (`mov r5, r0 / cmp r5, #0`), two instructions too many.
 *    A second local used only by loops 2 and 3 removes both.  Note this is NOT
 *    the inert kind of declaration-order change: all six orderings of the eight
 *    pointer locals measure identically at 49 (confirming
 *    docs/elevation.md's "declaration-order permutation is INERT when locals are
 *    all register-resident"); it is the NUMBER of locals that matters.
 *
 * 5. `q = b->rec; if (q != 0)` RATHER THAN `if (b->rec != 0)`, with `q` re-read
 *    inside.  49 -> 45.  It frees r0 across the Func_80b7f70 result's live
 *    range, so the ROM's `cmp r0, #0` (no copy) and its late `mov r2, r0 /
 *    add r2, #0x25` come back.  The reference reads `b->rec` twice, so `q` has
 *    two sets; that is the point -- one set and gcc commons the loads.
 *
 * THE RESIDUE, 45 encodings, is TWO register bindings and nothing else:
 *
 *  (a) `sub sp, #0x28` against the ROM's `sub sp, #0x2c` -- ONE 4-byte reload
 *      slot.  The ROM spills BOTH loops' `(v & 3) << 2` (sp+0 and sp+4) and
 *      keeps `v & 3` in r11; we spill only the second loop's and keep the third
 *      loop's pair in r10 and r11, because in our build `buf` (r10) is dead by
 *      the third loop's pre-header and the ROM's build left r10 alone.  One
 *      global-alloc tie, one differing immediate, and it shifts every stack
 *      offset in the function by 4 -- which is most of the 45.
 *  (b) `v` in r4 and `q` in r1 against the ROM's r1 and r2, in the first loop.
 *
 * MEASURED INERT (all still 45 or worse, none changed the length):
 *  - all declaration-order permutations of the eight pointer locals (45);
 *  - `Func_80b7f70` declared `int` with a cast at the use, `Func_80b7994`
 *    declared `int`, and both together (45 each) -- the callee-return-type lever
 *    does not reach this file;
 *  - `v` renamed onto the existing `k` (45); `v` written as a ternary (46);
 *  - `b->rec->st` inline instead of a named `q` (49);
 *  - swapping the two stores in the innermost `if` (50);
 *  - an explicit `unsigned short *w` walked with `*w++` in loops 2 and 3 (168 of
 *    205, 213 instructions) -- the ROM's walking pointer is loop.c's giv, not a
 *    source pointer;
 *  - all three loops as goto loops (204 of 205): loops 2 and 3 then get no
 *    invariant hoisting at all and the pre-header `and`/`lsl` disappear.
 *
 * NO SHIMS in this draft: no pin, no barrier, no volatile, no .equ, no .sym.
 *
 * STRUCTS: the four declared here are local inventions sized off the reference's
 * offsets (Part's byte 9 carries the 2-bit field, Anim +0xc/+0x50/+0x54,
 * Actor +0/+0x24, Rec +6).  docs/structs.md should be checked for an existing
 * name before this lands.
 *
 * The 2-bit `unsigned char` bitfield for the SImode `~0xc` (mov #0xd / neg /
 * and / orr) worked first time and needed no help, confirming that lever.
 */
/* Func_80b7738 (StepCombatantActors) -- 0x080b7738, only function in
 * asm/rom_b5000/rom_b7410_a_a_c_c_a_a_a.s (no data, 1 func_start).
 * `pop {r0}` epilogue -> void.
 * Frame 0x2c: unsigned short buf[14] at sp+0x10, int t[2] at sp+8, two reload
 * slots at sp+0/sp+4.
 * The `mov rX, #0xd / neg rX, rX / and / orr` triple is the documented 2-bit
 * unsigned char bitfield store (SImode ~0xc).
 */
struct Part {
	unsigned char pad[9];
	unsigned char lo : 2;
	unsigned char sel : 2;
	unsigned char hi : 4;
};

struct Anim {
	unsigned char pad0[0xc];
	int flag;
	unsigned char pad1[0x40];
	struct Part *part;
	unsigned char kind;
};

struct Rec {
	unsigned char pad[6];
	unsigned char st;
};

struct Actor {
	struct Anim *anim;
	unsigned char pad[0x20];
	struct Rec *rec;
};

extern int iwram_3001e80;
extern struct Actor *GetBattleActor(int id);
extern int Func_80b6c08(int kind, unsigned short *buf);
extern void Func_80b7994(struct Actor *b);
extern unsigned char *Func_80b7f70(struct Anim *a, int flag);

void Func_80b7738(void)
{
	unsigned short buf[14];
	struct Actor *b;
	struct Actor *b2;
	struct Anim *a;
	struct Part *e;
	struct Part **pp;
	struct Part *o;
	struct Rec *q;
	unsigned char *r;
	short *g;
	int i;
	int k;
	int n;
	int v;
	int t[2];
	int v1;

	Func_80b6c08(3, buf);
	i = 0;
	if (buf[i] == 0xff)
		goto done;
lp:
	{
		b = GetBattleActor(buf[i]);
		if (b != 0) {
			a = b->anim;
			Func_80b7994(b);
			q = b->rec;
			if (q != 0) {
				r = Func_80b7f70(a, 0);
				if (r != 0) {
					v = 0;
					if (a->flag != 0)
						v = 9;
					q = b->rec;
					if (q->st != v) {
						q->st = v;
						r[0x25] = 1;
					}
				}
			}
		}
	}
	i++;
	if (i > 0xd)
		goto done;
	if (buf[i] != 0xff)
		goto lp;
done:
	g = *(short **)&iwram_3001e80;
	if (g[0x1b] >= 0) {
		t[0] = 1;
		t[1] = 2;
	} else {
		t[0] = 2;
		t[1] = 1;
	}
	n = Func_80b6c08(1, buf);
	i = 0;
	if (i < n) {
	    do {
		v1 = t[0];
		b2 = GetBattleActor(buf[i]);
		if (b2 != 0) {
			a = b2->anim;
			switch (a->kind & 0xf) {
			case 1:
				e = a->part;
				e->sel = v1;
				break;
			case 2:
				pp = (struct Part **)a->part;
				for (k = 3; k >= 0; k--) {
					o = *pp++;
					if (o != 0)
						o->sel = v1;
				}
				break;
			}
		}
		i++;
	    } while (i < n);
	}
	n = Func_80b6c08(2, buf);
	i = 0;
	if (i < n) {
	    do {
		v1 = t[1];
		b2 = GetBattleActor(buf[i]);
		if (b2 != 0) {
			a = b2->anim;
			switch (a->kind & 0xf) {
			case 1:
				e = a->part;
				e->sel = v1;
				break;
			case 2:
				pp = (struct Part **)a->part;
				for (k = 3; k >= 0; k--) {
					o = *pp++;
					if (o != 0)
						o->sel = v1;
				}
				break;
			}
		}
		i++;
	    } while (i < n);
	}
}
