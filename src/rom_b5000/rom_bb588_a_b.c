/* Func_80bb588 (0x080bb588) -- FIRST OF THREE in asm/rom_b5000/rom_bb588_a.s
 * (the siblings WaitTextPrompt @0x080bb65c and Func_80bb7c0 @0x080bb7c0 stay as
 * .s, so landing this is a THREE-WAY TEXT SPLIT).  datacheck reports NO DATA.
 *
 * *** UN-PARKS src/non_matching/rom_b5000/80bb588.c, WHICH WAS BOOKED AT 56 OF
 * 98 DIFFERING.  EXACT: 212 bytes, 100 encodings and 4 relocations identical
 * (objcmp, 3 runs). ***  It is a BONUS -- not one of batch 285/E's five targets
 * -- found because my Func_80b5b18 candidate reproduced the very pattern the
 * park says nothing reproduces.
 *
 * The park's diagnosis was that gcc CSEs a run of constant-offset byte stores
 * into ONE offset register stepping by 1 where the ROM alternates TWO stepping
 * by 2, and that "nothing recorded" produces the ROM's form.  TWO EDITS FIX IT:
 *
 * 1. THE FOUR-BYTE CLEAR MUST BE AN INDEXED `for`, NOT A POINTER do/while.
 *    The park wrote `p = u + 0x12f; i = 3; do { i--; *p = 0; p--; } while
 *    (i >= 0);`.  Written `for (j = 3; j >= 0; j--) u[0x12c + j] = 0;` the
 *    SAME four instructions come out AND the 24 following stores switch to the
 *    ROM's two alternating offset registers -- 56 differing becomes 2.  The
 *    named pointer local is what cost the register the second offset needs; the
 *    address bookkeeping downstream was a register-pressure symptom, not a CSE
 *    law.  (Same shape as "Naming one level too many costs a callee-saved
 *    register" in docs/elevation.md, one level further out than that entry.)
 * 2. THE FUNCTION RETURNS THE LAST CALL'S VALUE.  `pop {r1}; bx r1` in the ROM
 *    means r0 is live at exit; `return Func_80b78e4(id, GetBattleActor(id));`
 *    with Func_80b78e4 declared `int` costs no instruction and closes the last
 *    2 encodings.
 */
extern unsigned char *_GetUnit(int id);
extern void _CalcStats(int id);
extern void *GetBattleActor(int id);
extern int Func_80b78e4(int id, void *a);

int Func_80bb588(int id)
{
	unsigned char *u;
	int j;

	u = _GetUnit(id);
	for (j = 3; j >= 0; j--)
		u[0x12c + j] = 0;
	u[0x131] = 0;
	u[0x132] = 0;
	u[0x133] = 0;
	u[0x134] = 0;
	u[0x135] = 0;
	u[0x136] = 0;
	u[0x137] = 0;
	u[0x138] = 0;
	u[0x139] = 0;
	u[0x13a] = 0;
	u[0x13b] = 0;
	u[0x13c] = 0;
	u[0x13d] = 0;
	u[0x13e] = 0;
	u[0x13f] = 0;
	u[0x140] = 0;
	u[0x141] = 0;
	u[0x142] = 0;
	u[0x143] = 0;
	u[0x144] = 0;
	u[0x145] = 0;
	u[0x146] = 0;
	u[0x147] = 0;
	u[0x148] = 0;
	_CalcStats(id);
	return Func_80b78e4(id, GetBattleActor(id));
}
