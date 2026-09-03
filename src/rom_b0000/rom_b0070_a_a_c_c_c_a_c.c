/* Func_80b196c  --  0x080b196c
 *
 * The whole of goldensun/asm/rom_b0000/rom_b0070_a_a_c_c_c_a_c.s: one function,
 * no data, no split. The ROM annotation calls it ConfirmTransaction; the body
 * reads more specifically as "clear the item in a slot unless it is a class-6
 * kind or carries flag 0x8".
 *
 * MATCHED ON THE FIRST CANDIDATE WITH NO ITERATION, and the reason is the whole
 * finding: the direct .s sibling was already solved. This file's parent was
 * split in batch 182, and src/rom_b0000/rom_b0070_a_a_c_c_c_a_b.c carried,
 * verbatim and reusable, the Unit struct whose `items[]` member reproduces the
 * ROM's register-offset halfword load base-first, the two callee prototypes with
 * their `unsigned char *` returns, the 0x1ff item-id mask, and the read-count
 * decision that a repeated lookup is a GENUINE SECOND READ rather than a named
 * local -- which is exactly what the two calls here need. One file read turned a
 * 42-instruction, 4-call, high-register function into a zero-iteration match.
 *
 * TWO FLAGGED LEVERS WERE NOT NEEDED, AND APPLYING EITHER WOULD HAVE BEEN WRONG.
 *
 * The [r8-r11] marker was again not a blocker. A high register holds -1 across
 * three call sites, which passes the recorded "a named constant that survives a
 * call" test and looks exactly like a forced local. It is not. Measured: bare
 * literals at both use sites reproduce the ROM. Thumb's compare-immediate cannot
 * encode -1, so gcc must materialise it for the comparison anyway, and ordinary
 * CSE then reuses that register as the argument. So: A CONSTANT THAT A THUMB
 * IMMEDIATE CANNOT ENCODE IS MATERIALISED FOR THE COMPARISON REGARDLESS -- its
 * survival in a callee-saved register is the allocator, not the source. That is
 * a discriminator the "necessary, not sufficient" note needs.
 *
 * And the batch-186 lever about an `orr`/`and`/`add` on the argument register
 * after a `bl` does not apply here, though the shape appears: the `and` follows
 * a call but operates on the loaded halfword rather than on r0, and the byte
 * loads are plain dereferences of the returned pointer. The tell is specifically
 * an operation on the ARGUMENT register, and checking which register it is takes
 * a second.
 *
 * Three `mov r0, #0` sites are three separate early returns, which extends the
 * recorded one-and-two-site reading rule to three unchanged.
 */
typedef struct { unsigned char pad00[0xd8]; unsigned short items[1]; } Unit;

extern Unit *_GetUnit(int unit);
extern unsigned char *_GetItemInfo(int item);
extern void Func_80b1f4c(int unit, int slot, int a);

int Func_80b196c(int unit, int slot)
{
	Unit *u;
	int item;

	u = _GetUnit(unit);
	if (slot == -1) {
		return 0;
	}
	item = u->items[slot] & 0x1ff;
	if (_GetItemInfo(item)[2] == 6) {
		return 0;
	}
	if ((_GetItemInfo(item)[3] & 8) != 0) {
		return 0;
	}
	Func_80b1f4c(unit, slot, -1);
	return 1;
}
