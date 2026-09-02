/* Func_80b3284 -- 0x080b3284, from goldensun/asm/rom_b0000/rom_b0070_c_c_c.s.
 *
 * The inn: bring up the innkeeper's portrait, quote the price, and on a yes
 * either refuse for want of coin or take payment, heal, and say goodnight.
 *
 * Preserves the original ROM layout when slotted before
 * asm/rom_b0000/rom_b0070_c_c_c_c.o in goldensun/stage1.ld. That remaining
 * piece keeps the file's four other functions AND its `.section .rodata`
 * block, which is why the split was taken here rather than converting the file
 * whole.
 *
 * THREE LEVERS, and the middle one is the interesting one.
 *
 *   THE MESSAGE ID IS A SYMBOL. The ROM holds 0xd1c in a callee-saved register
 *   and reaches 0xd1d..0xd20 with `add r0, #K`. Written as a plain integer gcc
 *   folds each site to its own `ldr r0, =0xd1f`, loses the high register
 *   entirely, and the prologue and epilogue push lists change with it. gcc will
 *   spend a callee-saved register on a SYMBOL ADDRESS and not on an integer it
 *   can rematerialise -- which is the same argument message.sym's earlier notes
 *   make, and the reason _MSG_d1c is now in that file.
 *
 *   DELETE THE HALFWORD POINTER LOCAL. Naming
 *   `hp = (unsigned short *)(base + (0xe9 << 2));` hoisted its
 *   `mov r2, #0xe9 / lsl r2, #2` ABOVE the actor lookup. Writing the address
 *   expression out at all four use sites lets CSE build the register where the
 *   ROM does, interleaved with the actor load chain, and the read-back falls
 *   out for free.
 *
 *   THE gSTATE BASE MUST BE NAMED IN THE BLOCK THAT USES IT. `gState + 0x10`
 *   written directly folds to `ldr r3, =gState+16`, so it needs a local -- but
 *   naming it ABOVE the `if` made gcc keep it in a fourth callee-saved register
 *   and rotate the other three, costing five instructions. Assigning
 *   `g = gState;` inside the `else` arm fixed the fold and the whole
 *   high-register allocation at once. Assignment placement again, not
 *   declaration.
 *
 * No return-type lever was needed anywhere: straight prototypes produced every
 * argument-fill order in the ROM, including the two differently-ordered box
 * calls and the four differently-ordered close calls.
 *
 * MEASURED (rom 115 lines):
 *   literal id, named halfword pointer, base above the if   108 lines, 28 aligned
 *   symbol id, pointer inlined, base still above the if     120, 24
 *   the same with the base named inside the else arm        115, MATCH
 *   the match re-screened without _MSG_d1c defined          115, 1
 */
#include "message.h"

extern unsigned char iwram_3001f2c[];
extern unsigned char gState[];

extern void Func_80b010c(void);
extern unsigned char *_MapActor_GetActor(int id);
extern int _Func_8019da8(int a, int b, int c, int d);
extern int Func_80b3210(int kind);
extern void _Func_8019908(int a, int b);
extern void Func_80b04dc(int msg);
extern void *_CreateUIBox(int a, int b, int c, int d, int e);
extern void Func_80b10cc(void);
extern unsigned int Func_80b0634(unsigned int arg0);
extern void _CloseUIBox(int h, int n);
extern void InnHeal(int cost);
extern void Func_80b0204(void);

int Func_80b3284(int kind, int actor)
{
	unsigned char *base;
	unsigned char *g;
	int msg;
	int box;
	unsigned int cost;

	Func_80b010c();
	base = *((unsigned char **) iwram_3001f2c);
	base[0x3a9] = 1;
	if (kind == 5)
		base[0x3ac] = 1;
	*((unsigned short *) (base + (0xe9 << 2))) =
	    *((unsigned short *) (*((int *) (*((int *) (_MapActor_GetActor(actor) + 0x50)) + 0x28))));
	box = _Func_8019da8(*((unsigned short *) (base + (0xe9 << 2))), 0, 0, 0);
	cost = Func_80b3210(kind);
	_Func_8019908(cost, 5);
	msg = MSG_d1c;
	Func_80b04dc(msg);
	*((void **) (base + 0xc)) = _CreateUIBox(0, 0x10, 0xc, 4, 2);
	Func_80b10cc();
	if (Func_80b0634(0) != 0) {
		Func_80b04dc(msg + 3);
		_CloseUIBox(*((int *) (base + 0xc)), 2);
	} else {
		g = gState;
		if (cost > *((unsigned int *) (g + 0x10))) {
			Func_80b04dc(msg + 2);
			_CloseUIBox(*((int *) (base + 0xc)), 2);
		} else {
			_CloseUIBox(*((int *) (base + 0xc)), 2);
			Func_80b04dc(msg + 1);
			_CloseUIBox(box, 2);
			InnHeal(cost);
			*((unsigned short *) (base + (0xe9 << 2))) =
			    *((unsigned short *) (*((int *) (*((int *) (_MapActor_GetActor(actor) + 0x50)) + 0x28))));
			box = _Func_8019da8(*((unsigned short *) (base + (0xe9 << 2))), 0, 0, 0);
			Func_80b04dc(msg + 4);
		}
	}
	_CloseUIBox(box, 2);
	Func_80b0204();
	return 0;
}
