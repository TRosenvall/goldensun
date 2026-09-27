/* Field_Cloak -- NON-MATCHING, 3 encodings of 104 (objcmp: ENCODINGS differ in 3
 * place(s), ref 104 / ours 104, first at index 69).  LENGTH EXACT, relocations identical.
 * The .s (asm/rom_8a000/rom_97b54_c_a_a_a.s) holds only this function -- no split.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_8a000/Field_Cloak.c asm/rom_8a000/rom_97b54_c_a_a_a.s --func Field_Cloak
 *
 * Template: Func_8099738 in src/rom_8a000/rom_97b54_a_c_c_c_c.c (same actor/Obj/Sfx
 * shape, same 2-frame blink loop).  THE LEVER that took it from 62 differing lines to 3:
 * the gState+0x24c store built at runtime, `g = gState; g += 0x93 << 2; *(short *)g = 1;`
 * reusing the SAME `g` local as the top `g + (0xfa << 1)` fetch.  Spelled inline
 * (`*(short *)(gState + (0x93 << 2))`) it folds to `ldr =gState+0x24c` AND swaps the
 * loop counter / constant-2 pseudos between r7 and r8.
 *
 * THE RESIDUE, one sched2 tie after the blink loop:
 *     rom  strb r6,[r3] / lsl r1,#4 / mov r0,r5 / bl StartTask
 *     ours mov r0,r5 / strb r6,[r3] / lsl r1,#4 / bl StartTask
 * From -fsched-verbose=5 .23.sched2 (block 2): after `r1=0xc8` issues, the ready list
 * holds 238 (the s->f25 strb), 417 (the lsl) and 245 (r0=r5), ALL priority 43.
 * rank_for_schedule then prefers the insn with more forward dependents: 245 has three
 * (248 the call, 262 the second call via r5 which clobbers r0, 267 `add r5,r3,r2` which
 * overwrites r5) against two for 238 and 417, so 245 wins.  The ROM's order needs 245 to
 * lose that tie, i.e. a different dependence count or a priority change on 238.
 *
 * INERT (each measured by tryc, all still 3 lines): a `void (*fn)(void) = Func_8099678` local for both
 * uses; StartTask typed as taking `void (*)(void)`; its priority as unsigned short;
 * `v->f05 = i = 0`.  Worse (tryc differing lines): fn assigned before `i = 0` (63 lines), a `do {} while (0)`
 * barrier before StartTask (6), f25 stored through `*(unsigned char *)&s->f25` (68).
 */
extern unsigned char gState[];
extern unsigned char *iwram_3001ebc;

struct Sfx {
	unsigned char pad00[5];
	unsigned char f05;
};

struct Obj {
	unsigned char pad00[0x25];
	unsigned char f25;
	unsigned char f26;
	unsigned char pad27[1];
	struct Sfx *f28;
};

extern unsigned char *GetFieldActor(int id);
extern void _PlaySound(int id);
extern void StartTask(void *task, int priority);
extern void Func_8099678(void);
extern void Func_8099738(void);
extern void _Actor_SetAnim(unsigned char *a, int n);
extern void WaitFrames(int n);

void Field_Cloak(void)
{
	unsigned char *m;
	unsigned char *g;
	unsigned char *a;
	struct Obj *s;
	struct Sfx *v;
	unsigned int i;
	short *f;

	m = iwram_3001ebc;
	g = gState;
	a = GetFieldActor(*(int *)(g + (0xfa << 1)));
	s = *(struct Obj **)(a + 0x50);
	v = s->f28;
	_PlaySound(0x82);
	_Actor_SetAnim(a, 0);
	*(int *)(a + 0x6c) = 0;
	for (i = 0; i <= 9; i++) {
		v->f05 = 7;
		s->f25 = 1;
		s->f26 = 2;
		WaitFrames(2);
		s->f25 = 1;
		s->f26 = 0;
		WaitFrames(2);
	}
	i = 0;
	v->f05 = i;
	s->f26 = 2;
	s->f25 = 1;
	StartTask(Func_8099678, 0xc8 << 4);
	g = gState;
	g += 0x93 << 2;
	*(short *)g = 1;
	Func_8099678();
	f = (short *)(m + (0xbf << 1));
	if (*f == 0x2092) {
		Func_8099738();
		*f = i;
	}
}
