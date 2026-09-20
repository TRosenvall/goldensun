// fakematch
/* Cluster Func_80b2da8..Func_80b2da8 extracted from goldensun/asm/rom_b0000/rom_b0070_c_c_a_c_c.s.
 *
 * Total .text for this TU = 136 bytes (= 0x88). Never attempted before batch 275.
 *
 * FAKEMATCH: ONE `volatile` READ. No register pins. Row added to fakematch.txt.
 *
 * WHY IT IS A FAKEMATCH AND NOT A LEVER. The ROM loads `*p` THREE times in the loop body:
 *
 *     ldrh r2, [r5] / mov r3, r8 / and r3, r2 / cmp r3, #0 / beq     <- the mask test
 *     ldrh r0, [r5] / bl _GetItemInfo                                <- the call argument
 *     ldrh r2, [r5] / mov r3, r8 / eor r3, r2 / strh r3, [r5]        <- the update
 *
 * The THIRD load is legitimate -- the call clobbers memory, so gcc must reload. But the
 * first two sit before the call with NO intervening write, so they are provably the same
 * value and CSE merging them is CORRECT. That is exactly the blocked sub-class
 * docs/elevation.md records: "the two loads are provably the same value, CSE is correct --
 * blocked ... Only `volatile` keeps both loads, and that is a fakematch."
 *
 * The recorded positive escape does not apply either: it works by giving the unsigned
 * value a use the merged value cannot satisfy (a shift count), and both of these loads are
 * plain 16-bit reads feeding a mask and an argument.
 *
 * So the `volatile` here is doing OPTIMISATION-BARRIER work, not describing memory -- this
 * is an IWRAM unit record, not a hardware register. It is booked as a fakematch on that
 * basis. Batches 272-274 landed 39 functions with no scaffolding at all; this is the first
 * row since 271, and it is one read rather than a pin.
 *
 * THE MINIMUM IS ONE READ, not the declaration. `volatile unsigned short *p` also matches,
 * but marking only the mask read keeps the other two loads ordinary and is strictly less
 * fiction. Measured (ref 63 encodings): the minimal form MATCH; whole-pointer volatile
 * MATCH; volatile on the CALL read instead 2 differing; no volatile at all 8; a two-member
 * union 26.
 *
 * On the whole-pointer form operand order stops mattering (`*p & m` also matches); with the
 * minimal form `m & *p` is required.
 *
 * THREE ORDINARY LEVERS also needed, none of them scaffolding:
 *   - a NAMED `int m` for the 0x200 mask, assigned BEFORE the pointer init, because the ROM
 *     builds the constant first in the preheader and a loop-invariant literal is emitted
 *     after the source-order statements. Inline `0x80 << 2` is 10 differing.
 *   - `i = 0xe` as a STATEMENT with an empty `for`-init. In the init it is 2 differing.
 *   - `void`, from `pop {r0} / bx r0`. Declared `int` is 2.
 */
extern unsigned char *_GetUnit(int id);
extern void _UpdateStatBarPercent(int id);
extern unsigned char *_GetItemInfo(int item);
extern void _CalcStats(int id);

void Func_80b2da8(int who, int kind)
{
	unsigned char *u;
	unsigned short *p;
	int m;
	int i;

	u = _GetUnit(who);
	if (kind == 0) {
		*(unsigned short *)(u + 0x38) = *(unsigned short *)(u + 0x34);
		_UpdateStatBarPercent(who);
	} else if (kind == 1) {
		u[0x131] = 0;
	} else if (kind == 2) {
		u[0xa0 << 1] = 0;
	} else if (kind == 3) {
		m = 0x80 << 2;
		i = 0xe;
		p = (unsigned short *)(u + 0xd8);
		for (; i >= 0; i--, p++) {
			if (m & *(volatile unsigned short *)p) {
				if (_GetItemInfo(*p)[3] & 1) {
					*p = m ^ *p;
					_CalcStats(who);
				}
			}
		}
	}
}
