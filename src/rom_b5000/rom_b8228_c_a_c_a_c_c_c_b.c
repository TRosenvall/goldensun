/* Func_80b8db8 -- 0x080b8db8, from
 * goldensun/asm/rom_b5000/rom_b8228_c_a_c_a_c_c_c.s.
 *
 * Applies damage to a combatant: subtract, clamp at zero, play the hit
 * animation, then narrate it -- a different message pair for party members and
 * enemies, and a further line if the target went down.
 *
 * Preserves the original ROM layout when slotted between
 * asm/rom_b5000/rom_b8228_c_a_c_a_c_c_c_a.o and ..._c.o in goldensun/stage1.ld.
 *
 * THREE MECHANISMS, and the third is a FIX FOR A BLOCKER CLASS THE NOTEBOOK
 * LISTS AS UNSOLVED.
 *
 *   A DEAD FOUR-BYTE STACK LOCAL SURVIVES -O2 AS A `char` ARRAY, NOT AS A
 *   STRUCT. With a struct of four `unsigned char` fields gcc merges the four
 *   zero stores into one `str r3, [sp]`; with `char buf[4]` it emits
 *   `mov r2, sp` and four `strb`, which is the ROM. The `mov rX, sp` is forced
 *   because Thumb `strb` has no sp-relative form -- so `mov rX, sp` followed by
 *   `strb` is the byte-granular analogue of the recorded address-taken tell.
 *
 *   ASSIGN `p = buf` BEFORE THE STORES, NOT AFTER. Writing
 *   `if (p == 0) { buf[0] = 0; ... }` lets CSE reuse the compared pointer
 *   register as the zero -- it knows `p == 0` on that path -- which keeps p's
 *   pseudo live to the last store, gives it the long live range, and pushes the
 *   frame base into r3, the opposite of the ROM. Assigning `p = buf` first
 *   kills that equivalence. Generally: WHEN GCC REUSES A COMPARED-AGAINST-ZERO
 *   REGISTER AS A STORED CONSTANT AND THE ROM DOES NOT, OVERWRITE THAT VARIABLE
 *   BEFORE THE STORES.
 *
 *   STORING A LITERAL ZERO TO A `short` LVALUE IS ALWAYS A HALFWORD POOL LOAD;
 *   STORING IT THROUGH A NAMED LOCAL IS A `mov`. Seven spellings were measured
 *   -- `short *`, `unsigned short *`, a struct field, an `int h : 16` bitfield,
 *   `&= 0` -- and every one produced `ldrh r3, .Ln / .word 0`. Only
 *   `short zero = 0; *p = zero;` (or an int local with a cast) gives
 *   `mov r3, #0 / strh`. The movhi expander force_const_mem's a CONST_INT when
 *   the destination is memory, and a register-allocated local is the escape.
 *   docs/elevation.md lists halfword constant pooling as unsolved; for the
 *   constant-zero case this is the fix.
 *
 *   That one root cause also produced a second visible difference. The `ldrh`
 *   needed its pool in range, so arm_reorg dumped the pool mid-function and
 *   branched over it, which reads as an unrelated redundant jump. A STRAY
 *   JUMP-TO-NEXT-LABEL IN THUMB OUTPUT IS USUALLY A POOL DUMP, NOT A
 *   JUMP-OPTIMISATION FAILURE.
 *
 * MEASURED (rom 108 lines):
 *   struct of four bytes, direct `= 0` clamp            106 lines, 28 aligned
 *   `char buf[4]`, stores before `p = buf`              110, 13
 *   chained `buf[3]=buf[2]=buf[1]=buf[0]=0`             110, 23
 *   `p = buf` first, literal zero clamp                 110, 13
 *   the same with `short zero = 0` for the clamp        108, MATCH
 *   a `short hp` local carrying the subtraction         109, 3
 */
extern unsigned char *_GetUnit(unsigned int id);
extern int *GetBattleActor(unsigned int id);
extern void _Actor_SetAnim(int a, int b);
extern void _Func_801f200(int a);
extern void _Func_80198dc(void);
extern void _Func_80175a0(unsigned int a);
extern void _Func_8019908(unsigned int a, unsigned int b);
extern void Func_80b8ec4(unsigned int a);

void Func_80b8db8(unsigned int id, unsigned int dmg, unsigned int flag, char *p)
{
	char buf[4];
	unsigned char *unit;

	if (p == 0) {
		p = buf; p[0] = 0; p[1] = 0; p[2] = 0; p[3] = 0;
	}
	unit = _GetUnit(id);
	*(short *)(unit + 0x38) -= dmg;
	if (*(short *)(unit + 0x38) < 0) {
		short zero = 0;
		*(short *)(unit + 0x38) = zero;
	}
	_Actor_SetAnim(*GetBattleActor(id), 5);
	_Func_801f200(0);
	_Func_80198dc();
	if (id <= 7) {
		if (flag)
			_Func_80175a0(0x823);
		_Func_8019908(dmg, 5);
		_Func_8019908(id, 1);
		_Func_80175a0(0x827);
	} else {
		if (flag)
			_Func_80175a0(0x822);
		_Func_8019908(dmg, 5);
		_Func_8019908(id, 1);
		_Func_80175a0(0x826);
		_Func_8019908(id, 1);
	}
	Func_80b8ec4(id);
	if (id <= 7) {
		if (*(short *)(unit + 0x38) <= 0) {
			_Func_8019908(id, 1);
			_Func_80175a0(0x825);
		}
	} else {
		if (*(short *)(unit + 0x38) <= 0) {
			_Func_8019908(id, 1);
			_Func_80175a0(0x838);
		}
	}
	_Actor_SetAnim(*GetBattleActor(id), 1);
}
