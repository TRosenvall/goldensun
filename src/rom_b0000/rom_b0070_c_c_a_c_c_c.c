/* WHOLE-FILE CONVERSION of asm/rom_b0000/rom_b0070_c_c_a_c_c_c.s -- both of its
 * functions, NO SPLIT, NO LINKER CHANGE, no data.  Whole-file object compare:
 * 532 bytes, 230 encodings and 29 relocations identical.
 *
 *   Func_80b2ffc     35 insns    84 bytes   37 encodings /  4 relocations
 *   Func_80b3050    182 insns   448 bytes  193 encodings / 25 relocations
 *
 * BOTH FUNCTIONS WERE PARKED BEFORE THIS, AND ONE PARK'S STATED REQUIREMENT WAS
 * WRONG.  src/non_matching/rom_b0000/80b3050.c said landing it "needs the file
 * SPLIT and a stage1.ld change at line 1416".  It does not: datacheck.py reports no
 * data sections, and stage1.ld:1416 already reads `...rom_b0070_c_c_a_c_c_c.o(.text)`,
 * which is the form landed siblings use.  The generic `asm/%.o: src/%.c` rule covers
 * it.  Requirement retired.
 *
 * ================================================================
 * Func_80b2ffc -- AND A PARK CONCLUSION THAT WAS RIGHT ABOUT THE BOUNDARY AND
 * WRONG ABOUT THE CURE
 * ================================================================
 *
 * Its park concluded the argument-fill blocker was "the argument-temporary
 * boundary".  Ladder: park as shipped 13 of 37 -> Random() moved INSIDE the
 * argument list 8 -> struct-typed state 0.
 *
 * WRITING THE CALL INSIDE THE ARGUMENT LIST IS NECESSARY BUT NOT SUFFICIENT.  It
 * removes the `mov r0, r3` and leaves 8.  THE FINISHER IS THAT THE FIRST ARGUMENT
 * MUST BE A COMPONENT_REF + ARRAY_REF, NOT A HAND-CAST:
 *
 *     s->sprites[idx]                                  <- keeps `ldr r0,[r7,r3]`
 *     *(void **)(base + (idx<<2) + (0x8a<<1))           <- fold REASSOCIATES it
 *
 * The hand-cast is reassociated by `fold` into `add r3,r7,r3 / ldr r0,[r3]`, while
 * the struct form keeps the ROM's reg+reg `ldr` AND gcc's own
 * `mov r2,#0x8a / lsl r2,#1` synthesis of 0x114.  That is batch 281's
 * reassociation lever confirmed at a second site.
 *
 * ================================================================
 * Func_80b3050 -- THREE LEVERS, ALL AGAINST rank_for_schedule's INSN_LUID
 * FALL-THROUGH
 * ================================================================
 *
 * Ladder: 10 -> 8 -> 7 -> 0.
 *
 * 1. `StartTask(f, 0xc8 << 4)` -> `prio = 0xc8; prio <<= 4;`.  THIS IS BATCH 281'S
 *    mov/lsl SPLIT USED FOR THE OPPOSITE PURPOSE -- there it separated the pair so
 *    a pinned `mov r0` could slot between them; here it is about LUID ORDER.  A
 *    late split of one `mov r1,#0xc80` lands AFTER the pool load of arg0 in the
 *    chain; two source statements land BEFORE it, and arm_adjust_cost's
 *    cost-1-for-a-load-feeding-a-call cannot rescue the pool load.
 *
 * 2. `movs r7,#0x17` MUST COME AFTER THE PREHEADER'S giv INIT, AND NO SOURCE
 *    STATEMENT CAN DO THAT.  move_movables and strength_reduce both
 *    `emit_insn_before (loop_start)`, so EVERY loop.c preheader insn outranks every
 *    source one in LUID.  The escape is check_dbra_loop: writing loops 2 and 3 as
 *    `for (k = 0; k < 0x18; k++)` over `s->ents[k]` makes it re-emit the counter
 *    init LAST and reverse the loop to the ROM's `subs r7,#1 / cmp r7,#0 / bge`
 *    while the pointer givs keep incrementing.  FIXED THREE OF THE FOUR WINDOWS AT
 *    ONCE.  check_dbra_loop is already in docs/elevation.md (~5912/20281/21069);
 *    this is a new site and the first where it clears three windows.
 *
 * 3. LOOP 3'S TWO giv INITS ARE EMITTED LAST-SOURCE-REFERENCE-FIRST
 *    (docs/elevation.md:14566).  Body order `s->ents[k].f45` then `&s->ents[k]`
 *    gave the ent-base giv the lower LUID; hoisting `e = &s->ents[k]` to the top of
 *    the body flips it, and the r6/r5 colouring follows the creation order for free.
 *
 * TWO SMALLER CONFIRMATIONS from the park, both held: store order is SOURCE order
 * for the e->f2c / e->f28 pair, and the ROM's `mov r3,#0xff / ... / add r3,#0x15`
 * proves the sentinel is written as 255 rather than -1, since cse's related_value
 * derives 0x114 from 0xff and -1 cannot reach it.
 *
 * REUSING ONE COUNTER ACROSS ALL THREE LOOPS IS REQUIRED (separate counters
 * measured 26).  No per-file Makefile flag override applies to this stem.
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

extern void _Func_809b804(void *p);

void Func_80b2ffc(void)
{
	struct State *s;
	struct Ent *e;
	int i;
	int idx;

	s = iwram_3001f2c;
	e = s->ents;
	i = 0x17;
	do {
		_Func_809b804(e);
		i--;
		e++;
	} while (i >= 0);
	idx = s->f3ab;
	if (idx != -1)
		_Sprite_SetColorswap(s->sprites[idx], (Random() * 7) >> 16);
}

void Func_80b3050(int idx)
{
	struct State *s;
	struct Ent *e;
	int saved;
	int v[3];
	int k;
	int prio;

	s = iwram_3001f2c;
	saved = s->box->f5;
	s->f3ab = 0xff;
	s->box->f5 = 0xd;
	_PlaySound(sfxtbl[s->lang]);
	Func_80b0840(0x202108);
	_Sprite_SetAnimSpeed(s->sprites[idx], 0);
	WaitFrames(0x14);
	prio = 0xc8;
	prio <<= 4;
	StartTask(Func_80b2ffc, prio);
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
	for (k = 0; k < 0x18; k++) {
		if (s->ents[k].f45 != 0)
			s->ents[k].f40 = 2;
	}
	WaitFrames(0x14);
	_PlaySound(0x7e);
	s->f3ab = 0xff;
	_Sprite_SetColorswap(s->sprites[idx], 0);
	WaitFrames(0x14);
	for (k = 0; k < 0x18; k++) {
		e = &s->ents[k];
		if (e->f45 != 0)
			_Func_809bb34(e);
	}
	StopTask(Func_80b2ffc);
	_Sprite_SetAnimSpeed(s->sprites[idx], 0x10);
	Func_80b0894();
	WaitFrames(0x1e);
	s->box->f5 = saved;
}
