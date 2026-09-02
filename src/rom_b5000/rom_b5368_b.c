/* Func_80b595c -- 0x080b595c, from goldensun/asm/rom_b5000/rom_b5368.s.
 *
 * Narrates a battle result: collect the result values, announce each in turn
 * with a different message for the last, then add one of two closing lines
 * depending on the outcome byte in the battle state.
 *
 * Preserves the original ROM layout when slotted between
 * asm/rom_b5000/rom_b5368_a.o and rom_b5368_c.o in goldensun/stage1.ld. THE
 * FILE WAS HAND-SPLIT THREE WAYS, not by split_s.py: it holds five functions
 * and then a `.section .rodata` block, and the splitter cuts at function
 * boundaries so the trailing data would have stayed with this function. The
 * data also needed `.global .Lc3f34`, because one of the four earlier functions
 * references it and a local label does not cross an object boundary -- the link
 * failed on exactly that before it was added.
 *
 * TWO LEVERS.
 *
 *   FOUR MESSAGE IDS ARE SYMBOLS, and 0x810 is the one that proves it. It is
 *   0x81 << 4, so gcc BUILDS it -- `mov r0, #0x81 / lsl r0, #0x4` -- and pools
 *   only what it cannot synthesise cheaply. The ROM spends a pool word on it
 *   anyway, which is the recorded tell that the original source had a symbol
 *   there. Added to message.sym as _MSG_810..._MSG_813, with the namespace
 *   fixed by the consumer: all four feed _Func_80175a0, the same sink the ids
 *   already in that file feed. Worth 2 of the 10.
 *
 *   DELETE THE WALKING POINTER. With an explicit `unsigned short *p` walked by
 *   `*p++`, the loop counter and the pointer land in each other's registers --
 *   r5 and r6 exchanged throughout, 8 differing. Indexing the buffer directly
 *   as `buf[i]` and letting gcc build the induction variable itself matches.
 *   That is the delete-the-local lever from batch 181 applied to a giv rather
 *   than to an address.
 *
 * The outcome byte is read ONCE into a local and compared twice, which is what
 * the ROM does -- the mirror of the read-count rule.
 *
 * MEASURED (rom 69 lines):
 *   literal message ids, explicit walking pointer        10 aligned
 *   message symbols, explicit walking pointer             8
 *   the counter assigned before the pointer              11
 *   declaration order swapped                             8 (inert)
 *   the buffer passed directly to the filler              8 (inert)
 *   `*p` and `p++` as separate statements                 9
 *   the pointer deleted, `buf[i]` indexed                MATCH
 */
#include "message.h"

extern unsigned char *iwram_3001e74;
extern void _Func_80198dc(void);
extern void Func_80b6ae0(unsigned short *buf);
extern void _Func_8019908(int a, int b);
extern void _Func_80175a0(int id);
extern void WaitTextPrompt(void);
extern void _Func_80174d8(void);

void Func_80b595c(int n)
{
	unsigned short buf[8];
	unsigned char *s;
	int i;
	int last;
	int k;

	s = iwram_3001e74;
	_Func_80198dc();
	Func_80b6ae0(buf);
	i = 0;
	if (n != 0) {
		last = n - 1;
		do {
			_Func_8019908(buf[i], 1);
			if (i == last)
				_Func_80175a0(MSG_811);
			else
				_Func_80175a0(MSG_810);
			i++;
			WaitTextPrompt();
		} while (i != n);
	}
	_Func_80174d8();
	k = s[0x45];
	if (k == 1) {
		_Func_80198dc();
		_Func_8019908(0, 1);
		_Func_80175a0(MSG_812);
		WaitTextPrompt();
	} else if (k == 2) {
		_Func_80198dc();
		_Func_8019908(0, 1);
		_Func_80175a0(MSG_813);
		WaitTextPrompt();
	}
}
