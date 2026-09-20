/* Cluster Func_80b17e4..Func_80b17e4 extracted from goldensun/asm/rom_b0000/rom_b0070_a_a_c_c_c_a.s.
 *
 * Total .text for this TU = 132 bytes (= 0x84).
 * Preserves the original ROM layout when slotted between
 * asm/rom_b0000/rom_b0070_a_a_c_c_c_a_a_a.o and asm/rom_b0000/rom_b0070_a_a_c_c_c_a_b.o in
 * goldensun/stage1.ld.
 *
 * Never attempted before batch 272. EXACT ON THE FIRST CANDIDATE, no pins, no flags.
 *
 * A shop purchase: look up the item, remember what the unit already has equipped,
 * play the sound, then give the item `count` times, charging its price each time.
 *
 * THE FILE-MATE HEURISTIC PAID AGAIN -- its two direct callees are already elevated
 * in the same directory (src/rom_b0000/rom_b0070_a_a_c_c_c_a_b.c and _a_c.c) and
 * carried `extern unsigned char *_GetItemInfo`, `extern int _GetEquippedItem(int
 * unit, int kind)`, `info[2]` and the `int Func_80b1868(int unit, int slot)`
 * signature. rom_b0070_a_a_c_c_c_b.c carried the
 * `(short)*(unsigned short *)_GetItemInfo(...)` price spelling that gives the ROM's
 * `ldrsh r0, [r6, r3]`. No Unit struct was needed -- this function never touches one.
 *
 * ONE READING WORTH KEEPING. The ROM guards the loop with `cmp r10, r5 / bge` -- a
 * register holding 0 against `count` -- but closes it with
 * `sub r5, #1 / cmp r5, #0 / bne`. That mismatch is gcc's own check_dbra_loop
 * REVERSING a plain `for (i = 0; i < count; i++)` whose `i` is unused in the body,
 * and the zero for the entry guard is CSE'd with `slot = 0`. Writing the loop the
 * way the back edge reads -- `while (count > 0) { ...; count--; }` -- gives `bgt`
 * on the back edge and cannot match.
 *
 * That is the recorded "an induction variable's final form is evidence about the
 * PASS, not the statement" rule, and this is a clean specimen: the guard and the
 * back edge disagree in the ROM precisely BECAUSE one is the source's and the other
 * is the optimiser's.
 */
extern unsigned char *_GetItemInfo(int item);
extern int _GetEquippedItem(int unit, int kind);
extern void _PlaySound(int sfx);
extern int _GiveItemTo(int who, int item);
extern void _AddCoins(int n);
extern void _AddCoinsSpent(int n);
extern void Func_80b10cc(void);
extern void Func_80b0574(int msg);
extern int Func_80b1868(int unit, int slot);
extern int Func_80b196c(int unit, int slot);

void Func_80b17e4(int unit, int item, int count)
{
	unsigned char *info;
	int eq;
	int slot;
	int i;

	info = _GetItemInfo(item);
	slot = 0;
	eq = _GetEquippedItem(unit, info[2]);
	_PlaySound(0x65);
	for (i = 0; i < count; i++) {
		slot = _GiveItemTo(unit, item);
		_AddCoins(-(short)*(unsigned short *)info);
		_AddCoinsSpent((short)*(unsigned short *)info);
		Func_80b10cc();
	}
	Func_80b0574(0xca1);
	if (Func_80b1868(unit, slot) != 0) {
		Func_80b196c(unit, eq);
	}
}
