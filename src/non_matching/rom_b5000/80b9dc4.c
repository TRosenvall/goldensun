/* Func_80b9dc4 (RunRetreatSequence) -- NON-MATCHING, 1 encoding of 108.
 * 0x080b9dc4, first of two in asm/rom_b5000/rom_b9b30_a_c.s (the second,
 * Func_80b9ec0, is parked in src/non_matching/rom_b5000/80b9ec0.c).
 * objcmp: size identical (216 bytes), 108 encodings, 1 differing, relocations
 * identical.  NO SHIM, NO PIN, NO FLAG.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_b5000/80b9dc4.c asm/rom_b5000/rom_b9b30_a_c.s \
 *     --func Func_80b9dc4
 *
 * THE RESIDUE IS ONE INSTRUCTION, index 25: the ROM has `mov r2, r7` where we
 * emit `mov r2, #0`.  r7 is `flag` (= 0, the return value); r2 is the local
 * `t` that `t = flag; if (b[0x45] != 2) t = 1; if (t == 0) ...` builds.
 *
 * WHICH PASS: cse (.03.cse) folds `(set t (reg flag))` to `(set t (const_int 0))`
 * -- flag's `= 0` is in the same extended block, so fold_rtx substitutes the
 * constant.  Nothing after reload can turn it back: reload_cse_simplify_set
 * refuses because rtx_cost(const 0, SET) = 0 < REGISTER_MOVE_COST 2, and
 * reload_cse_simplify_operands explicitly skips a "cheap CONST_INT"
 * (reload1.c ~8218).  So the ROM's copy must survive from before cse, i.e. in
 * the original cse did NOT know flag == 0 at that point.  How, is the open
 * question.
 *
 * INERT (all still exactly this 1 encoding): `t = b[0x45] != 2`; `t = 0` +
 * set-to-1; t as unsigned char / short; flag = 0 moved to the top of the
 * function; `if (*p > 7) goto rnd;` with a label; a do{}while(0) before
 * `t = flag` or around the t block.  WORSE: `t = flag = 0` / `flag = t = 0`
 * before the branch (6-7, the zero is hoisted but still a constant); a
 * `switch (*p) { case 0 ... 7: }` range (relocations change); folding t away
 * entirely (`if (b[0x45] == 2)`, 8 bytes short).
 *
 * Everything else is exact, including the frame (buf[14] declared BEFORE
 * pair[2] puts pair at sp, buf at sp+4), `(Random() * 10) >> 16 <= 6` with an
 * unsigned Random, the ldrsh loop over buf from n-1 down to -1, and the dead
 * `pair` stores (a local array survives -O2).
 */
extern unsigned char iwram_3001f00[];

extern unsigned char *_GetUnit(int id);
extern void Func_80c10e8(unsigned short *p, int n);
extern void _Func_80175a0(int id);
extern void WaitTextPrompt(void);
extern int Func_80b6b40(int kind, unsigned short *buf);
extern void Func_80b8064(int id);
extern void WaitFrames(int n);
extern unsigned int Random(void);
extern void Func_80bac6c(int id);
extern void Func_80b7e60(int id);

int Func_80b9dc4(unsigned char *p)
{
	short buf[14];
	unsigned short pair[2];
	int *g;
	unsigned char *b;
	unsigned char *u;
	int flag;
	int t;
	int i;

	g = *(int **)iwram_3001f00;
	b = *(unsigned char **)((char *)iwram_3001f00 - 0x8c);
	g[0] = 0x80 << 6;
	g[4] = 1;
	Func_80c10e8(0, 0);
	flag = 0;
	if (*p <= 7) {
		t = flag;
		if (b[0x45] != 2)
			t = 1;
		if (t == 0) {
			_Func_80175a0(0x847);
			WaitTextPrompt();
		} else {
			for (i = Func_80b6b40(1, (unsigned short *)buf) - 1; i != -1; i--) {
				u = _GetUnit(buf[i]);
				if (u[0x13b] == 0 && u[0x13c] == 0) {
					Func_80b8064(buf[i]);
					WaitFrames(8);
				}
			}
			WaitFrames(0x16);
			flag = 1;
		}
	} else if ((Random() * 10) >> 16 <= 6) {
		pair[0] = *p;
		pair[1] = 0xff;
		Func_80b8064(*p);
		WaitFrames(8);
		Func_80bac6c(*p);
		Func_80b7e60(*p);
	} else {
		_Func_80175a0(0x847);
		WaitTextPrompt();
	}
	g[4] = 0;
	return flag;
}
