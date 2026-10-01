/* Func_80b5534 -- 0x080b5534, from goldensun/asm/rom_b5000/rom_b5368_a.s.
 *
 * NON-MATCHING: 169 encodings of 187 differ (objcmp).
 * The SIZE also differs -- 436 bytes against 428, 191 instructions against 187 --
 * so 169 is NOT a distance to exact: four extra instructions early shift the
 * alignment and most of the 169 is that shift.
 *
 * The debug record editor: read string 0x903 into a 0x80-byte scratch, copy up
 * to 14 of its halfwords down into combatant 0's record as bytes, then run a
 * forever-looping UI box over the id space with the auto-repeat key state.  It
 * has no epilogue at all -- it never returns.
 *
 * *** THE HEADLINE, AND IT IS A .sym DECISION FOR THE USER. ***
 * The ROM computes the id-space limit AT RUNTIME:
 *
 *     ldr r2, =0x2850 / ldr r3, =0x26fa / sub r2, r3 / mov r8, r2
 *
 * Two pooled words subtracted at run time CANNOT come from two integer
 * literals -- gcc folds `0x2850 - 0x26fa` to 0x156 and pools one word.  So the
 * source had two SYMBOLS there, and this is the same argument message.sym
 * already accepted for `_MSG_ad0 = 0x0ad0`, which is in this very function
 * (`ldr r3, =0xad0`) feeding the same sink, `_Func_8017658`.  The two names this
 * function needs are
 *
 *     _MSG_26fa = 0x26fa;      _MSG_2850 = 0x2850;
 *
 * and both are used exactly as message.h's other ids are, `(int)&_MSG_x`.
 * 0x26fa is the base id the selector adds to, 0x2850 the one past the end.
 *
 * *** THIS FILE USES A VERIFICATION SHIM AND IT MUST NOT BE LANDED AS IS. ***
 * Because those two names are not in message.sym yet, the file carries
 *
 *     __asm__(".equ _MSG_26fa, 0x26fa");
 *     __asm__(".equ _MSG_2850, 0x2850");
 *     __asm__(".equ _MSG_ad0, 0x0ad0");
 *
 * so that it assembles standalone with the ROM's plain pool words.  All three
 * lines must be deleted and replaced by `#include "message.h"` once the two new
 * entries are added; `_MSG_ad0` already exists in message.sym and message.h and
 * is shimmed here only so the file needs no include.
 *
 * WHAT IS ALREADY RIGHT: the whole eight-test `gKeyRepeat` poll chain with its
 * masks and polarities, the `mode` toggle on bit 1, the `val < 0` clamp (signed,
 * `bge`) beside the `val >= limit + 5` clamp (UNSIGNED, `bcc` -- the tell that
 * the upper bound is unsigned while the lower is not), the 0x3f2 composite exit
 * mask, the 14-byte copy loop's peeled first iteration, and the outer forever
 * loop.
 *
 * TWO LEVERS ALREADY IN THE FILE:
 * 1. `volatile int gKeyRepeat` -- the ROM re-loads it before every one of the
 *    eight tests; a plain int is loaded once.
 * 2. A NAMED `unsigned short c` IN THE COPY LOOP.  `u[i] = buf[i]; if (buf[i]
 *    == 0)` loads the halfword twice; `c = buf[i]; u[i] = c; if (c == 0)` loads
 *    it once and gives the ROM's `lsls r3,#16 / cmp r3,#0` -- the shift is gcc's
 *    HImode compare, which is itself the evidence that the value went through a
 *    16-bit local rather than being compared as the loaded int.  Worth 7.
 *
 * THE BLOCKER: ONE CALLEE-SAVED REGISTER TOO MANY.  The ROM's prologue saves
 * only r8 and r10 (`mov r7,r10 / mov r6,r8 / push {r6,r7}`); this candidate
 * saves r8, r9 and r10.  r8 is the hoisted `0x2850 - 0x26fa` and r10 is `mode`;
 * the third is an extra invariant gcc lifts out of the OUTER loop that the ROM
 * keeps inside it.  Moving the `limit` assignment from the outer loop body into
 * the inner loop body (so loop.c can only lift it to the INNER pre-header, which
 * is where the ROM computes it, immediately before `b .Lb5614`) is worth 7 but
 * does not remove the third register.
 *
 * ALSO NOT YET REPRODUCED: the cross-jump at the top of the outer loop.  The ROM
 * has ONE `strb r2,[r3]` serving two different stores -- `gState[0x20c] = 2`
 * before the loop and `*(iwram_3001e8c + 0x12f8) = 0` at its bottom -- because
 * jump2's find_cross_jump merged the common tail (the store plus the entire loop
 * body).  Both stores are in this file in those places; whether the merge
 * happens has not been isolated from the register-count defect above.
 *
 * Verify with (run it inside the build container; /opt/gcc296 is not on the host):
 *   python3 tools/objcmp.py src/non_matching/rom_b5000/80b5534.c \
 *     asm/rom_b5000/rom_b5368_a.s --func Func_80b5534
 *
 * The .s holds FOUR functions (Debug_LoadPresetParty, Func_80b5534,
 * Debug_BattleTest, Func_80b5864) and NO data, so landing any one needs a split.
 */
/* VERIFICATION SHIM (see report): _MSG_26fa and _MSG_2850 are not yet in
 * message.sym, so they are .equ'd here so the candidate assembles with the
 * ROM's plain pool words. _MSG_ad0 IS already in message.sym/message.h. */
extern int _MSG_26fa;
extern int _MSG_2850;
extern int _MSG_ad0;
__asm__(".equ _MSG_26fa, 0x26fa");
__asm__(".equ _MSG_2850, 0x2850");
__asm__(".equ _MSG_ad0, 0x0ad0");
#define MSG_26fa ((int)&_MSG_26fa)
#define MSG_2850 ((int)&_MSG_2850)
#define MSG_ad0 ((int)&_MSG_ad0)

extern volatile int gKeyRepeat;
extern unsigned char gState[];
extern unsigned char *iwram_3001e8c;

extern unsigned char *_GetUnit(int id);
extern void _DecompressString2(int id, unsigned short *buf);
extern void _Func_8015f30(void);
extern void _PlaySound(int id);
extern void _Func_80198dc(void);
extern void _Func_8019908(int a, int b);
extern int _Func_8017658(int id, int a, int b, int c);
extern int WaitFrames(int n);
extern int _Func_8017364(void);
extern void _Func_80197c4(int a);
extern void _CloseUIBox(int box, int a);

void Func_80b5534(void)
{
	unsigned short buf[0x40];
	unsigned char *u;
	int mode;
	int i;
	int val;
	int limit;
	int box;
	unsigned short c;

	mode = 0;
	u = _GetUnit(0);
	_DecompressString2(0x903, buf);
	for (i = 0; i <= 0xd; i++) {
		c = buf[i];
		u[i] = c;
		if (c == 0)
			break;
	}
	u[0xe] = 0;
	_Func_8015f30();
	_PlaySound(0x47);
	val = 0;
	*(unsigned short *)0x04000000 = 0x1341;
	gState[0x20c] = 2;
	for (;;) {
		_Func_80198dc();
		_Func_8019908(0x3e7, 5);
		_Func_8019908(0, 3);
		_Func_8019908(1, 1);
		_Func_8019908(1, 2);
		_Func_8019908(2, 4);
		if (mode == 0)
			box = _Func_8017658(val + MSG_26fa, 2, 0xa, 4);
		else
			box = _Func_8017658(val + MSG_ad0, 2, 2, 4);
		WaitFrames(0xa);
		for (;;) {
			limit = MSG_2850 - MSG_26fa;
			if (gKeyRepeat & 2) {
				if (mode != 0) {
					mode = 0;
				} else {
					val++;
					mode = 1;
				}
			}
			if (gKeyRepeat & 0x10)
				val++;
			if (gKeyRepeat & 0x20)
				val -= 2;
			if (gKeyRepeat & 0x40)
				mode = 1;
			if (gKeyRepeat & 0x80)
				mode = 0;
			if (gKeyRepeat & 0x100)
				val += 0xa;
			if (gKeyRepeat & 0x200)
				val -= 0xa;
			if (val < 0)
				val = 0;
			if ((unsigned int)val >= (unsigned int)(limit + 5))
				val = limit + 5;
			if (gKeyRepeat & 0x3f2)
				break;
			if (_Func_8017364() != 0) {
				if (gKeyRepeat & 1)
					break;
			}
			WaitFrames(1);
		}
		_Func_80197c4(1);
		_CloseUIBox(box, 1);
		*(unsigned char *)(iwram_3001e8c + 0x12f8) = 0;
	}
}
