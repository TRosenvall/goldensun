/* Field_Cloak  --  0x08099838, was asm/rom_8a000/rom_97b54_c_a_a_a.s (this
 * function alone), so it converts whole.
 *
 * OUT OF A PARK written one batch earlier at 3 of 104, whose residue was one
 * rank_for_schedule dependent-count tie before `bl StartTask` (`mov r0,r5`
 * winning with three forward dependents). The ONLY change is declaring StartTask
 * `int` instead of `void` -- batch 286's callee-return-type lever: a void call
 * does not set r0 in RTL, so the later write to r0 added a dependent to the
 * argument copy. Runtime-built gState+0x24c (`g += 0x93 << 2`) from the park.
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
extern int StartTask(void *task, int priority);
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
