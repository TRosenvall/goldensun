/* Cluster Func_8099738..Func_8099738 extracted from goldensun/asm/rom_8a000/rom_97b54_a_c_c_c.s.
 *
 * Total .text for this TU = 216 bytes (= 0xd8). Never attempted before batch 275.
 * No pins, no flags, no split. Its callee Func_8099678 is the already-elevated file-mate
 * src/rom_8a000/rom_97b54_a_c_c_c_b.c, which supplied gState and GetFieldActor.
 *
 * THREE LEVERS, 53 differing to 45 to 43 to 6 to exact.
 *
 * 1. A BYTE FIELD PAST THUMB'S 5-BIT `strb` OFFSET WANTS A TYPED STRUCT FIELD, NOT A
 *    HAND-HELD POINTER. `pa = s + 0x25; *pa = 1;` gives `mov rX, s / add rX, #0x25` and puts
 *    the pointers in low callee-saved registers; `s->f25 = 1` gives the ROM's
 *    `mov r3, #0x25 / add r3, r6`, pushes the address pseudos up into r9/r11, and lets reload
 *    REMATERIALISE the single-use constants 7 and 2 inside the loop. That one change made 23
 *    consecutive instructions exact.
 *
 *    Third batch running for "a typed struct field is the fix", but a NEW reason: here the
 *    field form leaves the address computation to the preheader, which frees the low
 *    callee-saved registers the constants need.
 *
 * 2. THE TWO SEQUENTIAL LOOPS SHARE ONE COUNTER VARIABLE. Separate `i` and `j` let cse fold
 *    the second `j = 0` into the zero the loop body stores (`mov r8, r6`), costing an
 *    instruction and swapping r6/r7. One counter reproduces the ROM's two independent
 *    `mov #0`.
 *
 * 3. THE INCREMENT GOES AFTER THE CALL IT PRECEDES IN THE ROM. `i += 1;` written before
 *    `WaitFrames(2)` schedules ahead of the last store; written after the call -- or as a
 *    `for`-increment -- sched2 hoists it to exactly the ROM's slot. Both forms reach exact.
 */
extern unsigned char gState[];

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
extern void StopTask(void *f);
extern void Func_8099678(void);
extern void _Actor_SetAnim(unsigned char *a, int n);
extern void WaitFrames(int n);

void Func_8099738(void)
{
	unsigned char *g;
	unsigned char *g2;
	unsigned char *a;
	struct Obj *s;
	struct Sfx *v;
	unsigned int i;
	int zero;

	g = gState;
	a = GetFieldActor(*(int *)(g + (0xfa << 1)));
	s = *(struct Obj **)(a + 0x50);
	v = s->f28;
	_PlaySound(0x9a);
	StopTask(Func_8099678);
	_Actor_SetAnim(a, 0);
	*(int *)(a + 0x6c) = 0;
	for (i = 0; i <= 4; i += 1) {
		v->f05 = 7;
		s->f25 = 1;
		s->f26 = 2;
		WaitFrames(2);
		s->f25 = 1;
		s->f26 = 0;
		WaitFrames(2);
	}
	for (i = 0; i <= 4; i += 1) {
		v->f05 = 7;
		s->f25 = 1;
		s->f26 = 0;
		WaitFrames(2);
		v->f05 = 0;
		s->f25 = 1;
		WaitFrames(2);
	}
	s->f26 = 1;
	g2 = gState;
	zero = 0;
	*(short *)(g2 + (0x93 << 2)) = zero;
}
