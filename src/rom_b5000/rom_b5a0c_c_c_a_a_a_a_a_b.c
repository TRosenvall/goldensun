/* Func_80b5e14 (RunBattleIntroText)  --  0x080b5e14, first of two in
 * asm/rom_b5000/rom_b5a0c_c_c_a_a_a_a_a.s.
 * EXACT: 248 bytes, 110 encodings and 13 relocations identical (objcmp, 3 runs).
 * No pins, no barriers, no shim, no new .sym entry.  datacheck reports NO DATA
 * in the file, so landing is a plain text split (Func_80b5f0c stays as .s).
 *
 * THREE LEVERS, and the third is the one worth carrying out of this batch.
 *
 * 1. THE `int` RETURN TYPE.  `pop {r1} / bx r1` plus `mov r0, r10` before the
 *    shared epilogue: the function returns the counter.  Written `void` it is
 *    107 of 110 and 28 bytes short -- the return value is most of the function's
 *    register assignment, not two instructions.
 *
 * 2. THE EXPLICIT GUARD PLUS do/while FOR THE CHARACTER COUNT.  `k = 0;
 *    if (buf[k] != 0) { q = buf; do { k++; if (k > 4) break; q++; } while (*q); }`
 *    reproduces the ROM's peeled first test (`ldrh r3, [r5, r0]`, register
 *    offset because the index is a live variable holding 0 -- NOT an `off = 0`
 *    scaffold), the `mov r2, r5` pointer init INSIDE the guard, and the
 *    unrotated body.  A plain `while (buf[k] != 0) { ... }` rotates the loop,
 *    advances the pointer before the test, and is 81 of 110.
 *
 * 3. *** THE COUNTING LOOP MUST REUSE THE COPY LOOPS' COUNTER, AND THE RESULT
 *    MUST BE COPIED OUT. ***  The ROM keeps the character count in r0 while
 *    counting and then does `mov r4, r0`, because r0 is wanted next for the
 *    memmove's own counter.  Spelling the count with its own variable `n` leaves
 *    5 of 110 -- ONLY that one `mov r4, r0` missing, everything else exact --
 *    and neither `len = n;` afterwards NOR swapping the two declarations moves
 *    it (both 75 encodings differing).  What works is counting in `k`, the
 *    variable the two byte-copy loops use, and then `n = k;`.  The copy is then
 *    a real live-range split rather than a coalescable copy, which is exactly
 *    the distinction docs/elevation.md's "Separate variables do not defeat a
 *    COPY" draws -- and the new part is which side to name: give the SECOND
 *    consumer the shared name, not the first.
 *
 * INERT (both measured EXACT): `u[0x95 << 1]` against the hand cast
 * `*(unsigned char *)((int)u + (0x95 << 1))`, and `0x154`/`0x140` flat against
 * `0xaa << 1`/`0xa0 << 1`.
 *
 * Note gcc writes r4 here without saving it (`mov r4, r0`) and the ROM's push
 * list agrees, so nothing in the source has to arrange that.
 */
extern void *Func_8004970(int size);
extern void free(void *p);
extern unsigned char *_GetUnit(int id);
extern int Func_8006408(void);
extern void Func_8006488(void);
extern void WaitFrames(int n);
extern void _DecompressString2(int msg, unsigned short *dst);
extern void _Func_8077330(int side);

int Func_80b5e14(void)
{
	unsigned short buf[24];
	void *p;
	unsigned char *u;
	unsigned short *q;
	int cnt;
	int i;
	int n;
	int k;

	p = Func_8004970(0xaa << 1);
	cnt = 0;
	for (i = 0; i <= 2; i++) {
		u = _GetUnit(i + 0x80);
		if (Func_8006408() == -1)
			break;
		Func_8006488();
		if (u[0x95 << 1] != 0)
			cnt++;
		WaitFrames(2);
		_DecompressString2(0x80c, buf);
		k = 0;
		if (buf[k] != 0) {
			q = buf;
			do {
				k++;
				if (k > 4)
					break;
				q++;
			} while (*q != 0);
		}
		n = k;
		for (k = 0xe; k >= n; k--)
			u[k] = u[k - n];
		for (k = 0; k < n; k++)
			u[k] = buf[k];
		u[0xe] = 0;
	}
	free(p);
	p = Func_8004970(0xa0 << 1);
	_Func_8077330(1);
	if (Func_8006408() != -1) {
		Func_8006488();
		WaitFrames(2);
	}
	free(p);
	return cnt;
}
