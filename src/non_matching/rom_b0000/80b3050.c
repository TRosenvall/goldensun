/* Func_80b3050 -- NON-MATCHING, 10 ENCODINGS OF 193.  SIZE IDENTICAL, RELOCATIONS
 * IDENTICAL, INSTRUCTION COUNT IDENTICAL.  182 instructions.
 *
 * Blocker class: sched2 LUID TIE-BREAK.  All ten differences are FOUR SWAPS OF TWO
 * ADJACENT, INDEPENDENT, IDENTICAL-PRIORITY INSTRUCTIONS -- same instruction set,
 * same registers, one position apart.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_b0000/80b3050.c \
 *     asm/rom_b0000/rom_b0070_c_c_a_c_c_c.s --func Func_80b3050
 * The reference holds TWO functions.  Its sibling Func_80b2ffc is ALSO parked
 * (src/non_matching/rom_b0000/80b2ffc.c, 13 of 36), so landing this needs the file
 * SPLIT and a stage1.ld change at line 1416 -- neither done.
 *
 * ================================================================
 * THE FINDING HERE IS FOR THE SIBLING PARK, NOT FOR THIS FUNCTION
 * ================================================================
 *
 * **INLINING THE `Random()` CALL INTO THE ARGUMENT LIST IS THE FIX FOR THE
 * ARGUMENT-FILL BLOCKER THAT src/non_matching/rom_b0000/80b2ffc.c IS PARKED ON.**
 *
 * That park's whole note is that the ROM finishes the SECOND argument
 * (`(Random()*7)>>16`) before loading the FIRST (`e->spr`), so the load lands
 * straight in r0; that ours loads into r3 and copies; and that hoisting the multiply
 * into a local does not help.  It concludes this is "the argument-temporary boundary
 * recorded in docs/elevation.md".
 *
 * THAT READING IS RIGHT ABOUT THE BOUNDARY AND WRONG ABOUT THE CURE.  Writing
 * `r = Random();` as its own statement lets sched2 hoist the `ldr r0, [r5]` ABOVE
 * the multiply; writing the call INSIDE THE ARGUMENT LIST forces the load after it,
 * because the call clobbers the register and gcc must order the fill around it.
 * That removed the extra `mov r0, r3` and took 14 -> 12 in one edit.
 *
 * IT IS THE SAME CALL WITH THE SAME TWO ARGUMENTS IN BOTH FUNCTIONS.  Func_80b2ffc's
 * park should be re-attempted on this before anything else is tried there.
 *
 * ================================================================
 * THE RESIDUE -- four windows, all the same shape
 * ================================================================
 *
 *   window            ROM                                  ours
 *   StartTask fill    mov r1,#0xc8 / lsl r1,#4 / ldr r0,=  mov r1,#0xc8 / ldr r0,= / lsl r1,#4
 *   loop-2 preheader  mov r1,#2 / add r2,r8 / mov r7,#0x17 mov r7,#0x17 / mov r1,#2 / add r2,r8
 *   loop-3 preheader  ldr r6,=0x3f5 / mov r5,#0xec         mov r5,#0xec / ldr r6,=
 *   loop-3 preheader  add r6,r8 / add r5,r8 / mov r7,#0x17 add r5,r8 / mov r7,#0x17 / add r6,r8
 *
 * rank_for_schedule falls through to INSN_LUID -- current position in the chain --
 * when priority and dependent count tie, which they do here because ALL CONSUMERS
 * ARE IN THE LOOP BODY, A DIFFERENT BLOCK.  In the ROM's chain the counter's
 * `mov r7, #0x17` sits AFTER loop.c's hoisted invariant and the giv init; in ours it
 * sits before, which is where a source-level `k = 0x17` in the preheader lands.  No
 * source position was found that puts it later.
 *
 * MEASURED, normalised, in disagreeing regions of 188:
 *   as shipped                                                   8
 *   `r = Random();` as a separate statement, then (r*7)>>16      14, 189 lines
 *       ^ THE SINGLE BIGGEST LEVER, and it runs the WRONG way -- see above
 *   fully inlined into the call                                  12
 *   ... plus `k = 0;` as a statement ahead of `e = s->ents`       8
 *   StartTask(..., 0xc80) instead of 0xc8 << 4                    8  inert
 *   for (k = 0x17; ...) init inside the for                       8  inert
 *   SEPARATE counters/pointers per loop (k2,k3,e2,e3)            26  -- REUSING ONE
 *       COUNTER ACROSS ALL THREE LOOPS IS REQUIRED
 *   do { ... } while (k >= 0) for loops 2 and 3                   8  inert
 *   -fno-strict-aliasing                                          8  inert
 *   -fno-rerun-cse-after-loop                                    12
 *   -fno-schedule-insns2                                         54
 *   -fno-expensive-optimizations                                 55
 *   -fno-gcse                                                    65
 *
 * TWO SMALLER CONFIRMATIONS: store order is SOURCE order for the e->f2c / e->f28
 * pair (0x2c first); and the ROM's `mov r3, #0xff / ... / add r3, #0x15` PROVES THE
 * SENTINEL IS WRITTEN AS 255, NOT -1 -- cse's related_value derives 0x114 from 0xff,
 * which -1 cannot reach.
 *
 * No per-file Makefile flag override applies to this stem.
 *
 * NEXT: the sibling, using the inlining result above.  This function itself is four
 * LUID ties and wants .23.sched2's chain order read rather than more spellings.
 */
struct Ent {
	void *spr;
	unsigned char pad004[0x28 - 4];
	int f28;
	int f2c;
	unsigned char pad030[0x40 - 0x30];
	signed char f40;
	unsigned char pad041[0x45 - 0x41];
	signed char f45;
	unsigned char pad046[2];
};

struct Box {
	unsigned char pad00[5];
	unsigned char f5;
};

struct State {
	unsigned char pad000[0x114];
	void *sprites[8];
	short ax[8];
	short ay[8];
	unsigned char pad154[0x380 - 0x154];
	struct Box *box;
	unsigned char pad384[0x3aa - 0x384];
	signed char lang;
	signed char f3ab;
	unsigned char pad3ac[0x3b0 - 0x3ac];
	struct Ent ents[24];
};

extern struct State *iwram_3001f2c;
extern signed char sfxtbl[] __asm__(".Lb4ab2");

extern void Func_80b0840(int addr);
extern void Func_80b04c4(void);
extern void Func_80b0894(void);
extern void Func_80b2ffc(void);
extern void Func_80b2f4c(void);
extern void _PlaySound(int sfx);
extern void WaitFrames(int n);
extern void StartTask(void (*fn)(void), int prio);
extern void StopTask(void (*fn)(void));
extern void _Sprite_SetAnimSpeed(void *sprite, int speed);
extern void _Sprite_SetColorswap(void *sprite, unsigned int v);
extern unsigned int Random(void);
extern void _Func_809ba90(void *p, int a, int b, int c);
extern void _Func_809ba7c(void *p, void (*fn)(void));
extern void _Func_809ba70(void *p, int a);
extern void _Func_809bb34(void *p);

void Func_80b3050(int idx)
{
	struct State *s;
	struct Ent *e;
	int saved;
	int v[3];
	int k;

	s = iwram_3001f2c;
	saved = s->box->f5;
	s->f3ab = 0xff;
	s->box->f5 = 0xd;
	_PlaySound(sfxtbl[s->lang]);
	Func_80b0840(0x202108);
	_Sprite_SetAnimSpeed(s->sprites[idx], 0);
	WaitFrames(0x14);
	StartTask(Func_80b2ffc, 0xc8 << 4);
	v[0] = s->ax[idx] << 16;
	v[2] = (s->ay[idx] << 16) + 0xfff40000;
	k = 0;
	e = s->ents;
	for (; k <= 0x11; k++) {
		_Func_809ba90(e, 0x8e << 1, v[0], v[2]);
		_Func_809ba7c(e, Func_80b2f4c);
		_Func_809ba70(e, 7);
		_Sprite_SetColorswap(e->spr, (Random() * 7) >> 16);
		e->f2c = 0xb333;
		e->f28 = 0xb333;
		WaitFrames(3);
		if (k == 5)
			s->f3ab = idx;
		e++;
	}
	Func_80b04c4();
	e = s->ents;
	k = 0x17;
	for (; k >= 0; k--, e++) {
		if (e->f45 != 0)
			e->f40 = 2;
	}
	WaitFrames(0x14);
	_PlaySound(0x7e);
	s->f3ab = 0xff;
	_Sprite_SetColorswap(s->sprites[idx], 0);
	WaitFrames(0x14);
	e = s->ents;
	k = 0x17;
	for (; k >= 0; k--, e++) {
		if (e->f45 != 0)
			_Func_809bb34(e);
	}
	StopTask(Func_80b2ffc);
	_Sprite_SetAnimSpeed(s->sprites[idx], 0x10);
	Func_80b0894();
	WaitFrames(0x1e);
	s->box->f5 = saved;
}
